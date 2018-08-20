#!/usr/bin/env python
# -*- coding:utf-8 -*-


# ############################################################################
#  license :
# ============================================================================
#
#  File :        PDUEmulator.py
#
#  Project :     PDUEmulator
#
# This file is part of Tango device class.
# 
# Tango is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Tango is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Tango.  If not, see <http://www.gnu.org/licenses/>.
# 
#
#  $Author :      simone.riggi$
#
#  $Revision :    $
#
#  $Date :        $
#
#  $HeadUrl :     $
# ============================================================================
#            This file is generated by POGO
#     (Program Obviously used to Generate tango Object)
# ############################################################################

__all__ = ["PDUEmulator", "PDUEmulatorClass", "main"]

__docformat__ = 'restructuredtext'

import PyTango
import sys
# Add additional import
#----- PROTECTED REGION ID(PDUEmulator.additionnal_import) ENABLED START -----#

#----- PROTECTED REGION END -----#	//	PDUEmulator.additionnal_import

# Device States Description
# ON : 
# STANDBY : 
# OFF : 
# INIT : 


class PDUEmulator (PyTango.Device_4Impl):
    """PDU emulator"""
    
    # -------- Add you global variables here --------------------------
    #----- PROTECTED REGION ID(PDUEmulator.global_variables) ENABLED START -----#
    
    #----- PROTECTED REGION END -----#	//	PDUEmulator.global_variables

    def __init__(self, cl, name):
        PyTango.Device_4Impl.__init__(self,cl,name)
        self.debug_stream("In __init__()")
        PDUEmulator.init_device(self)
        #----- PROTECTED REGION ID(PDUEmulator.__init__) ENABLED START -----#
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.__init__
        
    def delete_device(self):
        self.debug_stream("In delete_device()")
        #----- PROTECTED REGION ID(PDUEmulator.delete_device) ENABLED START -----#
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.delete_device

    def init_device(self):
        self.debug_stream("In init_device()")
        self.get_device_properties(self.get_device_class())
        self.attr_current_read = 0.0
        self.attr_DDBH_read = False
        self.attr_NSDN_read = False
        self.attr_SAT_read = False
        self.attr_SPFC_read = False
        self.attr_SPFRx_read = False
        self.attr_voltage_read = 0.0
        self.set_change_event("current", True, False)
        self.set_change_event("DDBH", True, False)
        self.set_change_event("NSDN", True, False)
        self.set_change_event("SAT", True, False)
        self.set_change_event("SPFC", True, False)
        self.set_change_event("SPFRx", True, False)
        self.set_change_event("voltage", True, False)
        #----- PROTECTED REGION ID(PDUEmulator.init_device) ENABLED START -----#

        self.info_stream("Starting PDU emulator...")		

        # Init attributes
        self.attr_SPFC_read = True
        self.attr_SPFRx_read = True

				# Set polling period for State/Status (don't know why it is not set at the beginning)
        prop= self.get_device_attr().get_attr_by_name("State").get_properties()
        print prop
        #self.get_device_attr().get_attr_by_name("State").set_polling_period(3000)
        #self.get_device_attr().get_attr_by_name("Status").set_polling_period(3000)
        	
        self.set_state(PyTango.DevState.ON)
        self.info_stream("PDU emulator started...")	

        #----- PROTECTED REGION END -----#	//	PDUEmulator.init_device

    def always_executed_hook(self):
        self.debug_stream("In always_excuted_hook()")
        #----- PROTECTED REGION ID(PDUEmulator.always_executed_hook) ENABLED START -----#
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.always_executed_hook

    # -------------------------------------------------------------------------
    #    PDUEmulator read/write attribute methods
    # -------------------------------------------------------------------------
    
    def read_current(self, attr):
        self.debug_stream("In read_current()")
        #----- PROTECTED REGION ID(PDUEmulator.current_read) ENABLED START -----#
        attr.set_value(self.attr_current_read)
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.current_read
        
    def write_current(self, attr):
        self.debug_stream("In write_current()")
        data = attr.get_write_value()
        #----- PROTECTED REGION ID(PDUEmulator.current_write) ENABLED START -----#

        self.attr_current_read= data

        #----- PROTECTED REGION END -----#	//	PDUEmulator.current_write
        
    def read_DDBH(self, attr):
        self.debug_stream("In read_DDBH()")
        #----- PROTECTED REGION ID(PDUEmulator.DDBH_read) ENABLED START -----#
        attr.set_value(self.attr_DDBH_read)
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.DDBH_read
        
    def write_DDBH(self, attr):
        self.debug_stream("In write_DDBH()")
        data = attr.get_write_value()
        #----- PROTECTED REGION ID(PDUEmulator.DDBH_write) ENABLED START -----#
        
        self.attr_DDBH_read= data

        #----- PROTECTED REGION END -----#	//	PDUEmulator.DDBH_write
        
    def read_NSDN(self, attr):
        self.debug_stream("In read_NSDN()")
        #----- PROTECTED REGION ID(PDUEmulator.NSDN_read) ENABLED START -----#
        attr.set_value(self.attr_NSDN_read)
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.NSDN_read
        
    def write_NSDN(self, attr):
        self.debug_stream("In write_NSDN()")
        data = attr.get_write_value()
        #----- PROTECTED REGION ID(PDUEmulator.NSDN_write) ENABLED START -----#
        
        self.attr_NSDN_read= data

        #----- PROTECTED REGION END -----#	//	PDUEmulator.NSDN_write
        
    def read_SAT(self, attr):
        self.debug_stream("In read_SAT()")
        #----- PROTECTED REGION ID(PDUEmulator.SAT_read) ENABLED START -----#
        attr.set_value(self.attr_SAT_read)
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SAT_read
        
    def write_SAT(self, attr):
        self.debug_stream("In write_SAT()")
        data = attr.get_write_value()
        #----- PROTECTED REGION ID(PDUEmulator.SAT_write) ENABLED START -----#
        
        self.attr_SAT_read= data
 
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SAT_write
        
    def read_SPFC(self, attr):
        self.debug_stream("In read_SPFC()")
        #----- PROTECTED REGION ID(PDUEmulator.SPFC_read) ENABLED START -----#
        attr.set_value(self.attr_SPFC_read)
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFC_read
        
    def write_SPFC(self, attr):
        self.debug_stream("In write_SPFC()")
        data = attr.get_write_value()
        #----- PROTECTED REGION ID(PDUEmulator.SPFC_write) ENABLED START -----#
        
        self.attr_SPFC_read= data

        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFC_write
        
    def read_SPFRx(self, attr):
        self.debug_stream("In read_SPFRx()")
        #----- PROTECTED REGION ID(PDUEmulator.SPFRx_read) ENABLED START -----#
        attr.set_value(self.attr_SPFRx_read)
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFRx_read
        
    def write_SPFRx(self, attr):
        self.debug_stream("In write_SPFRx()")
        data = attr.get_write_value()
        #----- PROTECTED REGION ID(PDUEmulator.SPFRx_write) ENABLED START -----#
        
        self.attr_SPFRx_read= data

        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFRx_write
        
    def read_voltage(self, attr):
        self.debug_stream("In read_voltage()")
        #----- PROTECTED REGION ID(PDUEmulator.voltage_read) ENABLED START -----#
        self.attr_voltage_read = 220.0 + numpy.random.normal(0,1)
        round(self.attr_voltage_read,1)			
        attr.set_value(self.attr_voltage_read)
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.voltage_read
        
    def write_voltage(self, attr):
        self.debug_stream("In write_voltage()")
        data = attr.get_write_value()
        #----- PROTECTED REGION ID(PDUEmulator.voltage_write) ENABLED START -----#
        
        self.attr_voltage_read= data

        #----- PROTECTED REGION END -----#	//	PDUEmulator.voltage_write
        
    
    
            
    def read_attr_hardware(self, data):
        self.debug_stream("In read_attr_hardware()")
        #----- PROTECTED REGION ID(PDUEmulator.read_attr_hardware) ENABLED START -----#
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.read_attr_hardware


    # -------------------------------------------------------------------------
    #    PDUEmulator command methods
    # -------------------------------------------------------------------------
    
    def Close_SPFC(self):
        """ 
        """
        self.debug_stream("In Close_SPFC()")
        #----- PROTECTED REGION ID(PDUEmulator.Close_SPFC) ENABLED START -----#
        self.attr_SPFC_read = True
        #----- PROTECTED REGION END -----#	//	PDUEmulator.Close_SPFC
        
    def Open_SPFC(self):
        """ 
        """
        self.debug_stream("In Open_SPFC()")
        #----- PROTECTED REGION ID(PDUEmulator.Open_SPFC) ENABLED START -----#
        self.attr_SPFC_read = False
        #----- PROTECTED REGION END -----#	//	PDUEmulator.Open_SPFC
        
    def DDBH_(self):
        """ 
        """
        self.debug_stream("In DDBH_()")
        #----- PROTECTED REGION ID(PDUEmulator.DDBH_) ENABLED START -----#
        self.attr_DDBH_read = not self.attr_DDBH_read
        #----- PROTECTED REGION END -----#	//	PDUEmulator.DDBH_
        
    def DDBH_OFF(self):
        """ 
        """
        self.debug_stream("In DDBH_OFF()")
        #----- PROTECTED REGION ID(PDUEmulator.DDBH_OFF) ENABLED START -----#
        self.attr_DDBH_read= False
        #----- PROTECTED REGION END -----#	//	PDUEmulator.DDBH_OFF
        
    def DDBH_ON(self):
        """ 
        """
        self.debug_stream("In DDBH_ON()")
        #----- PROTECTED REGION ID(PDUEmulator.DDBH_ON) ENABLED START -----#
        self.attr_DDBH_read= True
        #----- PROTECTED REGION END -----#	//	PDUEmulator.DDBH_ON
        
    def NSDN_(self):
        """ 
        """
        self.debug_stream("In NSDN_()")
        #----- PROTECTED REGION ID(PDUEmulator.NSDN_) ENABLED START -----#
        self.attr_NSDN_read = not self.attr_NSDN_read
        #----- PROTECTED REGION END -----#	//	PDUEmulator.NSDN_
        
    def Ramp(self, argin):
        """ 
        :param argin: 
        :type argin: PyTango.DevDouble
        :rtype: PyTango.DevBoolean
        """
        self.debug_stream("In Ramp()")
        argout = False
        #----- PROTECTED REGION ID(PDUEmulator.Ramp) ENABLED START -----#
        # should do the ramping
        argout= True

        #----- PROTECTED REGION END -----#	//	PDUEmulator.Ramp
        return argout
        
    def SAT_(self):
        """ 
        """
        self.debug_stream("In SAT_()")
        #----- PROTECTED REGION ID(PDUEmulator.SAT_) ENABLED START -----#
        self.attr_SAT_read= not self.attr_SAT_read
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SAT_
        
    def SAT_OFF(self):
        """ 
        """
        self.debug_stream("In SAT_OFF()")
        #----- PROTECTED REGION ID(PDUEmulator.SAT_OFF) ENABLED START -----#
        self.attr_SAT_read= False
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SAT_OFF
        
    def SAT_ON(self):
        """ 
        """
        self.debug_stream("In SAT_ON()")
        #----- PROTECTED REGION ID(PDUEmulator.SAT_ON) ENABLED START -----#
        self.attr_SAT_read= True
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SAT_ON
        
    def SPFC_(self):
        """ 
        """
        self.debug_stream("In SPFC_()")
        #----- PROTECTED REGION ID(PDUEmulator.SPFC_) ENABLED START -----#
        self.attr_SPFC_read = not self.attr_SPFC_read
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFC_
        
    def SPFC_OFF(self):
        """ 
        """
        self.debug_stream("In SPFC_OFF()")
        #----- PROTECTED REGION ID(PDUEmulator.SPFC_OFF) ENABLED START -----#
        self.info_stream("Powering SPFC off ...") 
        self.attr_SPFC_read = False
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFC_OFF
        
    def SPFC_ON(self):
        """ 
        """
        self.debug_stream("In SPFC_ON()")
        #----- PROTECTED REGION ID(PDUEmulator.SPFC_ON) ENABLED START -----#
        self.info_stream("Powering SPFC on ...") 
        self.attr_SPFC_read = True
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFC_ON
        
    def SPFRx_(self):
        """ 
        """
        self.debug_stream("In SPFRx_()")
        #----- PROTECTED REGION ID(PDUEmulator.SPFRx_) ENABLED START -----#
        self.attr_SPFRx_read = not self.attr_SPFRx_read
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFRx_
        
    def SPFRx_OFF(self):
        """ 
        """
        self.debug_stream("In SPFRx_OFF()")
        #----- PROTECTED REGION ID(PDUEmulator.SPFRx_OFF) ENABLED START -----#
        self.info_stream("Powering RXPU off ...") 
        self.attr_SPFRx_read = False
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFRx_OFF
        
    def SPFRx_ON(self):
        """ 
        """
        self.debug_stream("In SPFRx_ON()")
        #----- PROTECTED REGION ID(PDUEmulator.SPFRx_ON) ENABLED START -----#
        self.info_stream("Powering RXPU on ...") 
        self.attr_SPFRx_read = True
        #----- PROTECTED REGION END -----#	//	PDUEmulator.SPFRx_ON
        
    def TurnOn(self):
        """ 
        """
        self.debug_stream("In TurnOn()")
        #----- PROTECTED REGION ID(PDUEmulator.TurnOn) ENABLED START -----#
        # turn on the actual power supply here
        self.set_state(PyTango.DevState.ON)
        #----- PROTECTED REGION END -----#	//	PDUEmulator.TurnOn
        
    def TurnOff(self):
        """ 
        """
        self.debug_stream("In TurnOff()")
        #----- PROTECTED REGION ID(PDUEmulator.TurnOff) ENABLED START -----#
        # turn off the actual power supply here
        self.set_state(PyTango.DevState.OFF)
        #----- PROTECTED REGION END -----#	//	PDUEmulator.TurnOff
        

    #----- PROTECTED REGION ID(PDUEmulator.programmer_methods) ENABLED START -----#
    
    #----- PROTECTED REGION END -----#	//	PDUEmulator.programmer_methods

