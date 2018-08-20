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
* @file ConfigureTask.cc
* @class ConfigureTask
* @brief ConfigureTask
*
* ConfigureTask
* @author S. Riggi
* @date 15/01/2017
*/


#include <ConfigureTask.h>
#include <DishManager.h>

#include <Logger.h>

// ============================================================================
// SOME USER MESSAGES AND CONSTS
// ============================================================================
#define kSTART_PERIODIC_MSG static_cast<size_t>((yat::FIRST_USER_MSG + 1000))
#define kSTOP_PERIODIC_MSG  static_cast<size_t>((yat::FIRST_USER_MSG + 1001))
#define kSTART_TIMEOUT_MSG static_cast<size_t>((yat::FIRST_USER_MSG + 1002))
#define kSTOP_TIMEOUT_MSG  static_cast<size_t>((yat::FIRST_USER_MSG + 1003))
#define kEXECUTE_CONFIGURE_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1004))
//-----------------------------------------------------------------------------

namespace DishManager_ns {

// ============================================================================
// ConfigureTask::ConfigureTask
// ============================================================================
ConfigureTask::ConfigureTask (const Config & cfg)
  : yat4tango::DeviceTask(cfg.host_device), 
    m_cfg(cfg)
{
  //- configure optional msg handling
  this->enable_timeout_msg(false);
  this->enable_periodic_msg(false);
  this->set_periodic_msg_period(cfg.task_activity_period_ms);
	this->set_timeout_msg_period(cfg.task_activity_timeout_ms);

	//- Init wait cond
	m_wait_cond= 0;

}//close constructor

// ============================================================================
// ConfigureTask::~ConfigureTask
// ============================================================================
ConfigureTask::~ConfigureTask () {

	//Delete wait cond
	if(m_wait_cond){
		delete m_wait_cond;
		m_wait_cond= 0;
	}

}//close destructor

// ============================================================================
// ConfigureTask::go
// ============================================================================
void ConfigureTask::go (size_t tmo_ms)
    throw (Tango::DevFailed)
{

	//- Initialize wait condition variable
	if(!m_wait_cond) m_wait_cond= new yat::Condition(m_data_lock);

  //- a timer
  yat::Timer t;

	INFO_STREAM << "ConfigureTask::go() - Initializing task thread..." << endl;

  //- finally call yat4tango::DeviceTask::go
  long remaining_tmo = static_cast<long>(tmo_ms) - static_cast<long>(t.elapsed_msec());
  this->yat4tango::DeviceTask::go(remaining_tmo > 1 ? remaining_tmo : 1);

  INFO_STREAM << "ConfigureTask::go() - End initialization" << endl;
}//close go()

// ============================================================================
// ConfigureTask::process_message
// ============================================================================
void ConfigureTask::process_message (yat::Message& msg)
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
			this->timeout_job_i();
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

		//- kSTART_TIMEOUT_MSG --------------
    case kSTART_TIMEOUT_MSG:
    {
   		__DEBUG_LOG(m_cfg.host_device,"kSTART_TIMEOUT_MSG - Start timeout message will be executed...");
      this->enable_timeout_msg(true);
    }
    break;

    //- kSTOP_TIMEOUT_MSG ---------------
    case kSTOP_TIMEOUT_MSG:
    {
   		__DEBUG_LOG(m_cfg.host_device,"kSTOP_TIMEOUT_MSG - Stop timeout message will be executed...");
    	this->enable_timeout_msg(false);
    }
    break;

    
		//- kEXECUTE_CONFIGURE_CMD_MSG ------------
    case kEXECUTE_CONFIGURE_CMD_MSG:
    {
      try {
				const ConfigMsg& msg_data = msg.get_data<ConfigMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_CONFIGURE_CMD_MSG - Received msg data: {bandId=" << msg_data.bandId<<"}");
				this->execute_configure_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_CONFIGURE_CMD_MSG - Failed to execute configure!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_CONFIGURE_CMD_MSG - Unknown exception while configuring!");
			}
    }
		break;

