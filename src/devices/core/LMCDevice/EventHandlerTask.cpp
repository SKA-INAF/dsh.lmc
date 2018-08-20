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
* @file EventHandlerTask.cc
* @class EventHandlerTask
* @brief EventHandlerTask
*
* EventHandlerTask
* @author S. Riggi
* @date 15/01/2017
*/


#include <EventHandlerTask.h>

#ifndef LMCDevice_H
	#include <LMCDevice.h>
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

// ============================================================================
// SOME USER MESSAGES AND CONSTS
// ============================================================================
#define kSTART_PERIODIC_MSG static_cast<size_t>((yat::FIRST_USER_MSG + 1000))
#define kSTOP_PERIODIC_MSG  static_cast<size_t>((yat::FIRST_USER_MSG + 1001))
#define kHANDLE_EVENT_MSG  static_cast<size_t>((yat::FIRST_USER_MSG + 1002))
//-----------------------------------------------------------------------------

namespace LMCDevice_ns {

// ============================================================================
// EventHandlerTask::EventHandlerTask
// ============================================================================
EventHandlerTask::EventHandlerTask (const Config & cfg)
  : yat4tango::DeviceTask(cfg.host_device), 
    m_cfg(cfg)
{
  //- configure optional msg handling
  this->enable_timeout_msg(false);
  this->enable_periodic_msg(false);
  this->set_periodic_msg_period(cfg.task_activity_period_ms);

	//- Init wait cond
	m_wait_cond= 0;

}//close constructor

// ============================================================================
// EventHandlerTask::~EventHandlerTask
// ============================================================================
EventHandlerTask::~EventHandlerTask () {

	//Delete wait cond
	if(m_wait_cond){
		delete m_wait_cond;
		m_wait_cond= 0;
	}

}//close destructor

// ============================================================================
// EventHandlerTask::go
// ============================================================================
void EventHandlerTask::go (size_t tmo_ms)
    throw (Tango::DevFailed)
{

	//- Initialize wait condition variable
	if(!m_wait_cond) m_wait_cond= new yat::Condition(m_data_lock);

  //- a timer
  yat::Timer t;

  //- finally call yat4tango::DeviceTask::go
  long remaining_tmo = static_cast<long>(tmo_ms) - static_cast<long>(t.elapsed_msec());
  this->yat4tango::DeviceTask::go(remaining_tmo > 1 ? remaining_tmo : 1);

}//close go()

// ============================================================================
// EventHandlerTask::process_message
// ============================================================================
void EventHandlerTask::process_message (yat::Message& msg)
  throw (Tango::DevFailed)
{

	__DEBUG_LOG(m_cfg.host_device,"Received message (type="<<msg.type()<<") will be processed in the queue...");

  //- handle msg
  switch (msg.type())
  {
    //- THREAD_INIT ----------------------
    case yat::TASK_INIT:
    {
	  	__DEBUG_LOG(m_cfg.host_device,"THREAD_INIT - Thread is starting up");
      if(m_cfg.enable_periodic_msg) this->enable_periodic_msg(true);
    } 
	  break;
      
	  //- THREAD_EXIT ----------------------
	  case yat::TASK_EXIT:
	  {
			__DEBUG_LOG(m_cfg.host_device,"THREAD_EXIT - Thread is quitting");
    }
		break;
      
	  //- THREAD_PERIODIC ------------------
	  case yat::TASK_PERIODIC:
	  {
			__DEBUG_LOG(m_cfg.host_device,"TASK_PERIODIC - Periodic task will be executed");
		  this->periodic_job_i();
    }
	  break;
      
	  //- THREAD_TIMEOUT -------------------
	  case yat::TASK_TIMEOUT:
	  {
    	//- not used in this example
			__DEBUG_LOG(m_cfg.host_device,"TASK_TIMEOUT - Timeout task will be executed...");
			//...
    }
    break;

    //- kSTART_PERIODIC_MSG --------------
    case kSTART_PERIODIC_MSG:
    {
   		__DEBUG_LOG(m_cfg.host_device,"kSTART_PERIODIC_MSG - Start periodic message will be executed...");
      this->enable_periodic_msg(true);
    }
    break;

    //- kSTOP_PERIODIC_MSG ---------------
    case kSTOP_PERIODIC_MSG:
    {
   		__DEBUG_LOG(m_cfg.host_device,"kSTOP_PERIODIC_MSG - Stop periodic message will be executed...");
    	this->enable_periodic_msg(false);
    }
    break;

		//- kHANDLE_EVENT_MSG ------------
    case kHANDLE_EVENT_MSG:
    {
			try {
				//Tango::EventData* msg_data= msg.get_data<Tango::EventData*>();
				Tango::EventData msg_data= msg.get_data<Tango::EventData>();
				__DEBUG_LOG(m_cfg.host_device,"kHANDLE_EVENT_MSG - Received msg data");
				this->handle_event_i(&msg_data);
				//this->handle_event_i(msg);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kHANDLE_EVENT_MSG - Failed to extract message data!");
				throw;
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kHANDLE_EVENT_MSG - Unknown exception while extracting message data!");
				throw;
			}
    }//close case
		break;

    //- UNHANDLED MSG --------------------
		default:
		{
			__DEBUG_LOG(m_cfg.host_device,"Unhandled msg type received");
		}
		break;

  }//close switch

}//close process_message()


// ============================================================================
// EventHandlerTask::exit
// ============================================================================
void EventHandlerTask::exit ()
    throw (Tango::DevFailed)
{
  //- Stop the periodic activity
	size_t wait_time= 2;//in ms
  try {
  	this->stop_periodic_activity(wait_time);
  }
  catch (...) {
    //- ignore errors cause we need to exit!
  }

	//...  

  //- Finally call call yat4tango::DeviceTask::exit
  this->yat4tango::DeviceTask::exit();

}//close exit()

// ============================================================================
// EventHandlerTask::start_periodic_activity
// ============================================================================
void EventHandlerTask::start_periodic_activity (size_t tmo_ms)
    throw (Tango::DevFailed)
{

  try {
    //- synchronous approach: "post then wait for the message to be handled"
    if (tmo_ms) {
      this->wait_msg_handled(kSTART_PERIODIC_MSG, tmo_ms);
		}
    //- asynchronous approach: "post the data then returm immediately"
    else {
      this->post(kSTART_PERIODIC_MSG);
		}
  }//close try 
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached)
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message 
    throw;
  }

}//close start_periodic_activity()


