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
* @file ProxyMonitorTask.cc
* @class ProxyMonitorTask
* @brief ProxyMonitorTask
*
* ProxyMonitorTask
* @author S. Riggi
* @date 15/01/2017
*/


#include <ProxyMonitorTask.h>

#ifndef LMCDevice_H
	#include <LMCDevice.h>
#endif

#ifndef _TANGO_PROXY_MANAGER_h
	#include <TangoProxyManager.h>
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

// ============================================================================
// SOME USER MESSAGES AND CONSTS
// ============================================================================
#define kSTART_PERIODIC_MSG static_cast<size_t>((yat::FIRST_USER_MSG + 1000))
#define kSTOP_PERIODIC_MSG  static_cast<size_t>((yat::FIRST_USER_MSG + 1001))
//-----------------------------------------------------------------------------

namespace LMCDevice_ns {

// ============================================================================
// ProxyMonitorTask::ProxyMonitorTask
// ============================================================================
ProxyMonitorTask::ProxyMonitorTask (const Config & cfg)
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
// ProxyMonitorTask::~ProxyMonitorTask
// ============================================================================
ProxyMonitorTask::~ProxyMonitorTask () {

	//Delete wait cond
	if(m_wait_cond){
		delete m_wait_cond;
		m_wait_cond= 0;
	}

}//close destructor

// ============================================================================
// ProxyMonitorTask::go
// ============================================================================
void ProxyMonitorTask::go (size_t tmo_ms)
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
// ProxyMonitorTask::process_message
// ============================================================================
void ProxyMonitorTask::process_message (yat::Message& msg)
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

    //- UNHANDLED MSG --------------------
		default:
		{
			__DEBUG_LOG(m_cfg.host_device,"Unhandled msg type received");
		}
		break;

  }//close switch

}//close process_message()


// ============================================================================
// ProxyMonitorTask::exit
// ============================================================================
void ProxyMonitorTask::exit ()
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
// ProxyMonitorTask::start_periodic_activity
// ============================================================================
void ProxyMonitorTask::start_periodic_activity (size_t tmo_ms)
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
// ProxyMonitorTask::stop_periodic_activity
// ============================================================================
void ProxyMonitorTask::stop_periodic_activity (size_t tmo_ms)
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
// ProxyMonitorTask::periodic_job_i
// ============================================================================
void ProxyMonitorTask::periodic_job_i ()
  throw (Tango::DevFailed)
{

	//Check proxy manager
	if(!m_cfg.proxy_manager){
		__WARN_LOG(m_cfg.host_device,"Null ptr to proxy manager (did you initialize it in host device?)!");
		return;
	}
	
	//Get list of registered proxies and store name of expcted proxy attr inf device
	std::vector<std::string> proxy_names= (m_cfg.proxy_manager)->GetRegisteredProxyNames();
	__DEBUG_LOG(m_cfg.host_device,"#"<<proxy_names.size()<<" to be monitored...");
	
	//Monitor proxies and update proxy attr values
	for(unsigned int i=0;i<proxy_names.size();i++){
		__DEBUG_LOG(m_cfg.host_device,"Monitoring proxy "<<proxy_names[i]<<"...");
		
		//Monitor proxy and get proxy updated info
		std::vector<std::string> proxy_info;
		if((m_cfg.proxy_manager)->MonitorProxy(proxy_names[i],proxy_info)<0){
			__WARN_LOG(m_cfg.host_device,"Failed to monitor proxy "<<proxy_names[i]<<", skip to next proxy in list...");
			continue;
		}
		__DEBUG_LOG(m_cfg.host_device,"Proxy monitoring info: runStatus="<<proxy_info[0]<<", info="<<proxy_info[1]);

		//If monitoring succeeded set proxy info to device attribute
		//First Parse proxy name to get domain, family and device name
		std::string proxy_facility_name= "";
		std::string proxy_domain_name= "";
		std::string proxy_family_name= "";
		std::string proxy_member_name= "";
		if(Utils_ns::TangoUtils::ParseFullProxyName(proxy_facility_name,proxy_domain_name,proxy_family_name,proxy_member_name,proxy_names[i])<0){
			__WARN_LOG(m_cfg.host_device,"Failed to parse proxy name "<<proxy_names[i]<<" (will not update proxy attr value), skip to next proxy in list...");	
			continue;
		}
		std::string proxy_dyn_attr_name= std::string("proxyRunStatus_") + 
			proxy_domain_name + std::string("_") + 
			proxy_family_name + std::string("_") +
			proxy_member_name;		
		
		//Set dyn proxy attr value
		if((m_cfg.host_device)->SetDynAttrValue<std::string>(proxy_info,proxy_dyn_attr_name)<0){
			__WARN_LOG(m_cfg.host_device,"Failed to set proxy info in device attr "<<proxy_dyn_attr_name<<" (will not update proxy attr value), skip to next proxy in list...");	
			//continue;
		}

		//Process proxy run status
		int proxy_run_status= Utils_ns::TangoProxyHelper::GetProxyRunStatusFromString(proxy_info[0]);
		if((m_cfg.host_device)->ProcessProxyRunStatusInfo(proxy_names[i],proxy_run_status)<0){
			__WARN_LOG(m_cfg.host_device,"Failed to process proxy "<<proxy_names[i]<<" run status info, skip to next proxy in list...");
			//continue;
		}

	}//end loop proxies to be monitored


}//close periodic_job_i()



}//close namespace