    //- UNHANDLED MSG --------------------
		default:
		  __DEBUG_LOG(m_cfg.host_device,"Unhandled msg type received");
			break;
  }//close switch

}//close process_message()


// ============================================================================
// ConfigureTask::exit
// ============================================================================
void ConfigureTask::exit ()
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
// ConfigureTask::start_periodic_activity
// ============================================================================
void ConfigureTask::start_periodic_activity (size_t tmo_ms)
    throw (Tango::DevFailed)
{
  try
  {
    //- synchronous approach: "post then wait for the message to be handled"
    if (tmo_ms)
      this->wait_msg_handled(kSTART_PERIODIC_MSG, tmo_ms);
    //- asynchronous approach: "post the data then returm immediately"
    else
      this->post(kSTART_PERIODIC_MSG);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached)
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message 
    throw;
  }
}


// ============================================================================
// ConfigureTask::stop_periodic_activity
// ============================================================================
void ConfigureTask::stop_periodic_activity (size_t tmo_ms)
    throw (Tango::DevFailed)
{
  try
  {
    //- synchronous approach: "post then wait for the message to be handled"
    if (tmo_ms)
      this->wait_msg_handled(kSTOP_PERIODIC_MSG, tmo_ms);
    //- asynchronous approach: "post the data then returm immediately"
    else
      this->post(kSTOP_PERIODIC_MSG);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached)
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message 
    throw;
  }
}


// ============================================================================
// ConfigureTask::execute_configure
// ============================================================================
void ConfigureTask::execute_configure (const ConfigMsg& configMsg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{

  try {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_CONFIGURE_CMD_MSG, configMsg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_CONFIGURE_CMD_MSG, configMsg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_configure_i) 
    throw;
  }
}//close execute_configure()


