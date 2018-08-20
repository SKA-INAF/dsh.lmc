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
import datetime
import time
import numpy as np
import math
from matplotlib import pyplot as plt

## COMMAND-LINE ARG MODULES
import getopt
import argparse
import collections

## TANGO MODULES
import tango
import PyTango
##################################################



#### GET SCRIPT ARGS ####
def get_args():
	"""This function parses and return arguments passed in"""
	parser = argparse.ArgumentParser(description="Parse args.")
	parser.add_argument('-d', '--alarmdevice', dest='alarm_device_name', required=True, type=str, action='store',help='Alarm device name')
	parser.add_argument('-D', '--device', dest='device_name', required=True, type=str, action='store',help='Device name')
	parser.add_argument('-A', '--attr', dest='attr_name', required=True, type=str, action='store',help='Attribute name')
	parser.add_argument('-a', '--alarmattr', dest='alarm_attr_name', required=True, type=str, action='store',help='Alarm attribute name')
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
	alarm_device_name= args.alarm_device_name
	attr_name= args.attr_name
	alarm_attr_name= args.alarm_attr_name
	
	nretries= args.nretries
	sleep_time= args.sleep_time

	if nretries is None:
		nretries = 3
	if sleep_time is None:
		sleep_time = 5

	print("*** ARGS ***")
	print("device_name: %s" % device_name)
	print("alarm_device_name: %s" % alarm_device_name)
	print("attr_name: %s" % attr_name)
	print("alarm_attr_name: %s" % alarm_attr_name)
	print("nretries: %d" % nretries) 
	print("sleep_time: %d" % sleep_time) 
	print("************")
		

	## Create device proxy
	creation_succeeded= False
	for retry_counter in range(0,nretries): 

		## Create device proxy
		print('INFO: Connecting to device proxy %s [attempt #%s]...' % (str(retry_counter),str(device_name)))		
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
		print ("ERROR: Failed to create device proxy %s after #%s attempts!"% (str(device_name),str(nretries)))
		return 1


	# Set device to test mode
	try:
		dp.write_attribute('testMode',True)	
	except PyTango.DevFailed as df:
		print(str(df))
		print 'ERROR: Failed to set device %s in test mode' % device_name
		return 1

	# Disable attribute randomization
	try:
		dp.write_attribute('attrRandomizationEnabled',False)	
	except PyTango.DevFailed as df:
		print(str(df))
		print 'ERROR: Failed to set device %s in test mode' % device_name
		return 1

	# Create attribute proxies
	queuesize= 5
	event_id= -1
	print 'INFO: Creating attribute proxy %s ...' % attr_name
	try:
		ap= PyTango.AttributeProxy(device_name + '/' + attr_name)	
		event_id= ap.subscribe_event(PyTango.EventType.CHANGE_EVENT, queuesize)
	except PyTango.DevFailed as df:
		print(str(df))
		print 'ERROR: Failed to create attribute proxy % s and subscribe events!' % attr_name
		return 1

	print 'INFO: Creating attribute proxy %s ...' % alarm_attr_name
	alarm_event_id= -1
	
	try:
		ap_alarm= PyTango.AttributeProxy(alarm_device_name + '/' + alarm_attr_name)	
		attr_alarm_config= ap_alarm.get_config()
		alarm_event_id= ap_alarm.subscribe_event(PyTango.EventType.CHANGE_EVENT, queuesize)
		alarm_event_id2= ap_alarm.subscribe_event(PyTango.EventType.USER_EVENT, queuesize)
	except PyTango.DevFailed as df:
		print(str(df))
		print 'ERROR: Failed to create attribute proxy % s and subscribe events!' % alarm_attr_name
		return 1

	# Get normal and alarm range
	attr_min= 0
	attr_max= 0
	attr_min_alarm= 0
	attr_max_alarm= 0
	try:
		attr_config= ap.get_config()
		attr_min= float(attr_config.min_value)
		attr_max= float(attr_config.max_value)
		attr_min_alarm= float(attr_config.min_alarm)
		attr_max_alarm= float(attr_config.max_alarm)
	except PyTango.DevFailed as df:
		print(str(df))
		print 'Failed to get attribute % config!' % attr_name
		return 1

	print 'INFO: Attribute %s range (%s,%s), alarm (%s,%s)' % (attr_name,str(attr_min),str(attr_max),str(attr_min_alarm),str(attr_max_alarm))

	# Set alarm & normal values for the test
	attr_alarm_value= attr_max_alarm + 0.5*(attr_max-attr_max_alarm)
	attr_norm_value= attr_min_alarm + 0.5*(attr_max_alarm-attr_min_alarm)
	print 'INFO: attr values normal/alarm=%s/%s' % (str(attr_norm_value),str(attr_alarm_value))


	# Set attribute to normal value
	da= ap.write_read(attr_norm_value)
	attr_set_value= da.value
	if attr_set_value!=attr_norm_value:
		print 'ERROR: Failed to reset attribute to norm value=%s' % str(attr_norm_value)
		return 1

	time.sleep(3)

	# Test alarm latency
	data= []
	for retry_counter in range(0,nretries): 
		print 'INFO: Testing alarm latency for attr %s ...' % alarm_attr_name
	
		# Set attribute in alarm mode
		da= ap.write_read(attr_alarm_value)
		attr_set_value= da.value
		print 'INFO: attr_set_value=%s' % str(attr_set_value)
		
		# Get alarm events
		if attr_set_value==attr_alarm_value:
			## Store start time and attribute read date
			t0 = datetime.datetime.now()
			t_write= da.time.todatetime()

			## Get events from alarm attr
			stop_loop = False
			while not stop_loop:
				event_list1= ap_alarm.get_events(alarm_event_id)	
				event_list2= ap_alarm.get_events(alarm_event_id2)
				event_list= event_list1 + event_list2
				if not event_list:
					continue

				## Loop over alarm attr events
				for event in event_list:
					da= event.attr_value
					if da is None or da.type!=PyTango.DevEnum :
						continue
					t1= datetime.datetime.now()
					alarm_value= attr_alarm_config.enum_labels[da.value]		
					t_alarm= da.time.todatetime()
					dt_attr= t_alarm-t_write
					dt_attr_ms= float(dt_attr.total_seconds() * 1000)
					print ('INFO: Alarm event %s (attr=%s): value=%s (enum=%s), quality=%s (date=%s, data_type=%s, data_format=%s), t_write=%s, t_alarm=%s, dt(ms)=%s') % (event.event,da.name, da.value,alarm_value, da.quality, da.time, da.type, da.data_format,t_write,t_alarm,dt_attr_ms)
					
					if (alarm_value=='UNACK'):
						dt= t1-t0
						dt_ms= float(dt.total_seconds() * 1000)	 
						if retry_counter>1:
							data.append(dt_ms)
						print 'INFO: Alarm no. %s generated : dt(ms)=%s (dt_attr(ms)=%s)' % (str(retry_counter),str(dt_ms),str(dt_attr_ms))
						stop_loop= True
						break

		# Reset attribute to normal state
		da= ap.write_read(attr_norm_value)
		attr_set_value= da.value
		if attr_set_value!=attr_norm_value:
			print 'ERROR: Failed to reset attribute to norm value=%s' % str(attr_norm_value)
			return 1
		else:
			print 'INFO: Reset attribute to normal value %s' % str(attr_norm_value)

		# Sleeping and retry alarm	
		print 'INFO: Sleeping %s seconds ...' % str(sleep_time)
		time.sleep(sleep_time)


	# Enable attribute randomization
	try:
		dp.write_attribute('attrRandomizationEnabled',True)	
	except PyTango.DevFailed as df:
		print(str(df))
		print 'ERROR: Failed to set device %s in test mode' % device_name
		return 1

	## Plot histogram
	if data:
		nbins= 20
		bins = np.linspace(min(data),max(data),nbins) # fixed number of bins
		plt.xlim([min(data)-10, max(data)+10])
		plt.hist(data, bins=bins, alpha=0.5)
		plt.title('Alarm latency')
		plt.xlabel('dt (ms)')
		plt.ylabel('counts')
		plt.show()
	

###################
##   MAIN EXEC   ##
###################
if __name__ == "__main__":
	#main()
	sys.exit(main())
