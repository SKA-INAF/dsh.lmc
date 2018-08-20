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
* @file DSEmulatorTask.cc
* @class DSEmulatorTask
* @brief DSEmulatorTask
*
* DSEmulatorTask
* @author S. Riggi
* @date 15/01/2017
*/


#include "DSEmulatorTask.h"
#include "DSEmulator.h"
#include "TrackingThread.h"

#include <Logger.h>

// ============================================================================
// SOME USER MESSAGES AND CONSTS
// ============================================================================
#define kSTART_PERIODIC_MSG static_cast<size_t>((yat::FIRST_USER_MSG + 1000))
#define kSTOP_PERIODIC_MSG  static_cast<size_t>((yat::FIRST_USER_MSG + 1001))
#define kEXECUTE_CMD_MSG    static_cast<size_t>((yat::FIRST_USER_MSG + 1002))
#define kEXECUTE_STARTUP_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1003))
#define kEXECUTE_STOW_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1004))
#define kEXECUTE_MOVE_INDEXER_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1005))

#define kEXECUTE_SET_STANDBY_LP_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1006))
#define kEXECUTE_SET_STANDBY_FP_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1007))
#define kEXECUTE_SET_POINT_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1008))
#define kEXECUTE_SHUTDOWN_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1009))
#define kEXECUTE_STOW_SIMPLE_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1010))
//-----------------------------------------------------------------------------

namespace DSEmulator_ns {

// ============================================================================
// DSEmulatorTask::DSEmulatorTask
// ============================================================================
DSEmulatorTask::DSEmulatorTask (const Config & cfg)
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
// DSEmulatorTask::~DSEmulatorTask
// ============================================================================
DSEmulatorTask::~DSEmulatorTask () {

	//Delete wait cond
	if(m_wait_cond){
		delete m_wait_cond;
		m_wait_cond= 0;
	}

}//close destructor

// ============================================================================
// DSEmulatorTask::go
// ============================================================================
void DSEmulatorTask::go (size_t tmo_ms)
    throw (Tango::DevFailed)
{

	//- Initialize wait condition variable
	if(!m_wait_cond) m_wait_cond= new yat::Condition(m_data_lock);

  //- a timer
  yat::Timer t;

	INFO_STREAM << "DSEmulatorTask::go() - Initializing task thread..." << endl;

  //- finally call yat4tango::DeviceTask::go
  long remaining_tmo = static_cast<long>(tmo_ms) - static_cast<long>(t.elapsed_msec());
  this->yat4tango::DeviceTask::go(remaining_tmo > 1 ? remaining_tmo : 1);

  INFO_STREAM << "DSEmulatorTask::go() - End initialization" << endl;
}//close go()

// ============================================================================
// DSEmulatorTask::process_message
// ============================================================================
void DSEmulatorTask::process_message (yat::Message& msg)
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

		//- kEXECUTE_STOW_CMD_MSG ------------
    case kEXECUTE_STOW_CMD_MSG:
    {
			try {
				const StowMsg& msg_data = msg.get_data<StowMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_STOW_CMD_MSG - Received msg data");
				this->execute_stow_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_STOW_CMD_MSG - Failed to extract message data!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_STOW_CMD_MSG - Unknown exception while extracting message data!");
			}
    }//close case
		break;

		//- kEXECUTE_STOW_SIMPLE_CMD_MSG ------------
    case kEXECUTE_STOW_SIMPLE_CMD_MSG:
    {
			try {
				const StowMsg& msg_data = msg.get_data<StowMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_STOW_SIMPLE_CMD_MSG - Received msg data");
				this->execute_stow_simple_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_STOW_SIMPLE_CMD_MSG - Failed to extract message data!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_STOW_SIMPLE_CMD_MSG - Unknown exception while extracting message data!");
			}
    }//close case
		break;

		//- kEXECUTE_MOVE_INDEXER_CMD_MSG ------------
    case kEXECUTE_MOVE_INDEXER_CMD_MSG:
    {
			try {
				const MoveIndexerMsg& msg_data = msg.get_data<MoveIndexerMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_MOVE_INDEXER_CMD_MSG - Received msg data");
				this->execute_move_indexer_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_MOVE_INDEXER_CMD_MSG - Failed to extract message data!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_MOVE_INDEXER_CMD_MSG - Unknown exception while extracting message data!");
			}
    }//close case
		break;


		//- kEXECUTE_SET_STANDBY_LP_CMD_MSG ------------
    case kEXECUTE_SET_STANDBY_LP_CMD_MSG:
    {
			try {
				const StandbyLPMsg& msg_data = msg.get_data<StandbyLPMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_SET_STANDBY_LP_CMD_MSG - Received msg data");
				this->execute_set_standby_lp_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_SET_STANDBY_LP_CMD_MSG - Failed to extract message data!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_SET_STANDBY_LP_CMD_MSG - Unknown exception while extracting message data!");
			}
    }//close case
		break;


		//- kEXECUTE_SET_STANDBY_FP_CMD_MSG ------------
    case kEXECUTE_SET_STANDBY_FP_CMD_MSG:
    {
			try {
				const StandbyFPMsg& msg_data = msg.get_data<StandbyFPMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_SET_STANDBY_FP_CMD_MSG - Received msg data");
				this->execute_set_standby_fp_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_SET_STANDBY_FP_CMD_MSG - Failed to extract message data!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_SET_STANDBY_FP_CMD_MSG - Unknown exception while extracting message data!");
			}
    }//close case
		break;