// ============================================================================
// EventHandlerTask::stop_periodic_activity
// ============================================================================
void EventHandlerTask::stop_periodic_activity (size_t tmo_ms)
    throw (Tango::DevFailed)
{
  try {
    //- synchronous approach: "post then wait for the message to be handled"
    if (tmo_ms) {
      this->wait_msg_handled(kSTOP_PERIODIC_MSG, tmo_ms);
		}
    //- asynchronous approach: "post the data then returm immediately"
    else {
      this->post(kSTOP_PERIODIC_MSG);
		}
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached)
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message 
    throw;
  }
}//close stop_periodic_activity()




// ============================================================================
// EventHandlerTask::handle_event
// ============================================================================
//void EventHandlerTask::handle_event (yat::Message& msg, bool wait_cmd_reply)
void EventHandlerTask::handle_event (Tango::EventData* msg, bool wait_cmd_reply)
//void EventHandlerTask::handle_event (Tango::EventData& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{

  try {
    if (wait_cmd_reply){
      //this->wait_msg_handled(kHANDLE_EVENT_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
			this->wait_msg_handled(kHANDLE_EVENT_MSG, msg, true, kDEFAULT_MSG_TMO_MSECS);
		}
    else{
      //this->post(kHANDLE_EVENT_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
			this->post(kHANDLE_EVENT_MSG, msg, true, kDEFAULT_MSG_TMO_MSECS);
		}
  }//close try block
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_startup_i) 
    throw;
  }
}//close handle_event()


