#!/usr/bin/python

##################################################
###          MODULE IMPORT
##################################################
import os
import sys
import subprocess
import string
import time
from threading import Thread

import getopt
import argparse
import collections
##from configparser import ConfigParser, BasicInterpolation, ExtendedInterpolation
##from configobj import ConfigObj
import logging
##import json
##import jsonref

import xml.etree.ElementTree as ET
import lxml
from lxml import etree
from lxml import objectify

import tango
import PyTango
##################################################

#### CLASS EXECUTE PROCESS ####
class ProcessExecutor:

	""" Class to execute a process"""    
	def __init__(self, processDef):
		self.processDef = processDef
		self.t= None

	def start(self):
		try:
			self.proc = subprocess.Popen( self.processDef, 
																		shell=True,
																		stdin=subprocess.PIPE,
																		stdout=subprocess.PIPE,
																		stderr=subprocess.STDOUT)
		except:
			logging.error('Exception occurred while defining process %s to be executed!',self.processDef)
			return -1
   
		#self.__sysout('[ctl] Started ' + string.join(self.processDef, ' ') + '\n')
		self.__sysout('[ctl] Started ' + self.processDef + '\n')

		try:
			self.t = Thread(target=self.consumeOutput)
			self.t.start()
			#while self.t.isAlive(): 
			#	self.t.join(1)
		except (KeyboardInterrupt, SystemExit):
			logging.info('Received keyboard interrupt, quitting threads...')
			sys.exit()

		return 0

	def consumeOutput(self):
		proc = self.proc
		while True:
			line = proc.stdout.readline()
			if not line:
				break
			#self.__sysout(line)
		#self.__sysout('[ctl] Terminated\n')

	def __sysout(self, line):
		sys.stdout.write('procex[%d]: %s' % (self.proc.pid, line))


### CLASS SERVERS
class ServerInfo :
	def __init__ (self,server_name,server_instance):
		self.name = server_name
		self.instance = server_instance
		self.devices = []
	def add_device(self,device_name):	
		self.devices.append(device_name)

### COMVERT DATA ####
def convert(data):
	if isinstance(data, basestring):
		return str(data)
	elif isinstance(data, collections.Mapping):
		return dict(map(convert, data.iteritems()))
	elif isinstance(data, collections.Iterable):
		return type(data)(map(convert, data))
	else:
		return data


#### GET SCRIPT ARGS ####
def get_args():
	"""This function parses and return arguments passed in"""
	parser = argparse.ArgumentParser(description="Parse args.")
	parser.add_argument('-i', '--input', dest='inputfile', required=True, type=str, action='store',help='Config file')
	parser.add_argument('-s', '--schema', dest='schemafile', nargs='?', required=False, type=str, const='', action='store',help='Config Schema file')
	parser.add_argument('-v', '--verbosity', dest='loglevel', nargs='?',required=False, type=str, const='INFO', choices=['CRITICAL','ERROR','WARN','INFO','DEBUG','OFF'], action='store',help='Script log level')
	parser.add_argument('-c', '--clear', dest='clear_servers', required=False, action='store_true', default=False, help='Clear existing servers')

	args = parser.parse_args()	
	inputfile= args.inputfile
	schemafile= args.schemafile
	loglevel= args.loglevel
	clear_servers= args.clear_servers
	return inputfile, schemafile, loglevel, clear_servers

#### CHECK SECTION ####
def is_section(config_section):
	""" Check that given config element is a section"""
	try:
		config_section.keys()
	except AttributeError:
		return False
	return True



#### FIND TANGO SERVER IN DB ####
def has_server(db,server_full_name):
	""" Check if server is present in DB """
	logger= logging.getLogger(__name__)

	## Find server is DB
	hasServer= False
	try :
		matched_server_name= db.get_server_list(server_full_name).value_string
		if matched_server_name :
			hasServer= True	 		
	except PyTango.DevFailed as df:
		print(str(df))
		return False

	return hasServer

#### FIND TANGO DEVICE IN DB ####
def has_device(db,server_name,class_name,dev_name):
	""" Check is given device is already registered in DB """
	logger= logging.getLogger(__name__)

	hasDevice= False

	## Find device in DB
	try :
		dev_info= db.get_device_info(dev_name)
	except PyTango.DevFailed as df:
		#print(str(df))
		logging.info("Device %s not present in TangoDB...",dev_name)
		return False

	## If it is found check that dev info corresponds to
	## class & server names
	logger.debug("class_name: %s ",dev_info.class_name)	
	logger.debug("ds_full_name: %s ",dev_info.ds_full_name)	
	if class_name==dev_info.class_name and server_name==dev_info.ds_full_name :
		hasDevice= True

	return hasDevice