		//- kEXECUTE_SET_POINT_CMD_MSG ------------
    case kEXECUTE_SET_POINT_CMD_MSG:
    {
			try {
				const PointMsg& msg_data = msg.get_data<PointMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_SET_POINT_CMD_MSG - Received msg data");
				this->execute_set_point_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_SET_POINT_CMD_MSG - Failed to extract message data!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_SET_POINT_CMD_MSG - Unknown exception while extracting message data!");
			}
    }//close case
		break;

		//- kEXECUTE_SHUTDOWN_CMD_MSG ------------
    case kEXECUTE_SHUTDOWN_CMD_MSG:
    {
			try {
				const ShutdownMsg& msg_data = msg.get_data<ShutdownMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_SHUTDOWN_CMD_MSG - Received msg data");
				this->execute_shutdown_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_SHUTDOWN_CMD_MSG - Failed to extract message data!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_SHUTDOWN_CMD_MSG - Unknown exception while extracting message data!");
			}
    }//close case
		break;


    //- UNHANDLED MSG --------------------
		default:
		  __DEBUG_LOG(m_cfg.host_device,"Unhandled msg type received");
			break;
  }//close switch

}//close process_message()


// ============================================================================
// DSEmulatorTask::exit
// ============================================================================
void DSEmulatorTask::exit ()
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
// DSEmulatorTask::start_periodic_activity
// ============================================================================
void DSEmulatorTask::start_periodic_activity (size_t tmo_ms)
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
// DSEmulatorTask::periodic_job_i
// ============================================================================
void DSEmulatorTask::periodic_job_i ()
  throw (Tango::DevFailed)
{

	//...
	//...

}//close periodic_job_i()

// ============================================================================
// DSEmulatorTask::stop_periodic_activity
// ============================================================================
void DSEmulatorTask::stop_periodic_activity (size_t tmo_ms)
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
// DSEmulatorTask::execute_cmd
// ============================================================================
void DSEmulatorTask::execute_cmd (const std::string& cmd_name, bool wait_cmd_reply)
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
// DSEmulatorTask::execute_cmd_i
// ============================================================================
void DSEmulatorTask::execute_cmd_i (const std::string& cmd_name)
  throw (Tango::DevFailed)
{
	//...
	//...
}

// ============================================================================
// DSEmulatorTask::execute_startup
// ============================================================================
void DSEmulatorTask::execute_startup (const StartupMsg& msg, bool wait_cmd_reply)
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
// DSEmulatorTask::execute_startup_i
// ============================================================================
void DSEmulatorTask::execute_startup_i (const StartupMsg& msg)
  throw (Tango::DevFailed)
{

	//## Get device instance
	DSEmulator* dev= static_cast<DSEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
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
	std::string mode_init_str= "STARTUP"; 
	std::string mode_str= "STANDBY-LP";
	std::string mode_fault_str= "ESTOP"; 
	std::string powerState_str= "LOW-POWER";
	std::string pntModelParams_attr_name= "pointingModelParams";
	//=====================================================

	//Retrieve init mode value corresponding to label
	Tango::DevShort mode_init;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode_init,dev,attr_name,mode_init_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
	}

	//Retrieve mode value corresponding to label
	Tango::DevShort mode;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,attr_name,mode_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
	}

	//Retrieve mode value corresponding to label
	Tango::DevShort mode_fault;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode_fault,dev,attr_name,mode_fault_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
	}

	//Retrieve powerState value corresponding to label
	Tango::DevShort powerState;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerState_str)<0){
		std::stringstream ss;
		ss<<"Invalid powerState enumeration given in argument (check enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
	}


	//=====================================================
	//==         SET INIT POWER STATE
	//=====================================================
	//Set DS power state to LOW-POWER
	__DEBUG_LOG(dev,"Setting power state attr ("<<powerState_attr_name<<") to "<< powerState_str<<"...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(powerState,powerState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
	}

	//=====================================================
	//==         SET INIT MODE
	//=====================================================
	//Set DS mode to STARTUP
	__INFO_LOG(dev,"Setting mode attr ("<<attr_name<<") to "<< mode_init_str << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode_init,attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
	}


	//=====================================================
	//==         INIT PNT MODEL PARS
	//=====================================================
	__DEBUG_LOG(dev,"Initializing pnt model pars to defaults...");
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		if( dev->SetDynAttrValue<Tango::DevFloat>(dev->pointingModelParamsDefault,pntModelParams_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<pntModelParams_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
		}
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

			//Set configuration fail event attribute
			//...
			//...

			//Set mode to ERROR
			dev->SetDynAttrValue<Tango::DevShort>(mode_fault,attr_name);
	
			//Set capability to UNAVAILABLE
			//dev->SetDynAttrValue<Tango::DevShort>(capability_fault,cap_attr_name);
	
			//Throw exception
			//THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));

			//return 
			return;
		}//close if
	}//close if
	__DEBUG_LOG(dev,"Startup completed with success");
	

	//=====================================================
	//==         SET FINAL MODE
	//=====================================================
	//Set mode to STANDBY-LP
	__DEBUG_LOG(dev,"Setting mode attr ("<<attr_name<<") to "<< mode_str << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode,attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_startup_i"));
	}


	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to STANDBY
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		__DEBUG_LOG(dev,"Startup completed with success");
		dev->set_state(Tango::STANDBY);
  	dev->set_status("DS device initialization completed");	
	} //- leave critical section 
	
}//close execute_startup_i()