// ============================================================================
// EventHandlerTask::handle_event_i
// ============================================================================
//void EventHandlerTask::handle_event_i (yat::Message& msg)
void EventHandlerTask::handle_event_i (Tango::EventData* event_data)
//void EventHandlerTask::handle_event_i (Tango::EventData& event_data)
  throw (Tango::DevFailed)
{
	
	//## Check device ptr & get message info
	if(!m_cfg.host_device){
		std::string errMsg("Device instance is null!");
		__ERROR_LOG(m_cfg.host_device,errMsg);
		_THROW_TANGO_EXCEPTION("HANDLE_EVENT_FAILED",errMsg);
	}

	//## Check proxy manager
	if(!m_cfg.host_device->m_proxyManager){
		std::string errMsg("Proxy manager was not initialized (done at device init)!");
		__ERROR_LOG(m_cfg.host_device,errMsg);
		_THROW_TANGO_EXCEPTION("HANDLE_EVENT_FAILED",errMsg);
	}

	//## Check formula manager
	if(!m_cfg.host_device->m_formulaManager){
		std::string errMsg("Formula manager was not initialized (done at device init)!");
		__ERROR_LOG(m_cfg.host_device,errMsg);
		_THROW_TANGO_EXCEPTION("HANDLE_EVENT_FAILED",errMsg);
	}

	/*
	//## Extract message data
	Tango::EventData* event_data = 0;
	try {
		event_data= msg.get_data<Tango::EventData*>();
	}
	catch(yat::Exception& ex){
		std::string errMsg("Failed to extract EventData from message (check if valid data type)!");
		__ERROR_LOG(m_cfg.host_device,errMsg);
		_THROW_TANGO_EXCEPTION("HANDLE_EVENT_FAILED",errMsg);
	}
	*/

	
	//## Check event data
	if(!event_data){
		std::string errMsg("Null ptr to event data!");
		__ERROR_LOG(m_cfg.host_device,errMsg);
		_THROW_TANGO_EXCEPTION("HANDLE_EVENT_FAILED",errMsg);
	}
	

	//## Get and parse event info	
	//std::string event_attr_name= event_data.attr_name;
	//std::string event_type_str= event_data.event;
	//Tango::DeviceAttribute* dev_attr= event_data.attr_value;
	std::string event_attr_name= event_data->attr_name;
	std::string event_type_str= event_data->event;
	Tango::DeviceAttribute* dev_attr= event_data->attr_value;
	Tango::AttrQuality attr_quality= dev_attr->quality;
	Tango::TimeVal attr_timestamp= dev_attr->time;

	std::string domain_name= "";
	std::string proxy_name= "";
	std::string proxy_attr_name= "";
	bool hasQualityTag= false;
	if(Utils_ns::TangoUtils::ParseFullAttrName(domain_name,proxy_name,proxy_attr_name,hasQualityTag,event_attr_name)<0){
		std::string errMsg("Failed to parse event attr name!");
		__WARN_LOG(m_cfg.host_device,errMsg);
		return;
	}
	std::string proxy_name_full= domain_name+ std::string("/") + proxy_name;
	__DEBUG_LOG(m_cfg.host_device,"Handling received event (proxy="<<proxy_name_full<<", proxy_attr="<<proxy_attr_name<<", event_type="<<event_type_str<<", timestamp=("<<attr_timestamp.tv_sec<<","<<attr_timestamp.tv_usec<<"), quality="<<attr_quality<<")...");
	
	//## Find event subcription data
	__DEBUG_LOG(m_cfg.host_device,"Findind subscription data for received event (proxy="<<proxy_name_full<<", proxy_attr="<<proxy_attr_name<<", event_type="<<event_type_str<<")...");
	Utils_ns::EvtSubscriptionDataPtr evt_sub_data_ptr= (m_cfg.host_device->m_proxyManager)->FindEvtSubscription(proxy_name_full,proxy_attr_name,event_type_str);
	if(!evt_sub_data_ptr){
		__WARN_LOG(m_cfg.host_device,"Cannot find any registered subscription data for event "<<event_attr_name<<", no handlers and attr formulas will be called!");
		return;
	}

	//## Evaluate formulas linked to this event
	if( (m_cfg.host_device->m_formulaManager)->HasFormulas() && m_cfg.host_device->EvaluateFormulasOnEvent(evt_sub_data_ptr,event_data)<0){
		__WARN_LOG(m_cfg.host_device,"Failed to evaluate formulas linked to received event (proxy="<<proxy_name_full<<", proxy_attr="<<proxy_attr_name<<", event_type="<<event_type_str<<")...");
	}

	//## Execute event handler commands registered for this event
	if(m_cfg.host_device->ExecuteEventCmdHandlers(evt_sub_data_ptr,event_data)<0){
		__WARN_LOG(m_cfg.host_device,"One or more event handler commands failed for received event (proxy="<<proxy_name_full<<", proxy_attr="<<proxy_attr_name<<", event_type="<<event_type_str<<")...");
	}
	

}//close handle_event_i()


// ============================================================================
// EventHandlerTask::periodic_job_i
// ============================================================================
void EventHandlerTask::periodic_job_i ()
  throw (Tango::DevFailed)
{

	//...
	//...

}//close periodic_job_i()



}//close namespace

