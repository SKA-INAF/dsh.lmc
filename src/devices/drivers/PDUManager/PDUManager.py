#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""PDU tango device server"""
import os, sys
import commands

import time
import numpy
import threading

from PyTango import AttrQuality, AttrWriteType, DispLevel, DevState, DebugIt
from PyTango.server import Device, DeviceMeta, attribute, command, run
from PyTango.server import device_property

adresse ='140.105.77.31'
snmpset1 ='snmpset -v 2c -c tripplite '
snmpget1 ='snmpget -v 2c -c tripplite '
snmpset2 =' 1.3.6.1.4.1.850.100.1.10.2.1.4.'
snmpget2 =' 1.3.6.1.4.1.850.100.1.10.2.1.2.'
on =' i 2'
off =' i 1'

sset1on = snmpset1 + adresse + snmpset2 + '1' + on
sset2on = snmpset1 + adresse + snmpset2 + '2' + on
sset3on = snmpset1 + adresse + snmpset2 + '3' + on
sset4on = snmpset1 + adresse + snmpset2 + '4' + on
sset5on = snmpset1 + adresse + snmpset2 + '5' + on
sset6on = snmpset1 + adresse + snmpset2 + '6' + on
sset7on = snmpset1 + adresse + snmpset2 + '7' + on
sset8on = snmpset1 + adresse + snmpset2 + '8' + on

sset1off = snmpset1 + adresse + snmpset2 + '1' + off
sset2off = snmpset1 + adresse + snmpset2 + '2' + off
sset3off = snmpset1 + adresse + snmpset2 + '3' + off
sset4off = snmpset1 + adresse + snmpset2 + '4' + off
sset5off = snmpset1 + adresse + snmpset2 + '5' + off
sset6off = snmpset1 + adresse + snmpset2 + '6' + off
sset7off = snmpset1 + adresse + snmpset2 + '7' + off
sset8off = snmpset1 + adresse + snmpset2 + '8' + off

sget1 = snmpget1 + adresse + snmpget2 + '1'
sget2 = snmpget1 + adresse + snmpget2 + '2'
sget3 = snmpget1 + adresse + snmpget2 + '3'
sget4 = snmpget1 + adresse + snmpget2 + '4'
sget5 = snmpget1 + adresse + snmpget2 + '5'
sget6 = snmpget1 + adresse + snmpget2 + '6'
sget7 = snmpget1 + adresse + snmpget2 + '7'
sget8 = snmpget1 + adresse + snmpget2 + '8'

a1=0
b1=0