// ============================================================================
// DSEmulatorTask::execute_stow_simple
// ============================================================================
void DSEmulatorTask::execute_stow_simple (const StowMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_STOW_SIMPLE_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_STOW_SIMPLE_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
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
// DSEmulatorTask::execute_stow_simple_i
// ============================================================================
void DSEmulatorTask::execute_stow_simple_i (const StowMsg& msg)
  throw (Tango::DevFailed)
{

	//## Get device instance
	DSEmulator* dev= static_cast<DSEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//Set status to RUNNING
	__INFO_LOG(dev,"Stowing started");
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetStowStatus("RUNNING","Stowing started",true);
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
	std::string mode_str= "STOW"; 
	std::string mode_fault_str= "ESTOP"; 
	std::string powerState_str= "LOW-POWER";
	std::string powerState_full_str= "FULL-POWER";
	//=====================================================


	//Retrieve mode value corresponding to label
	Tango::DevShort mode;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,attr_name,mode_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());

		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStowStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//Retrieve mode value corresponding to label
	Tango::DevShort mode_fault;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode_fault,dev,attr_name,mode_fault_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());

		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStowStatus("FAILED",ss.str(),true);
		}
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//Retrieve powerState value corresponding to label
	Tango::DevShort powerState;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerState_str)<0){
		std::stringstream ss;
		ss<<"Invalid powerState enumeration given in argument (check enum values)!";
		__ERROR_LOG(dev,ss.str());

		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStowStatus("FAILED",ss.str(),true);
		}
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//=====================================================
	//==         SIMULATE STOWING MOVEMENT ...
	//=====================================================
	//Simulate stowing time
	__INFO_LOG(dev,"Simulate stowing time (sleeping for "<<task_duration<<" s ...");	
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit stowing task...");

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStowStatus("ABORTED","Wait condition signalled, exit stowing task...",true);
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
			ss<<"Failure occurred in stowing, reverting mode to safe state!";
			__ERROR_LOG(dev,ss.str());

			//Set stow fail event attribute	
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStowStatus("FAILED","Stow command failed in execution",true);
			}

			//Set mode to ERROR
			dev->SetDynAttrValue<Tango::DevShort>(mode_fault,attr_name);
	
			//Throw exception
			//THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));

			//return 
			return;
		}//close if
	}//close if
	__DEBUG_LOG(dev,"Stow completed with success");

	//=====================================================
	//==         SET MODE
	//=====================================================
	//Set mode to STOW
	__DEBUG_LOG(dev,"Setting mode attr ("<<attr_name<<") to "<< mode_str << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode,attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStowStatus("FAILED","Stow command failed in execution (cannot set mode to STOW)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//=====================================================
	//==         SET POWER STATE
	//=====================================================
	//Retrieve current powerState attr value
	Tango::DevShort current_powerState;
	if( dev->GetDynAttrValue(current_powerState,powerState_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStowStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	std::string current_powerState_str= "";
	if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(current_powerState_str,dev,powerState_attr_name,current_powerState)<0){
		std::stringstream ss;
		ss<<"Failed to get enum label for "<<powerState_attr_name<<" corresponding to value "<<current_powerState<<"!";
		__ERROR_LOG(dev,ss.str());

		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStowStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//If powerState=FULL-POWER set DS power state to LOW-POWER
	if(current_powerState_str==powerState_full_str){
		__DEBUG_LOG(dev,"Setting power state attr ("<<powerState_attr_name<<") to "<< powerState_str<<"...");
		if( dev->SetSMDynAttrValue<Tango::DevEnum>(powerState,powerState_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStowStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
		}
	}//close if

	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to DISABLE and set status to COMPLETED
	{ //- enter critical section
		//yat::AutoMutex<> guard(m_data_lock);
		omni_mutex_lock guard(dev->m_mutex);
		__INFO_LOG(dev,"Stowing completed with success");
		dev->set_state(Tango::DISABLE);
  	dev->set_status("DS stow completed");	

		dev->SetStowStatus("COMPLETED","Stow completed with success",true);

	} //- leave critical section 
	

}//close execute_stow_i()



// ============================================================================
// DSEmulatorTask::execute_move_indexer
// ============================================================================
void DSEmulatorTask::execute_move_indexer (const MoveIndexerMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_MOVE_INDEXER_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_MOVE_INDEXER_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
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
// DSEmulatorTask::execute_move_indexer_i
// ============================================================================
void DSEmulatorTask::execute_move_indexer_i (const MoveIndexerMsg& msg)
  throw (Tango::DevFailed)
{

	//## Get device instance
	DSEmulator* dev= static_cast<DSEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_move_indexer_i"));
	}

	//Set status to RUNNING
	__INFO_LOG(dev,"Indexing started");
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetIndexingStatus("RUNNING","Indexing started",true);
	}

	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	short indexer_pos= msg.indexer_position;

	//###########################################################################
	//##    HARD-CODED PARAMETERS 
	//##    (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "indexerPosition";
	std::string indexerPosStr_moving= "MOVING";
	//=====================================================

	//Retrieve indexer position label
	std::string indexerPos_str= ""; 
	if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(indexerPos_str,dev,attr_name,indexer_pos)<0){
		std::stringstream ss;
		ss<<"Failed to get enum label for "<<attr_name<<" corresponding to value "<<indexer_pos<<"!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetIndexingStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_move_indexer_i"));
	}

	//Retrieve indexer position corresponding to MOVING
	Tango::DevShort indexerPos_moving;
	Utils_ns::TangoUtils::GetEnumAttrValue(indexerPos_moving,dev,attr_name,indexerPosStr_moving);

	//=====================================================
	//==         SIMULATE INDEXER MOVEMENT ...
	//=====================================================

	//## Set device state to MOVING
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		dev->set_state(Tango::MOVING);
  	dev->set_status("Moving indexer");	
	} //- leave critical section 
	
	//Set indexer position to MOVING
	__DEBUG_LOG(dev,"Setting attr ("<<attr_name<<") to "<< indexerPosStr_moving << "...");
	if( dev->SetDynAttrValue<Tango::DevEnum>(indexerPos_moving,attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetIndexingStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_move_indexer_i"));
	}

	//For simplicity take a random number from 1 to maximum indexing time
	std::uniform_int_distribution<int> distr(1,task_duration);
	int indexing_duration= distr(dev->m_randGenerator);

	__INFO_LOG(dev,"Simulate indexer movement time ... sleep ...");	
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(indexing_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit startup...");

			//Set ABORTED status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetIndexingStatus("ABORTED","Wait condition signalled, exit indexing task...",true);
			}

			return;
		}
	} //- leave critical section 

	//Simulate failure?
	//...
	//...

	
	//=====================================================
	//==         SET INDEXER POSITION TO BAND POS
	//=====================================================
	//Set indexer position 
	__DEBUG_LOG(dev,"Setting attr ("<<attr_name<<") to "<< indexerPos_str << "...");
	if( dev->SetDynAttrValue<Tango::DevEnum>(indexer_pos,attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetIndexingStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_move_indexer_i"));
	}

	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	{ //- enter critical section
		omni_mutex_lock guard(dev->m_mutex);
		std::stringstream ss;
		ss<<"Indexing to band "<<indexer_pos<<" completed with success";
		__INFO_LOG(dev,ss.str());
  	dev->set_status(ss.str().c_str());	
		dev->SetIndexingStatus("COMPLETED",ss.str(),true);
	} //- leave critical section 
	
}//close execute_move_indexer_i()




