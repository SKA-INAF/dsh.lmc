// **************************************************************************
// * License and Disclaimer                                                 *
// *                                                                        *
// * Copyright 2016 Simone Riggi																			      *
// *																																	      *
// * This file is part of SKA DSH.LMC 																		  *
// * SKA DSH.LMC is free software: you can redistribute it and/or modify it *
// * under the terms of the GNU General Public License as published by      *
// * the Free Software Foundation, either * version 3 of the License,       * 
// * or (at your option) any later version.                                 * 
// * DSH.LMC is distributed in the hope that it will be useful, but 			    *
// * WITHOUT ANY WARRANTY; without even the implied warranty of             * 
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
// * See the GNU General Public License for more details. You should        * 
// * have received a copy of the GNU General Public License along with      * 
// * DSH.LMC. If not, see http://www.gnu.org/licenses/.                      *
// **************************************************************************

/**
* @file TestEnvironment.cc
* @class TestEnvironment
* @brief Class to setup the test enviroment 
*
* Class to setup the test enviroment (performed at beginning and end of all tests)
* @author S. Riggi
* @date 15/01/2016
*/

#include <TestEnvironment.h>

#include "gtest/gtest.h"

#include <tango.h>

// Check if crash reporting is used.
#if defined(ENABLE_CRASH_REPORT)
#  include <crashreporting/crash_report.h>
#else
#  define DECLARE_CRASH_HANDLER
#  define INSTALL_CRASH_HANDLER
#endif

DECLARE_CRASH_HANDLER;

//Init static members
//TestDevice_ns::TestDeviceClass* TestEnvironment::device_class= 0;
//TestDevice_ns::TestDevice* TestEnvironment::device= 0;
Tango::DeviceImpl* TestEnvironment::device= 0;

TestEnvironment::TestEnvironment()
{


}

TestEnvironment::~TestEnvironment()
{


}


void TestEnvironment::SetUp(){

	//Setup a test Tango device
	/*
	if(SetUpTestDevice()<0){
		cerr<<"ERROR: Failed to set up the Tango test device!"<<endl;
		return;
	}
	*/

}//close SetUp()


void TestEnvironment::TearDown(){


}//close TearDown()


int TestEnvironment::SetUpTestDevice(){

	//Options
	std::string class_name= "TangoTest";
	std::string ds_instance= "test";
	std::string ds_verbosity= "-v1";
	std::string ds_name= class_name + std::string("/") + ds_instance;//exec/instance
	std::string dev_name= "sys/tg_test/1";
	
	//Skip if already created
	if(device){
		cout<<"INFO: test device "<<dev_name<<" already instantiated..."<<endl;
		return 0;
	}

	
	
	//Create a device class
	
	//if(!device_class) {
	//	cout<<"INFO: Creating test device class..."<<endl;
	//	device_class= TestDevice_ns::TestDeviceClass::init("TestDeviceClass");
	//}

	//Create a device impl
	//if(!device) {
	//	cout<<"INFO: Creating test device impl..."<<endl;
	//	device= new TestDevice_ns::TestDevice(device_class,dev_name);
	//}	
	
	/*
	//Register device in DB
	cout<<"INFO: Registering server "<<ds_name<<" in TangoDB..."<<endl;
	try {
		Tango::Database db;
	
		Tango::DbDevInfos ddi;
		Tango::DbDevInfo tmp_dbi;

		//Fill DevInfo for Device
		tmp_dbi.name = dev_name;
		tmp_dbi._class = class_name;
		tmp_dbi.server = ds_name;
		ddi.push_back(tmp_dbi);

		//Fill DevInfo for adm device
		tmp_dbi.name = "dserver/" + ds_name;
		tmp_dbi._class = "DServer";
		tmp_dbi.server = ds_name;
		ddi.push_back(tmp_dbi);

		//Register server
		db.add_server(ds_name,ddi);
	
	}//close try block
	catch (Tango::DevFailed &e) {
		Tango::Except::print_exception(e);
		cerr<<"ERROR: Tango exception occurred while creating and registering the test device "<<ds_name<<"!"<<endl;
		return -1;
	}
	*/

	//Set up server
	INSTALL_CRASH_HANDLER
	Tango::Util* tg = 0;
	try
	{
		cout<<"INFO: Creating Tango::Util instance ...";
		char* dummy_argv[] = {(char*)class_name.c_str(),(char*)ds_instance.c_str(),(char*)ds_verbosity.c_str()};
		tg = Tango::Util::init(3,dummy_argv);
		cout<<"done!"<<endl;

		cout<<"INFO: Initializing server "<<ds_name<<" ...";
		tg->server_init(false);
		cout<<"done!"<<endl;

		
	}
	catch (bad_alloc &)
	{
		cout << "Can't allocate memory to store device object !!!" << endl;
		cout << "Exiting" << endl;

		return -1;
	}
	catch (CORBA::Exception &e)
	{
		Tango::Except::print_exception(e);
		
		cout << "Received a CORBA_Exception" << endl;
		cout << "Exiting" << endl;
		return -1;
	}

	//## Retrieving the TangoTest device
	cout<<"INFO: Retrieving test server "<<dev_name<<" in TangoDB..."<<endl;
	try {
		device= tg->get_device_by_name(dev_name);
	}
	catch (Tango::DevFailed &e) {
		Tango::Except::print_exception(e);
		cerr<<"ERROR: Tango exception occurred while retrieving the Tango test device "<<dev_name<<"!"<<endl;
		return -1;
	}

	//Cleanup	
	Tango::Util::instance()->server_cleanup();
	
	return 0;

}//close SetUpTestDevice()