class PDUEmulatorClass(PyTango.DeviceClass):
    # -------- Add you global class variables here --------------------------
    #----- PROTECTED REGION ID(PDUEmulator.global_class_variables) ENABLED START -----#
    
    #----- PROTECTED REGION END -----#	//	PDUEmulator.global_class_variables


    #    Class Properties
    class_property_list = {
        }


    #    Device Properties
    device_property_list = {
        'host':
            [PyTango.DevString, 
             '',
            [] ],
        'port':
            [PyTango.DevLong, 
             '',
            [9788]],
        }


    #    Command definitions
    cmd_list = {
        'Close_SPFC':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'Open_SPFC':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'DDBH_':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'DDBH_OFF':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'DDBH_ON':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'NSDN_':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'Ramp':
            [[PyTango.DevDouble, "none"],
            [PyTango.DevBoolean, "none"]],
        'SAT_':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'SAT_OFF':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'SAT_ON':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'SPFC_':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'SPFC_OFF':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'SPFC_ON':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'SPFRx_':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'SPFRx_OFF':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'SPFRx_ON':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'TurnOn':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        'TurnOff':
            [[PyTango.DevVoid, "none"],
            [PyTango.DevVoid, "none"]],
        }


    #    Attribute definitions
    attr_list = {
        'current':
            [[PyTango.DevDouble,
            PyTango.SCALAR,
            PyTango.READ_WRITE],
            {
                'label': "Current",
                'unit': "A",
                'max value': "8.5",
                'min value': "0",
                'max alarm': "8.4",
                'min alarm': "0.1",
                'Polling period': "3000",
                'period': "60000",
                'rel_change': "1",
            } ],
        'DDBH':
            [[PyTango.DevBoolean,
            PyTango.SCALAR,
            PyTango.READ_WRITE],
            {
                'label': "SaDT DDBH",
                'Polling period': "3000",
                'period': "60000",
            } ],
        'NSDN':
            [[PyTango.DevBoolean,
            PyTango.SCALAR,
            PyTango.READ_WRITE],
            {
                'label': "SaDT NSDN",
                'Polling period': "3000",
                'period': "60000",
            } ],
        'SAT':
            [[PyTango.DevBoolean,
            PyTango.SCALAR,
            PyTango.READ_WRITE],
            {
                'label': "SaDT SAT",
                'Polling period': "3000",
                'period': "60000",
            } ],
        'SPFC':
            [[PyTango.DevBoolean,
            PyTango.SCALAR,
            PyTango.READ_WRITE],
            {
                'label': "SPFC",
                'Polling period': "3000",
                'period': "60000",
            } ],
        'SPFRx':
            [[PyTango.DevBoolean,
            PyTango.SCALAR,
            PyTango.READ_WRITE],
            {
                'label': "SPFRx",
                'Polling period': "3000",
                'period': "60000",
            } ],
        'voltage':
            [[PyTango.DevDouble,
            PyTango.SCALAR,
            PyTango.READ_WRITE],
            {
                'label': "Voltage",
                'unit': "V",
                'Polling period': "3000",
                'period': "60000",
                'rel_change': "1",
            } ],
        }


def main():
    try:
        py = PyTango.Util(sys.argv)
        py.add_class(PDUEmulatorClass, PDUEmulator, 'PDUEmulator')
        #----- PROTECTED REGION ID(PDUEmulator.add_classes) ENABLED START -----#
        
        #----- PROTECTED REGION END -----#	//	PDUEmulator.add_classes

        U = PyTango.Util.instance()
        U.server_init()
        U.server_run()

    except PyTango.DevFailed as e:
        print ('-------> Received a DevFailed exception:', e)
    except Exception as e:
        print ('-------> An unforeseen exception occured....', e)

if __name__ == '__main__':
    main()
