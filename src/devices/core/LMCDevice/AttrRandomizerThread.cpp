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
* @file AttrRandomizerThread.cc
* @class AttrRandomizerThread
* @brief AttrRandomizerThread class
*
* AttrRandomizerThread class
* @author S. Riggi
* @date 29/01/2018
*/


#include <AttrRandomizerThread.h>

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

AttrRandomizerThread::AttrRandomizerThread(LMCDevice* dev) 
	: m_dev(dev), m_thread() {
			
	m_stopThread= false;
	m_threadStarted= false;
	m_randomizationEnabled= false;
	m_randomizationPeriod= 3000;//ms
	
}//close constructor
    
AttrRandomizerThread::~AttrRandomizerThread(){
	
	__DEBUG_LOG(m_dev,"Deleting task thread...");

	//{
  //	std::lock_guard<std::mutex> lock( mutex );
    	m_stopThread = true;
  //}
  if(m_thread.joinable()) m_thread.join();
	__DEBUG_LOG(m_dev,"Attribute randomizer thread destroyed!");
	
}//close destructor


void AttrRandomizerThread::Run(){
    
	__INFO_LOG(m_dev,"Starting attr randomizer thread...");
	m_threadStarted= true;	 

	//Starting infinite loop to process task queue
	while(!m_stopThread){
		
		//## Wait until randomization is enabled (or thread is signalled to stop)
		__DEBUG_LOG(m_dev,"Waiting until randomization is enabled (or thread is signalled to stop)...");
		{//- critical region start
			std::unique_lock<std::mutex> mlock(m_mutex);
			if(m_randomizationEnabled) {
				m_cond.wait_for(mlock,std::chrono::milliseconds(m_randomizationPeriod),[this]{return (m_stopThread || !m_randomizationEnabled);});
			}
			else {
				m_cond.wait(mlock,[this]{return (m_stopThread || m_randomizationEnabled);});
			}
		}//- critical region end

		//## Check exit condition
    __DEBUG_LOG(m_dev,"Randomization thread exiting from wait state (stop?"<<m_stopThread<<", randomizationEnabled? "<<m_randomizationEnabled<<") ...");
		if(m_stopThread){
			__INFO_LOG(m_dev,"Randomization thread signalled to exit ...");
			break;
		}
		if(!m_randomizationEnabled){
			__INFO_LOG(m_dev,"Randomization is disabled...");
			continue;
		}
		
		//## Randomize attributes
		__DEBUG_LOG(m_dev,"Randomizing dyn attrs ...");
		try{
			m_dev->RandomizeDynAttrValues();
		}
		catch(...){
			__WARN_LOG(m_dev,"Dynamic attribute randomization failed!");
			continue;
		}

  }//end randomization loop
    
	__DEBUG_LOG(m_dev,"End randomizer thread run");

}//close Run()



}//close namespace 


