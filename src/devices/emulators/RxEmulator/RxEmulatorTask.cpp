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
* @file RxEmulatorTask.cc
* @class RxEmulatorTask
* @brief RxEmulatorTask
*
* RxEmulatorTask
* @author S. Riggi
* @date 15/01/2017
*/


#include <RxEmulatorTask.h>
#include <RxEmulator.h>

#include <Logger.h>

// ============================================================================
// SOME USER MESSAGES AND CONSTS
// ============================================================================
#define kSTART_PERIODIC_MSG static_cast<size_t>((yat::FIRST_USER_MSG + 1000))
#define kSTOP_PERIODIC_MSG  static_cast<size_t>((yat::FIRST_USER_MSG + 1001))
#define kEXECUTE_CMD_MSG    static_cast<size_t>((yat::FIRST_USER_MSG + 1002))
#define kEXECUTE_STARTUP_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1003))
#define kEXECUTE_CONFIGURE_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1004))
//-----------------------------------------------------------------------------

namespace RxEmulator_ns {

// ============================================================================
// RxEmulatorTask::RxEmulatorTask
// ============================================================================
RxEmulatorTask::RxEmulatorTask (const Config & cfg)
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
// RxEmulatorTask::~RxEmulatorTask
// ============================================================================
RxEmulatorTask::~RxEmulatorTask () {

	//Delete wait cond
	if(m_wait_cond){
		delete m_wait_cond;
		m_wait_cond= 0;
	}

}//close destructor

// ============================================================================
// RxEmulatorTask::go
// ============================================================================
void RxEmulatorTask::go (size_t tmo_ms)
    throw (Tango::DevFailed)
{

	//- Initialize wait condition variable
	if(!m_wait_cond) m_wait_cond= new yat::Condition(m_data_lock);

  //- a timer
  yat::Timer t;

	INFO_STREAM << "RxEmulatorTask::go() - Initializing task thread..." << endl;

  //- finally call yat4tango::DeviceTask::go
  long remaining_tmo = static_cast<long>(tmo_ms) - static_cast<long>(t.elapsed_msec());
  this->yat4tango::DeviceTask::go(remaining_tmo > 1 ? remaining_tmo : 1);

  INFO_STREAM << "RxEmulatorTask::go() - End initialization" << endl;
}//close go()

// ============================================================================
// RxEmulatorTask::process_message
// ============================================================================
void RxEmulatorTask::process_message (yat::Message& msg)
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

    //- kEXECUTE_CMD_MSG ------------
    case kEXECUTE_CMD_MSG:
    {
      const std::string & msg_data = msg.get_data<std::string>();
      __DEBUG_LOG(m_cfg.host_device,"kEXECUTE_CMD_MSG - Received msg data: " << msg_data);
      this->execute_cmd_i(msg_data);
    }
		break;

    //- kEXECUTE_STARTUP_CMD_MSG ------------
    case kEXECUTE_STARTUP_CMD_MSG:
    {
			try {
				const StartupMsg& msg_data = msg.get_data<StartupMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_STARTUP_CMD_MSG - Received msg data");
				this->execute_startup_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_STARTUP_CMD_MSG - Failed to extract message data!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_STARTUP_CMD_MSG - Unknown exception while extracting message data!");
			}
    }//close case
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
// RxEmulatorTask::exit
// ============================================================================
void RxEmulatorTask::exit ()
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
// RxEmulatorTask::start_periodic_activity
// ============================================================================
void RxEmulatorTask::start_periodic_activity (size_t tmo_ms)
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
// RxEmulatorTask::periodic_job_i
// ============================================================================
void RxEmulatorTask::periodic_job_i ()
  throw (Tango::DevFailed)
{

	//...
	//...

}//close periodic_job_i()

// ============================================================================
// RxEmulatorTask::stop_periodic_activity
// ============================================================================
void RxEmulatorTask::stop_periodic_activity (size_t tmo_ms)
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
// RxEmulatorTask::execute_cmd
// ============================================================================
void RxEmulatorTask::execute_cmd (const std::string& cmd_name, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_CMD_MSG, cmd_name, 2750);
    else
      this->post(kEXECUTE_CMD_MSG, cmd_name, 2750);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_cmd_i) 
    throw;
  }
}

