
#!/usr/bin/python

##################################################
###          MODULE IMPORT
##################################################
import subprocess
import sys

def is_service_running(service_name):
	
	"""Check if service is running"""

	## Command to check service status (can also use init.d)
	## If command is wrong or status is off the command does not return 0 (an exception in check_call)
	cmd= 'systemctl status ' + str(service_name) + ' -n 0'

	try:
		p = subprocess.check_call(cmd, shell=True,stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	except subprocess.CalledProcessError as e:
		ret =   e.returncode
		print 'Return code: ' + str(ret)
		return False
        
	return True


##############
##   MAIN   ##
##############
def main():
	"""Main function"""

	## Get arguments
	nargs= len(sys.argv)
	if nargs<2 :
		sys.exit('Missing arguments (please specify service_name to be checked as argument)!')

	service_name= str(sys.argv[1])
	
	## Check if service is running
	isRunning= is_service_running(service_name)
	if isRunning:
		print ('Service ' + service_name + ' is RUNNING')
	else:
		print ('Service ' + service_name + ' is OFF')

###################
##   MAIN EXEC   ##
###################
if __name__ == "__main__":
	#main()
	sys.exit(main())
