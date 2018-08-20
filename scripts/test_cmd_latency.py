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
	parser.add_argument('-d', '--device', dest='device_name', required=True, type=str, action='store',help='Device name')
	parser.add_argument('-c', '--cmd', dest='cmd_name', required=True, type=str, action='store',help='Cmd name')
	parser.add_argument('-C', '--cmdpost', dest='cmd_name_post', required=True, type=str, action='store',help='Cmd name after')	
	parser.add_argument('-a', '--cmdargin', dest='cmd_argin', required=False, type=str, action='store',help='Cmd argin')
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
	cmd_name= args.cmd_name
	cmd_name_post= args.cmd_name_post
	if args.cmd_argin is None:
		cmd_argin= None
	else:
		cmd_argin= str(args.cmd_argin)

	nretries= args.nretries
	sleep_time= args.sleep_time

	if nretries is None:
		nretries = 3
	if sleep_time is None:
		sleep_time = 5

	print("*** ARGS ***")
	print("device_name: %s" % device_name)
	print("cmd_name: %s" % cmd_name)
	print("cmd_name (post): %s" % cmd_name_post)
	print("cmd_argin: %s" % cmd_argin)
	print("nretries: %d" % nretries) 
	print("sleep_time: %d" % sleep_time) 
	print("************")
		

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


	## Execute command
	data= []
	for retry_counter in range(0,nretries): 
		## Call command
		print 'INFO: Calling cmd %s (attempt no. %s)' % (str(cmd_name),str(retry_counter))
		t0 = datetime.datetime.now()
		try:
			dp.command_inout(cmd_name,cmd_argin)	
		except PyTango.DevFailed as df:
			print(str(df))
			print("Failed to call command on device!")
			continue
		t1 = datetime.datetime.now()
		dt= t1-t0
		dt_ms= float(dt.total_seconds() * 1000)
		data.append(dt_ms)
		print 'INFO: Called cmd %s (attempt no. %s): dt(ms)=%s' % (str(cmd_name),str(retry_counter),str(dt_ms))

		## Sleeping a bit	
		print 'INFO: Sleeping for %s seconds ...' % str(sleep_time)
		time.sleep(sleep_time)

		## Call post command to restore initial condition
		print 'INFO: Calling post cmd %s (attempt no. %s)' % (str(cmd_name_post),str(retry_counter))
		t0 = datetime.datetime.now()
		try:
			dp.command_inout(cmd_name_post,cmd_argin)	
		except PyTango.DevFailed as df:
			print(str(df))
			print("Failed to call command on device!")
			continue	
		t1 = datetime.datetime.now()
		dt= t1-t0
		dt_ms= float(dt.total_seconds() * 1000)
		data.append(dt_ms)
		print 'INFO: Called cmd %s (attempt no. %s): dt(ms)=%s' % (str(cmd_name_post),str(retry_counter),str(dt_ms))

		## Sleeping a bit	
		print 'INFO: Sleeping for %s seconds ...' % str(sleep_time)
		time.sleep(sleep_time)


	print data

	## Plot histogram
	if data:
		nbins= 20
		bins = np.linspace(min(data),max(data),nbins) # fixed number of bins
		plt.xlim([0, max(data)])
		plt.hist(data, bins=bins, alpha=0.5)
		plt.title('Command latency')
		plt.xlabel('dt (ms)')
		plt.ylabel('counts')
		plt.show()
	

###################
##   MAIN EXEC   ##
###################
if __name__ == "__main__":
	#main()
	sys.exit(main())
