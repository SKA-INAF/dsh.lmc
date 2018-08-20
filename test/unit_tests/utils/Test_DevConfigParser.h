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
* @file DevConfigParserTest.h
* @class DevConfigParserTest
* @brief Class for testing DevConfigParser class
*
* Test class for DevConfigParser class
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef DEV_CONFIG_PARSER_TEST_H
#define DEV_CONFIG_PARSER_TEST_H

#include <gtest/gtest.h>
#include <gtest/gtest_prod.h>
#include <DevConfigParser.h>

#include <tango.h>



namespace Utils_ns {


class DevConfigParserTest : public ::testing::Test {

	protected:

		DevConfigParserTest() {
    	// You can do set-up work for each test here.
  	 	parser= new Utils_ns::DevConfigParser;

		}//close constructor

  	virtual ~DevConfigParserTest() {
    	// You can do clean-up work that doesn't throw exceptions here.

			if(parser){
				delete parser;
				parser= 0;
			}
  	}

	protected:
		// If the constructor and destructor are not enough for setting up
  	// and cleaning up each test, you can define the following methods:
  	virtual void SetUp() {
			// Code here will be called immediately after the constructor (right
    	// before each test).
  	}

  	virtual void TearDown() {
			// Code here will be called immediately after each test (right
    	// before the destructor).
 	 	}
	
	// Objects declared here can be used by all tests in the test case for Foo.
	//...
	//...
	Utils_ns::DevConfigParser* parser;	

};//close class 


}//close namespace

#endif