class PDUManager(Device):
    __metaclass__ = DeviceMeta

    LMC = attribute(label="LMC", dtype=bool,
                        display_level=DispLevel.OPERATOR,
                        access=AttrWriteType.READ_WRITE,
			#access=AttrWriteType.READ,
                        #unit="V",format="8.4f",
                        fget="get_lmc",
                        fset="set_lmc",
                        doc="LMC power supply")

    SPFC = attribute(label="SPFC", dtype=bool,
                        display_level=DispLevel.OPERATOR,
                        access=AttrWriteType.READ_WRITE,
			#access=AttrWriteType.READ,
                        #unit="V",format="8.4f",
                        fget="get_spfc",
                        fset="set_spfc",
                        doc="SPFC power supply")

    SPFRx = attribute(label="SPFRx", dtype=bool,
                        display_level=DispLevel.OPERATOR,
                        access=AttrWriteType.READ_WRITE,
			#access=AttrWriteType.READ,
                        #unit="V",format="8.4f",
                        fget="get_spfrx",
                        fset="set_spfrx",
                        doc="SPFRx power supply")

    DDBH = attribute(label="SaDT DDBH", dtype=bool,
                        display_level=DispLevel.OPERATOR,
                        access=AttrWriteType.READ_WRITE,
			#access=AttrWriteType.READ,
                        #unit="V",format="8.4f",
                        fget="get_ddbh",
                        fset="set_ddbh",
                        doc="SaDT DDBH power supply")

    STFRRX = attribute(label="SaDT STFR.RX", dtype=bool,
                        display_level=DispLevel.OPERATOR,
                        access=AttrWriteType.READ_WRITE,
			#access=AttrWriteType.READ,
                        #unit="V",format="8.4f",
                        fget="get_stfrrx",
                        fset="set_stfrrx",
                        doc="SaDT STFR.RX power supply")

    STFRWR = attribute(label="SaDT STFR.WR", dtype=bool,
                        display_level=DispLevel.OPERATOR,
                        access=AttrWriteType.READ_WRITE,
			#access=AttrWriteType.READ,
                        #unit="V",format="8.4f",
                        fget="get_stfrwr",
                        fset="set_stfrwr",
                        doc="SaDT STFR.WR power supply")

    NSDN = attribute(label="SaDT NSDN", dtype=bool,
                        display_level=DispLevel.OPERATOR,
                        access=AttrWriteType.READ_WRITE,
			#access=AttrWriteType.READ,
                        #unit="V",format="8.4f",
                        fget="get_nsdn",
                        fset="set_nsdn",
                        doc="SaDT NSDN power supply")

    SPARE = attribute(label="SPARE", dtype=bool,
                        display_level=DispLevel.OPERATOR,
                        access=AttrWriteType.READ_WRITE,
			#access=AttrWriteType.READ,
                        #unit="V",format="8.4f",
                        fget="get_spare",
                        fset="set_spare",
                        doc="SPARE power supply")

    host = device_property(dtype=str)
    port = device_property(dtype=int, default_value=9788)
    
    def outlets(self):
       b= commands.getstatusoutput(sget1)
       b1= ''.join(map(str, b))[-1]
       #print b1
       if b1=='1':
	  self.__stfrrx = False
       if b1=='2':
	  self.__stfrrx = True
       b= commands.getstatusoutput(sget2)
       b1= ''.join(map(str, b))[-1]
       #print b1
       if b1=='1':
	  self.__nsdn = False
       if b1=='2':
	  self.__nsdn = True
       b= commands.getstatusoutput(sget3)
       b1= ''.join(map(str, b))[-1]
       #print b1
       if b1=='1':
	  self.__lmc = False
       if b1=='2':
	  self.__lmc = True
       b= commands.getstatusoutput(sget4)
       b1= ''.join(map(str, b))[-1]
       #print b1
       if b1=='1':
	  self.__spfc = False
       if b1=='2':
	  self.__spfc = True
       b= commands.getstatusoutput(sget5)
       b1= ''.join(map(str, b))[-1]
       #print b1
       if b1=='1':
	  self.__spfrx = False
       if b1=='2':
	  self.__spfrx = True
       b= commands.getstatusoutput(sget6)
       b1= ''.join(map(str, b))[-1]
       #print b1
       if b1=='1':
	  self.__ddbh = False
       if b1=='2':
	  self.__ddbh = True
       b= commands.getstatusoutput(sget7)
       b1= ''.join(map(str, b))[-1]
       #print b1
       if b1=='1':
	  self.__stfrwr = False
       if b1=='2':
	  self.__stfrwr = True
       b= commands.getstatusoutput(sget8)
       b1= ''.join(map(str, b))[-1]
       #print b1
       if b1=='1':
	  self.__spare = False
       if b1=='2':
	  self.__spare = True

    def init_device(self):
        Device.init_device(self)
        self.__lmc = False
        self.__spfc = False
	self.__spfrx = False
	self.__ddbh = False
	self.__stfrrx = False
	self.__stfrwr = False
	self.__nsdn = False
        self.__spare = False
	self.outlets()
        #self.set_state(DevState.STANDBY)
        self.set_state(DevState.ON)
        #self.t = threading.Thread(target=self.run1)
        #t = threading.Thread(target=self.run1)
        #self.t.setDaemon(True)
        #t.setDaemon(True)
        #self.t.start()
        #t.start()
        #self.run1()
    
    
    def get_lmc(self):
        return self.__lmc

    def set_lmc(self, value):
        # should set power supply ON/OFF
        self.__lmc = value
    
    def get_spfc(self):
        return self.__spfc

    def set_spfc(self, value):
        # should set power supply ON/OFF
        self.__spfc = value
    
    def get_spfrx(self):
        return self.__spfrx

    def set_spfrx(self, value):
        # should set power supply ON/OFF
        self.__spfrx = value

    def get_ddbh(self):
        return self.__ddbh

    def set_ddbh(self, value):
        # should set power supply ON/OFF
        self.__ddbh = value

    def get_stfrrx(self):
        return self.__stfrrx

    def set_stfrrx(self, value):
        # should set power supply ON/OFF
        self.__stfrrx = value

    def get_stfrwr(self):
        return self.__stfrwr

    def set_stfrwr(self, value):
        # should set power supply ON/OFF
        self.__stfrwr = value

    def get_nsdn(self):
        return self.__nsdn

    def set_nsdn(self, value):
        # should set power supply ON/OFF
        self.__nsdn = value

    def get_spare(self):
        return self.__spare

    def set_spare(self, value):
        # should set power supply ON/OFF
        self.__spare = value

    def stfrrxon(self):
      a1=0;b1=1
      while a1!=b1:
       a= commands.getstatusoutput(sset1on)
       a1= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget1)
       b1= ''.join(map(str, b))[-1]
       if a1==b1:
        self.__stfrrx = True
        #sys.exit()

    def stfrrxoff(self):
      a1=0;b1=1
      while a1!=b1:
       a= commands.getstatusoutput(sset1off)
       a1= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget1)
       b1= ''.join(map(str, b))[-1]
       if a1==b1:
        self.__stfrrx = False
        #sys.exit()

    @command
    def STFRRX_ON(self):
       self.t1 = threading.Thread(target=self.stfrrxon)
       self.t1.start()

    @command
    def STFRRX_OFF(self):
       self.t1 = threading.Thread(target=self.stfrrxoff)
       self.t1.start()

    def nsdnon(self):
      a2=0;b2=1
      while a2!=b2:
       a= commands.getstatusoutput(sset2on)
       a2= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget2)
       b2= ''.join(map(str, b))[-1]
       if a2==b2:
        self.__nsdn = True
        #sys.exit()

    def nsdnoff(self):
      a2=0;b2=1
      while a2!=b2:
       a= commands.getstatusoutput(sset2off)
       a2= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget2)
       b2= ''.join(map(str, b))[-1]
       if a2==b2:
        self.__nsdn = False
        #sys.exit()

    @command
    def NSDN_ON(self):
       self.t2 = threading.Thread(target=self.nsdnon)
       self.t2.start()

    @command
    def NSDN_OFF(self):
       self.t2 = threading.Thread(target=self.nsdnoff)
       self.t2.start()

    def lmcon(self):
      a3=0;b3=1
      while a3!=b3:
       a= commands.getstatusoutput(sset3on)
       a3= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget3)
       b3= ''.join(map(str, b))[-1]
       if a3==b3:
        self.__lmc = True
        #sys.exit()

    def lmcoff(self):
      a3=0;b3=1
      while a3!=b3:
       a= commands.getstatusoutput(sset3off)
       a3= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget3)
       b3= ''.join(map(str, b))[-1]
       if a3==b3:
        self.__lmc = False
        #sys.exit()

    @command
    def LMC_ON(self):
       self.t3 = threading.Thread(target=self.lmcon)
       self.t3.start()

    @command
    def LMC_OFF(self):
       self.t3 = threading.Thread(target=self.lmcoff)
       self.t3.start()

    def spfcon(self):
      a4=0;b4=1
      while a4!=b4:
       a= commands.getstatusoutput(sset4on)
       a4= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget4)
       b4= ''.join(map(str, b))[-1]
       if a4==b4:
        self.__spfc = True
        #sys.exit()

    def spfcoff(self):
      a4=0;b4=1
      while a4!=b4:
       a= commands.getstatusoutput(sset4off)
       a4= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget4)
       b4= ''.join(map(str, b))[-1]
       if a4==b4:
        self.__spfc = False
        #sys.exit()

    @command
    def SPFC_ON(self):
       self.t4 = threading.Thread(target=self.spfcon)
       self.t4.start()

    @command
    def SPFC_OFF(self):
       self.t4 = threading.Thread(target=self.spfcoff)
       self.t4.start()

    def spfrxon(self):
      a5=0;b5=1
      while a5!=b5:
       a= commands.getstatusoutput(sset5on)
       a5= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget5)
       b5= ''.join(map(str, b))[-1]
       if a5==b5:
        self.__spfrx = True
        #sys.exit()

    def spfrxoff(self):
      a5=0;b5=1
      while a5!=b5:
       a= commands.getstatusoutput(sset5off)
       a5= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget5)
       b5= ''.join(map(str, b))[-1]
       if a5==b5:
        self.__spfrx = False
        #sys.exit()

    @command
    def SPFRx_ON(self):
       self.t5 = threading.Thread(target=self.spfrxon)
       self.t5.start()

    @command
    def SPFRx_OFF(self):
       self.t5 = threading.Thread(target=self.spfrxoff)
       self.t5.start()

    def ddbhon(self):
      a6=0;b6=1
      while a6!=b6:
       a= commands.getstatusoutput(sset6on)
       a6= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget6)
       b6= ''.join(map(str, b))[-1]
       if a6==b6:
        self.__ddbh = True
        #sys.exit()

    def ddbhoff(self):
      a6=0;b6=1
      while a6!=b6:
       a= commands.getstatusoutput(sset6off)
       a6= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget6)
       b6= ''.join(map(str, b))[-1]
       if a6==b6:
        self.__ddbh = False
        #sys.exit()

    @command
    def DDBH_ON(self):
       self.t6 = threading.Thread(target=self.ddbhon)
       self.t6.start()

    @command
    def DDBH_OFF(self):
       self.t6 = threading.Thread(target=self.ddbhoff)
       self.t6.start()

    def stfrwron(self):
      a7=0;b7=1
      while a7!=b7:
       a= commands.getstatusoutput(sset7on)
       a7= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget7)
       b7= ''.join(map(str, b))[-1]
       if a7==b7:
        self.__stfrwr = True
        #sys.exit()

    def stfrwroff(self):
      a7=0;b7=1
      while a7!=b7:
       a= commands.getstatusoutput(sset7off)
       a7= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget7)
       b7= ''.join(map(str, b))[-1]
       if a7==b7:
        self.__stfrwr = False
        #sys.exit()

    @command
    def STFRWR_ON(self):
      self.t7 = threading.Thread(target=self.stfrwron)
      self.t7.start()


    @command
    def STFRWR_OFF(self):
      self.t7 = threading.Thread(target=self.stfrwroff)
      self.t7.start()

    def spareon(self):
      a8=0;b8=1
      while a8!=b8:
       a= commands.getstatusoutput(sset8on)
       a8= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget8)
       b8= ''.join(map(str, b))[-1]
       if a8==b8:
        self.__spare = True
        #sys.exit()

    def spareoff(self):
      a8=0;b8=1
      while a8!=b8:
       a= commands.getstatusoutput(sset8off)
       a8= ''.join(map(str, a))[-1]
       b= commands.getstatusoutput(sget8)
       b8= ''.join(map(str, b))[-1]
       if a8==b8:
        self.__spare = False
        #sys.exit()

    @command
    def SPARE_ON(self):
      self.t8 = threading.Thread(target=self.spareon)
      self.t8.start()

    @command
    def SPARE_OFF(self):
      self.t8 = threading.Thread(target=self.spareoff)
      self.t8.start()

    @command
    def TurnOn(self):
        # turn on the actual power supply here
        self.set_state(DevState.ON)

    @command
    def TurnOff(self):
        # turn off the actual power supply here
        self.set_state(DevState.OFF)

    @command(dtype_in=float, doc_in="Ramp target current",
             dtype_out=bool, doc_out="True if ramping went well, False otherwise")
    def Ramp(self, target_current):
        # should do the ramping
        return True
   
    def run1 (self):
	while(1):
#		noise = numpy.random.normal(0,1)
#               self.__noise = self.__noise + 0.1
                #print "cucco"
               time.sleep(2)
    #            pass

               b= commands.getstatusoutput(sget1)
               b1= ''.join(map(str, b))[-1]
               if a1==b1:
                if a1=='2':
                  self.__stfrrx = True
                else:
                  self.__stfrrx = False 

	#	self.voltage = 220.0 + noise 
    
if __name__ == "__main__":
    run([PDUManager])
