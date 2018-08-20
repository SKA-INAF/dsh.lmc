#!/usr/bin/env python

# Import modules
try:
	import sys
	import os
	import time
except ImportError, e:
	print("Failed to import python sys/os/time modules!")
	sys.exit(1)

try:
	import PyTango
except ImportError, e:
	print("Failed to import python PyTango module!")
	sys.exit(1)


##############
##   MAIN   ##
##############
def main():
	"""Main function"""

	## Get arguments
	nargs= len(sys.argv)
	if nargs<2:
		sys.exit('Missing arguments (please specify starter device name as argument)!')

	device_name= str(sys.argv[1])
	

	# Get Starter proxy
	
	print("Connecting to Starter proxy device " + device_name + "...")
	creation_succeeded= False
	nretries= 3
	sleep_time= 3

	for retry_counter in range(0,nretries): 

		## Create device proxy
		print('Connecting to device proxy %s [attempt #%s]...' % (str(device_name),str(retry_counter)))		
		try:
			device = PyTango.DeviceProxy(device_name)
	
		except PyTango.DevFailed as df:
			print("Failed to connect to device proxy %s, sleeping for %s seconds and retry..." % (str(device_name), str(sleep_time)))
			time.sleep(sleep_time)
			continue
	
		creation_succeeded= True
		break

	if not creation_succeeded:
		sys.exit("Failed to connect to Starter device (all attempts failed)!")
		

	# Check if there are controlled servers
	cmd_ok= False
	for retry_counter in range(0,nretries): 
		try:
			da= device.read_attribute('servers')
			if da.is_empty:
				print("List of controlled servers is empty, nothing to be stopped")
				sys.exit(0)
		except PyTango.DevFailed as df:
			print("Failed to read list of controlled servers, sleeping for %s seconds and retry..." % str(sleep_time))
			time.sleep(sleep_time)
			continue

		cmd_ok= True
		break

	if not cmd_ok:
		sys.exit('Failed to read list of controlled servers from starter (hint: check if starter daemon is running/stuck)')


	# Stop servers with ordered levels (from 5 to 1)
	stop_levels= [5,4,3,2,1]
	status= 0
	for stop_level in stop_levels:
		print("Stopping device servers @ level " + str(stop_level) + "...")
		try :
			device.command_inout("DevStopAll", stop_level)
		except PyTango.DevFailed as df:
			print('Tango exception while stopping servers @ level ' + str(stop_level) + ' (err=' + str(df) + ')')
			status= -1
			continue
			
	if status<0:
		print('')
		sys.exit('One/more failures occurred when stopping controlled servers')
	else:
		print("All servers stopped with success")


###################
##   MAIN EXEC   ##
###################
if __name__ == "__main__":
	##main()
	sys.exit(main())