// ============================================================================
// RxEmulatorTask::execute_cmd_i
// ============================================================================
void RxEmulatorTask::execute_cmd_i (const std::string& cmd_name)
  throw (Tango::DevFailed)
{
	//...
	//...
}

// ============================================================================
// RxEmulatorTask::execute_startup
// ============================================================================
void RxEmulatorTask::execute_startup (const StartupMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_STARTUP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_STARTUP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_startup_i) 
    throw;
  }
}


// ============================================================================
// RxEmulatorTask::execute_startup_i
// ============================================================================
void RxEmulatorTask::execute_startup_i (const StartupMsg& msg)
  throw (Tango::DevFailed)
{

	//## Get device instance
	RxEmulator* dev= static_cast<RxEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));
	}

	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;

	//###########################################################################
	//##    HARD-CODED PARAMETERS 
	//##    (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "operatingMode";
	std::string powerState_attr_name= "powerState";
	std::vector<std::string> cap_attr_names= {
		"b1CapabilityState",
		"b2CapabilityState",
		"b3CapabilityState",
		"b4CapabilityState",
		"b5aCapabilityState",
		"b5bCapabilityState",
	};
	std::string activeBand_attr_name= "configuredBand";
	std::string mode_init_str= "STARTUP"; 
	std::string mode_str= "STANDBY-LP"; 
	std::string mode_fault_str= "ERROR"; 
	std::string powerState_str= "LOW-POWER";
	std::string capability_str= "STANDBY"; 
	std::string activeBand_str= "NONE"; 
	//=====================================================

	//Retrieve init mode value corresponding to label
	Tango::DevShort mode_init;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode_init,dev,attr_name,mode_init_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));
	}

	//Retrieve mode value corresponding to label
	Tango::DevShort mode;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,attr_name,mode_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));
	}

	//Retrieve powerState value corresponding to label
	Tango::DevShort powerState;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerState_str)<0){
		std::stringstream ss;
		ss<<"Invalid powerState enumeration given in argument (check enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));
	}

	
	//=====================================================
	//==         SET INIT POWER STATE
	//=====================================================
	//Set Rx power state to LOW-POWER
	__DEBUG_LOG(dev,"Setting power state attr ("<<powerState_attr_name<<") to "<< powerState_str<<"...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(powerState,powerState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));
	}

	//=====================================================
	//==         SET INIT MODE
	//=====================================================
	//Set Rx mode to STARTUP
	__INFO_LOG(dev,"Setting mode attr ("<<attr_name<<") to "<< mode_init_str << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode_init,attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));
	}
	
	//=====================================================
	//==         INIT ATTRS 
	//=====================================================
	//Initialize attrs to defaults
	__INFO_LOG(dev,"Initializing attrs to defaults ...");
	if(dev->InitAttrsToDefault()<0){
		std::string errMsg("Failed to initialize attributes to their default values!");
		__ERROR_LOG(dev,errMsg);
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));
	}

	//=====================================================
	//==         SIMULATE STARTUP ...
	//=====================================================
	//Simulate startup time
	__INFO_LOG(dev,"Simulate startup time (sleeping for "<<task_duration<<" s ...)");	
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit startup...");
			return;
		}
	} //- leave critical section 

	//Simulate failure?
	float random_prob= 1;
	if(enable_failure){
		std::uniform_real_distribution<float> distr(0,1);
		random_prob= distr(dev->m_randGenerator);
		
		if(random_prob<=failure_prob){//--> failure!
			std::stringstream ss;
			ss<<"Failure occurred in startup, reverting mode and capability to safe state!";
			__ERROR_LOG(dev,ss.str());

			//Set startup event attribute
			//...
			//...

			//Set mode to ERROR
			//dev->SetDynAttrValue<Tango::DevShort>(mode_fault,attr_name);
	
			//Set capability to UNAVAILABLE
			//dev->SetDynAttrValue<Tango::DevShort>(capability_fault,cap_attr_name);
	
			//Throw exception
			//THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));

			//return 
			return;
		}//close if
	}//close if
	__DEBUG_LOG(dev,"Startup completed with success");
	

	//=====================================================
	//==         SET FINAL MODE
	//=====================================================
	//Set mode to STANDBY-LP
	__INFO_LOG(dev,"Setting mode attr ("<<attr_name<<") to "<< mode_str << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode,attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));
	}

	//=====================================================
	//==         SET CAPABILITIES
	//=====================================================
	//Set capabilities to STANDBY
	
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		std::string cap_attr_name= cap_attr_names[i];	
		__INFO_LOG(dev,"Setting capability attr ("<<cap_attr_name<<") to "<< capability_str << "...");
	
		Tango::DevShort capability;
		Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_name,capability_str);
		if(dev->SetSMDynAttrValue<Tango::DevShort>(capability,cap_attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to set capability attr "<<cap_attr_name<<" to "<<capability<<"!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_startup_i"));	
		}//close if

	}//end loop capabilities

	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to ON
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		__INFO_LOG(dev,"SPFRx startup completed with success");
		dev->set_state(Tango::ON);
  	dev->set_status("SPFRx startup completed with success");	
	} //- leave critical section 
	
}//close execute_startup_i()