#### REGISTER DEVICE ####
def register_device(db,server_name,class_name,dev_name,dev_alias):
	""" Register a device in DB """
	logger= logging.getLogger(__name__)

	## Create a DevInfo
	dev_info = PyTango.DbDevInfo()
	dev_info.name = dev_name
	dev_info._class = class_name
	dev_info.server = server_name

	## Add device to DB
	try:
		db.add_device(dev_info)
	except PyTango.DevFailed as df:
		print(str(df))
		logger.error("Failed to add device %s to DB!",dev_name)
		return 1
			
	## Register alias
	if dev_alias:
		try:
			db.put_device_alias(dev_name,dev_alias)
		except PyTango.DevFailed as df:
			print(str(df))
			logger.error("Failed to register alias (%s) to device %s in DB!",dev_alias,dev_name)
			return 1

	return 0


### INIT DEVICE PROPERTIES ###
def init_device_properties(db,dev_name,props):
	""" Init given properties in device """
	## Check if property list is empty
	if not props:
		logging.warn("Empty property list given, nothing to be done!")
		return 0
	print(props)
	
	## Get properties present in device
	try:
		dev_prop_data= db.get_device_property_list(dev_name,'*')
	except PyTango.DevFailed as df:
		print(str(df))
		logging.error("Failed to get property list of device %s!",dev_name)
		return 1
	##print(dev_prop_data)

	## Check if device has properties
	dev_props= dev_prop_data.value_string
	if not dev_props:
		logging.info("Device has no property defined yet...")
	

	## Loop over input properties 
	for prop in props:
		## Get prop name
		try:
			prop_name= prop['name']
			prop_value= prop['value']
		except KeyError:
			logging.error("Failed to get keys from device property!")
			return 1
		logging.debug("Initializing dev property %s with value %s ...",prop_name,prop_value)

		## Check if given config property indeed exist for device
		#if not prop_name in dev_props:
		#	logging.error("Property %s to be configured does not exist in device (check your config file)!",prop_name)
		#	return 1

		## Fill dev prop dict	
		#try:
		#	dev_prop= db.get_property()
		#	#dev_props[prop_name]= str(prop_value)
		#except KeyError:
		#	logging.error("Failed to get keys from device property!")
		#	return 1

		## Set property value in DB
		try:
			d = dict([(prop_name, prop_value)])
			##db.put_device_property(dev_name, d )
			db.put_device_property(dev_name, {prop_name:prop_value} )	
			##db.put_device_property(dev_name,dev_props )
		except PyTango.DevFailed as df:
			print(str(df))
			logging.error("Failed to put property %s to DB!",prop_name)
			return 1

	return 0


