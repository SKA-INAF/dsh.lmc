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
* @file TaskThread.h
* @class TaskThread
* @brief TaskThread class
*
* TaskThread class
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _TASK_THREAD_H
#define _TASK_THREAD_H

#ifndef LMCDevice_H
	#include <LMCDevice.h>
#endif

#ifndef _TANGO_H
	#include <tango.h>
#endif

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

//Forward declarations
namespace Utils_ns {
	class Task;
	class TaskManager;
}

namespace LMCDevice_ns {
  
//Forward declarations
class LMCDevice;

// ============================================================================
// class: TaskThread
// ============================================================================
class TaskThread {

	public:

		/** 
		\brief Constructor
 		*/
    TaskThread(LMCDevice* dev);

		/** 
		\brief Destructor
 		*/
    ~TaskThread();

	public:
		/** 
		\brief Start the thread
 		*/		
		void Start(){
    	m_thread = std::thread(&TaskThread::Run,this);
    }
		
		/** 
		\brief Stop
 		*/
		void Stop(){
			__DEBUG_LOG(m_dev,"Called Stop in task thread...");
			std::lock_guard<std::mutex> lock( m_mutex );
			m_stopThread = true;
			__DEBUG_LOG(m_dev,"Stop() ended");
		}

	private:
		/** 
		\brief Main thread function 
 		*/
		void Run();

	private:
	
		mutable std::mutex m_mutex;	
		std::atomic<bool> m_stopThread;
		LMCDevice* m_dev;	
		std::thread m_thread;
		
};//close TaskThread class

}//close namespace

#endif
