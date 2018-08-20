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
* @file Test_TangoUtils.cc
* @class Test_TangoUtils
* @brief Class for testing TangoUtils class
*
* Test class for TangoUtils class
* @author S. Riggi
* @date 15/01/2016
*/


#include <Test_TangoUtils.h>
#include <TangoUtils.h>

#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

#include <tango.h>


namespace Utils_ns {


//===========================================
//==           DEFINE TESTS              ====
//===========================================

TEST_F(TangoUtilsTest, TestTangoStateToString)
{

	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::ON),"ON");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::OFF),"OFF");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::CLOSE),"CLOSE");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::OPEN),"OPEN");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::INSERT),"INSERT");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::EXTRACT),"EXTRACT");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::MOVING),"MOVING");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::STANDBY),"STANDBY");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::FAULT),"FAULT");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::INIT),"INIT");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::RUNNING),"RUNNING");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::ALARM),"ALARM");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::DISABLE),"DISABLE");
	ASSERT_EQ(TangoUtils::TangoStateToString(Tango::UNKNOWN),"UNKNOWN");
	
}//close TestTangoStateToString()

TEST_F(TangoUtilsTest, TestStringToTangoState)
{
	//Test invalid string
	Tango::DevState state;
	ASSERT_EQ(TangoUtils::StringToTangoState(state,"FOO"),-1);
	ASSERT_EQ(TangoUtils::StringToTangoState(state,"ON "),-1);
	ASSERT_EQ(TangoUtils::StringToTangoState(state," ON"),-1);

	//Test valid codes
	std::vector<Tango::DevState> exp_states {
		Tango::ON, Tango::OFF, Tango::CLOSE, Tango::OPEN, Tango::INSERT, Tango::EXTRACT, 
		Tango::MOVING, Tango::STANDBY, Tango::FAULT, Tango::INIT, Tango::RUNNING, 
		Tango::ALARM, Tango::DISABLE, Tango::UNKNOWN
	};	
	std::vector<Tango::DevState> states;
	std::vector<std::string> states_str {
		"ON", "OFF", "CLOSE", "OPEN", "INSERT", "EXTRACT", 
		"MOVING", "STANDBY", "FAULT", "INIT", "RUNNING", 
		"ALARM", "DISABLE", "UNKNOWN"
	};
	for(unsigned int i=0;i<states_str.size();i++){
		ASSERT_EQ(TangoUtils::StringToTangoState(state,states_str[i]),0) << "State string to Tango::DevState mapping failed (it should not!)";
		states.push_back(state);
	}
	
	EXPECT_THAT(states, ::testing::ContainerEq(exp_states)) << "Mapping state string to Tango::DevState failed (it should not!)";

}//close TestStringToTangoState()

TEST_F(TangoUtilsTest, TestParseFullAttrName)
{
	std::string device_name;
	std::string attr_name;
	std::string domain_name;
	bool hasQualityTag;

	//Test empty name
	ASSERT_EQ(TangoUtils::ParseFullAttrName(device_name,attr_name,""),-1);
	ASSERT_EQ(TangoUtils::ParseFullAttrName(domain_name,device_name,attr_name,hasQualityTag,""),-1);

	//Test non FQDN attr name 	
	ASSERT_EQ(TangoUtils::ParseFullAttrName(device_name,attr_name,"dev/family/id/myattr"),0);//test attr name (not FQDN)
	ASSERT_EQ(device_name,"dev/family/id");
	ASSERT_EQ(attr_name,"myattr");

	ASSERT_EQ(TangoUtils::ParseFullAttrName(domain_name,device_name,attr_name,hasQualityTag,"dev/family/id/myattr"),0);
	ASSERT_EQ(domain_name,"");
	ASSERT_EQ(device_name,"dev/family/id");
	ASSERT_EQ(attr_name,"myattr");
	ASSERT_FALSE(hasQualityTag);

	ASSERT_EQ(TangoUtils::ParseFullAttrName(domain_name,device_name,attr_name,hasQualityTag,"dev/family/id/myattr.quality"),0);
	ASSERT_EQ(domain_name,"");
	ASSERT_EQ(device_name,"dev/family/id");
	ASSERT_EQ(attr_name,"myattr");
	ASSERT_TRUE(hasQualityTag);

	//Test FQDN attr name
	ASSERT_EQ(TangoUtils::ParseFullAttrName(device_name,attr_name,"tango://localhost:10000/dev/family/id/myattr"),0);//test attr name (not FQDN)
	ASSERT_EQ(device_name,"tango://localhost:10000/dev/family/id");
	ASSERT_EQ(attr_name,"myattr");

	ASSERT_EQ(TangoUtils::ParseFullAttrName(domain_name,device_name,attr_name,hasQualityTag,"tango://localhost:10000/dev/family/id/myattr"),0);
	ASSERT_EQ(domain_name,"tango://localhost:10000");
	ASSERT_EQ(device_name,"dev/family/id");
	ASSERT_EQ(attr_name,"myattr");
	ASSERT_FALSE(hasQualityTag);

	ASSERT_EQ(TangoUtils::ParseFullAttrName(domain_name,device_name,attr_name,hasQualityTag,"tango://localhost:10000/dev/family/id/myattr.quality"),0);
	ASSERT_EQ(domain_name,"tango://localhost:10000");
	ASSERT_EQ(device_name,"dev/family/id");
	ASSERT_EQ(attr_name,"myattr");
	ASSERT_TRUE(hasQualityTag);

	//Test invalid FQDN name
	ASSERT_EQ(TangoUtils::ParseFullAttrName(domain_name,device_name,attr_name,hasQualityTag,"localhost:10000/dev/family/id/myattr"),-1);
	ASSERT_EQ(TangoUtils::ParseFullAttrName(domain_name,device_name,attr_name,hasQualityTag,"dev//family/id/myattr"),-1);

}//close TestParseFullAttrName()

TEST_F(TangoUtilsTest, TestParseFullProxyName)
{


}//close TestParseFullProxyName()


TEST_F(TangoUtilsTest, TestExtractCmdName)
{


}//close TestExtractCmdName()



}//close namespace