#### INIT DEVICES FROM XML ####
def init_dev_from_xml_config(tree,clear_servers):
	""" Loop over xml config and initialize devices """

	registered_servers= []

	## Initialize TangoDB
	try :
		db= PyTango.Database()
	except PyTango.DevFailed as df:
		print(str(df))
		errMsg= "Failed to get access to TangoDB (did you forget to start Tango?)!"
		logging.error(errMsg)
		#return 1
		raise Exception(errMsg)

	## Traverse the XML document
	for cs_node in tree.iter('ControlSystem'):
    
		## Get domain name
		domain_name_node = cs_node.find('./domain_name')
		if domain_name_node is None :
			errMsg= "Cannot find 'domain_name' in configuration file (missing?)!"
			logging.error(errMsg)
			#return 1
			raise Exception(errMsg)

		domain_name_node.text = domain_name_node.text.strip()
		if domain_name_node.tail is not None :
			domain_name_node.tail = domain_name_node.tail.strip()
		domain_name= domain_name_node.text
		if domain_name is '' :
			errMsg= "Empty domain name found in config file (check config)!"
			logging.error(errMsg)
			#return 1
			raise Exception(errMsg)

		logging.info("Domain name: %s",str(domain_name))

		## Get host name
		host_name_node = cs_node.find('./host_name')
		if host_name_node is None :
			errMsg= "Cannot find 'host_name' in configuration file (missing?)!"
			logging.error(errMsg)
			#return 1
			raise Exception(errMsg)

		host_name_node.text = host_name_node.text.strip()
		if host_name_node.tail is not None :
			host_name_node.tail = host_name_node.tail.strip()
		host_name= host_name_node.text
		if host_name is '' :
			errMsg= "Empty host name found in config file (check config)!"
			logging.error(errMsg)
			#return 1
			raise Exception(errMsg)

		logging.info("Host name: %s",str(host_name))

		## Get device servers
		servers_node= cs_node.find('./device_servers')
		if servers_node is None :
			errMsg= "Cannot find 'device_servers' in configuration file (missing?)!"
			logging.error(errMsg)
			raise Exception(errMsg)
			#return 1		

		## Iterate over device servers found
		for server_node in servers_node.iter('server'):

			## Get server name & instance 
			if 'name' not in server_node.attrib or 'instance' not in server_node.attrib:
				errMsg= "Cannot find 'name' and/or 'instance' attr in server element!"
				logging.error(errMsg)
				raise Exception(errMsg)
				#return 1	

			server_name= server_node.attrib.get('name').strip()		
			server_id= server_node.attrib.get('instance').strip()
			server_full_name= server_name + "/" + server_id
			server_info= ServerInfo(server_name,server_id)

			## Get server info (optional)
			if 'startup_level' in server_node.attrib :
				startup_level= server_node.attrib.get('startup_level')
			else :
				startup_level= 1

			if 'astor_controlled' in server_node.attrib :
				astor_controlled= server_node.attrib.get('astor_controlled')	
			else :
				astor_controlled= 0

			logging.debug("Server info: Host=%s, StartupLevel=%d, AstorControlled=%s",str(host_name),str(startup_level),str(astor_controlled))

			## Check if server is already present
			if has_server(db,server_full_name):
				logging.info('Server %s is already present in TangoDB',server_full_name)
				if clear_servers:
					logging.info('Deleting server %s from TangoDB',server_full_name)
					try:
						db.delete_server(server_full_name)
					except PyTango.DevFailed as df:
						#print(str(df))
						logging.error("Failed to delete server %s from DB!",server_full_name)
						#return 1
						raise

			logging.info("Configuring device server %s [instance %s]",server_name,server_id)

			## Iterate over classes
			for class_node in server_node.iter('class'):
				## Get class name 
				if 'name' not in class_node.attrib :
					errMsg= "Cannot find 'name' attr in class element!"
					logging.error(errMsg)
					#return 1	
					raise Exception(errMsg)
									
				class_name= class_node.attrib.get('name').strip()

				## Iterate over devices
				for device_node in class_node.iter('device'):
					## Get device domain name (if not present use main domain property) 
					if 'domain' not in device_node.attrib :
						logging.debug("No 'domain' attr present in class element, using main domain name property...")
						device_domain= domain_name
					else :
						device_domain= device_node.attrib.get('domain').strip()	

					## Get device name 
					if 'name' not in device_node.attrib :
						errMsg= "Cannot find 'name' attr in class element!"
						logging.error(errMsg)
						#return 1	
						raise Exception(errMsg)

					device_name= device_node.attrib.get('name').strip()
					
					## Get device family name 
					if 'family' not in device_node.attrib :
						errMsg= "Cannot find 'family' attr in class element!"
						logging.error(errMsg)
						#return 1	
						raise Exception(errMsg)

					device_family= device_node.attrib.get('family').strip()

					## Get device alias
					if 'alias' in device_node.attrib and device_node.attrib.get('alias') is not '':
						has_device_alias= True
						device_alias= device_node.attrib.get('alias').strip()
					else :
						has_device_alias= False
						device_alias= ''

					device_full_name= device_domain + str('/') + device_family + str('/') + device_name
					server_info.add_device(device_full_name)
					logging.info("Registering device server %s [instance %s, class %s]: device_full_name=%s [alias=%s]",server_name,server_id,class_name,device_full_name,device_alias)


					## Check if device is already registered in DB
					hasDevice= has_device(db,server_full_name,class_name,device_full_name)
					if hasDevice :
						logging.info("Device %s is already registered in DB...",device_full_name)
				
						## db.delete_device(device_full_name) 

					else :
						logging.info("Device %s is not present in DB, registering it...",device_full_name)
					if register_device(db,server_full_name,class_name,device_full_name,device_alias) != 0 :
						errMsg= "Failed to registed device " + device_full_name + " in DB!"
						logging.error(errMsg)
						#return 1
						raise Exception(errMsg)
						

					## Get device configuration node
					device_configuration_node= device_node.find('./DeviceConfiguration')
					if device_configuration_node is None :
						errMsg= "Cannot find 'DeviceConfiguration' node in configuration file (missing?)!"
						logging.error(errMsg)
						#return 1		
						raise Exception(errMsg)

					## Get device properties
					device_properties_node= device_configuration_node.find('./DeviceProperties')
					if device_properties_node is None :
						logging.warn("Cannot find 'DeviceProperties' node in configuration file...nothing to be configured!")
						#return 0	
						continue

					## Iterate over device properties and create dictionary
					device_properties = []
					
					#device_property_dict = {}
					for dev_prop_node in device_properties_node.findall('DeviceProperty') :
						## Get prop name
						if 'name' not in dev_prop_node.attrib :
							errMsg= "Cannot find 'name' attr in device_property element!"
							logging.error(errMsg)
							#return 1	
							raise Exception(errMsg)

						dev_prop_name= dev_prop_node.attrib.get('name').strip()

						## Get prop value(s)
						device_property_dict = {}
						device_property_dict['name']= dev_prop_name
						nvalues= len(dev_prop_node.xpath(".//Value"))
						if nvalues==1 :
							dev_prop_value_node= dev_prop_node.find('./Value')
							dev_prop_value_node.text = dev_prop_value_node.text.strip()
							if dev_prop_value_node.tail is not None :
								dev_prop_value_node.tail = dev_prop_value_node.tail.strip()
							dev_prop_value= dev_prop_value_node.text
							device_prop_values= dev_prop_value
						else :
							device_prop_values= []
							for dev_prop_value_node in dev_prop_node.findall('./Value') :
								dev_prop_value_node.text = dev_prop_value_node.text.strip()
								if dev_prop_value_node.tail is not None :
									dev_prop_value_node.tail = dev_prop_value_node.tail.strip()
								dev_prop_value= dev_prop_value_node.text
								device_prop_values.append(dev_prop_value)
								logging.info("Processing dev prop %s : value %s",dev_prop_name,str(dev_prop_value))					

							#if dev_prop_name in device_property_dict:
								# append the new number to the existing array at this slot
								#device_property_dict[dev_prop_name].append(dev_prop_value)
							#	device_prop_values.append(dev_prop_value)
							#else:	
							#	if nvalues == 1:
							#		logging.info("Single dev prop value")
									#device_property_dict[dev_prop_name] = dev_prop_value
							#		device_prop_values= dev_prop_value									
							#	else :
									# create a new array in this slot
									#device_property_dict[dev_prop_name] = [dev_prop_value]
							#		device_prop_values.append(dev_prop_value)

							if not device_prop_values :
								errMsg= "Empty list of property values!"
								logging.error(errMsg)
								#return 1
								raise Exception(errMsg)
								
						## Fill dev prop value key	
						device_property_dict['value']= device_prop_values 
				
						## Append dictionary to list
						device_properties.append(device_property_dict)
						print(device_property_dict)	
					
					## End loop device properties						
					print(device_properties)

					if device_properties :
						if init_device_properties(db,device_full_name,convert(device_properties)) != 0 :
							errMsg= "Failed to initialize dev properties of device " + device_full_name + "!" 
							logging.error(errMsg)
							#return 1
							raise Exception(errMsg)
					else :
						errMsg= "Empty device property list detected, check!"
						logging.warn(errMsg)
						#return 1
						raise Exception(errMsg)

			## Put server info in DB (startup level, etc)
			serv_info= PyTango.DbServerInfo()
			serv_info.name= str(server_full_name)
			serv_info.host= str(host_name)
			serv_info.mode= int(astor_controlled=='true')
			serv_info.level= int(startup_level)
			try :
				db.put_server_info(serv_info)
			except PyTango.DevFailed as df:
				print(str(df))
				errMsg= "Failed to put server info in TangoDB (TangoDB off or invalid syntax?)!"
				logging.error(errMsg)
				#return 1
				raise Exception(errMsg)

			## If registration is completed with success add server_info to list
			registered_servers.append(server_info)

			## Start the server process
			#logging.info("Starting server %s process...",server_name)
			#server_verbosity= 1
			#[start_status,pe]= start_server_process(db,server_name,server_id,server_verbosity)
			#if start_status != 0 :
			#	logging.error("Failed to start server %s ",server_name)
			#server_threads.append(pe.t)
			
			## Wait until device server has completed startup (e.g. they should be in ON State)
			#final_state= tango._tango.DevState.ON
			#startup_timeout= 10 # in seconds
			#sleep_time= 2 # in seconds
			#startup_reached_status= wait_server_startup(db,server_full_name,final_state,startup_timeout,sleep_time)
			#if startup_reached_status !=0 :
			#	logging.error("Server %s did not startup within the assumed timeout!",server_full_name)
				
			## Joining device server thread?
	

	logging.info("End config file processing")


	#return 0
	return registered_servers

