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
* @file TaskThread.cc
* @class TaskThread
* @brief TaskThread class
*
* TaskThread class
* @author S. Riggi
* @date 15/01/2016
*/


#include <TaskThread.h>

#ifndef _TASK_MANAGER_h
	#include <TaskManager.h>
#endif

#ifndef _TASK_h
	#include <Task.h>
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

#ifndef _TANGO_H
	#include <tango.h>
#endif

//## Standard headers
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctime>
#include <stdexcept>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

#include <map>
#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <chrono>

using namespace std;

namespace LMCDevice_ns {

TaskThread::TaskThread(LMCDevice* dev) 
	: m_dev(dev), m_thread() {
			
	m_stopThread= false;

}//close constructor
    
TaskThread::~TaskThread(){
	
	__DEBUG_LOG(m_dev,"Deleting task thread...");

	//{
  //	std::lock_guard<std::mutex> lock( mutex );
    	m_stopThread = true;
  //}
  if(m_thread.joinable()) m_thread.join();
	__DEBUG_LOG(m_dev,"Task thread destroyed!");
	
}//close destructor


void TaskThread::Run(){
    
	__INFO_LOG(m_dev,"Starting task thread...");
	 
	//Starting infinite loop to process task queue
	while(!m_stopThread){
    __DEBUG_LOG(m_dev,"Task thread looping (exit? "<<m_stopThread<<") ...");
		if(m_stopThread){
			__DEBUG_LOG(m_dev,"Task thread signalled and exit from infinite loop...");
			break;
		}

		//Pop task from queue	
		Utils_ns::Task task;
		if( (m_dev->m_taskManager)->PopTask(task)<0 ) continue;
  
		//Execute task popped from the queue
		try{
			(m_dev->m_taskManager)->ExecuteTask(task);
		}	
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			__ERROR_LOG(m_dev,"Tango exception occurred while executing task "<<task.cmd_name<<" in queue!");
			continue;
		}
		catch(std::exception& e){
			__ERROR_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while executing task "<<task.cmd_name<<" in queue!");
			continue;
		}
		catch(...){
			__ERROR_LOG(m_dev,"Unknown exception occurred while executing task "<<task.cmd_name<<" in queue!");		
			continue;
		}

  }//end infinite loop
    
	__DEBUG_LOG(m_dev,"End task thread run");

}//close Run()



}//close namespace 


