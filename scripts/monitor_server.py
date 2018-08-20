#!/usr/bin/python

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

## Graphics modules
import matplotlib.pyplot as plt
import datetime
import numpy as np

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
	parser.add_argument('-s', '--sleep', dest='sleep_time', nargs='?', const=5, type=int, required=False, action='store',help='SleepTime')
	
	args = parser.parse_args()	

	return args

##############
##   MAIN   ##
##############
def main():
	"""Main function"""

	## Get script args
	print('Get script args')
	try:
		args= get_args()
	except Exception as ex:
		print("Failed to get and parse options (err=%s)",str(ex))
		return 1

	device_name= args.device_name
	sleep_time= args.sleep_time

	print("*** ARGS ***")
	print("device_name: %s" % device_name)
	print("sleep_time: %d" % sleep_time) 
	print("************")
		

	## Create device proxy
	print('Connecting to device proxy %s ...') % str(device_name)
	try:
		dp= PyTango.DeviceProxy(device_name)	
	
	except PyTango.DevFailed as df:
		print(str(df))
		print("Failed to connect to device proxy %s, sleeping for %s seconds and retry...", str(device_name), str(sleep_time))
		sys.exit(1)

	plt.axis([0, 1000, 0, 50])
	plt.ion()
	
	## Pinging
	counter= 0
	while True:

		try:
			t= datetime
			ping_time= dp.ping()
			plt.scatter(counter, ping_time/1000.)
			print("ping time(ms): %s") % str(ping_time/1000.)
		except PyTango.DevFailed as df:
			print("Failed to ping device proxy %s...", str(device_name))
			
		counter= counter+1
		#time.sleep(sleep_time)
		plt.pause(sleep_time)
			
###################
##   MAIN EXEC   ##
###################
if __name__ == "__main__":
	#main()
	sys.exit(main())