// ============================================================================
// ConfigureTask::periodic_job_i
// ============================================================================
void ConfigureTask::periodic_job_i ()
  throw (Tango::DevFailed)
{

	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string configureStatus_attr_name= "configureStatus";
	std::string configureStatus_onband= "ON-BAND";
	std::string configureStatus_ongoing= "RUNNING";
	std::string configureStatus_failed= "FAILED";	
	std::string configureStatus_completed= "COMPLETED";

	std::string configureStatusFlag_attr_name= "configureStatusFlag";
	std::string configureStatusFlag_onband= "ON-BAND";
	std::string configureStatusFlag_ongoing= "RUNNING";
	std::string configureStatusFlag_failed= "FAILED";	
	std::string configureStatusFlag_idle= "IDLE";
	//###########################################################################

	//## Get device instance
	DishManager* dev= static_cast<DishManager*>(m_cfg.host_device);
	if(!dev){//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("ConfigureTask::periodic_job_i"));
	}
	
	//## Read configureStatus device attr
	//## NB: When configuration completes on sub-element this is switched to 'ON-BAND' by the event formula system
	Tango::DevShort configureStatusFlag;
	if(dev->GetDynAttrValue(configureStatusFlag,configureStatusFlag_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get dyn attr "<<configureStatusFlag_attr_name<<" value!";
		__WARN_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("ConfigureTask::periodic_job_i"));	
	}

	std::string configureStatusFlagStr= "";
	if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(configureStatusFlagStr,dev,configureStatusFlag_attr_name,configureStatusFlag)<0){
		std::stringstream ss;
		ss<<"Failed to retrieve enum label corresponding to value "<<configureStatusFlag<<" for attr "<<configureStatusFlag_attr_name<<"!";
		__WARN_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("ConfigureTask::periodic_job_i"));
	}

	//If configure is found started/running do nothing...
	if(configureStatusFlagStr==configureStatusFlag_ongoing){//RUNNING
		std::string infoMsg("Configuration task is found in RUNNING state...");
		__DEBUG_LOG(dev,infoMsg);

		//Set configuration as RUNNING
		Tango::DevShort configStatus_ongoing;
		Utils_ns::TangoUtils::GetEnumAttrValue(configStatus_ongoing,dev,configureStatus_attr_name,configureStatus_ongoing);
		dev->SetDynAttrValue<Tango::DevShort>(configStatus_ongoing,configureStatus_attr_name);

	}//close if RUNNINF

	else if(configureStatusFlagStr==configureStatusFlag_idle){//IDLE
		std::string infoMsg("Configuration task is found in IDLE state (i.e. configuration not yet started in sub-elements?) ...");
		__WARN_LOG(dev,infoMsg);
	}//close if IDLE

	else if(configureStatusFlagStr==configureStatusFlag_onband){//ON-BAND

		//## Execute post-configuration actions (e.g. Synchronize band)
		try {
			post_config_action_i();
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			std::stringstream ss;
			ss<<"Tango exception occurred while synchronizing band (post-configuration action)!";
			__ERROR_LOG(dev,ss.str());

			//Set configuration as FAILED
			Tango::DevShort configStatus_failed;
			Utils_ns::TangoUtils::GetEnumAttrValue(configStatus_failed,dev,configureStatus_attr_name,configureStatus_failed);
			dev->SetDynAttrValue<Tango::DevShort>(configStatus_failed,configureStatus_attr_name);

		}//close catch

		//## If post-actions succeeded set configuration as COMPLETED and stop periodic/timeout actions
		Tango::DevShort configStatus_completed;
		Utils_ns::TangoUtils::GetEnumAttrValue(configStatus_completed,dev,configureStatus_attr_name,configureStatus_completed);
		dev->SetDynAttrValue<Tango::DevShort>(configStatus_completed,configureStatus_attr_name);

		//## Stop periodic task
		__DEBUG_LOG(dev,"Stopping periodic configuration monitoring for "<<m_cfg.bandId<<" ...");
		this->enable_periodic_msg(false);

		//## Stop timeout task
		__DEBUG_LOG(dev,"Disabling configuration timeout for "<<m_cfg.bandId<<" ...");
		this->enable_timeout_msg(false);

		//## Set device state to ON
		{
			std::stringstream ss;
			ss<<"Dish configuration for band "<<m_cfg.bandId<<" completed with success";
			(dev->m_mutex).lock();
			dev->set_state(Tango::ON);
			dev->set_status(ss.str().c_str());
			(dev->m_mutex).unlock();
		}

	}//close else if COMPLETED
	else if(configureStatusFlagStr==configureStatusFlag_failed){//FAILED
		std::string infoMsg("Configuration task is found in FAILED state (i.e. capability became UNAVAILABLE?) ...");
		__DEBUG_LOG(dev,infoMsg);

		//Set configuration as FAILED
		Tango::DevShort configStatus_failed;
		Utils_ns::TangoUtils::GetEnumAttrValue(configStatus_failed,dev,configureStatus_attr_name,configureStatus_failed);
		dev->SetDynAttrValue<Tango::DevShort>(configStatus_failed,configureStatus_attr_name);

		//## Set device state to ALARM
		{
			std::stringstream ss;
			ss<<"Dish configuration for band "<<m_cfg.bandId<<" completed with success";
			(dev->m_mutex).lock();
			dev->set_state(Tango::ALARM);
			dev->set_status(ss.str().c_str());
			(dev->m_mutex).unlock();
		}

		//## Stop periodic task
		__DEBUG_LOG(dev,"Stopping periodic configuration monitoring for "<<m_cfg.bandId<<" ...");
		this->enable_periodic_msg(false);

		//## Stop timeout task
		__DEBUG_LOG(dev,"Disabling configuration timeout for "<<m_cfg.bandId<<" ...");
		this->enable_timeout_msg(false);

	}//close else if FAILED
	else{
		std::string infoMsg("Configuration task is found in a unexpected/unknown state!");
		__WARN_LOG(dev,infoMsg);
		
	}//close else
	

}//close periodic_job_i()

