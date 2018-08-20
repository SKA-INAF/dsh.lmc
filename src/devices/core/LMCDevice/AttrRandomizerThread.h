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
* @file AttrRandomizerThread.h
* @class AttrRandomizerThread
* @brief AttrRandomizerThread class
*
* AttrRandomizerThread class
* @author S. Riggi
* @date 29/01/2018
*/

#ifndef _ATTR_RANDOMIZER_THREAD_H
#define _ATTR_RANDOMIZER_THREAD_H

#ifndef LMCDevice_H
	#include "LMCDevice.h"
#endif

#ifndef _TANGO_H
	#include <tango.h>
#endif

//C++ headers
#include <random>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>


namespace LMCDevice_ns {
  
//Forward declarations
class LMCDevice;

// ============================================================================
// class: AttrRandomizerThread
// ============================================================================
class AttrRandomizerThread {

	public:

		/** 
		\brief Constructor
 		*/
    AttrRandomizerThread(LMCDevice* dev);

		/** 
		\brief Destructor
 		*/
    ~AttrRandomizerThread();

	public:
	
		/** 
		\brief Start the thread
 		*/
		void SetRandomizationFlag(bool value){m_randomizationEnabled=value;}

		/** 
		\brief Start the thread
 		*/
		bool GetRandomizationFlag(){
			std::lock_guard<std::mutex> lock( m_mutex );
			return m_randomizationEnabled;		
		}

		/** 
		\brief Set randomization period
 		*/
		void SetRandomizationPeriod(long period){
			std::lock_guard<std::mutex> lock( m_mutex );
			m_randomizationPeriod= period;
		}

		/** 
		\brief Get randomization period
 		*/
		long GetRandomizationPeriod(){
			std::lock_guard<std::mutex> lock( m_mutex );
			return m_randomizationPeriod;
		}
	
		/** 
		\brief Start the thread
 		*/
		bool IsStarted(){
			std::lock_guard<std::mutex> lock( m_mutex );
			return m_threadStarted;
		}

		/** 
		\brief Start the thread
 		*/		
		void Start(){
    	m_thread = std::thread(&AttrRandomizerThread::Run,this);
    }
		
		/** 
		\brief Stop
 		*/
		void Stop(){
			__DEBUG_LOG(m_dev,"Called Stop in randomizer thread...");
			std::lock_guard<std::mutex> lock( m_mutex );
			m_stopThread = true;
			m_cond.notify_all();
			__DEBUG_LOG(m_dev,"Stop() ended");
		}

		/** 
		\brief Enable randomization
 		*/
		void EnableRandomization(){
			__DEBUG_LOG(m_dev,"Called EnableRandomization in randomizer thread...");
			std::lock_guard<std::mutex> lock( m_mutex );
			m_randomizationEnabled = true;
			m_cond.notify_all();
			__DEBUG_LOG(m_dev,"EnableRandomization() ended");
		}

		/** 
		\brief Disable randomization
 		*/
		void DisableRandomization(){
			__DEBUG_LOG(m_dev,"Called DisableRandomization in randomizer thread...");
			std::lock_guard<std::mutex> lock( m_mutex );
			m_randomizationEnabled = false;
			m_cond.notify_all();
			__DEBUG_LOG(m_dev,"DisableRandomization() ended");
		}


	private:
		/** 
		\brief Main thread function 
 		*/
		void Run();


	private:
	
		//Member classes
		LMCDevice* m_dev;	
		
		//Thread stuff (mutexes, cond vars, etc)
		mutable std::mutex m_mutex;	
		bool m_stopThread;
		bool m_threadStarted;
		std::condition_variable m_cond;
		std::thread m_thread;

		//Vars
		bool m_randomizationEnabled;
		long m_randomizationPeriod;	
		bool m_randomizationChanged;

};//close AttrRandomizerThread class

}//close namespace

#endif
