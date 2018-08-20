#!/usr/bin/python

import sys
from tango import DeviceProxy
import PyTango

##############
##   MAIN   ##
##############
def main():
	"""Main function"""

	## Get arguments
	nargs= len(sys.argv)
	if nargs<2 :
		sys.exit('Missing arguments (please specify starter device name as argument)!')

	device_name= str(sys.argv[1])

	# Get Starter proxy
	print("Retrieving Starter proxy device " + device_name + "...")
	try:
		device = DeviceProxy(device_name)
	except PyTango.DevFailed as df:
		sys.exit('Tango exception while retrieving device proxy (err=' + str(df) + ')')
		#print(str(df))

	# Stop servers with ordered levels (from 5 to 1)
	stop_levels= [5,4,3,2,1]
	for stop_level in stop_levels:
		print("Stopping device servers @ level " + str(stop_level) + "...")
		try :
			device.command_inout("DevStopAll", stop_level)
		except PyTango.DevFailed as df:
			sys.exit('Tango exception while stopping servers @ level ' + str(stop_level) + ' (err=' + str(df) + ')')

	print("All servers stopped with success")

###################
##   MAIN EXEC   ##
###################
if __name__ == "__main__":
	##main()
	sys.exit(main())