#### INIT DEVICES ####
def init_dev_from_config(config):
	""" Loop over config and initialize devices """

	## Initialize TangoDB
	try :
		db= PyTango.Database()
	except PyTango.DevFailed as df:
		print(str(df))
		logging.error("Failed to get access to TangoDB (did you forget to start Tango?)!")
		return 1

	## Get Tango servers list
	server_key= 'servers'
	try :
		servers= config[server_key]
	except KeyError:
		logging.error("Invalid key (%s) given ",server_key)
		return 1
	nservers= len(servers)
	if nservers==0 :
		logging.warning("No Tango servers found in config file (check it), nothing to be done...")
		return 1
	logging.info("#%d Tango servers to be registered...",nservers)

	## Iterate over servers
	for server in servers:
		try:
			server_name= server['name']
			server_id= server['instance']
			server_full_name= server_name + "/" + server_id
			devices= server['devices']
			#print(server)
			#print("devices: ndev=%d",len(devices))
		except KeyError:
			logging.error("Invalid key detected!")
			return 1

		## Iterate over devices in server
		for device in devices:
			try:
				device_name= device['name']
				class_name= device['class']
			except KeyError:
				logging.error("Invalid key detected!")
				return 1

			logging.info("Registering device %s (class=%s) in server %s ...", device_name,class_name,server_full_name)
	
			## Check if device is already registered in DB
			hasDevice= has_device(db,server_full_name,class_name,device_name)
			if hasDevice :
				logging.info("Device %s is already registered in DB...",device_name)
			else :
				logging.info("Device %s is not present in DB, registering it...",device_name)
				if register_device(db,server_full_name,class_name,device_name) != 0 :
					logging.error("Failed to registed device %s in DB!",device_name)
					return 1

			## Initialize device properties (Json is expressed in unicode, convert() before passing to function)
			hasDevProperties= False
			try:
				dev_properties= device['device_properties']							
				if dev_properties :
					hasDevProperties= True
				else:
					logging.warn("Empty device property list detected, check!")
					return 1
			except KeyError:
				logging.info("No device properties to be initialized for this device...")

			if hasDevProperties and init_device_properties(db,device_name,convert(dev_properties)) != 0 :
				logging.error("Failed to initialize dev properties of device %s!",device_name)
				return 1
				

	logging.info("End config file processing")

	return 0


