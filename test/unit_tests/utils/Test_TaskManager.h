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
* @file Test_TaskManager.h
* @class Test_TaskManager
* @brief Class for testing TaskManager class
*
* Test class for TaskManager class
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef TASK_MANAGER_TEST_H
#define TASK_MANAGER_TEST_H

#include <gtest/gtest.h>
#include <gtest/gtest_prod.h>

#include <TestEnvironment.h>
#include <TestDevice.h>
#include <TestDeviceClass.h>

#include <TaskManager.h>
#include <Task.h>

#include <tango.h>


namespace Utils_ns {


class TaskManagerTest : public ::testing::Test {

	protected:

		TaskManagerTest() {
    	// You can do set-up work for each test here.

		}//close constructor

  	virtual ~TaskManagerTest() {
    	// You can do clean-up work that doesn't throw exceptions here.

  	}//close destructor

	protected:
		// If the constructor and destructor are not enough for setting up
  	// and cleaning up each test, you can define the following methods:
  	virtual void SetUp() {
			// Code here will be called immediately after the constructor (right
    	// before each test).
			//Flush the queue if filled during the test
			if(m_taskManager) m_taskManager->FlushQueue();

  	}

  	virtual void TearDown() {
			// Code here will be called immediately after each test (right
    	// before the destructor).

			//Flush the queue if filled during the test
			if(m_taskManager) m_taskManager->FlushQueue();
 	 	}

		// Per-test-case set-up.
  	// Called before the first test in this test case.
  	// Can be omitted if not needed.
		static void SetUpTestCase() {
			/*
			//Retrieve Test device from environment
			if(!TestEnvironment::device){
				m_setUpFailed= true;
				return;
			}

			//Create task manager if not created already
			if(!m_taskManager) m_taskManager= new TaskManager(TestEnvironment::device);
			*/

			if(!m_taskManager) m_taskManager= new TaskManager(nullptr);
	
		}//close SetUpTestCase()
	
		// Per-test-case tear-down.
  	// Called after the last test in this test case.
  	// Can be omitted if not needed.
  	static void TearDownTestCase() {
    	//Delete task manager
			if(m_taskManager){
				delete m_taskManager;
				m_taskManager= 0;
			}
  	}//close TearDownTestCase()

	// Objects declared here can be used by all tests in the test case for Foo.
	//...
	//...
	static Utils_ns::TaskManager* m_taskManager;	
	static bool m_setUpFailed;

};//close class 


}//close namespace

#endif
