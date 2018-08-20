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
* @file EventCallBack.cpp
* @class EventCallBack
* @brief EventCallBack
*
* EventCallBack
* @author S. Riggi
* @date 15/01/2017
*/

#include <EventCallBack.h>

#ifndef LMCDevice_H
	#include <LMCDevice.h>
#endif

#ifndef _EVENT_HANDLER_TASK_H_
	#include <EventHandlerTask.h>
#endif

#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _TANGO_H
	#include <tango.h>
#endif

//## Yat headers
#ifndef _YAT_EXCEPTION_H_
	#include <yat/Exception.h>
#endif

#ifndef _YAT_MESSAGE_H_
	#include <yat/threading/Message.h>
#endif

//## Standard headers
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <signal.h>
#include <ctime>
#include <stdexcept>
#include <stdlib.h>

#include <ratio>
#include <chrono>
#include <map>
#include <vector>

namespace LMCDevice_ns {

LMCDevice* EventCallBack::m_dev= 0;

EventCallBack::EventCallBack (LMCDevice* dev) {
  
	m_dev = dev;
	
}//close constructor
	

EventCallBack::~EventCallBack(){

}//destructor


void EventCallBack::push_event(Tango::EventData* event) { 
    
	//## Get access to event info
	/*
	[device] : The DeviceProxy object on which the call was executed (Tango::DeviceProxy *)
	[attr_name] : The attribute name (std::string &)
	[event] : The event name (std::string &)
	[attr_value] : The attribute data (DeviceAttribute *)
	[err] : A boolean flag set to true if the request failed. False otherwise (bool)
	[errors] : The error stack (DevErrorList &)
	*/

	if(!m_dev) {
		return;
	}

	if(!event){
		__INFO_LOG(m_dev,"Received nullptr EventData!"); 
		return;
	}
	
	
  try {   	
    	
		//Process event
		if(process_event(event)<0){
			std::string errMsg("Failed to process event");
			//_THROW_TANGO_EXCEPTION("PROCESS_EVENT_FAILED","Failed to process event");
			Tango::Except::throw_exception(
				(const char *)"PROCESS_EVENT_FAILED",
				errMsg.c_str(),
				(const char *)"EventCallBack::process_event()"
			);
		}//close if

  }//close try 	
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		__ERROR_LOG(m_dev,"Failed to access to event data and process event!");
		return;
	}	
  catch (...) { 
  	__ERROR_LOG(m_dev,"Unknown exception occurred while accessing to event data");
		return;
  }
 
	return;
	
}//close EventCallBack::push_event()

int EventCallBack::process_event(Tango::EventData* event){

	//## Check event data
	if(!event) return -1;

	std::string event_attr_name= event->attr_name;
	if (event->err) {
		std::stringstream ss;
		ss<<"Invalid event "<<event_attr_name<<" received, skip processing. Error list: [";
		for (unsigned long j=0;j<event->errors.length();j++){
			ss<<"{reason="<<event->errors[j].reason.in()<<", desc="<<event->errors[j].desc.in()<<", origin="<<event->errors[j].origin.in()<<" severity="<<event->errors[j].severity<<"}, ";
		}
		ss<<"]";
		__WARN_LOG(m_dev,ss.str());
		return -1;
	}

	//## Update event counter in device
	{
		m_dev->m_mutex.lock();
		m_dev->m_eventCounter++;
		m_dev->m_mutex.unlock();
	}

	if(m_dev->processEventsInQueueThread){
		//## Move events to queue task thread
		//## Check event handler task
		if(!m_dev->m_eventHandlerTask){
			__ERROR_LOG(m_dev,"Event handler task was not initialized!");
			return -1;
		}
	
		//## Prepare event handler data
		Tango::EventData* msg_data= new Tango::EventData;	
		*msg_data= *event;

		//## Post message to EventHandler task queue
		bool isSynchronous= false;
		try {	
			(m_dev->m_eventHandlerTask)->handle_event(msg_data, isSynchronous);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			std::string errMsg("Tango exception occurred while posting message to EventHandler task!");
			__ERROR_LOG(m_dev,errMsg);
			return -1;
		}	
		catch(...){
			std::string errMsg("Unknown C++ exception occurred while posting message to EventHandler task!");
			__ERROR_LOG(m_dev,errMsg);	
			return -1;
		}
	}//close if
	else{
		//## Invoke device method to handle event in the same callback thread
		if(m_dev->HandleEvent(event)<0){
			__ERROR_LOG(m_dev,"Failed to handle event "<<event_attr_name<<"!");
			return -1;
		}
	}//close else
	

	return 0;

}//close process_event()

}//close namespace