## VALIDATE XML CONFIG AGAINST SCHEMA
def parse_xml_config(xml_filename,schema_filename) :
	""" Validate xml config file """
	 	
	## Clear any previous errors
	lxml.etree.clear_error_log()

	# Parse string of XML
	try:		
		xml_doc = lxml.etree.parse(xml_filename)
		print(xml_doc)
	
	except lxml.etree.XMLSyntaxError, xse:
			# XML not well formed
			print "XMLSyntaxError occurred!"
			print xse
  
	# Validate XML against schema?	  	
	if schema_filename :
		
		try:
			# Get the XML schema to validate against
			schema = lxml.etree.XMLSchema(file = schema_filename)
			# Validate parsed XML against schema returning a readable message on failure
			schema.assertValid(xml_doc)
			# Validate parsed XML against schema returning boolean value indicating success/failure
			print 'schema.validate() returns "%s".' % schema.validate(xml_doc)

		except lxml.etree.XMLSchemaParseError, xspe:
			# Something wrong with the schema (getting from URL/parsing)
			print "XMLSchemaParseError occurred!"
			print xspe

		except lxml.etree.XMLSyntaxError, xse:
			# XML not well formed
			print "XMLSyntaxError occurred!"
			print xse
    
		except lxml.etree.DocumentInvalid, di:
			# XML failed to validate against schema
			print "DocumentInvalid occurred!"

		error = schema.error_log.last_error
		if error:
			# All the error properties (from libxml2) describing what went wrong
			print 'domain_name: ' + error.domain_name
			print 'domain: ' + str(error.domain)
			print 'filename: ' + error.filename # '<string>' cos var is a string of xml
			print 'level: ' + str(error.level)
			print 'level_name: ' + error.level_name # an integer
			print 'line: ' + str(error.line) # a unicode string that identifies the line where the error occurred.
			print 'message: ' + error.message # a unicode string that lists the message.
			print 'type: ' + str(error.type) # an integer
			print 'type_name: ' + error.type_name
			return None

	return xml_doc

