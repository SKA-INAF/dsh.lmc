# ----------------------------------------------------------------------------
# STEPS:
# ----------------------------------------------------------------------------

from behave import given, when, then
import nose
from tango import DeviceProxy
import PyTango


@given('TangoDB is up and running')
def step_impl(context):
	""" Check if TangoDB server is running """
	tangodb_service= 'tango-db'
	is_tangodb_up= is_service_running(tangodb_service)	
	#assert is_tangodb_up is True
	nose.tools.assert_equal(is_tangodb_up, True)

@given('LMC is up and running')
def step_impl(context):
	""" Check if LMCManager server (lmc-starter service) is running """
	lmc_service= 'lmc-starter'
	is_lmc_up= is_service_running(lmc_service)	
	#assert is_lmc_up is True
	nose.tools.assert_equal(is_lmc_up,True)

@given('I am an "{LMC}" client (TM or operator)')
def step_impl(context,LMC):
	""" Create LMC client """
	print("Creating a proxy device " + str(LMC) + " ...")

	try:
		context.lmc_device= DeviceProxy(LMC)
		status= 0
	except PyTango.DevFailed as df:
		status= 1
	
	#assert status==0
	nose.tools.assert_equal(status,0)

################################################
##    HELPER METHODS
################################################
import subprocess
import sys

## Check is service is running
def is_service_running(service_name):
	
	"""Check if a given service is running"""

	## Command to check service status (can also use init.d)
	## If command is wrong or status is off the command does not return 0 (an exception in check_call)
	cmd= 'systemctl status ' + str(service_name) + ' -n 0'

	try:
		process = subprocess.check_call(cmd, shell=True,stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	except subprocess.CalledProcessError as e:
		ret= e.returncode
		#print "Return code: " + str(ret)
		return False
        
	return True

## Start a service
def start_service(service_name):

	""" Start a OS service """
	
	# Start process
	#cmd= 'systemctl status ' + str(service_name) + ' -n 0'

	#process = subprocess.Popen(path + ' > /dev/null 2> /dev/null &', shell=True)

