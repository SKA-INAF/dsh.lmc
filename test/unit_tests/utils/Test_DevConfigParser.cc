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
* @file DevConfigParserTest.cc
* @class DevConfigParserTest
* @brief Class for testing DevConfigParser class
*
* Test class for DevConfigParser class
* @author S. Riggi
* @date 15/01/2016
*/

#include <Test_DevConfigParser.h>
#include <DevConfigParser.h>

#include "gtest/gtest.h"

#include <tango.h>


namespace Utils_ns {


//===========================================
//==           DEFINE TESTS              ====
//===========================================
TEST_F(DevConfigParserTest, TestGetAttrDataFormat)
{
  EXPECT_EQ(Tango::SCALAR,Utils_ns::DevConfigParser::GetAttrDataFormat("SCALAR"));
  EXPECT_EQ(Tango::SPECTRUM,Utils_ns::DevConfigParser::GetAttrDataFormat("SPECTRUM"));
  EXPECT_EQ(Tango::IMAGE,Utils_ns::DevConfigParser::GetAttrDataFormat("IMAGE"));
	EXPECT_EQ(-1,Utils_ns::DevConfigParser::GetAttrDataFormat(" SCALAR"));
	EXPECT_EQ(-1,Utils_ns::DevConfigParser::GetAttrDataFormat("BLABLA"));

}//close test

TEST_F(DevConfigParserTest, TestGetAttrDataType)
{
  EXPECT_EQ(Tango::DEV_DOUBLE,Utils_ns::DevConfigParser::GetAttrDataType("DOUBLE"));
  EXPECT_EQ(Tango::DEV_FLOAT,Utils_ns::DevConfigParser::GetAttrDataType("FLOAT"));
  EXPECT_EQ(Tango::DEV_STRING,Utils_ns::DevConfigParser::GetAttrDataType("STRING"));
	EXPECT_EQ(-1,Utils_ns::DevConfigParser::GetAttrDataType(" DOUBLE"));
	EXPECT_EQ(-1,Utils_ns::DevConfigParser::GetAttrDataType("BLABLA"));

}//close test


}//close namespace


