# ----------------------------------------------------------------------------
# STEPS:
# ----------------------------------------------------------------------------
from behave import given, when, then
import nose
from tango import DeviceProxy
import PyTango

@given('Dish is in STANDBY-LP mode')
def step_impl(context):
	pass

@when('I request dish LMC to stow')
def step_impl(context):
	
	try:
		context.lmc_device.command_inout("SetStowMode")
		status= 0
	except PyTango.DevFailed as df:
		status= 1

	nose.tools.assert_equal(status,0)

@then('I should be acknowledged that stowing was initiated by LMC')
def step_impl(context):
	pass

@then('I should be notified within 30 s that dish is in STOW mode')
def step_impl(context):
	pass