// ============================================================================
// RxEmulatorTask::execute_configure
// ============================================================================
void RxEmulatorTask::execute_configure (const ConfigMsg& configMsg, bool wait_cmd_reply)
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
// RxEmulatorTask::execute_configure_i
// ============================================================================
void RxEmulatorTask::execute_configure_i (const ConfigMsg& configMsg)
  throw (Tango::DevFailed)
{

	// == COMMAND BEHAVIOR ==
	// 1) Set mode to CONFIGURE 
	// 2) Stop data capture on active band and check mode (TBD)
	//    NB: This switches the active band capability from OPERATE to STANDBY if there is an already configured band
	// 3) Switch given band capability to CONFIGURE 
	// 4) Wait for a while to emulate long transition [transition may take up to 30 s]
	// 5) If successful, automatically start data capture and switch mode to DATA-CAPTURE
	// 6) If successful, switch capability of selected band to OPERATE
	// 7) Update configuredBand attr 

	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "operatingMode";
	std::vector<std::string> cap_attr_names= {
		"b1CapabilityState",
		"b2CapabilityState",
		"b3CapabilityState",
		"b4CapabilityState",
		"b5aCapabilityState",
		"b5bCapabilityState",
	};
	std::string activeBand_attr_name= "configuredBand";
	std::vector<std::string> attenuation_h_attr_names= {
		"b1PolHAttenuationDefault",
		"b2PolHAttenuationDefault",
		"b3PolHAttenuationDefault",
		"b4PolHAttenuationDefault",
		"b5aPolHAttenuationDefault",
		"b5bPolHAttenuationDefault",
	};
	std::vector<std::string> attenuation_v_attr_names= {
		"b1PolVAttenuationDefault",
		"b2PolVAttenuationDefault",
		"b3PolVAttenuationDefault",
		"b4PolVAttenuationDefault",
		"b5aPolVAttenuationDefault",
		"b5bPolVAttenuationDefault",
	};
	std::string inactiveBand_str= "NONE";
	std::string mode_str= "CONFIGURE";
	std::string mode_safe_str= "STANDBY-FP";
	std::string capability_str= "CONFIGURE";
	std::string capability_safe_str= "STANDBY";
	std::string synchronise_attr_name= "samplingClockSynchronised";
	//===========================================================================
	
	//## Get device instance
	RxEmulator* dev= static_cast<RxEmulator*>(m_cfg.host_device);
	if(!dev){
		//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}

	//## Get message info
	Tango::DevShort bandId= configMsg.bandId;
	unsigned long int task_duration= configMsg.task_duration;
	bool enable_failure= configMsg.enable_failure;
	float failure_prob= configMsg.failure_probability;

	//## Set task status to RUNNING
	{ 	
		std::stringstream ss;
		ss<<"Configure band "<<bandId<<" started";
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetConfigureStatus("RUNNING",ss.str(),true);
	}
	__INFO_LOG(dev,"Starting configuration task for band "<<bandId<<" ...");


	//=====================================================
	//==         INIT
	//=====================================================
	//Retrieve mode value corresponding to label
	Tango::DevShort mode;
	Tango::DevShort mode_safe;	
	Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,attr_name,mode_str);
	Utils_ns::TangoUtils::GetEnumAttrValue(mode_safe,dev,attr_name,mode_safe_str);

	//Retrieve capability corresponding to label
	std::string cap_attr_name= cap_attr_names[bandId-1];
	Tango::DevShort capability;
	Tango::DevShort capability_safe;
	Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_name,capability_str);
	Utils_ns::TangoUtils::GetEnumAttrValue(capability_safe,dev,cap_attr_name,capability_safe_str);

	//Retrieve default attenuations
	Tango::DevFloat attenuation_h;
	Tango::DevFloat attenuation_v;
	if(dev->GetDynAttrValue(attenuation_h,attenuation_h_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to get dyn attr "<<attenuation_h_attr_names[bandId-1]<<" value!";
		__WARN_LOG(dev,ss.str());
		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetConfigureStatus("FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}
	if(dev->GetDynAttrValue(attenuation_v,attenuation_v_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to get dyn attr "<<attenuation_v_attr_names[bandId-1]<<" value!";
		__WARN_LOG(dev,ss.str());
		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetConfigureStatus("FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}


	//Retrieve current configured band
	Tango::DevShort activeBand;
	if(dev->GetDynAttrValue(activeBand,activeBand_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<activeBand_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		
		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetConfigureStatus("FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}
	std::string activeBand_str;	
	Utils_ns::TangoUtils::GetEnumAttrLabelValue(activeBand_str,dev,activeBand_attr_name,activeBand);

	bool hasActiveBand= (activeBand_str!=inactiveBand_str);

	//=====================================================
	//==      STOP CAPTURE ON ACTIVE BAND (IF ANY)
	//=====================================================
	//Stop capture on active band
	if(hasActiveBand){
		try {
			dev->capture_data(false);
		}
		catch(Tango::DevFailed& e){
			std::stringstream ss;
			ss<<"Failed to stop data capture on active band!";
			__ERROR_LOG(dev,ss.str());

			//Set configuration fail event attribute
			{	 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetConfigureStatus("FAILED",ss.str(),true);
			}

			//Throw exception
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
		}//close catch
	}//close if hasActiveBand

	//=====================================================
	//==      SET SYNCHRONISE TO FALSE
	//=====================================================
	__INFO_LOG(dev,"Setting synchronise flag to FALSE ...");
	Tango::DevBoolean synchronised= false;
	if(dev->SetDynAttrValue<Tango::DevBoolean>(synchronised,synchronise_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<synchronise_attr_name<<" to "<<synchronised<<"!";
		__ERROR_LOG(dev,ss.str());

		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetConfigureStatus("FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}

	//=====================================================
	//==         SET MODE TO CONFIGURE
	//=====================================================
	__INFO_LOG(dev,"Setting mode to "<<mode<<" ("<<mode_str<<")...");
	if(dev->SetSMDynAttrValue<Tango::DevShort>(mode,attr_name,&mode_safe)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" to "<<mode<<"!";
		__ERROR_LOG(dev,ss.str());

		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetConfigureStatus("FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}
		
	//=====================================================
	//==         SET CAPABILITY TO CONFIGURE
	//=====================================================
	//Set capability to CONFIGURE (check if allowed and if it fails revert it to STANDBY)
	if(dev->SetSMDynAttrValue<Tango::DevShort>(capability,cap_attr_name,&capability_safe)<0){
		std::stringstream ss;
		ss<<"Failed to set capability "<<cap_attr_name<<" to "<<capability<<"!";
		__ERROR_LOG(dev,ss.str());

		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetConfigureStatus("FAILED",ss.str(),true);
		}

		//Revert mode to STANDBY-FP	
		dev->SetDynAttrValue<Tango::DevShort>(mode_safe,attr_name);
	
		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}

	//=====================================================
	//==         CONFIGURE TIME...
	//=====================================================
	//Simulate long configuring time
	__INFO_LOG(dev,"Simulating band configuration processing (sleeping for "<<task_duration<<" s...)");	
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit configuration...");

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetConfigureStatus("ABORTED","Wait condition signalled, exit configure band task...",true);
			}

			return;
		}
	} //- leave critical section 
	

	//Simulate failure?
	float random_prob= 1;
	if(enable_failure){
		std::uniform_real_distribution<float> distr(0,1);
		random_prob= distr(dev->m_randGenerator);
		
		if(random_prob<=failure_prob){//--> failure!
			std::stringstream ss;
			ss<<"Failure occurred in configuration, reverting mode and capability to safe state!";
			__ERROR_LOG(dev,ss.str());

			//Set configuration fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);				
				dev->SetConfigureStatus("FAILED","Configure band command failed in execution",true);
			}

			//Revert mode to STANDBY-FP	
			dev->SetDynAttrValue<Tango::DevShort>(mode_safe,attr_name);
	
			//Revert capability to STANDBY
			dev->SetDynAttrValue<Tango::DevShort>(capability_safe,cap_attr_name);
	
			//Throw exception
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
		}//close if
	}//close if
	
	__INFO_LOG(dev,"Configure band completed with success for bandId "<<bandId<<", automatically start data capture...");
	
	//=====================================================
	//==         SET ATTENUATION
	//=====================================================
	//Set attenuations for configured band
	__INFO_LOG(dev,"Setting current attenuation for H/V channels to default value of band "<<bandId<<" ...");
	if(dev->SetDynAttrValue<Tango::DevFloat>(attenuation_h,attenuation_h_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set h-channel attenuation attr "<<attenuation_h_attr_names[bandId-1]<<" to "<<attenuation_h<<"!";
		__ERROR_LOG(dev,ss.str());

		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);				
			dev->SetConfigureStatus("FAILED","Configure band command failed in execution",true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}
	if(dev->SetDynAttrValue<Tango::DevFloat>(attenuation_v,attenuation_v_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set v-channel attenuation attr "<<attenuation_v_attr_names[bandId-1]<<" to "<<attenuation_v<<"!";
		__ERROR_LOG(dev,ss.str());
	
		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);				
			dev->SetConfigureStatus("FAILED","Configure band command failed in execution",true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}

	//=====================================================
	//==        UPDATE SELECTED BAND (DO BEFORE START CAPTURE)
	//=====================================================
	//Set current band attr to the selected (MISSING IN ICD!!!)
	if(dev->SetDynAttrValue<Tango::DevShort>(bandId,activeBand_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<activeBand_attr_name<<" to "<<bandId<<"!";
		__ERROR_LOG(dev,ss.str());

		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetConfigureStatus("FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}

	//=====================================================
	//==      START CAPTURE ON SELECTED BAND
	//=====================================================
	//Start data capture: if successful mode=DATA-CAPTURE, cap=OPERATE
	try {
		dev->capture_data(true);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		std::stringstream ss;
		ss<<"Failed to stop data capture on selected band!";
		__ERROR_LOG(dev,ss.str());

		//Revert mode to STANDBY-FP	(already done in capture)
		dev->SetDynAttrValue<Tango::DevShort>(mode_safe,attr_name);
	
		//Revert capability to STANDBY (already done in capture)
		dev->SetDynAttrValue<Tango::DevShort>(capability_safe,cap_attr_name);

		//Set configuration fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetConfigureStatus("FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("RxEmulatorTask::execute_configure_i"));
	}//close catch

	

	//=====================================================
	//==         SET TASK COMPLETED
	//=====================================================
	{ //- enter critical section
		omni_mutex_lock guard(dev->m_mutex);
		__INFO_LOG(dev,"Configure band task completed with success ...");
		dev->SetConfigureStatus("COMPLETED","Configure band completed with success",true);
	} //- leave critical section 
	


}//close execute_configure_i()


}//close namespace