// ============================================================================
// DSEmulatorTask::execute_set_standby_lp
// ============================================================================
void DSEmulatorTask::execute_set_standby_lp (const StandbyLPMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_SET_STANDBY_LP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_SET_STANDBY_LP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
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
// DSEmulatorTask::execute_set_standby_lp_i
// ============================================================================
void DSEmulatorTask::execute_set_standby_lp_i (const StandbyLPMsg& msg)
  throw (Tango::DevFailed)
{

	//==========================================
	//Command behavior
	//1) Disable tracking
	//2) Apply brakes
	//3) Disable servo axis
	//4) Set pointing state to NONE
	//4) Set power state to LOW-POWER (if not in UPS)
	//5) Set mode to STANDBY-LP
	//==========================================

	//## Get device instance
	DSEmulator* dev= static_cast<DSEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
	}

	
	
	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	bool checkPreConditions= msg.checkPreConditions;
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS 
	//##    (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string cmd_name= "SetStandbyLPMode";
	std::string mode_attr_name= "operatingMode";
	std::string modeStr= "STANDBY-LP";
	std::string powerState_attr_name= "powerState";
	std::string powerStateStr= "LOW-POWER";
	std::string upsPowerStateStr= "UPS";
	std::string pointingState_attr_name= "pointingState";
	std::string pointingStateStr= "NONE";
	std::vector<std::string> brakeState_attr_names {
		"indexerBrakeState",
		"azimuthBrakeState",
		"elevationBrakeState"
	};
	std::vector<std::string> servoState_attr_names {
		"indexerServoState",
		"azimuthServoState",
		"elevationServoState"
	};
	//=====================================================

	//Retrieve mode corresponding to STANDBY-LP
	Tango::DevShort mode;
	Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,mode_attr_name,modeStr);
	
	//Retrieve power corresponding to LOW-POWER
	Tango::DevShort powerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerStateStr);

	//Retrieve power corresponding to UPS
	Tango::DevShort upsPowerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(upsPowerState,dev,powerState_attr_name,upsPowerStateStr);

	//Retrieve pointing state corresponding to NONE
	Tango::DevShort pointingState;
	Utils_ns::TangoUtils::GetEnumAttrValue(pointingState,dev,pointingState_attr_name,pointingStateStr);


	//=====================================================
	//==         CHECK PRE-CONDITIONS?
	//=====================================================
	if(checkPreConditions){
		//## Check if a previous task is already running
		__DEBUG_LOG(dev,"Check if a previous SetStandbyLPMode task is already running...");
		bool taskAlreadyRunning= false;
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			taskAlreadyRunning= (strcmp(dev->attr_setStandbyLPModeProgress_read[0],"RUNNING")==0  || strcmp(dev->attr_setStandbyLPModeProgress_read[0],"IDLE")==0);
		}	

		if(taskAlreadyRunning){
			std::string errMsg("Another SetStandbyLPMode task is running or waiting for execution in device!");
			__INFO_LOG(dev,errMsg);
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
		}

		//## Set status to RUNNING
		__INFO_LOG(dev,"SetStandbyLPMode task started");
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStandbyLPStatus("RUNNING","SetStandbyLPMode task started",true);
		}

		//## Check STATE-MACHINE
		__DEBUG_LOG(dev,"Check if cmd "<<cmd_name<<" is allowed...");
		bool check_cmd= true;
		try {
			dev->IsCmdAllowed(cmd_name,check_cmd);
		}
		catch(Tango::DevFailed& e){
			std::string errMsg("SetStandbyLPMode is not allowed due to state machine restrictions!");
			__ERROR_LOG(dev,errMsg);

			//Set ABORTED status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("ABORTED",errMsg,true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
		}//close catch

		//## Check if STANDBY-LP mode is already active
		Tango::DevShort mode_current;	
		if(dev->GetDynAttrValue(mode_current,mode_attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<mode_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set ABORTED status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("ABORTED",ss.str(),true);
			}	

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
		}//close if

		if(mode==mode_current){
			std::stringstream ss;	
			ss<<"Mode "<<modeStr<<" already active, nothing to be done";
			__INFO_LOG(dev,ss.str());

			//Set status to COMPLETED
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("COMPLETED",ss.str(),true);
			}

			return;
		}//close if

	}//close check pre-conditions


	//## Set status to RUNNING
	__INFO_LOG(dev,"SetStandbyLPMode task started");
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetStandbyLPStatus("RUNNING","SetStandbyLPMode task started",true);
	}


	//=====================================================
	//==         SIMULATE SET STANDBY-LP TIME ...
	//=====================================================

	//For simplicity take a random number from 1 to maximum 
	std::uniform_int_distribution<int> distr(1,task_duration);
	int duration= distr(dev->m_randGenerator);

	__INFO_LOG(dev,"Simulate SetStandbyLPMode command time ... sleep for "<<duration<<" seconds ...");	
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit startup...");

			//Set ABORTED status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("ABORTED","Wait condition signalled, exit SetStandbyLPMode task...",true);
			}

			return;
		}
	} //- leave critical section 

	//Simulate failure?
	//...
	//...

	//=====================================================
	//==         DISABLE TRACKING
	//=====================================================
	//{ 	
	//	omni_mutex_lock guard(dev->m_mutex);
		dev->DisableTracking();
	//}

	//=====================================================
	//==         APPLY BRAKES
	//=====================================================
	//Apply brakes
	Tango::DevBoolean brakeState= true;
 	for(size_t i=0;i<brakeState_attr_names.size();i++){
		__DEBUG_LOG(dev,"Setting attr ("<<brakeState_attr_names[i]<<") to "<< brakeState << "...");
		if( dev->SetDynAttrValue<Tango::DevBoolean>(brakeState,brakeState_attr_names[i])<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<brakeState_attr_names[i]<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
		}
	}//end loop attrs


	//=====================================================
	//==         DISABLE SERVO AXIS
	//=====================================================
	//Disable servo axis
	Tango::DevBoolean servoState= false;
 	for(size_t i=0;i<servoState_attr_names.size();i++){
		__DEBUG_LOG(dev,"Setting attr ("<<servoState_attr_names[i]<<") to "<< servoState << "...");
		if( dev->SetDynAttrValue<Tango::DevBoolean>(servoState,servoState_attr_names[i])<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<servoState_attr_names[i]<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
		}
	}//end loop attrs

	//=====================================================
	//==         SET POWER STATE
	//=====================================================
	//Get current power state	
	Tango::DevShort powerState_current;
	if( dev->GetDynAttrValue<Tango::DevEnum>(powerState_current,powerState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to get attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStandbyLPStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
	}

	//Set power state (only if not in UPS)
	if(powerState_current!=upsPowerState){
		__DEBUG_LOG(dev,"Setting attr ("<<powerState_attr_name<<") to "<< powerStateStr << "...");
		if( dev->SetSMDynAttrValue<Tango::DevEnum>(powerState,powerState_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
		}
	}//close if

	//=====================================================
	//==         SET POINTING STATE
	//=====================================================
	//Set pointing state
	__DEBUG_LOG(dev,"Setting attr ("<<pointingState_attr_name<<") to "<< pointingStateStr << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(pointingState,pointingState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<pointingState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStandbyLPStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
	}

	//=====================================================
	//==         SET MODE
	//=====================================================
	//Set mode
	__DEBUG_LOG(dev,"Setting attr ("<<mode_attr_name<<") to "<< modeStr << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode,mode_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<mode_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStandbyLPStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_lp_i"));
	}

	
	//=====================================================
	//==         UPDATE DEVICE STATE/STATUS
	//=====================================================
	//## Set device state to STANDBY
	{ //- enter critical section
		omni_mutex_lock guard(dev->m_mutex);
		std::string msg("SetStandbyLPMode completed with success");
		__INFO_LOG(dev,msg);
		dev->set_state(Tango::STANDBY);
  	dev->set_status(msg.c_str());	
		dev->SetStandbyLPStatus("COMPLETED",msg,true);
	} //- leave critical section 
	

}//close execute_set_standby_lp_i()


// ============================================================================
// DSEmulatorTask::execute_set_standby_fp
// ============================================================================
void DSEmulatorTask::execute_set_standby_fp (const StandbyFPMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_SET_STANDBY_FP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_SET_STANDBY_FP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
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
// DSEmulatorTask::execute_set_standby_fp_i
// ============================================================================
void DSEmulatorTask::execute_set_standby_fp_i (const StandbyFPMsg& msg)
  throw (Tango::DevFailed)
{

	//## Get device instance
	DSEmulator* dev= static_cast<DSEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_set_standby_fp_i"));
	}

	//Set status to RUNNING
	__INFO_LOG(dev,"SetStandbyFPMode task started");
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetStandbyFPStatus("RUNNING","SetStandbyFPMode task started",true);
	}

	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS 
	//##    (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string mode_attr_name= "operatingMode";
	std::string modeStr= "STANDBY-FP";
	std::string powerState_attr_name= "powerState";
	std::string powerStateStr= "FULL-POWER";
	std::string upsPowerStateStr= "UPS";
	std::string pointingState_attr_name= "pointingState";
	std::string pointingStateStr= "NONE";
	std::vector<std::string> brakeState_attr_names {
		"indexerBrakeState",
		"azimuthBrakeState",
		"elevationBrakeState"
	};
	std::vector<std::string> servoState_attr_names {
		"indexerServoState",
		"azimuthServoState",
		"elevationServoState"
	};
	//=====================================================

	//Retrieve mode corresponding to STANDBY-FP
	Tango::DevShort mode;
	Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,mode_attr_name,modeStr);
	
	//Retrieve power corresponding to FULL-POWER
	Tango::DevShort powerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerStateStr);

	//Retrieve power corresponding to UPS
	Tango::DevShort upsPowerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(upsPowerState,dev,powerState_attr_name,upsPowerStateStr);

	//Retrieve pointing state corresponding to NONE
	Tango::DevShort pointingState;
	Utils_ns::TangoUtils::GetEnumAttrValue(pointingState,dev,pointingState_attr_name,pointingStateStr);


	//=====================================================
	//==         SIMULATE SET STANDBY-FP TIME ...
	//=====================================================

	//For simplicity take a random number from 1 to maximum 
	std::uniform_int_distribution<int> distr(1,task_duration);
	int duration= distr(dev->m_randGenerator);

	__INFO_LOG(dev,"Simulate SetStandbyFPMode command time ... sleep for "<<duration<<" seconds ...");	
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit startup...");

			//Set ABORTED status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyFPStatus("ABORTED","Wait condition signalled, exit SetStandbyFPMode task...",true);
			}

			return;
		}
	} //- leave critical section 

	//Simulate failure?
	//...
	//...

	//=====================================================
	//==         DISABLE TRACKING
	//=====================================================
	//{ 	
	//	omni_mutex_lock guard(dev->m_mutex);
		dev->DisableTracking();
	//}

	//=====================================================
	//==         APPLY BRAKES
	//=====================================================
	//Apply brakes
	Tango::DevBoolean brakeState= true;
 	for(size_t i=0;i<brakeState_attr_names.size();i++){
		__DEBUG_LOG(dev,"Setting attr ("<<brakeState_attr_names[i]<<") to "<< brakeState << "...");
		if( dev->SetDynAttrValue<Tango::DevBoolean>(brakeState,brakeState_attr_names[i])<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<brakeState_attr_names[i]<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyFPStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_fp_i"));
		}
	}//end loop attrs


	//=====================================================
	//==         ENABLE SERVO AXIS
	//=====================================================
	//Disable servo axis
	Tango::DevBoolean servoState= true;
 	for(size_t i=0;i<servoState_attr_names.size();i++){
		__DEBUG_LOG(dev,"Setting attr ("<<servoState_attr_names[i]<<") to "<< servoState << "...");
		if( dev->SetDynAttrValue<Tango::DevBoolean>(servoState,servoState_attr_names[i])<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<servoState_attr_names[i]<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyFPStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_fp_i"));
		}
	}//end loop attrs

	//=====================================================
	//==         SET POWER STATE
	//=====================================================
	//Get current power state	
	Tango::DevShort powerState_current;
	if( dev->GetDynAttrValue<Tango::DevEnum>(powerState_current,powerState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to get attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStandbyFPStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_fp_i"));
	}

	//Set power state (only if not in UPS)
	if(powerState_current!=upsPowerState){
		__DEBUG_LOG(dev,"Setting attr ("<<powerState_attr_name<<") to "<< powerStateStr << "...");
		if( dev->SetSMDynAttrValue<Tango::DevEnum>(powerState,powerState_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyFPStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_fp_i"));
		}
	}//close if

	//=====================================================
	//==         SET POINTING STATE
	//=====================================================
	//Set pointing state
	__DEBUG_LOG(dev,"Setting attr ("<<pointingState_attr_name<<") to "<< pointingStateStr << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(pointingState,pointingState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<pointingState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStandbyFPStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_fp_i"));
	}

	//=====================================================
	//==         SET MODE
	//=====================================================
	//Set mode
	__DEBUG_LOG(dev,"Setting attr ("<<mode_attr_name<<") to "<< modeStr << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode,mode_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<mode_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetStandbyFPStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_standby_fp_i"));
	}

	
	//=====================================================
	//==         UPDATE DEVICE STATE/STATUS
	//=====================================================
	//## Set device state to ON
	{ //- enter critical section
		omni_mutex_lock guard(dev->m_mutex);
		std::string msg("SetStandbyFPMode completed with success");
		__INFO_LOG(dev,msg);
		dev->set_state(Tango::ON);
  	dev->set_status(msg.c_str());	
		dev->SetStandbyFPStatus("COMPLETED",msg,true);
	} //- leave critical section 
	


}//close execute_set_standby_fp_i()


// ============================================================================
// DSEmulatorTask::execute_set_point
// ============================================================================
void DSEmulatorTask::execute_set_point (const PointMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_SET_POINT_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_SET_POINT_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
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
// DSEmulatorTask::execute_set_point_i
// ============================================================================
void DSEmulatorTask::execute_set_point_i (const PointMsg& msg)
  throw (Tango::DevFailed)
{

	//## Get device instance
	DSEmulator* dev= static_cast<DSEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_set_point_i"));
	}

	//Set status to RUNNING
	__INFO_LOG(dev,"SetPointMode task started");
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetPointStatus("RUNNING","SetPointMode task started",true);
	}

	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS 
	//##    (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string mode_attr_name= "operatingMode";
	std::string modeStr= "POINT";
	std::string powerState_attr_name= "powerState";
	std::string powerStateStr= "FULL-POWER";	
	std::string upsPowerStateStr= "UPS";
	std::string pointingState_attr_name= "pointingState";
	std::string pointingStateStr= "READY";
	std::vector<std::string> brakeState_attr_names {
		"indexerBrakeState",
		"azimuthBrakeState",
		"elevationBrakeState"
	};
	std::vector<std::string> servoState_attr_names {
		"indexerServoState",
		"azimuthServoState",
		"elevationServoState"
	};
	//=====================================================

	//Retrieve mode corresponding to POINT
	Tango::DevShort mode;
	Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,mode_attr_name,modeStr);
	
	//Retrieve power corresponding to FULL-POWER
	Tango::DevShort powerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerStateStr);

	//Retrieve power corresponding to UPS
	Tango::DevShort upsPowerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(upsPowerState,dev,powerState_attr_name,upsPowerStateStr);

	//Retrieve pointing state corresponding to READY
	Tango::DevShort pointingState;
	Utils_ns::TangoUtils::GetEnumAttrValue(pointingState,dev,pointingState_attr_name,pointingStateStr);


	//=====================================================
	//==         SIMULATE SET POINT TIME ...
	//=====================================================

	//For simplicity take a random number from 1 to maximum 
	std::uniform_int_distribution<int> distr(1,task_duration);
	int duration= distr(dev->m_randGenerator);

	__INFO_LOG(dev,"Simulate SetPointMode command time ... sleep for "<<duration<<" seconds ...");	
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit startup...");

			//Set ABORTED status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetPointStatus("ABORTED","Wait condition signalled, exit SetPointMode task...",true);
			}

			return;
		}
	} //- leave critical section 

	//Simulate failure?
	//...
	//...

	//=====================================================
	//==         ENABLE SERVO AXIS
	//=====================================================
	//Disable servo axis
	Tango::DevBoolean servoState= true;
 	for(size_t i=0;i<servoState_attr_names.size();i++){
		__DEBUG_LOG(dev,"Setting attr ("<<servoState_attr_names[i]<<") to "<< servoState << "...");
		if( dev->SetDynAttrValue<Tango::DevBoolean>(servoState,servoState_attr_names[i])<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<servoState_attr_names[i]<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetPointStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_point_i"));
		}
	}//end loop attrs

	//=====================================================
	//==         DISABLE BRAKES
	//=====================================================
	//Disable brakes
	Tango::DevBoolean brakeState= false;
 	for(size_t i=0;i<brakeState_attr_names.size();i++){
		__DEBUG_LOG(dev,"Setting attr ("<<brakeState_attr_names[i]<<") to "<< brakeState << "...");
		if( dev->SetDynAttrValue<Tango::DevBoolean>(brakeState,brakeState_attr_names[i])<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<brakeState_attr_names[i]<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetPointStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_point_i"));
		}
	}//end loop attrs


	
	//=====================================================
	//==         SET POWER STATE
	//=====================================================
	//Get current power state	
	Tango::DevShort powerState_current;
	if( dev->GetDynAttrValue<Tango::DevEnum>(powerState_current,powerState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to get attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetPointStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_point_i"));
	}

	//Set power state
	if(powerState_current!=upsPowerState){
		__DEBUG_LOG(dev,"Setting attr ("<<powerState_attr_name<<") to "<< powerStateStr << "...");
		if( dev->SetSMDynAttrValue<Tango::DevEnum>(powerState,powerState_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set FAULT status
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetPointStatus("FAILED",ss.str(),true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_point_i"));
		}
	}//close if

	//=====================================================
	//==         SET POINTING STATE
	//=====================================================
	//Set pointing state
	__DEBUG_LOG(dev,"Setting attr ("<<pointingState_attr_name<<") to "<< pointingStateStr << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(pointingState,pointingState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<pointingState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetPointStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_point_i"));
	}

	//=====================================================
	//==         SET MODE
	//=====================================================
	//Set mode
	__DEBUG_LOG(dev,"Setting attr ("<<mode_attr_name<<") to "<< modeStr << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode,mode_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<mode_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set FAULT status
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetPointStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_set_point_i"));
	}

	//=====================================================
	//==         ENABLE TRACKING
	//=====================================================
	//{ 	
	//	omni_mutex_lock guard(dev->m_mutex);
		dev->EnableTracking();
	//}

	
	//=====================================================
	//==         UPDATE DEVICE STATE/STATUS
	//=====================================================
	//## Set device state to RUNNING
	{ //- enter critical section
		omni_mutex_lock guard(dev->m_mutex);
		std::string msg("SetPointMode completed with success");
		__INFO_LOG(dev,msg);
		dev->set_state(Tango::RUNNING);
  	dev->set_status(msg.c_str());	
		dev->SetPointStatus("COMPLETED",msg,true);
	} //- leave critical section 
	
}//close execute_set_point_i()




// ============================================================================
// DSEmulatorTask::execute_stow
// ============================================================================
void DSEmulatorTask::execute_stow (const StowMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_STOW_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_STOW_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
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
// DSEmulatorTask::execute_stow_i
// ============================================================================
void DSEmulatorTask::execute_stow_i (const StowMsg& msg)
  throw (Tango::DevFailed)
{

	//## Get device instance
	DSEmulator* dev= static_cast<DSEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;

	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string cmd_name= "Stow";
	std::string mode_attr_name= "operatingMode";
	std::string mode_str= "STOW";
	//===========================================================================

	//=====================================================
	//==         CHECK IF CMD ALLOWED
	//=====================================================
	__DEBUG_LOG(dev,"Check if cmd "<<cmd_name<<" is allowed...");
	bool check_cmd= true;
	try {
		dev->IsCmdAllowed(cmd_name,check_cmd);
	}
	catch(Tango::DevFailed& e){
		std::string errMsg("Stow command cannot be executed due to state machine restrictions.");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//=====================================================
	//==         CHECK IF TASK IS ALREADY RUNNING
	//=====================================================
	__DEBUG_LOG(dev,"Check if a previous Stow task is already running...");
	if( strcmp(dev->attr_stowProgress_read[0],"RUNNING")==0  || strcmp(dev->attr_stowProgress_read[0],"IDLE")==0 ){
		std::stringstream ss;
		ss<<"Another Stow task is running or waiting for execution in device!";
		__INFO_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//=====================================================
	//==         CHECK MODE
	//=====================================================
	//Retrieve current mode attr value
	Tango::DevShort current_mode;
	if(dev->GetDynAttrValue(current_mode,mode_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<mode_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_stow_i"));
	}

	//Get string label
	std::string current_mode_str= "";
	Utils_ns::TangoUtils::GetEnumAttrLabelValue(current_mode_str,dev,mode_attr_name,current_mode);

	//Check if same mode is given
	if(mode_str==current_mode_str){
		__INFO_LOG(dev,"Mode "<<mode_str<<" is already selected, nothing to be done");
		dev->SetStowStatus("COMPLETED","Stow mode already active",true);
		return;
	}	

	//=====================================================
	//==    STOW TASK (PERFORMED IN TRACKING THREAD)
	//=====================================================
	__INFO_LOG(dev,"Stow task being scheduled...");
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
	
		//Disable tracking
		__INFO_LOG(dev,"Disabling tracking to stop existing tracking/slewing tasks...");
		(dev->m_trackingThread)->DisableTracking();

		//Clear tracking queue
		__INFO_LOG(dev,"Clearing tracking queue ...");
		(dev->m_pntSchedule)->FlushQueue();

		//Add stow pointing coord to tracking queue
		__INFO_LOG(dev,"Adding stow coordinates in tracking queue ...");
		PointingDataPtr pData= std::make_shared<PointingData>(dev->stowAzimuthPos,dev->stowElevationPos,-1,PointingData::eSTOW);
		if((dev->m_pntSchedule)->Push(pData)<0){//failed to add data to queue
			std::string errMsg("Failed to add stow pointing coords to queue!");
			__ERROR_LOG(dev,errMsg);
			dev->SetStowStatus("ABORTED","Stow command failed to be scheduled!",true);
			_THROW_TANGO_EXCEPTION("STOW_FAILED",errMsg);
		}
			
		//Enable tracking
		__INFO_LOG(dev,"Enable tracking to start stow slewing ...");
		(dev->m_trackingThread)->EnableTracking();

	}//close critical region


}//close execute_stow_i()



// ============================================================================
// DSEmulatorTask::execute_shutdown
// ============================================================================
void DSEmulatorTask::execute_shutdown (const ShutdownMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{
  try
  {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_SHUTDOWN_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_SHUTDOWN_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_startup_i) 
    throw;
  }
}//close execute_shutdown()


// ============================================================================
// DSEmulatorTask::execute_shutdown_i
// ============================================================================
void DSEmulatorTask::execute_shutdown_i (const ShutdownMsg& msg)
  throw (Tango::DevFailed)
{

	//## Get device instance
	DSEmulator* dev= static_cast<DSEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_shutdown_i"));
	}


	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	unsigned long int shutdown_time= msg.shutdown_time;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS 
	//##    (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string powerState_attr_name= "powerState";
	std::string powerState_str= "UPS";
	//=====================================================

	//================================================
	//==           CHECK CURRENT POWER CUT
	//================================================
	//## Check is power cut is already active
	bool powerCutActive= false;
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		powerCutActive= ( *(dev->attr_powerCut_read) == true );
		if(powerCutActive){
			__INFO_LOG(dev,"Power cut is already active, nothing to be done!");
			return;
		}
	}
	
	//================================================
	//==           SET POWER STATE TO UPS
	//================================================
	//## Set powerState to UPS
	//Retrieve mode value corresponding to label
	Tango::DevShort powerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerState_str);

	//Set dyn attr value
	if(dev->SetDynAttrValue(powerState,powerState_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("DSEmulatorTask::execute_shutdown_i"));
	}	

	__INFO_LOG(dev,"Dish power cut occurred, starting safe actions!!!");

	//=====================================================
	//==         GO TO STANDBY-LP
	//=====================================================
	//Run STANDBY-LP task
	StandbyLPMsg standbylpMsg;
	standbylpMsg.task_duration= dev->setModeTime;
	standbylpMsg.enable_failure= dev->enableSetModeFailure;
	standbylpMsg.failure_probability= dev->setModeFailureProbability;
	standbylpMsg.checkPreConditions= true;
	
	try{
		execute_set_standby_lp_i(standbylpMsg);
	}
	catch(...){
		std::string errMsg("Shutdown task failed in step: SetStandbyLPMode()");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_shutdown_i"));
	}
	
	//================================================
	//==           SET POWER CUT
	//================================================
	//Set powerCut=1
	__INFO_LOG(dev,"Setting power cut=true...");
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		*(dev->attr_powerRestored_read)= false;	
		*(dev->attr_powerCut_read)= true;
	}

	//================================================
	//==          WAIT FOR 30 s BEFORE STOWING
	//================================================
	__INFO_LOG(dev,"Waiting "<<dev->shutdownTime<<" seconds before stowing dish...");	
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(dev->shutdownTime*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit shutdown task...");
			return;
		}
	} //- leave critical section 


	//=====================================================
	//==         GO TO STOW
	//=====================================================
	StowMsg stowMsg;
	stowMsg.task_duration= dev->setModeTime;
	stowMsg.enable_failure= dev->enableSetModeFailure;
	stowMsg.failure_probability= dev->setModeFailureProbability;

	try{
		execute_stow_i(stowMsg);
	}
	catch(...){
		std::string errMsg("Shutdown task failed in step: Stow()");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("DSEmulatorTask::execute_shutdown_i"));
	}
	
}//close execute_shutdown_i()



}//close namespace