## PARSE CONFIG  ##
def parse_xml_config2(filename):
	""" Parse xml config file """
	
	try:
		with open(filename, 'rt') as data_file:
			cfg = ET.parse(data_file)
	except IOError:
		logging.error("Cannot find/open config file %s!",filename)	
		return None
	except SyntaxError:
		logging.error("Syntax errors, failed to parse file %s!",filename)
		return None
	except ValueError:
		logging.error("Failed to parse file %s!",filename)
		return None

	print(cfg)

	return cfg

## PARSE CONFIG ##
def parse_config(filename):
	""" Parse config file """
	try:
		with open(filename) as data_file:
			cfg = json.load(data_file)
			#cfg = jsonref.loads(data_file)
	except IOError:
		logging.error("Cannot find/open config file %s!",filename)	
		return None
	except SyntaxError:
		logging.error("Syntax errors, failed to parse file %s!",filename)
		return None
	except ValueError:
		logging.error("Failed to parse file %s!",filename)
		return None

	cfg_str= json.dumps(cfg)
	print(cfg_str)

	## Do some other checks
	## ...

	## Evaluate references
	cfg_ref = jsonref.loads(cfg_str)
	print(cfg_ref)

	return cfg_ref
	#return cfg

## INIT LOGGER ##
def init_logger(loglevel):
	""" Initialize logger """
	logger = logging.getLogger(__name__)
	
	if loglevel == "CRITICAL" :
		level= logging.CRITICAL
	elif loglevel == "ERROR" :
		level= logging.ERROR
	elif loglevel == "WARN" :
		level= logging.WARNING
	elif loglevel == "INFO" :
		level= logging.INFO
	elif loglevel == "DEBUG" :
		level= logging.DEBUG
	elif loglevel == "OFF" :
		level= logging.OFF
	else :
		level= logging.INFO
	
	logging.basicConfig(level=level, format="%(asctime)s [%(levelname)s] - %(message)s")


### START DEVICE SERVER PROCESS ###
def start_server_process(db,server_name,server_instance,verbosity):

	""" Start a device server process"""	      

	# Check device install dir environment variable 
	install_env_var= 'DSHLMC_DEVICE_DIR'
	try :
		dev_install_dir= os.environ[install_env_var]	
	except KeyError :
		logging.error("Cannot get value of env variable %s (check given variable name) where devices are installed!",install_env_var)
		return -1

	# Set cmd name & args
	server_full_name= server_name + '/' + server_instance 
	cmd_args = ' %s -v%s' % (server_instance, verbosity)
	cmd = dev_install_dir + '/' + server_name + cmd_args
	logging.info("Starting server %s (cmd=%s)",server_name,cmd)	

	# Run the command in a different thread
	pe = ProcessExecutor(cmd)
	status= pe.start()
	if status !=0 :
		logging.error('Failed to start device server %s!',server_name)
		return [-1,None]

	## Wait until device server has completed startup (e.g. they should be in ON State)
	final_state= tango._tango.DevState.INIT
	startup_timeout= 10 # in seconds
	sleep_time= 2 # in seconds
	startup_reached_status= wait_server_startup(db,server_full_name,final_state,startup_timeout,sleep_time)
	if startup_reached_status !=0 :
		logging.error("Server %s did not startup within the assumed timeout!",server_full_name)

	return [0,pe]

