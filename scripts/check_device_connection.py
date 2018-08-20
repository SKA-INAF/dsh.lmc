#!/usr/bin/env python

##################################################
###          MODULE IMPORT
##################################################


## STANDARD MODULES
import os
import sys
import subprocess
import string
import time
import signal
from threading import Thread


## COMMAND-LINE ARG MODULES
import getopt
import argparse
import collections

## TANGO MODULES
import tango
import PyTango
##################################################

#### CALLBACK CLASS ####
class CallBack(object):

	def push_event(self, event):
		#print event
		da= event.attr_value
		if da is not None:
			if da.type==PyTango.DevEnum:
			#	print '==> ENUM ATTR!'
				ap= PyTango.AttributeProxy(event.attr_name)
				attr_config= ap.get_config()
				enum_label= attr_config.enum_labels[da.value]
				
				print ('Event %s (attr=%s): value=%s (enum=%s), quality=%s (date=%s, data_type=%s, data_format=%s)') % (event.event,da.name, da.value,enum_label, da.quality, da.time, da.type, da.data_format)
			else:
				print ('Event %s (attr=%s): value=%s, quality=%s (date=%s, data_type=%s, data_format=%s)') % (event.event,da.name, da.value, da.quality, da.time, da.type,da.data_format)
			

#### GET SCRIPT ARGS ####
def get_args():
	"""This function parses and return arguments passed in"""
	parser = argparse.ArgumentParser(description="Parse args.")
	parser.add_argument('-d', '--device', dest='device_name', required=True, type=str, action='store',help='Device name')
	parser.add_argument('-a', '--attr', dest='attr_name', required=True, type=str, action='store',help='Attr name')
	parser.add_argument('-e', '--event', dest='event_type', nargs='?',required=False, type=str, const='PERIODIC', choices=['PERIODIC','CHANGE','USER','ARCHIVE'], action='store',help='Event type')
	parser.add_argument('-r', '--retries', dest='nretries', nargs='?', const=3, type=int, required=False, action='store',help='nRetries')
	parser.add_argument('-s', '--sleep', dest='sleep_time', nargs='?', const=5, type=int, required=False, action='store',help='SleepTime')
	
	args = parser.parse_args()	

	return args

##############
##   MAIN   ##
##############
def main():
	"""Main function"""

	## Get script args
	try:
		args= get_args()
	except Exception as ex:
		logging.error("Failed to get and parse options (err=%s)",str(ex))
		return 1

	device_name= args.device_name
	attr_name= args.attr_name
	event_type= args.event_type
	nretries= args.nretries
	sleep_time= args.sleep_time

	if nretries is None:
		nretries = 3
	if sleep_time is None:
		sleep_time = 5

	print("*** ARGS ***")
	print("device_name: %s" % device_name)
	print("attr_name: %s" % attr_name)
	print("event_type: %s" % event_type) 
	print("nretries: %d" % nretries) 
	print("sleep_time: %d" % sleep_time) 
	print("************")
		
	if event_type == 'PERIODIC' :
		evt_type= PyTango.EventType.PERIODIC_EVENT
	elif event_type=='CHANGE' :
		evt_type= PyTango.EventType.CHANGE_EVENT
	elif event_type=='USER' :
		evt_type= PyTango.EventType.CHANGE_EVENT
	elif event_type=='ARCHIVE' :
		evt_type= PyTango.EventType.ARCHIVE_EVENT
	else:
		print('Invalid event type given (%s)',event_type)
		return 1

	## Create callback
	cb= CallBack()

	## Create device proxy
	creation_succeeded= False
	for retry_counter in range(0,nretries): 

		## Create device proxy
		print('Connecting to device proxy %s [attempt #%s]...' % (str(retry_counter),str(device_name)))		
		try:
			dp= PyTango.DeviceProxy(device_name)	
	
		except PyTango.DevFailed as df:
			print(str(df))
			print("Failed to connect to device proxy %s, sleeping for %s seconds and retry...", str(device_name), str(sleep_time))
			time.sleep(sleep_time)
			continue
	
		creation_succeeded= True
		break

	if not creation_succeeded:
		print ("Failed to create device proxy %s after #%s attempts!"% (str(device_name),str(nretries)))
		return 1
	
	## Try some pinging
	nping_failed_to_stop= 10
	npings= 100
	ping_failed= False
	npings_good= 0.
	npings_failed= 0.
	ping_time_sum= 0.
	sleep_ping_time= 1

	for ping_counter in range(0,npings): 
		try:
			ping_time= dp.ping()
			ping_time_sum= ping_time_sum+ping_time
			npings_good= npings_good+1
			
		except PyTango.DevFailed as df:
			print("Failed to ping device proxy %s, sleeping for %s seconds and retry..." % (str(device_name), str(sleep_ping_time)))
			npings_failed= npings_failed+1
			time.sleep(sleep_ping_time)
			continue

  # Print some stats
	print("*** PING RESULTS ***")
	print("Good pings: %s") % str(npings_good/npings*100.)
	print("Ping avg (ms)= %s") % str(ping_time_sum/npings_good)
	print("Failed pings: %s ") % str(npings_failed/npings*100.)
	print("********************")

	## Subscribe to events
	try:
		dp.subscribe_event(attr_name, evt_type, cb, [], True) 
	except PyTango.DevFailed as df:
		print(str(df))
		print("Failed to subscribe to event!")
		return 1
	
	print("Tango event watcher start")

	while True:
		time.sleep(1)
	

###################
##   MAIN EXEC   ##
###################
if __name__ == "__main__":
	#main()
	sys.exit(main())