// ============================================================================
// ConfigureTask::post_config_action_i
// ============================================================================
void ConfigureTask::post_config_action_i ()
  throw (Tango::DevFailed)
{

	//## Get device instance
	DishManager* dev= static_cast<DishManager*>(m_cfg.host_device);
	if(!dev){//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("ConfigureTask::post_config_action_i"));
	}

	//## Synchronize band
	try {
		dev->synchronise();
	}
	catch(...){
		throw;
	}


}//close post_config_action_i()

// ============================================================================
// ConfigureTask::timeout_job_i
// ============================================================================
void ConfigureTask::timeout_job_i ()
  throw (Tango::DevFailed)
{

	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string configureStatus_attr_name= "configureStatus";
	std::string configureStatus_timeout= "TIMED-OUT";
	std::string configureTimestamp_attr_name= "configureTimestamp";
	std::string configureDate_attr_name= "configureDate";
	std::string configurationTime_attr_name= "configurationTime";
	//###########################################################################

	//## Get device instance
	DishManager* dev= static_cast<DishManager*>(m_cfg.host_device);
	if(!dev){//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("ConfigureTask::execute_configure_i"));
	}

	//## Stop periodic task
	__DEBUG_LOG(dev,"Stopping periodic configuration monitoring for "<<m_cfg.bandId<<" ...");
	this->enable_periodic_msg(false);

	//## Stop timeout task
	__DEBUG_LOG(dev,"Disabling configuration timeout for "<<m_cfg.bandId<<" ...");
	this->enable_timeout_msg(false);

	
	//## Set device state to ALARM
	{
		std::stringstream ss;
		ss<<"Dish configuration for band "<<m_cfg.bandId<<" timed-out!";
		(dev->m_mutex).lock();
		dev->set_state(Tango::ALARM);
		dev->set_status(ss.str().c_str());
		(dev->m_mutex).unlock();
	}

	//## Set configuration failure (timeout) in device
	__INFO_LOG(dev,"Configuration timeout for band "<<m_cfg.bandId<<" detected!");
	Tango::DevShort configStatus_timeout;
	Utils_ns::TangoUtils::GetEnumAttrValue(configStatus_timeout,dev,configureStatus_attr_name,configureStatus_timeout);
	dev->SetDynAttrValue<Tango::DevShort>(configStatus_timeout,configureStatus_attr_name);

	//## Set configuration end date
	bool isUTC= false;
	yat::Time now;
	now.set_current(isUTC);
	Tango::DevLong configure_timestamp= static_cast<Tango::DevLong>(now.long_unix());
	std::string configure_date= now.to_ISO8601_ms();

	dev->SetDynAttrValue<Tango::DevLong>(configure_timestamp,configureTimestamp_attr_name);
	dev->SetDynAttrValue<std::string>(configure_date,configureDate_attr_name);

	//## Set configuration time elapsed in ms
	double configure_time_elapsed= static_cast<double>(configure_timestamp - m_cfg.configureStartTimestamp);
	dev->SetDynAttrValue<Tango::DevDouble>(configure_time_elapsed,configurationTime_attr_name);

	//## Reset variables
	m_cfg.bandId= -1;
	m_cfg.configureStartTimestamp= -1;  
	
}//close timeout_job_i()

// ============================================================================
// ConfigureTask::execute_configure_i
// ============================================================================
void ConfigureTask::execute_configure_i (const ConfigMsg& configMsg)
  throw (Tango::DevFailed)
{

	//## Get message info
	short bandId= configMsg.bandId;
	long startTime= configMsg.startTime;
	
	//## Update task config
	m_cfg.bandId= bandId;
	m_cfg.configureStartTimestamp= startTime;  

	//## Start timeout task (CONFIGURATION TIMEOUT)
	this->enable_timeout_msg(true);

	//## Start periodic task (CONFIGURATION STATUS MONITORING)
	this->enable_periodic_msg(true);

}//close execute_configure_i()


}//close namespace