### WAIT FOR DEVICE SERVER PROCESS TO COMPLETE STARTUP ###
def wait_server_startup(db,full_server_name,final_state,timeout,sleep_time):

	""" Wait for device server to complete the startup """
	status= -1

	## Get all devices registered in this server 
	try:
		devices_in_server= db.get_device_class_list(full_server_name)
	except PyTango.DevFailed as df:
		print(str(df))
		logging.error("Failed to get list of devices present in server %s ",full_server_name)
		return -1

	device_names= []
	for item in xrange(0,devices_in_server.size()-1,2) :
		device_name= devices_in_server[item]
		class_name= devices_in_server[item+1]
		if class_name=='DServer':
			continue
		device_names.append(device_name)

	print("device_names= ",device_names)	

	## Check if devices have started
	for device_name in device_names:
		logging.debug("Checking if device %s is started...", device_name)

		## Start timeout
		timeout_start = time.time()
		device_proxy= None
		device_started= False

		while time.time() < timeout_start + timeout:
		
			## Create proxy to device 
			## NB: If fails sleep and retry later
			if device_proxy is None:
				try:
					device_proxy= PyTango.DeviceProxy(device_name)			

				except PyTango.DevFailed as df:
					print(str(df))
					logging.debug("Failed to create proxy to device %s, will sleep a bit and retry later...",device_name)
					time.sleep(sleep_time)
					continue

			## Get device state and check if final state is reached
			if device_proxy is not None:
				try:
					state = device_proxy.state()
					if state==final_state:
						logging.info("Device %s has reached desired state, exit loop..",device_name)
						device_started= True
						break

				except PyTango.DevFailed as df:
					print(str(df))
					logging.debug("Failed to get device %s State info, will sleep a bit and retry later...",device_name)
					time.sleep(sleep_time)
					continue

			time.sleep(sleep_time)
		
		## Kill server at the end
		if device_proxy is not None:	
			logging.info("Killing device %s [adm dev=%s]",device_name,device_proxy.adm_name())
			admin_device= PyTango.DeviceProxy(device_proxy.adm_name())
			admin_device.Kill()

		## Kill OS process at the end
		try:
			dev_info = db.get_device_info(device_name)
			if dev_info.pid != 0:
				os.kill(dev_info.pid, signal.SIGTERM)  #o r signal.SIGKILL
				logging.info("Killed process PID %s for device %s",dev_info.pid,device_name)
		except Exception as ex:
			logging.warn("No process to be killed for device %s",device_name)

		## Check if this device was started
		if not device_started:
			logging.error("Device %s in server %s did not start within the timeout (%s s)!",device_name,full_server_name,str(timeout))
			return -1

		
	return 0

##############
##   MAIN   ##
##############
def main():
	"""Main function"""
	
	## Get script args
	filename, filename_schema, loglevel, clear_servers = get_args()
	
	## Initialize logger
	init_logger(loglevel)
	logging.info('Configuring Tango devices from config file %s ...', filename)

	## Check args
	if not filename:
		logging.error('Empty config filename given!')
		return 1
	if filename_schema:
		hasSchema= True
		logging.info('Config schema provided: %s', filename_schema)
	else :
		hasSchema= False
		logging.info('No config schema provided...')

	if clear_servers:
		logging.info('Servers will be cleared from TangoDB before configuration...')
	else:
		logging.info('Servers will not be cleared from TangoDB...')

	## Parse config file
	#cfg= parse_config(filename)
	#if cfg is None:
	#	logging.error("Failed to open and/or parse config file %s ",filename)
	#	return 1

	#xml_cfg= parse_xml_config(filename)
	#if xml_cfg is None:
	#	logging.error("Failed to open and/or parse xml config file %s ",filename)
	#	return 1

	if hasSchema is True :
		cfg= parse_xml_config(filename,filename_schema)
	else :
		cfg= parse_xml_config(filename,'')

	if cfg is None:
		logging.error("Failed to open and/or parse config file %s ",filename)
		return 1

	## Initialize devices
	#status= init_dev_from_config(cfg)
	#if status != 0 :
	#	logging.error("Tango device initialization failed!")
	#	return 1

	status= init_dev_from_xml_config(cfg,clear_servers)
	if status != 0 :
		logging.error("Tango device initialization failed!")
		return 1
	return 0


###################
##   MAIN EXEC   ##
###################
if __name__ == "__main__":
	#main()
	sys.exit(main())
