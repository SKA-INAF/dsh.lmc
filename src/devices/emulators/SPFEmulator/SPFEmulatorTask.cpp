
#include <SPFEmulatorTask.h>
#include <SPFEmulator.h>

#include <Logger.h>

// ============================================================================
// SOME USER MESSAGES AND CONSTS
// ============================================================================
#define kSTART_PERIODIC_MSG static_cast<size_t>((yat::FIRST_USER_MSG + 1000))
#define kSTOP_PERIODIC_MSG  static_cast<size_t>((yat::FIRST_USER_MSG + 1001))
#define kEXECUTE_CMD_MSG    static_cast<size_t>((yat::FIRST_USER_MSG + 1002))
#define kEXECUTE_STARTUP_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1003))
#define kEXECUTE_GO_OPERATIONAL_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1004))
#define kEXECUTE_GO_STANDBY_LP_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1005))
#define kEXECUTE_GO_MAINTENANCE_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1006))
#define kEXECUTE_FEED_GO_FULL_PERF_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1007))
#define kEXECUTE_FEED_GO_REGENERATION_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1008))
#define kEXECUTE_FEED_GO_STANDBY_LP_CMD_MSG     static_cast<size_t>((yat::FIRST_USER_MSG + 1009))
//-----------------------------------------------------------------------------

namespace SPFEmulator_ns {

// ============================================================================
// SPFEmulatorTask::SPFEmulatorTask
// ============================================================================
SPFEmulatorTask::SPFEmulatorTask (const Config & cfg)
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
// SPFEmulatorTask::~SPFEmulatorTask
// ============================================================================
SPFEmulatorTask::~SPFEmulatorTask () {

	if(m_wait_cond){
		delete m_wait_cond;
		m_wait_cond= 0;
	}

}//close destructor

// ============================================================================
// SPFEmulatorTask::go
// ============================================================================
void SPFEmulatorTask::go (size_t tmo_ms)
    throw (Tango::DevFailed)
{
	//- Initialize wait condition variable
	if(!m_wait_cond) m_wait_cond= new yat::Condition(m_data_lock);

  //- a timer
  yat::Timer t;

	INFO_STREAM << "SPFEmulatorTask::go() - Initializing task thread..." << endl;

  //- finally call yat4tango::DeviceTask::go
  long remaining_tmo = static_cast<long>(tmo_ms) - static_cast<long>(t.elapsed_msec());
  this->yat4tango::DeviceTask::go(remaining_tmo > 1 ? remaining_tmo : 1);

  INFO_STREAM << "SPFEmulatorTask::go() - End initialization" << endl;
}//close go()

// ============================================================================
// SPFEmulatorTask::process_message
// ============================================================================
void SPFEmulatorTask::process_message (yat::Message& msg)
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
				//const unsigned long& msg_data = msg.get_data<unsigned long>();
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
			
    }
		break;

		//- kEXECUTE_GO_OPERATIONAL_CMD_MSG ------------
    case kEXECUTE_GO_OPERATIONAL_CMD_MSG:
    {
      try {
				const GoOperationalMsg& msg_data = msg.get_data<GoOperationalMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_GO_OPERATIONAL_CMD_MSG - Received msg data");
				this->execute_go_operational_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_GO_OPERATIONAL_CMD_MSG - Failed to execute go operational!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_GO_OPERATIONAL_CMD_MSG - Unknown exception while going operational!");
			}
    }
		break;

		//- kEXECUTE_GO_STANDBY_LP_CMD_MSG ------------
    case kEXECUTE_GO_STANDBY_LP_CMD_MSG:
    {
      try {
				const GoStandbyLPMsg& msg_data = msg.get_data<GoStandbyLPMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_GO_STANDBY_LP_CMD_MSG - Received msg data");
				this->execute_go_standbylp_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_GO_STANDBY_LP_CMD_MSG - Failed to execute go standbylp!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_GO_STANDBY_LP_CMD_MSG - Unknown exception while going standbylp!");
			}
    }
		break;

		//- kEXECUTE_GO_MAINTENANCE_CMD_MSG ------------
    case kEXECUTE_GO_MAINTENANCE_CMD_MSG:
    {
      try {
				const GoMaintenanceMsg& msg_data = msg.get_data<GoMaintenanceMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_GO_MAINTENANCE_CMD_MSG - Received msg data");
				this->execute_go_maintenance_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_GO_MAINTENANCE_CMD_MSG - Failed to execute go miantenance!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_GO_MAINTENANCE_CMD_MSG - Unknown exception while going maintenance!");
			}
    }
		break;

		//- kEXECUTE_FEED_GO_FULL_PERF_CMD_MSG ------------
    case kEXECUTE_FEED_GO_FULL_PERF_CMD_MSG:
    {
      try {
				const FeedGoOperateMsg& msg_data = msg.get_data<FeedGoOperateMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_FULL_PERF_CMD_MSG - Received msg data");
				this->execute_go_feed_operate_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_FULL_PERF_CMD_MSG - Failed to execute go to full perf task!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_FULL_PERF_CMD_MSG - Unknown exception while setting feed to full perf !");
			}
    }
		break;

		case kEXECUTE_FEED_GO_REGENERATION_CMD_MSG:
    {
      try {
				const FeedGoRegenerationMsg& msg_data = msg.get_data<FeedGoRegenerationMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_REGENERATION_CMD_MSG - Received msg data");
				this->execute_go_feed_regeneration_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_REGENERATION_CMD_MSG - Failed to execute go to regeneration task!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_REGENERATION_CMD_MSG - Unknown exception while setting feed to regeneration!");
			}
    }
		break;
		
		case kEXECUTE_FEED_GO_STANDBY_LP_CMD_MSG:
    {
      try {
				const FeedGoStandbyLPMsg& msg_data = msg.get_data<FeedGoStandbyLPMsg>();
      	__DEBUG_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_STANDBY_LP_CMD_MSG - Received msg data");
				this->execute_go_feed_standby_lp_i(msg_data);
			}
			catch(Tango::DevFailed& df){
				Tango::Except::print_exception(df);
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_STANDBY_LP_CMD_MSG - Failed to execute go to standbyLP task!");
			}
			catch(...){
				__ERROR_LOG(m_cfg.host_device,"kEXECUTE_FEED_GO_STANDBY_LP_CMD_MSG - Unknown exception while setting feed to standbyLP!");
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
// SPFEmulatorTask::exit
// ============================================================================
void SPFEmulatorTask::exit ()
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
// SPFEmulatorTask::start_periodic_activity
// ============================================================================
void SPFEmulatorTask::start_periodic_activity (size_t tmo_ms)
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
// GroupManagerTask::periodic_job_i
// ============================================================================
void SPFEmulatorTask::periodic_job_i ()
  throw (Tango::DevFailed)
{

	//...
	//...

}//close periodic_job_i()

// ============================================================================
// SPFEmulatorTask::stop_periodic_activity
// ============================================================================
void SPFEmulatorTask::stop_periodic_activity (size_t tmo_ms)
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
// SPFEmulatorTask::execute_cmd
// ============================================================================
void SPFEmulatorTask::execute_cmd (const std::string& cmd_name, bool wait_cmd_reply)
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
    //- handling the message (i.e. SPFEmulatorTask::execute_cmd_i) 
    throw;
  }
}

// ============================================================================
// SPFEmulatorTask::execute_cmd_i
// ============================================================================
void SPFEmulatorTask::execute_cmd_i (const std::string& cmd_name)
  throw (Tango::DevFailed)
{
	
}

// ============================================================================
// SPFEmulatorTask::execute_startup
// ============================================================================
void SPFEmulatorTask::execute_startup (const StartupMsg& msg, bool wait_cmd_reply)
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
    //- handling the message (i.e. SPFEmulatorTask::execute_startup_i) 
    throw;
  }
}


// ============================================================================
// SPFEmulatorTask::execute_startup_i
// ============================================================================
void SPFEmulatorTask::execute_startup_i (const StartupMsg& msg)
  throw (Tango::DevFailed)
{

	// == COMMAND BEHAVIOR ==
	// 1) Initialize attributes
  //     1.1) operatingMode=STARTUP
	//     1.2) powerState=LOW-POWER
	// 		 1.3) bjCapabilityState=UNAVAILABLE
  //     1.4) healthState=UNKNOWN
	//     1.4) rfe_Temp to default setpoints
  //     1.5) bjOperatingState= STANDBY-LP
  //     1.6) bjHealthState= UNKNOWN
  //     1.7) heHealthState= UNKNOWN, heState= ENABLE?
  //     1.8) vaHealthState= UNKNOWN, vaState= ENABLE?
	// 2) Simulate a startup time (with random error generation)
	// 3) Set STANDBY-LP SPF mode
	//     3.1) operatingMode= STANDBY-LP
	//     3.2) powerState= LOW-POWER
	//     3.3) bjCapabilityState= STANDBY
	//     3.4) healthState & bjHealthState= NORMAL (without error)
	//     3.5) LNAs disabled (set as attr init value)
  //     3.6) 1st and 2nd stage temperature (bjRfe2TempCtrlState, j=1,...,5) controllers disabled (set as attr init value)
	//     3.7) Vacuum valve closed (set as attr init value)
  //     3.8) Cryocooler disabled

	//## Get device instance
	SPFEmulator* dev= static_cast<SPFEmulator*>(m_cfg.host_device);
	if(!dev){
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
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
		"b5CapabilityState",
	};
	/*
	std::vector<Tango::DevShort> rfe1SetPointDefaults {
		dev->b1Rfe1TempDefault,
		dev->b2Rfe1TempDefault,
		dev->b3Rfe1TempDefault,
		dev->b4Rfe1TempDefault,
		dev->b5Rfe1TempDefault
	};
	*/
	std::vector<std::string> rfe1DefaultSetPointAttrNames= {
		"b1DefaultLnaHTempSetPoint",
		"b1DefaultLnaVTempSetPoint",	
		"b2DefaultRfe1TempSetPoint",
		"b3DefaultRfe1TempSetPoint",
		"b4DefaultRfe1TempSetPoint",
		"b5DefaultRfe1TempSetPoint"
	};	
	std::vector<std::string> rfe1SetPointAttrNames= {
		//"b1Rfe1TempSetPoint",	
		"b1LnaHTempSetPoint",
		"b1LnaVTempSetPoint",
		"b2Rfe1TempSetPoint",
		"b3Rfe1TempSetPoint",
		"b4Rfe1TempSetPoint",
		"b5Rfe1TempSetPoint"
	};

	std::vector<std::string> rfe2DefaultSetPointAttrNames= {
		//"b1DefaultRfe2TempSetPoint",	
		"b3DefaultRfe2TempSetPoint",
		"b4DefaultRfe2TempSetPoint",
		"b5DefaultRfe2TempSetPoint"
	};	
	std::vector<std::string> rfe2SetPointAttrNames= {
		//"b1Rfe2TempSetPoint",	
		"b3Rfe2TempSetPoint",
		"b4Rfe2TempSetPoint",
		"b5Rfe2TempSetPoint"
	};


	std::vector<std::string> calSourceDefaultSetPointAttrNames= {
		"b1DefaultCalSourceTempSetPoint",	
		"b2DefaultCalSourceTempSetPoint",
		"b3DefaultCalSourceTempSetPoint",
		"b4DefaultCalSourceTempSetPoint",
		"b5DefaultCalSourceTempSetPoint"
	};	
	std::vector<std::string> calSourceSetPointAttrNames= {
		"b1CalSourceTempSetPoint",	
		"b2CalSourceTempSetPoint",
		"b3CalSourceTempSetPoint",
		"b4CalSourceTempSetPoint",
		"b5CalSourceTempSetPoint"
	};

	std::string activeBand_attr_name= "activeBandId";
	std::string mode_init_str= "STARTUP"; 
	std::string mode_str= "STANDBY-LP"; 
	std::string mode_fault_str= "ERROR";
	std::string powerState_str= "LOW-POWER";
	std::string capability_init_str= "UNAVAILABLE"; 
	std::string capability_str= "STANDBY"; 
	std::string activeBand_str= "NONE"; 
	//=====================================================

	//Retrieve init mode value corresponding to label
	Tango::DevShort mode_init;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode_init,dev,attr_name,mode_init_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
	}

	//Retrieve mode value corresponding to label
	Tango::DevShort mode;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,attr_name,mode_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
	}

	//Retrieve mode value corresponding to label
	Tango::DevShort mode_fault;
	if(Utils_ns::TangoUtils::GetEnumAttrValue(mode_fault,dev,attr_name,mode_fault_str)<0){
		std::stringstream ss;
		ss<<"Invalid mode enumeration given in argument (check mode enum values)!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
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
	//Set SPF power state to LOW-POWER
	__DEBUG_LOG(dev,"Setting power state attr ("<<powerState_attr_name<<") to "<< powerState_str<<"...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(powerState,powerState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
	}

	//=====================================================
	//==         SET INIT CAPABILITIES
	//=====================================================
	//Set capabilities to UNAVAILABLE
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		std::string cap_attr_name= cap_attr_names[i];	
			
		Tango::DevShort capability;
		Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_name,capability_init_str);
		if(dev->SetSMDynAttrValue<Tango::DevShort>(capability,cap_attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to set capability attr "<<cap_attr_name<<" to "<<capability<<"!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));	
		}//close if

	}//end loop capabilities

	//=====================================================
	//==         SET INIT MODE
	//=====================================================
	//Set SPF mode to STARTUP
	__DEBUG_LOG(dev,"Setting mode attr ("<<attr_name<<") to "<< mode_init_str << "...");
	if( dev->SetSMDynAttrValue<Tango::DevEnum>(mode_init,attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
	}

	//=====================================================
	//==         SET RFE SET POINT TO DEFAULT
	//=====================================================
	//Initialize RFE1 temp set point to default values
	
	__DEBUG_LOG(dev,"Initializing SPF rfe1_tempSetPoint...");

	for(unsigned int i=0;i<rfe1SetPointAttrNames.size();i++){
		//- Get RFE default setpoint		
		//Tango::DevShort rfe1SetPointDefaultValue= rfe1SetPointDefaults[i];
		Tango::DevShort rfe1SetPointDefaultValue;
		if(dev->GetDynAttrValue<Tango::DevShort>(rfe1SetPointDefaultValue,rfe1DefaultSetPointAttrNames[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<rfe1DefaultSetPointAttrNames[i]<<" value!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
		}	
	
		//- Set RFE setpoint to current default
		std::string rfe_attr_name= rfe1SetPointAttrNames[i];
		if( dev->SetDynAttrValue<Tango::DevEnum>(rfe1SetPointDefaultValue,rfe_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
		}	

	}//end loop rfe setpoints


	//Initialize RFE2 temp set point to default values
	__DEBUG_LOG(dev,"Initializing SPF rfe2_tempSetPoint...");

	for(unsigned int i=0;i<rfe2SetPointAttrNames.size();i++){
		//- Get RFE default setpoint		
		Tango::DevShort rfe2SetPointDefaultValue;
		if(dev->GetDynAttrValue<Tango::DevShort>(rfe2SetPointDefaultValue,rfe2DefaultSetPointAttrNames[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<rfe2DefaultSetPointAttrNames[i]<<" value!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
		}	
	
		//- Set RFE setpoint to current default
		std::string rfe_attr_name= rfe2SetPointAttrNames[i];
		if( dev->SetDynAttrValue<Tango::DevEnum>(rfe2SetPointDefaultValue,rfe_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
		}	

	}//end loop rfe setpoints

	//=====================================================
	//==         SET CAL SOURCE SET POINT TO DEFAULT
	//=====================================================
	//Initialize Cal Source temp set point to default values
	
	__DEBUG_LOG(dev,"Initializing SPF calSourceTempSetPoint...");

	for(unsigned int i=0;i<calSourceSetPointAttrNames.size();i++){
		//- Get Cal Source default setpoint		
		Tango::DevShort calSourceSetPointDefaultValue;
		if(dev->GetDynAttrValue<Tango::DevShort>(calSourceSetPointDefaultValue,calSourceDefaultSetPointAttrNames[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<calSourceDefaultSetPointAttrNames[i]<<" value!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
		}	
	
		//- Set Cal Source setpoint to current default
		std::string calsource_attr_name= calSourceSetPointAttrNames[i];
		if( dev->SetDynAttrValue<Tango::DevEnum>(calSourceSetPointDefaultValue,calsource_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<calsource_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
		}	

	}//end loop rfe setpoints



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
			//THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));

			//Return 
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
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));
	}

	//=====================================================
	//==         SET CAPABILITIES
	//=====================================================
	//Set capabilities to STANDBY
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		std::string cap_attr_name= cap_attr_names[i];	
			
		Tango::DevShort capability;
		Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_name,capability_str);
		if(dev->SetSMDynAttrValue<Tango::DevShort>(capability,cap_attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to set capability attr "<<cap_attr_name<<" to "<<capability<<"!";
			__ERROR_LOG(dev,ss.str());
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_startup_i"));	
		}//close if

	}//end loop capabilities

	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to ON
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		__DEBUG_LOG(dev,"SPF startup completed with success");
		dev->set_state(Tango::ON);
  	dev->set_status("SPF initialization completed");	
	} //- leave critical section 
	

}//close execute_startup_i()

// ============================================================================
// SPFEmulatorTask::execute_go_operational
// ============================================================================
void SPFEmulatorTask::execute_go_operational (const GoOperationalMsg& configMsg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{

 try {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_GO_OPERATIONAL_CMD_MSG, configMsg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_GO_OPERATIONAL_CMD_MSG, configMsg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_go_operational_i) 
    throw;
  }
}//close execute_go_operational()

// ============================================================================
// SPFEmulatorTask::execute_go_operational_i
// ============================================================================
void SPFEmulatorTask::execute_go_operational_i (const GoOperationalMsg& configMsg)
  throw (Tango::DevFailed)
{

	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string mode_attr_name= "operatingMode";
	std::string powerState_attr_name= "powerState";
	std::vector<std::string> cap_attr_names= {
		"b1CapabilityState",
		"b2CapabilityState",
		"b3CapabilityState",
		"b4CapabilityState",
		"b5CapabilityState",
	};
	std::vector<std::string> rfe1TempCtrl_attr_names= {
		"b1LnaHTempCtrlState",
		"b1LnaVTempCtrlState",
		"b2Rfe1TempCtrlState",
		"b3Rfe1TempCtrlState",
		"b4Rfe1TempCtrlState",
		"b5Rfe1TempCtrlState"
	};
	std::vector<bool> hasRfe2TempCtrl {0,0,1,1,1};
	std::vector<std::string> rfe2TempCtrl_attr_names= {
		"b1Rfe2TempCtrlState",
		"b2Rfe2TempCtrlState",
		"b3Rfe2TempCtrlState",
		"b4Rfe2TempCtrlState",
		"b5Rfe2TempCtrlState"
	};

	std::vector<std::string> rfe1DefaultSetPointAttrNames= {
		"b1DefaultLnaHTempSetPoint",
		"b1DefaultLnaVTempSetPoint",
		"b2DefaultRfe1TempSetPoint",
		"b3DefaultRfe1TempSetPoint",
		"b4DefaultRfe1TempSetPoint",
		"b5DefaultRfe1TempSetPoint"
	};	
	std::vector<std::string> rfe1SetPointAttrNames= {
		//"b1Rfe1TempSetPoint",	
		"b1LnaHTempSetPoint",
		"b1LnaVTempSetPoint",
		"b2Rfe1TempSetPoint",
		"b3Rfe1TempSetPoint",
		"b4Rfe1TempSetPoint",
		"b5Rfe1TempSetPoint"
	};

	std::vector<std::string> rfe2DefaultSetPointAttrNames= {
		//"b1DefaultRfe2TempSetPoint",	
		"b3DefaultRfe2TempSetPoint",
		"b4DefaultRfe2TempSetPoint",
		"b5DefaultRfe2TempSetPoint"
	};	
	std::vector<std::string> rfe2SetPointAttrNames= {
		//"b1Rfe2TempSetPoint",	
		"b3Rfe2TempSetPoint",
		"b4Rfe2TempSetPoint",
		"b5Rfe2TempSetPoint"
	};


	std::vector<bool> hasVaValveState {0,1,1};
	std::vector<std::string> vaValveState_attr_names= {
		"b1VaValveState",
		"b2VaValveState",
		"b345VaValveState"
	};
	std::vector<std::string> amp2_h_attr_names= {
		"b1Amp2HPowerState",
		"b2Amp2HPowerState",
		"b3Amp2HPowerState",
		"b4Amp2HPowerState",
		"b5Amp2HPowerState"
	};	
	std::vector<std::string> amp2_v_attr_names= {
		"b1Amp2VPowerState",
		"b2Amp2VPowerState",
		"b3Amp2VPowerState",
		"b4Amp2VPowerState",
		"b5Amp2VPowerState"
	};
	std::vector<bool> hasCryoMotor {0,1,1};
	std::vector<std::string> cryoMotorState_attr_names= {
		"b1CryoMotorState",
		"b2CryoMotorState",
		"b345CryoMotorState"
	};
	std::string mode_str= "OPERATE";
	std::string mode_safe_str= "STANDBY-LP";
	std::string mode_fault_str= "ERROR";
	std::string powerState_str= "FULL-POWER";
	std::string capability_str= "OPERATE-FULL";
	std::string capability_degraded_str= "OPERATE-DEGRADED";
	std::string capability_safe_str= "STANDBY";	
	std::string capability_faulty_str= "UNAVAILABLE";
	//===========================================================================
	
	
	//## Get device instance
	SPFEmulator* dev= static_cast<SPFEmulator*>(m_cfg.host_device);
	if(!dev){
		//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
	}

	__DEBUG_LOG(dev,"Starting go cold operation task ...");

	//## Set task status to RUNNING
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetOperateStatus("RUNNING","Go operational started",true);
	}

	//## Get message info
	unsigned long int task_duration= configMsg.task_duration;
	bool enable_failure= configMsg.enable_failure;
	float failure_prob= configMsg.failure_probability;
	

	//=====================================================
	//==         INIT
	//=====================================================
	//Retrieve mode value corresponding to label
	Tango::DevShort mode;
	Tango::DevShort mode_fault;
	Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,mode_attr_name,mode_str);	
	Utils_ns::TangoUtils::GetEnumAttrValue(mode_fault,dev,mode_attr_name,mode_fault_str);

	//Retrieve powerState value corresponding to label
	Tango::DevShort powerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerState_str);

	//Retrieve the capabilities value corresponding to label
	std::vector<Tango::DevShort> capabilities;
	std::vector<Tango::DevShort> capabilities_degraded;
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		Tango::DevShort capability= -1;
		Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_names[i],capability_str);
		capabilities.push_back(capability);

		Tango::DevShort capability_degraded= -1;
		Utils_ns::TangoUtils::GetEnumAttrValue(capability_degraded,dev,cap_attr_names[i],capability_degraded_str);
		capabilities_degraded.push_back(capability_degraded);
	}
	
	//=====================================================
	//==      CHECK CURRENT MODE/POWER/CAPABILITIES
	//=====================================================
	//Get current mode
	Tango::DevShort mode_current;	
	if(dev->GetDynAttrValue(mode_current,mode_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<mode_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetOperateStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
	}	

	//Get current powerState
	Tango::DevShort powerState_current;	
	if(dev->GetDynAttrValue(powerState_current,powerState_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetOperateStatus("FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
	}	

	//Get capability values
	std::vector<Tango::DevShort> capabilities_current;
	std::vector<std::string> capabilities_current_str;	
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		Tango::DevShort cap;	
		if(dev->GetDynAttrValue(cap,cap_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<cap_attr_names[i]<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED",ss.str(),true);
			}

			//Throw exception
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}		
		capabilities_current.push_back(cap);

		std::string cap_str= "";
		Utils_ns::TangoUtils::GetEnumAttrLabelValue(cap_str,dev,cap_attr_names[i],cap);
		capabilities_current_str.push_back(cap_str);
	}//end loop capabilities

	
	//=====================================================
	//==         SET RFE SET POINT TO DEFAULT
	//=====================================================
	//Initialize RFE1 temp set point to default values
	__DEBUG_LOG(dev,"Initializing SPF rfe1_tempSetPoint...");

	for(size_t i=0;i<rfe1SetPointAttrNames.size();i++){
		//- Get RFE default setpoint		
		Tango::DevShort rfe1SetPointDefaultValue;
		if(dev->GetDynAttrValue<Tango::DevShort>(rfe1SetPointDefaultValue,rfe1DefaultSetPointAttrNames[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<rfe1DefaultSetPointAttrNames[i]<<" value!";
			__ERROR_LOG(dev,ss.str());
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Set operate mode command failed in execution (cannot set rfe1SetPoint)",true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}	
	
		//- Set RFE setpoint to current default
		std::string rfe_attr_name= rfe1SetPointAttrNames[i];
		if( dev->SetDynAttrValue<Tango::DevEnum>(rfe1SetPointDefaultValue,rfe_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Set operate mode command failed in execution (cannot set rfe1SetPoint)",true);
			}
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}	

	}//end loop rfe setpoints


	//Initialize RFE2 temp set point to default values
	__DEBUG_LOG(dev,"Initializing SPF rfe2_tempSetPoint...");

	for(size_t i=0;i<rfe2SetPointAttrNames.size();i++){
		//- Get RFE default setpoint		
		Tango::DevShort rfe2SetPointDefaultValue;
		if(dev->GetDynAttrValue<Tango::DevShort>(rfe2SetPointDefaultValue,rfe2DefaultSetPointAttrNames[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<rfe2DefaultSetPointAttrNames[i]<<" value!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Go regeneration command failed in execution (cannot set rfe2SetPoint)",true);
			}
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}	
	
		//- Set RFE setpoint to current default
		std::string rfe_attr_name= rfe2SetPointAttrNames[i];
		if( dev->SetDynAttrValue<Tango::DevEnum>(rfe2SetPointDefaultValue,rfe_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Go regeneration command failed in execution (cannot set rfe2SetPoint)",true);
			}
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}	
	}//end loop rfe setpoints





	//=====================================================
	//==         ENABLE 2nd-stage temp controllers
	//=====================================================
	Tango::DevBoolean rfe2TempCtrl= true;
	for(size_t i=0;i<rfe2TempCtrl_attr_names.size();i++){
		if(hasRfe2TempCtrl[i] && dev->SetDynAttrValue<Tango::DevBoolean>(rfe2TempCtrl,rfe2TempCtrl_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe2TempCtrl_attr_names[i]<<" to "<<rfe2TempCtrl<<"!";
			__ERROR_LOG(dev,ss.str());	

			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set rfe2TempCtrl)",true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop attrs

	//=====================================================
	//==        SET CAPABILITIES
	//=====================================================
	//Set capabilities to OPERATE-DEGRADED
	for(size_t i=0;i<cap_attr_names.size();i++){
		if(capabilities_current_str[i]!=capability_faulty_str && dev->SetSMDynAttrValue(capabilities_degraded[i],cap_attr_names[i],&capabilities_current[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<cap_attr_names[i]<<" to "<<capabilities[i]<<"!";
			__ERROR_LOG(dev,ss.str());
				
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set capabilities)",true);
			}
	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop capabilities

	//=====================================================
	//==        OPEN VACUUM VALVE 
	//=====================================================
	Tango::DevBoolean vaValveState= true;
	for(unsigned int i=0;i<vaValveState_attr_names.size();i++){
		if(hasVaValveState[i] && dev->SetSMDynAttrValue(vaValveState,vaValveState_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<vaValveState_attr_names[i]<<" to "<<vaValveState<<"!";
			__ERROR_LOG(dev,ss.str());
				
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set vaValveState)",true);
			}
	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop attrs

	//=====================================================
	//==         GO COLD/OPERATIONAL TIME...
	//=====================================================
	//Simulate long configuring time
	__DEBUG_LOG(dev,"Go operational time...");
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit go_operational...");

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("ABORTED","Wait condition signalled, exit operate task...",true);
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
				dev->SetOperateStatus("FAILED","Operate command failed in execution",true);
			}

			//Set mode to ERROR
			dev->SetDynAttrValue<Tango::DevShort>(mode_fault,mode_attr_name);
	
			//Set capability to UNAVAILABLE
			//dev->SetDynAttrValue<Tango::DevShort>(capability_fault,cap_attr_name);
	
			//Throw exception
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}//close if
	}//close if
	
	//=====================================================
	//==        ENABLE CRYO-COOLERS
	//=====================================================
	Tango::DevBoolean cryocooler_enabled= true;
	for(unsigned int i=0;i<cryoMotorState_attr_names.size();i++){
		if(hasCryoMotor[i] && dev->SetDynAttrValue(cryocooler_enabled,cryoMotorState_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<cryoMotorState_attr_names[i]<<" to "<<cryocooler_enabled<<"!";
			__ERROR_LOG(dev,ss.str());
				
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set cryoMotorState)",true);
			}
	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop attrs
	

	//=====================================================
	//==        CLOSE VACUUM VALVE 
	//=====================================================
	vaValveState= false;
	for(unsigned int i=0;i<vaValveState_attr_names.size();i++){
		if(hasVaValveState[i] && dev->SetDynAttrValue(vaValveState,vaValveState_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<vaValveState_attr_names[i]<<" to "<<vaValveState<<"!";
			__ERROR_LOG(dev,ss.str());
				
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set vaValveState)",true);
			}
	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop attrs

	//=====================================================
	//==        SWITCH-ON 2nd-stage AMP 
	//=====================================================
	//Switch on the 2nd stage amplifiers for all bands, i.e. bXAmp2HPowerState=true, bXAmp2VPowerState=true (X=1, ..., 5)
	Tango::DevBoolean amp2_enabled= true;
	for(unsigned int i=0;i<amp2_h_attr_names.size();i++){
		if(dev->SetDynAttrValue(amp2_enabled,amp2_h_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<amp2_h_attr_names[i]<<" to "<<amp2_enabled<<"!";
			__ERROR_LOG(dev,ss.str());
				
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set amp2HPowerState)",true);
			}
	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop attrs

	for(unsigned int i=0;i<amp2_v_attr_names.size();i++){
		if(dev->SetDynAttrValue(amp2_enabled,amp2_v_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<amp2_v_attr_names[i]<<" to "<<amp2_enabled<<"!";
			__ERROR_LOG(dev,ss.str());
				
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set amp2VPowerState)",true);
			}
	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop attrs


	//=====================================================
	//==        SWITCH-ON 1st-stage temp controllers 
	//=====================================================
	Tango::DevBoolean rfe1TempCtrl= true;
	for(unsigned int i=0;i<rfe1TempCtrl_attr_names.size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(rfe1TempCtrl,rfe1TempCtrl_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe1TempCtrl_attr_names[i]<<" to "<<rfe1TempCtrl<<"!";
			__ERROR_LOG(dev,ss.str());	

			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set rfe1TempCtrl)",true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop attrs

	//=====================================================
	//==        SET POWER STATE
	//=====================================================
	//Set power state to FULL-POWER (revert back to previous state in case of error)
	if(dev->SetSMDynAttrValue(powerState,powerState_attr_name,&powerState_current)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<powerState_attr_name<<" to "<<powerState<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set powerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
	}
	
	//=====================================================
	//==        SET CAPABILITIES
	//=====================================================
	//Set capabilities to OPERATE-FULL
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		if(capabilities_current_str[i]!=capability_faulty_str && dev->SetSMDynAttrValue(capabilities[i],cap_attr_names[i],&capabilities_current[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<cap_attr_names[i]<<" to "<<capabilities[i]<<"!";
			__ERROR_LOG(dev,ss.str());
				
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set capabilities)",true);
			}
	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
		}
	}//end loop capabilities

	//=====================================================
	//==        SET MODE
	//=====================================================
	//Set mode to OPERATE and revert back to previous in case of failure
	if(dev->SetSMDynAttrValue(mode,mode_attr_name,&mode_current)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<mode_attr_name<<" to "<<mode<<"!";
		__ERROR_LOG(dev,ss.str());

		///Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetOperateStatus("FAILED","Operate command failed in execution (cannot set mode)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_operational_i"));
	}


	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to DISABLE and set status to COMPLETED
	{ //- enter critical section
		//yat::AutoMutex<> guard(m_data_lock);
		omni_mutex_lock guard(dev->m_mutex);
		__INFO_LOG(dev,"Go operational task completed with success ...");

		//dev->set_state(Tango::ON);
  	//dev->set_status("SPF operate completed");	

		dev->SetOperateStatus("COMPLETED","Operate completed with success",true);
	} //- leave critical section 
	


}//close execute_go_operational_i()


// ============================================================================
// SPFEmulatorTask::execute_go_standbylp
// ============================================================================
void SPFEmulatorTask::execute_go_standbylp (const GoStandbyLPMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{

 try {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_GO_STANDBY_LP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_GO_STANDBY_LP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_go_operational_i) 
    throw;
  }
}//close execute_go_standbylp()

// ============================================================================
// SPFEmulatorTask::execute_go_standbylp_i
// ============================================================================
void SPFEmulatorTask::execute_go_standbylp_i (const GoStandbyLPMsg& msg)
  throw (Tango::DevFailed)
{

	// == COMMAND BEHAVIOR ==
	// 1) Check if cmd mode-allowed {"OPERATE","MAINTENANCE"} (DONE IN STATE MACHINE)
	// 2) Check if mode transition allowed {"OPERATE","MAINTENANCE"}-->"STANDBY-LP" (DONE IN STATE MACHINE)
	// 3) Check if powerState transition allowed {"LOW-POWER","FULL-POWER"}-->"LOW-POWER" (DONE IN STATE MACHINE)
	// 4) Turn off all LNAs 
	// 5) Close the vacuum valves 
	// 6) Switch off all the RF amplifiers.
	// 7) Disable the cryocooler 
	// 8) Disable both temp controllers
	// 9) The SM will pause for 2 seconds to ensure that the valve closing mechanism has been closed properly before the vacuum pump can be switched off. 
	//    It then releases the requests for the vacuum pump and the compressor.
	// 10) Set power state to LOW-POWER
	// 11) Set capabilities to STANDBY 
	// 12) Set mode to STANDBY-LP and feed modes to GO-STANDBY

	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string mode_attr_name= "operatingMode";
	std::string powerState_attr_name= "powerState";
	std::vector<std::string> cap_attr_names= {
		"b1CapabilityState",
		"b2CapabilityState",
		"b3CapabilityState",
		"b4CapabilityState",
		"b5CapabilityState",
	};
	std::vector<std::string> feed_mode_attr_names {
		"b1OperatingState",
		"b2OperatingState",	
		"b3OperatingState",
		"b4OperatingState",
		"b5OperatingState"
	};
	std::string activeBand_attr_name= "activeBandId";
	std::vector<std::string> lnaHPowerState_attr_names= {
		"b1LnaHPowerState",
		"b2LnaHPowerState",
		"b3LnaHPowerState",
		"b4LnaHPowerState",
		"b5LnaHPowerState"
	};
	std::vector<std::string> lnaVPowerState_attr_names= {
		"b1LnaVPowerState",
		"b2LnaVPowerState",
		"b3LnaVPowerState",
		"b4LnaVPowerState",
		"b5LnaVPowerState"
	};
	std::vector<bool> hasCryoMotor {0,1,1};
	std::vector<std::string> cryoMotorState_attr_names= {
		"b1CryoMotorState",
		"b2CryoMotorState",
		"b345CryoMotorState"
	};
	std::vector<std::string> rfe1TempCtrl_attr_names= {
		"b1LnaHTempCtrlState",
		"b1LnaVTempCtrlState",
		"b2Rfe1TempCtrlState",
		"b3Rfe1TempCtrlState",
		"b4Rfe1TempCtrlState",
		"b5Rfe1TempCtrlState"
	};
	std::vector<bool> hasRfe2TempCtrl {0,0,1,1,1};
	std::vector<std::string> rfe2TempCtrl_attr_names= {
		"b1Rfe2TempCtrlState",
		"b2Rfe2TempCtrlState",
		"b3Rfe2TempCtrlState",
		"b4Rfe2TempCtrlState",
		"b5Rfe2TempCtrlState"
	};
	std::vector<bool> hasVaValveState {0,1,1};
	std::vector<std::string> vaValveState_attr_names= {
		"b1VaValveState",
		"b2VaValveState",
		"b345VaValveState"
	};
	std::vector<std::string> amp2_h_attr_names= {
		"b1Amp2HPowerState",
		"b2Amp2HPowerState",
		"b3Amp2HPowerState",
		"b4Amp2HPowerState",
		"b5Amp2HPowerState"
	};	
	std::vector<std::string> amp2_v_attr_names= {
		"b1Amp2VPowerState",
		"b2Amp2VPowerState",
		"b3Amp2VPowerState",
		"b4Amp2VPowerState",
		"b5Amp2VPowerState"
	};
	std::string mode_str= "STANDBY-LP"; 
	std::string mode_fault_str= "ERROR";
	std::string feed_mode_str= "STANDBY-LP";
	std::string powerState_str= "LOW-POWER";
	std::string capability_str= "STANDBY"; 
	std::string capability_operate_str= "OPERATE-FULL"; 
	std::string capability_degraded_str= "OPERATE-DEGRADED"; 
	std::string capability_faulty_str= "UNAVAILABLE"; 
	//===========================================================================

	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	
	//## Get device instance
	SPFEmulator* dev= static_cast<SPFEmulator*>(m_cfg.host_device);
	if(!dev){
		//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get access to device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
	}

	__DEBUG_LOG(dev,"Starting set STANDBY-LP mode ...");

	//## Set task status to RUNNING
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetStandbyLPStatus("RUNNING","Set mode to STANDBY-LP started",true);
	}

	//=====================================================
	//==         INIT
	//=====================================================
	//Retrieve mode value corresponding to label
	Tango::DevShort mode;
	Tango::DevShort mode_fault;
	Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,mode_attr_name,mode_str);	
	Utils_ns::TangoUtils::GetEnumAttrValue(mode_fault,dev,mode_attr_name,mode_fault_str);
	
	//Retrieve powerState value corresponding to label
	Tango::DevShort powerState;
	Utils_ns::TangoUtils::GetEnumAttrValue(powerState,dev,powerState_attr_name,powerState_str);

	//Retrieve capabilities corresponding to label
	std::vector<Tango::DevShort> capabilities;
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		Tango::DevShort capability= -1;
		Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_names[i],capability_str);
		capabilities.push_back(capability);
	}

	//Retrieve mode value corresponding to label
	std::vector<Tango::DevShort> feed_modes;
	for(unsigned int i=0;i<feed_mode_attr_names.size();i++){
		Tango::DevShort feed_mode;
		Utils_ns::TangoUtils::GetEnumAttrValue(feed_mode,dev,feed_mode_attr_names[i],feed_mode_str);
		feed_modes.push_back(feed_mode);
	}

	//=====================================================
	//==      CHECK CURRENT MODE/POWER/CAPABILITIES
	//=====================================================
	//Get current mode
	Tango::DevShort mode_current;	
	if(dev->GetDynAttrValue(mode_current,mode_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<mode_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set mode)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
	}	

	//Get current powerState
	Tango::DevShort powerState_current= -1;	
	if(dev->GetDynAttrValue(powerState_current,powerState_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<powerState_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set powerState)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
	}	

	//Get capability values
	std::vector<Tango::DevShort> capabilities_current;
	
	std::vector<std::string> capabilities_current_str;	
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		Tango::DevShort cap;	
		if(dev->GetDynAttrValue(cap,cap_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<cap_attr_names[i]<<" value!";
			__ERROR_LOG(dev,ss.str());

			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set capabilities)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}		
		capabilities_current.push_back(cap);

		std::string cap_str= "";
		Utils_ns::TangoUtils::GetEnumAttrLabelValue(cap_str,dev,cap_attr_names[i],cap);
		capabilities_current_str.push_back(cap_str);
	}//end loop capabilities

	//Get capabilities degraded enum
	std::vector<Tango::DevShort> capabilities_degraded;
	for(unsigned int i=0;i<cap_attr_names.size();i++){
		Tango::DevShort cap;	
		Utils_ns::TangoUtils::GetEnumAttrValue(cap,dev,cap_attr_names[i],capability_degraded_str);	
		capabilities_degraded.push_back(cap);
	}

	//=====================================================
	//==         Switch OFF LNAs
	//=====================================================
	Tango::DevBoolean lna_power_state= false;

	for(unsigned int i=0;i<lnaHPowerState_attr_names.size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(lna_power_state,lnaHPowerState_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<lnaHPowerState_attr_names[i]<<" to "<<lna_power_state<<"!";
			__ERROR_LOG(dev,ss.str());	
			
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set lnaHPowerState)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop H LNAs
		
	for(unsigned int i=0;i<lnaVPowerState_attr_names.size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(lna_power_state,lnaVPowerState_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<lnaVPowerState_attr_names[i]<<" to "<<lna_power_state<<"!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set lnaVPowerState)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop V LNAs

	//=====================================================
	//==        CLOSE VACUUM VALVES
	//=====================================================
	Tango::DevBoolean vaValveState= false;

	for(unsigned int i=0;i<vaValveState_attr_names.size();i++){
		if(hasVaValveState[i] && dev->SetDynAttrValue(vaValveState,vaValveState_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<vaValveState_attr_names[i]<<" to "<<vaValveState<<"!";
			__ERROR_LOG(dev,ss.str());
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set vaValveState)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop attrs
	
	//=====================================================
	//==         Switch OFF 2nd stage amplifier
	//=====================================================
	Tango::DevBoolean amp2_power_state= false;

	for(unsigned int i=0;i<amp2_h_attr_names.size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(amp2_power_state,amp2_h_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<amp2_h_attr_names[i]<<" to "<<amp2_power_state<<"!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set amp2HPowerState)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));	
		}
	}//end loop H amp2

	for(unsigned int i=0;i<amp2_v_attr_names.size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(amp2_power_state,amp2_v_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<amp2_v_attr_names[i]<<" to "<<amp2_power_state<<"!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set amp2VPowerState)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop V amp2

	//=====================================================
	//==         Switch OFF CRYOCOOLERS
	//=====================================================
	Tango::DevBoolean cryo_motor_state= false;

	for(unsigned int i=0;i<cryoMotorState_attr_names.size();i++){
		if(hasCryoMotor[i] && dev->SetDynAttrValue<Tango::DevBoolean>(cryo_motor_state,cryoMotorState_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<cryoMotorState_attr_names[i]<<" to "<<cryo_motor_state<<"!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set cryoMotorState)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop cryo motors

	//=====================================================
	//==         DISABLE 1st-state temp controllers
	//=====================================================
	Tango::DevBoolean rfe1TempCtrl= false;
	for(unsigned int i=0;i<rfe1TempCtrl_attr_names.size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(rfe1TempCtrl,rfe1TempCtrl_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe1TempCtrl_attr_names[i]<<" to "<<rfe1TempCtrl<<"!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set rfe1TempCtrl)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop attrs

	//=====================================================
	//==         DISABLE 2nd-state temp controllers
	//=====================================================
	Tango::DevBoolean rfe2TempCtrl= false;
	for(unsigned int i=0;i<rfe2TempCtrl_attr_names.size();i++){
		if(hasRfe2TempCtrl[i] && dev->SetDynAttrValue<Tango::DevBoolean>(rfe2TempCtrl,rfe2TempCtrl_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe2TempCtrl_attr_names[i]<<" to "<<rfe2TempCtrl<<"!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set rfe2TempCtrl)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop attrs


	//===========================================================================
	//==        SET CAPABILITY TO OPERATE DEGRADED (IF CURRENT MODE IS OPERATE)
	//===========================================================================
	for(size_t i=0;i<cap_attr_names.size();i++){
		if(capabilities_current_str[i]==capability_faulty_str) continue;//skip is capability unavailable
		if(capabilities_current_str[i]==capability_operate_str && dev->SetSMDynAttrValue(capabilities_degraded[i],cap_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<cap_attr_names[i]<<" to "<<capabilities_degraded[i]<<"!";
			__ERROR_LOG(dev,ss.str());

			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set capabilities)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop capabilities



	//=====================================================
	//==         GO STANDBY-LP TIME...
	//=====================================================
	//Simulate long standby lp wait time
	__DEBUG_LOG(dev,"Go STANDBY-LP time...");
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit go standbylp...");

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("ABORTED","Wait condition signalled, exit go standbylp task...",true);
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
			ss<<"Failure occurred in set standby mode, reverting mode and capability to safe state!";
			__ERROR_LOG(dev,ss.str());

			//Set configuration fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetStandbyLPStatus("FAILED","Set standby command failed in execution",true);
			}

			//Set mode to ERROR
			dev->SetDynAttrValue<Tango::DevShort>(mode_fault,mode_attr_name);
	
			//Set capability to UNAVAILABLE
			//dev->SetDynAttrValue<Tango::DevShort>(capability_fault,cap_attr_name);
	
			//Throw exception
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}//close if
	}//close if



	//=====================================================
	//==        SET POWER STATE
	//=====================================================
	//Set power state to LOW-POWER
	if(dev->SetSMDynAttrValue(powerState,powerState_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<powerState_attr_name<<" to "<<powerState<<"!";
		__ERROR_LOG(dev,ss.str());	
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set powerState)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
	}

	//=====================================================
	//==        SET CAPABILITY TO STANDBY
	//=====================================================
	//Set capability to STANDBY
	for(size_t i=0;i<cap_attr_names.size();i++){
		if(capabilities_current_str[i]!=capability_faulty_str && dev->SetSMDynAttrValue(capabilities[i],cap_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<cap_attr_names[i]<<" to "<<capabilities[i]<<"!";
			__ERROR_LOG(dev,ss.str());
			//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set capabilities)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop capabilities

	//=====================================================
	//==        SET MODE to STANDBY-LP
	//=====================================================
	//Set mode to STANDBY-LP
	if(dev->SetSMDynAttrValue(mode,mode_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<mode_attr_name<<" to "<<mode<<"!";
		__ERROR_LOG(dev,ss.str());	
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set mode)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
	}

	//=====================================================
	//==        SET FEED MODES to STANDBY-LP
	//=====================================================
	for(unsigned int i=0;i<feed_mode_attr_names.size();i++){
		if(dev->SetSMDynAttrValue(feed_modes[i],feed_mode_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<feed_mode_attr_names[i]<<" to "<<feed_modes[i]<<"!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetStandbyLPStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set feedModes)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_standbylp_i"));
		}
	}//end loop attr

	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to DISABLE and set status to COMPLETED
	{ //- enter critical section
		//yat::AutoMutex<> guard(m_data_lock);
		omni_mutex_lock guard(dev->m_mutex);
		__INFO_LOG(dev,"Go standby task completed with success ...");

		//dev->set_state(Tango::ON);
  	//dev->set_status("SPF operate completed");	

		dev->SetStandbyLPStatus("COMPLETED","Set STANDBY-LP mode completed with success",true);
	} //- leave critical section 
	

}//close execute_go_standbylp_i()



// ============================================================================
// SPFEmulatorTask::execute_go_maintenance
// ============================================================================
void SPFEmulatorTask::execute_go_maintenance (const GoMaintenanceMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{

 try {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_GO_MAINTENANCE_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_GO_MAINTENANCE_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_go_operational_i) 
    throw;
  }
}//close execute_go_maintenance()

// ============================================================================
// SPFEmulatorTask::execute_go_maintenance_i
// ============================================================================
void SPFEmulatorTask::execute_go_maintenance_i (const GoMaintenanceMsg& msg)
  throw (Tango::DevFailed)
{

	// == COMMAND BEHAVIOR ==
	// 1) Check if cmd mode-allowed {"OPERATE","MAINTENANCE"} (DONE IN STATE MACHINE)
	// 2) Check if mode transition allowed {"OPERATE","MAINTENANCE"}-->"STANDBY-LP" (DONE IN STATE MACHINE)
	// 3) Check if powerState transition allowed {"LOW-POWER","FULL-POWER"}-->"LOW-POWER" (DONE IN STATE MACHINE)
	// 4) Turn off all LNAs 
	// 5) Close the vacuum valves 
	// 6) Switch off all the RF amplifiers.
	// 7) Disable the cryocooler 
	// 8) Disable both temp controllers
	// 9) The SM will pause for 2 seconds to ensure that the valve closing mechanism has been closed properly before the vacuum pump can be switched off. 
	//    It then releases the requests for the vacuum pump and the compressor.
	// 10) Set power state to LOW-POWER
	// 11) Set capabilities to STANDBY 
	// 12) Set mode to STANDBY-LP and feed modes to GO-STANDBY

	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string mode_attr_name= "operatingMode";
	std::string powerState_attr_name= "powerState";
	std::vector<std::string> feed_mode_attr_names {
		"b1OperatingState",
		"b2OperatingState",	
		"b3OperatingState",
		"b4OperatingState",
		"b5OperatingState"
	};
	std::string vaState_attr_name= "vaOperatingState";
	std::string heState_attr_name= "heOperatingState";
	
	std::string mode_str= "MAINTENANCE"; 
	std::string mode_fault_str= "ERROR";
	std::string feed_mode_str= "MAINTENANCE";
	std::string va_state_str= "MAINTENANCE";
	std::string he_state_str= "MAINTENANCE";
	//===========================================================================

	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	
	//## Get device instance
	SPFEmulator* dev= static_cast<SPFEmulator*>(m_cfg.host_device);
	if(!dev){
		//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get access to device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("SPFEmulatorTask::execute_go_maintenance_i"));
	}

	__DEBUG_LOG(dev,"Starting set STANDBY-LP mode ...");

	//## Set task status to RUNNING
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetMaintenanceStatus("RUNNING","Set mode to MAINTENANCE started",true);
	}

	//=====================================================
	//==         INIT
	//=====================================================
	//Retrieve mode value corresponding to label
	Tango::DevShort mode;
	Tango::DevShort mode_fault;
	Utils_ns::TangoUtils::GetEnumAttrValue(mode,dev,mode_attr_name,mode_str);	
	Utils_ns::TangoUtils::GetEnumAttrValue(mode_fault,dev,mode_attr_name,mode_fault_str);

	//Retrieve mode value corresponding to label
	std::vector<Tango::DevShort> feed_modes;
	for(unsigned int i=0;i<feed_mode_attr_names.size();i++){
		Tango::DevShort feed_mode;
		Utils_ns::TangoUtils::GetEnumAttrValue(feed_mode,dev,feed_mode_attr_names[i],feed_mode_str);
		feed_modes.push_back(feed_mode);
	}
		
	//Retrieve vaState corresponding to label
	Tango::DevShort va_state;
	Utils_ns::TangoUtils::GetEnumAttrValue(va_state,dev,vaState_attr_name,va_state_str);	
	
	//Retrieve heState corresponding to label
	Tango::DevShort he_state;
	Utils_ns::TangoUtils::GetEnumAttrValue(he_state,dev,heState_attr_name,he_state_str);	

	//=====================================================
	//==      CHECK CURRENT MODE
	//=====================================================
	//Get current mode
	Tango::DevShort mode_current;	
	if(dev->GetDynAttrValue(mode_current,mode_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<mode_attr_name<<" value!";
		__ERROR_LOG(dev,ss.str());
		
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetMaintenanceStatus("FAILED","Set STANDBY-LP command failed in execution (cannot set mode)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_maintenance_i"));
	}	

	
	//=====================================================
	//==        SET HE STATE TO MAINTENANCE
	//=====================================================
	if(dev->SetDynAttrValue(he_state,heState_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<heState_attr_name<<" to "<<he_state<<"!";
		__ERROR_LOG(dev,ss.str());	
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetMaintenanceStatus("FAILED","Set maintenance command failed in execution (cannot set heState)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_maintenance_i"));
	}

	//=====================================================
	//==        SET VA STATE TO MAINTENANCE
	//=====================================================
	if(dev->SetDynAttrValue(va_state,vaState_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<vaState_attr_name<<" to "<<va_state<<"!";
		__ERROR_LOG(dev,ss.str());	
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetMaintenanceStatus("FAILED","Set maintenance command failed in execution (cannot set vaState)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_maintenance_i"));
	}
	
	//=====================================================
	//==         GO MAINTENANCE TIME...
	//=====================================================
	//Simulate long maintenance wait time
	__DEBUG_LOG(dev,"Go MAINTENANCE time...");
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit go maintenance...");

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetMaintenanceStatus("ABORTED","Wait condition signalled, exit go maintenance task...",true);
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
			ss<<"Failure occurred in set maintenance mode, reverting mode and capability to safe state!";
			__ERROR_LOG(dev,ss.str());

			//Set configuration fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetMaintenanceStatus("FAILED","Set maintenance command failed in execution",true);
			}

			//Set mode to ERROR
			dev->SetDynAttrValue<Tango::DevShort>(mode_fault,mode_attr_name);
	
			//Set capability to UNAVAILABLE
			//dev->SetDynAttrValue<Tango::DevShort>(capability_fault,cap_attr_name);
	
			//Throw exception
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_maintenance_i"));
		}//close if
	}//close if



	
	//=====================================================
	//==        SET MODE to MAINTENANCE
	//=====================================================
	//Set mode to MAINTENANCE
	if(dev->SetSMDynAttrValue(mode,mode_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<mode_attr_name<<" to "<<mode<<"!";
		__ERROR_LOG(dev,ss.str());	
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetMaintenanceStatus("FAILED","Set maintenance command failed in execution (cannot set mode)",true);
		}	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_maintenance_i"));
	}

	//=====================================================
	//==        SET FEED MODES TO MAINTENANCE
	//=====================================================
	for(unsigned int i=0;i<feed_mode_attr_names.size();i++){
		if(dev->SetSMDynAttrValue(feed_modes[i],feed_mode_attr_names[i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<feed_mode_attr_names[i]<<" to "<<feed_modes[i]<<"!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);		
				dev->SetMaintenanceStatus("FAILED","Set maintenance command failed in execution (cannot set feedModes)",true);
			}	
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_maintenance_i"));
		}
	}//end loop attr

	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to DISABLE and set status to COMPLETED
	{ //- enter critical section
		//yat::AutoMutex<> guard(m_data_lock);
		omni_mutex_lock guard(dev->m_mutex);
		__INFO_LOG(dev,"Go maintenance task completed with success ...");

		//dev->set_state(Tango::ON);
  	//dev->set_status("SPF operate completed");	

		dev->SetMaintenanceStatus("COMPLETED","Maintenance completed with success",true);
	} //- leave critical section 
	

}//close execute_go_maintenance_i()





// ============================================================================
// SPFEmulatorTask::execute_go_feed_operate
// ============================================================================
void SPFEmulatorTask::execute_go_feed_operate (const FeedGoOperateMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{

 try {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_FEED_GO_FULL_PERF_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_FEED_GO_FULL_PERF_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_go_operational_i) 
    throw;
  }
}//close execute_go_feed_operate()




// ============================================================================
// SPFEmulatorTask::execute_go_feed_operate_i
// ============================================================================
void SPFEmulatorTask::execute_go_feed_operate_i (const FeedGoOperateMsg& msg)
  throw (Tango::DevFailed)
{

	// == COMMAND BEHAVIOR ==
	//  1) Request vacuum pump service is on: vaState=START
	//  2) Switch on the 2nd stage RFE temperature controller: bXRFE2TempCtrlState=true
  //  3) Perform start-up checks. This step can take several seconds (>20 s) if the temperature and pressure optimal conditions are not already in place, otherwise the sequence jumps directly to the next step. During the checks (when performed) the 1st stage RFE temperature controller is switched off and the vacuum valve opened. All band capabilities are set to OPERATE-DEGRADED, .i.e. bXCapability=OPERATE-DEGRADED
  //  4) Request to switch on the compressor, i.e. SetHeliumState(‘start’) and check if it is running
  //  5) Switch on the cryocooler motors for all bands with motor speeds set to the default, i.e. b2CryoMotorState=true, b345CryoMotorState=true.
  //  6) If cryostat pressure is less than the cryo-pump pressure constant close the vacuum valve, i.e. vaValveState=true
  //  7) If the cryostat pressure does not increase over time, enter the cold transient 2 state 
	//  8) Switch on the 2nd stage amplifiers for all bands, i.e. bXAmp2HPowerState=true, bXAmp2VPowerState=true (X=1, ..., 5)
	//  9) Stay in the CT2 state until the 1st stage RFE temperature is smaller than the cold operational temperature constant
	// 10) Release the request for the vacuum pump to be on
	// 11) Switch on the 1st stage RFE temperature controller and set it to the user selected 1st stage RFE temperature set point. The FP*_AO enters the degraded state until the correct temperature setpoint and pressure is reached.
  // 12) LNAs are not enabled by default (i.e. left to user request)
  // 13) Set band capabilities to OPERATE-FULL, i.e. bXCapability=OPERATE-FULL (X=1, ..., 5)
  // 14) Set power state to FULL-POWER, i.e. powerState=FULL-POWER
  // 15) Set mode attribute to OPERATE, i.e. mode=OPERATE
  // 16) Set operateStatus attribute to COMPLETED and notify a change event to LMC



	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string mode_attr_name= "operatingMode";
	std::string powerState_attr_name= "powerState";
	std::vector<std::string> feed_mode_attr_names {
		"b1OperatingState",
		"b2OperatingState",	
		"b3OperatingState",
		"b4OperatingState",
		"b5OperatingState"
	};
	std::string va_state_attr_name = "vaOperatingState";
	std::string va_state_str= "STANDBY-LP";//"START";//CHECK!!!	
	std::string he_state_attr_name = "heOperatingState";
	std::string he_state_str= "STANDBY-LP";//"START";//CHECK!!!!		
	std::vector<std::string> cap_attr_names= {
		"b1CapabilityState",
		"b2CapabilityState",
		"b3CapabilityState",
		"b4CapabilityState",
		"b5CapabilityState",
	};
	std::vector<std::vector<std::string>> rfe1TempCtrl_attr_names= {
		{"b1LnaHTempCtrlState","b1LnaVTempCtrlState"},
		{"b2Rfe1TempCtrlState"},
		{"b3Rfe1TempCtrlState"},
		{"b4Rfe1TempCtrlState"},
		{"b5Rfe1TempCtrlState"}
	};
	std::vector<bool> hasRfe2TempCtrl {0,0,1,1,1};
	std::vector<std::string> rfe2TempCtrl_attr_names= {
		"b1Rfe2TempCtrlState",
		"b2Rfe2TempCtrlState",
		"b3Rfe2TempCtrlState",
		"b4Rfe2TempCtrlState",
		"b5Rfe2TempCtrlState"
	};
	std::vector<bool> hasVaValveState {0,1,1,1,1};
	std::vector<std::string> vaValveState_attr_names= {
		"b1VaValveState",
		"b2VaValveState",
		"b345VaValveState",
		"b345VaValveState",
		"b345VaValveState"
	};

	std::vector<std::string> amp2_h_attr_names= {
		"b1Amp2HPowerState",
		"b2Amp2HPowerState",
		"b3Amp2HPowerState",
		"b4Amp2HPowerState",
		"b5Amp2HPowerState"
	};	
	std::vector<std::string> amp2_v_attr_names= {
		"b1Amp2VPowerState",
		"b2Amp2VPowerState",
		"b3Amp2VPowerState",
		"b4Amp2VPowerState",
		"b5Amp2VPowerState"
	};
	std::vector<bool> hasCryoMotorState {0,1,1,1,1};
	std::vector<std::string> cryoMotorState_attr_names= {
		"b1CryoMotorState",
		"b2CryoMotorState",
		"b345CryoMotorState",
		"b345CryoMotorState",
		"b345CryoMotorState"
	};
	std::string mode_str= "OPERATE";
	std::string mode_safe_str= "STANDBY-LP";
	std::string mode_fault_str= "ERROR";
	std::string feed_mode_str= "OPERATE";
	std::string powerState_str= "FULL-POWER";
	std::string capability_str= "OPERATE-FULL";
	std::string capability_degraded_str= "OPERATE-DEGRADED";
	std::string capability_safe_str= "STANDBY";	
	std::string capability_faulty_str= "UNAVAILABLE";
	//===========================================================================
	
	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	int bandId= msg.bandId;
	
	//## Get device instance
	SPFEmulator* dev= static_cast<SPFEmulator*>(m_cfg.host_device);
	if(!dev){
		//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get access to device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}

	__DEBUG_LOG(dev,"Starting set feed to OPERATE ...");

	//## Set task status to RUNNING
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetFeedOperateStatus(bandId,"RUNNING","Set feed to OPERATE mode task started",true);
	}

	//=====================================================
	//==         INIT
	//=====================================================
	//Retrieve mode value corresponding to label
	Tango::DevShort feed_mode;
	Tango::DevShort feed_mode_fault;
	Utils_ns::TangoUtils::GetEnumAttrValue(feed_mode,dev,feed_mode_attr_names[bandId-1],feed_mode_str);	
	
	//Retrieve the capabilities value corresponding to label
	Tango::DevShort capability;
	Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_names[bandId-1],capability_str);
		
	Tango::DevShort capability_degraded;
	Utils_ns::TangoUtils::GetEnumAttrValue(capability_degraded,dev,cap_attr_names[bandId-1],capability_degraded_str);
		
	
	//=====================================================
	//==      CHECK CURRENT FEED MODE/CAPABILITY
	//=====================================================
	
	//Get current feed mode
	Tango::DevShort feed_mode_current;	
	if(dev->GetDynAttrValue(feed_mode_current,feed_mode_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<feed_mode_attr_names[bandId-1]<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}	
	

	//Get capability values
	Tango::DevShort capability_current;
	
	if(dev->GetDynAttrValue(capability_current,cap_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<cap_attr_names[bandId-1]<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}		
	
	std::string capability_current_str;	
	Utils_ns::TangoUtils::GetEnumAttrLabelValue(capability_current_str,dev,cap_attr_names[bandId-1],capability_current);
	
	//=====================================================
	//==         Set Vacuum Service to START
	//=====================================================
	Tango::DevShort va_state;
	Utils_ns::TangoUtils::GetEnumAttrValue(va_state,dev,va_state_attr_name,va_state_str);

	if(dev->SetDynAttrValue<Tango::DevEnum>(va_state,va_state_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<va_state_attr_name<<" to "<<va_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode failed in execution (cannot set vaState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	
	//=====================================================
	//==         ENABLE 2nd-stage temp controllers
	//=====================================================
	Tango::DevBoolean rfe2TempCtrl= true;
	if(hasRfe2TempCtrl[bandId-1] && dev->SetDynAttrValue<Tango::DevBoolean>(rfe2TempCtrl,rfe2TempCtrl_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<rfe2TempCtrl_attr_names[bandId-1]<<" to "<<rfe2TempCtrl<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set rfe2TempCtrl)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	
	//=====================================================
	//==        SET CAPABILITY
	//=====================================================
	//Set capability to OPERATE-DEGRADED
	if(capability_current_str!=capability_faulty_str && dev->SetSMDynAttrValue(capability_degraded,cap_attr_names[bandId-1],&capability_current)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cap_attr_names[bandId-1]<<" to "<<capability<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set capabilities)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	
	//=====================================================
	//==        OPEN VACUUM VALVE 
	//=====================================================
	Tango::DevBoolean vaValveState= true;
	if(hasVaValveState[bandId-1] && dev->SetSMDynAttrValue(vaValveState,vaValveState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<vaValveState_attr_names[bandId-1]<<" to "<<vaValveState<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set vaValveState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	
	//=====================================================
	//==         Set Helium Service to START
	//=====================================================
	Tango::DevShort he_state;
	Utils_ns::TangoUtils::GetEnumAttrValue(he_state,dev,he_state_attr_name,he_state_str);

	if(dev->SetDynAttrValue<Tango::DevEnum>(he_state,he_state_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<he_state_attr_name<<" to "<<he_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode failed in execution (cannot set heState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}

	//=====================================================
	//==         SET FEED OPERATE MODE TIME...
	//=====================================================
	//Simulate long set feed OPERATE mode time
	__DEBUG_LOG(dev,"Simulating set feed OPERATE time (sleeping for "<<task_duration<<" s...)");
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit go_full_perf...");

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedOperateStatus(bandId,"ABORTED","Wait condition signalled, exit set feed OPERATE mode task...",true);
			}
			return;
		}
	} //- leave critical section 

	//=====================================================
	//==        ENABLE CRYO-COOLERS
	//=====================================================
	Tango::DevBoolean cryocooler_enabled= true;
	if(hasCryoMotorState[bandId-1] && dev->SetDynAttrValue(cryocooler_enabled,cryoMotorState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cryoMotorState_attr_names[bandId-1]<<" to "<<cryocooler_enabled<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set cryoMotorState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	

	//=====================================================
	//==        CLOSE VACUUM VALVE 
	//=====================================================
	vaValveState= false;
	if(hasVaValveState[bandId-1] && dev->SetDynAttrValue(vaValveState,vaValveState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<vaValveState_attr_names[bandId-1]<<" to "<<vaValveState<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set vaValveState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	
	//=====================================================
	//==        SWITCH-ON 2nd-stage AMP 
	//=====================================================
	//Switch on the 2nd stage amplifiers for all bands, i.e. bXAmp2HPowerState=true, bXAmp2VPowerState=true (X=1, ..., 5)
	Tango::DevBoolean amp2_enabled= true;
	if(dev->SetDynAttrValue(amp2_enabled,amp2_h_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<amp2_h_attr_names[bandId-1]<<" to "<<amp2_enabled<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set amp2HPowerState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	
	if(dev->SetDynAttrValue(amp2_enabled,amp2_v_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<amp2_v_attr_names[bandId-1]<<" to "<<amp2_enabled<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set amp2VPowerState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	

	//=====================================================
	//==        SWITCH-ON 1st-stage temp controllers 
	//=====================================================
	Tango::DevBoolean rfe1TempCtrl= true;
	for(size_t i=0;i<rfe1TempCtrl_attr_names[bandId-1].size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(rfe1TempCtrl,rfe1TempCtrl_attr_names[bandId-1][i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe1TempCtrl_attr_names[bandId-1][i]<<" to "<<rfe1TempCtrl<<"!";
			__ERROR_LOG(dev,ss.str());	

			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set rfe1TempCtrl)",true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
		}
	}//end loop attrs per band

	//=====================================================
	//==        SET CAPABILITY
	//=====================================================
	//Set capability to OPERATE-FULL
	if(capability_current_str!=capability_faulty_str && dev->SetSMDynAttrValue(capability,cap_attr_names[bandId-1],&capability_current)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cap_attr_names[bandId-1]<<" to "<<capability<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set capabilities)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}
	
	//=====================================================
	//==        SET FEED MODE
	//=====================================================
	//Set mode to OPERATE and revert back to previous in case of failure
	if(dev->SetSMDynAttrValue(feed_mode,feed_mode_attr_names[bandId-1],&feed_mode_current)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<feed_mode_attr_names[bandId-1]<<" to "<<feed_mode<<"!";
		__ERROR_LOG(dev,ss.str());

		///Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetFeedOperateStatus(bandId,"FAILED","Set feed OPERATE mode command failed in execution (cannot set mode)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_operate_i"));
	}


	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to DISABLE and set status to COMPLETED
	{ //- enter critical section
		omni_mutex_lock guard(dev->m_mutex);
		__INFO_LOG(dev,"Set feed OPERATE mode command completed with success ...");
		dev->SetFeedOperateStatus(bandId,"COMPLETED","Set feed OPERATE mode command completed with success",true);
	} //- leave critical section 
	

}//close execute_go_feed_operate_i()




// ============================================================================
// SPFEmulatorTask::execute_go_feed_regeneration
// ============================================================================
void SPFEmulatorTask::execute_go_feed_regeneration (const FeedGoRegenerationMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{

 try {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_FEED_GO_REGENERATION_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_FEED_GO_REGENERATION_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_go_feed_regeneration_i) 
    throw;
  }
}//close execute_go_feed_regeneration()

// ============================================================================
// SPFEmulatorTask::execute_go_feed_regeneration_i
// ============================================================================
void SPFEmulatorTask::execute_go_feed_regeneration_i (const FeedGoRegenerationMsg& msg)
  throw (Tango::DevFailed)
{

	// == COMMAND BEHAVIOR ==
	// 1) Switch off the cryocooler motor.
	// 2) Release the request for compressor to be on.
  // 3) Adjust the 1st stage RFE temperature controller to the warm setting.
	// 4) Switch off the LNA’s and 2nd stage amplifiers.
	// 5) Request the vacuum pump service once the pressure reaches the PE_CryoPump value (P CPE )
	//		or the 1 st stage RFE temperature reaches the T_RegenerationAmbient value (T AMB ).
	// 6) Check that the vacuum pump is on.
	// 7) Wait until the manifold pressure decreases over time.
	// 8) Perform a VSC before opening the vacuum valve.
	// 9) Switch off the 1st stage RFE temperature controller when ambient is reached.
	//10) Enter into warm transient state in order to start cool down process again. (OPERATE step 3.7)
  //       - Wait until the cryostat pressure is less than the Cryocooler-ON-pressure constant (P CO ), and then continue to the cold states.
  //       - Cold states: Request to switch on the compressor.
  //       - Check whether the compressor is running (supply pressure > P CSO and status signal from compressor).
	//       - Switch on the cryocooler motor.
  //       - Check that the 1 st stage RFE temperature (T RFE1 ) decrease over time. If this happens the FP*_AO is now in the cold transient 1 state.
	//       - Remain in this state until the cryostat pressure is less than the cryo-pump pressure constant (P CP ). When this happens, close the vacuum valve
	//       - If the cryostat pressure does not increase over time, go into cold transient 2 state, else return to warm transient state to open the valve again. (step 3.5)
	//       - Switch on the 2 nd stage amplifiers.
  //       - Stay in the CT2 state until the 1 st stage RFE temperature is smaller than the cold operational temperature constant (T CO ). If the pressure rises above the
  //         P_CryoPumpRising constant (P CPR ), the cryopumping effect is not sufficient yet and the valve will be opened while the vacuum pump is still switched on. 
	//         The valve will be closed once the pressure drops below the P_CryoPumpSatisfy constant (P CPS ) value again.
	//       - Release the request for the vacuum pump to be on.
	//       - Switch on the 1 st stage RFE temperature controller and set it to the user selected 1 st stage RFE temperature set point. The FP*_AO enters the degraded 
	//         state until the correct temperature setpoint and pressure is reached. Once the 1 st stage RFE temperature is within range of the set point it will enter 
	//         the cold operational state and notify that the FP is available.
	//       - The user decides when to switch on the LNAs

	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string mode_attr_name= "operatingMode";
	std::string powerState_attr_name= "powerState";
	std::vector<std::string> feed_mode_attr_names {
		"b1OperatingState",
		"b2OperatingState",	
		"b3OperatingState",
		"b4OperatingState",
		"b5OperatingState"
	};
	std::string va_state_attr_name = "vaOperatingState";
	std::string va_state_str= "STANDBY-LP";//"START";//CHECK!!!
	std::string he_state_attr_name = "heOperatingState";
	std::string he_state_str= "STANDBY-LP";//"START";//CHECK!!!		
	std::vector<std::string> cap_attr_names= {
		"b1CapabilityState",
		"b2CapabilityState",
		"b3CapabilityState",
		"b4CapabilityState",
		"b5CapabilityState",
	};
	

	std::vector<std::string> rfe1DefaultSetPointAttrNames= {
		"b1DefaultLnaHTempSetPoint",
		"b1DefaultLnaVTempSetPoint",
		"b2DefaultRfe1TempSetPoint",
		"b3DefaultRfe1TempSetPoint",
		"b4DefaultRfe1TempSetPoint",
		"b5DefaultRfe1TempSetPoint"
	};	
	std::vector<std::string> rfe1SetPointAttrNames= {
		//"b1Rfe1TempSetPoint",	
		"b1LnaHTempSetPoint",
		"b1LnaVTempSetPoint",
		"b2Rfe1TempSetPoint",
		"b3Rfe1TempSetPoint",
		"b4Rfe1TempSetPoint",
		"b5Rfe1TempSetPoint"
	};

	std::vector<std::string> rfe2DefaultSetPointAttrNames= {
		//"b1DefaultRfe2TempSetPoint",	
		"b3DefaultRfe2TempSetPoint",
		"b4DefaultRfe2TempSetPoint",
		"b5DefaultRfe2TempSetPoint"
	};	
	std::vector<std::string> rfe2SetPointAttrNames= {
		//"b1Rfe2TempSetPoint",	
		"b3Rfe2TempSetPoint",
		"b4Rfe2TempSetPoint",
		"b5Rfe2TempSetPoint"
	};

	
	std::vector<std::vector<std::string>> rfe1TempCtrl_attr_names= {
		{"b1LnaHTempCtrlState","b1LnaVTempCtrlState"},
		{"b2Rfe1TempCtrlState"},
		{"b3Rfe1TempCtrlState"},
		{"b4Rfe1TempCtrlState"},
		{"b5Rfe1TempCtrlState"}
	};
	std::vector<bool> hasRfe2TempCtrl {0,0,1,1,1};
	std::vector<std::string> rfe2TempCtrl_attr_names= {
		"b1Rfe2TempCtrlState",
		"b2Rfe2TempCtrlState",
		"b3Rfe2TempCtrlState",
		"b4Rfe2TempCtrlState",
		"b5Rfe2TempCtrlState"
	};
	std::vector<bool> hasVaValveState {0,1,1,1,1};
	std::vector<std::string> vaValveState_attr_names= {
		"b1VaValveState",
		"b2VaValveState",
		"b345VaValveState",
		"b345VaValveState",
		"b345VaValveState"
	};
	std::vector<std::string> amp2_h_attr_names= {
		"b1Amp2HPowerState",
		"b2Amp2HPowerState",
		"b3Amp2HPowerState",
		"b4Amp2HPowerState",
		"b5Amp2HPowerState"
	};	
	std::vector<std::string> amp2_v_attr_names= {
		"b1Amp2VPowerState",
		"b2Amp2VPowerState",
		"b3Amp2VPowerState",
		"b4Amp2VPowerState",
		"b5Amp2VPowerState"
	};
	std::vector<bool> hasCryoMotorState {0,1,1,1,1};
	std::vector<std::string> cryoMotorState_attr_names= {
		"b1CryoMotorState",
		"b2CryoMotorState",
		"b345CryoMotorState",
		"b345CryoMotorState",
		"b345CryoMotorState"
	};
	std::vector<std::string> lnaHPowerState_attr_names= {
		"b1LnaHPowerState",
		"b2LnaHPowerState",
		"b3LnaHPowerState",
		"b4LnaHPowerState",
		"b5LnaHPowerState"
	};
	std::vector<std::string> lnaVPowerState_attr_names= {
		"b1LnaVPowerState",
		"b2LnaVPowerState",
		"b3LnaVPowerState",
		"b4LnaVPowerState",
		"b5LnaVPowerState"
	};
	std::string mode_str= "OPERATE";
	std::string mode_safe_str= "STANDBY-LP";
	std::string mode_fault_str= "ERROR";
	std::string feed_mode_regeneration_str= "REGENERATION";
	std::string feed_mode_str= "OPERATE";
	std::string powerState_str= "FULL-POWER";
	std::string capability_str= "OPERATE-FULL";
	std::string capability_degraded_str= "OPERATE-DEGRADED";
	std::string capability_safe_str= "STANDBY";	
	std::string capability_faulty_str= "UNAVAILABLE";
	//===========================================================================
	
	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	int bandId= msg.bandId;
	
	//## Get device instance
	SPFEmulator* dev= static_cast<SPFEmulator*>(m_cfg.host_device);
	if(!dev){
		//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get access to device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}

	__DEBUG_LOG(dev,"Starting set feed to regeneration ...");

	//## Set task status to RUNNING
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetFeedRegenerationStatus(bandId,"RUNNING","Set feed to REGENERATION mode task started",true);
	}

	//=====================================================
	//==         INIT
	//=====================================================
	//Retrieve mode value corresponding to label
	Tango::DevShort feed_mode;
	Tango::DevShort feed_mode_regeneration;	
	Utils_ns::TangoUtils::GetEnumAttrValue(feed_mode,dev,feed_mode_attr_names[bandId-1],feed_mode_str);	
	Utils_ns::TangoUtils::GetEnumAttrValue(feed_mode_regeneration,dev,feed_mode_attr_names[bandId-1],feed_mode_regeneration_str);

	//Retrieve the capabilities value corresponding to label
	Tango::DevShort capability;
	Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_names[bandId-1],capability_str);
		
	Tango::DevShort capability_degraded;
	Utils_ns::TangoUtils::GetEnumAttrValue(capability_degraded,dev,cap_attr_names[bandId-1],capability_degraded_str);
		
	
	//Retrieve rfe1 temp set points defaults
	/*
	std::vector<Tango::DevShort> rfe1SetPointDefaults {
		dev->b1Rfe1TempDefault,
		dev->b2Rfe1TempDefault,
		dev->b3Rfe1TempDefault,
		dev->b4Rfe1TempDefault,
		dev->b5Rfe1TempDefault
	};
	*/	
	

	//=====================================================
	//==      CHECK CURRENT FEED MODE/CAPABILITY
	//=====================================================
	//Get current feed mode
	Tango::DevShort feed_mode_current;	
	if(dev->GetDynAttrValue(feed_mode_current,feed_mode_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<feed_mode_attr_names[bandId-1]<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}	
	

	//Get capability values
	Tango::DevShort capability_current;
	
	if(dev->GetDynAttrValue(capability_current,cap_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<cap_attr_names[bandId-1]<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}		
	
	std::string capability_current_str;	
	Utils_ns::TangoUtils::GetEnumAttrLabelValue(capability_current_str,dev,cap_attr_names[bandId-1],capability_current);
	

	//=====================================================
	//==      SET FEED MODE TO REGENERATION
	//=====================================================
	//Set feed mode to REGENERATION and revert back to previous in case of failure
	if(dev->SetSMDynAttrValue(feed_mode_regeneration,feed_mode_attr_names[bandId-1],&feed_mode_current)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<feed_mode_attr_names[bandId-1]<<" to "<<feed_mode_regeneration<<"!";
		__ERROR_LOG(dev,ss.str());

		///Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Set feed regeneration command failed in execution (cannot set feed mode)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}

	//=====================================================
	//==         Switch OFF CRYOCOOLER
	//=====================================================
	Tango::DevBoolean cryo_motor_state= false;

	if(hasCryoMotorState[bandId-1] && dev->SetDynAttrValue<Tango::DevBoolean>(cryo_motor_state,cryoMotorState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cryoMotorState_attr_names[bandId-1]<<" to "<<cryo_motor_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration failed in execution (cannot set cryoMotorState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}

	//=====================================================
	//==         Switch OFF LNAs
	//=====================================================
	Tango::DevBoolean lna_power_state= false;

	if(dev->SetDynAttrValue<Tango::DevBoolean>(lna_power_state,lnaHPowerState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<lnaHPowerState_attr_names[bandId-1]<<" to "<<lna_power_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration failed in execution (cannot set lnaHPowerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}
	if(dev->SetDynAttrValue<Tango::DevBoolean>(lna_power_state,lnaVPowerState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<lnaVPowerState_attr_names[bandId-1]<<" to "<<lna_power_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration failed in execution (cannot set lnaVPowerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}
	
	//=====================================================
	//==         Switch OFF 2nd stage amplifier
	//=====================================================
	Tango::DevBoolean amp2_power_state= false;

	if(dev->SetDynAttrValue<Tango::DevBoolean>(amp2_power_state,amp2_h_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<amp2_h_attr_names[bandId-1]<<" to "<<amp2_power_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration failed in execution (cannot set amp2HPowerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}
	if(dev->SetDynAttrValue<Tango::DevBoolean>(amp2_power_state,amp2_v_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<amp2_v_attr_names[bandId-1]<<" to "<<amp2_power_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration failed in execution (cannot set amp2VPowerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}

	//=====================================================
	//==         Set Vacuum Service to START
	//=====================================================
	Tango::DevShort va_state;
	Utils_ns::TangoUtils::GetEnumAttrValue(va_state,dev,va_state_attr_name,va_state_str);

	if(dev->SetDynAttrValue<Tango::DevEnum>(va_state,va_state_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<va_state_attr_name<<" to "<<va_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration failed in execution (cannot set vaState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}

	//=====================================================
	//==        SET CAPABILITY
	//=====================================================
	//Set capability to OPERATE-DEGRADED
	if(capability_current_str!=capability_faulty_str && dev->SetSMDynAttrValue(capability_degraded,cap_attr_names[bandId-1],&capability_current)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cap_attr_names[bandId-1]<<" to "<<capability<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set capabilities)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_regeneration_i"));
	}

	//=====================================================
	//==         GO REGENERATION TIME...
	//=====================================================
	//Simulate Wait until the manifold pressure decreases over time
	__DEBUG_LOG(dev,"Go regeneration time...");
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit go_full_perf...");

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedRegenerationStatus(bandId,"ABORTED","Wait condition signalled, exit set feed regeneration task...",true);
			}
			return;
		}
	} //- leave critical section 


	//=====================================================
	//==        OPEN VACUUM VALVE 
	//=====================================================
	Tango::DevBoolean vaValveState= true;
	if(hasVaValveState[bandId-1] && dev->SetSMDynAttrValue(vaValveState,vaValveState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<vaValveState_attr_names[bandId-1]<<" to "<<vaValveState<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set vaValveState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}

	//=====================================================
	//==        SWITCH OFF 1st stage controllers
	//=====================================================
	Tango::DevBoolean rfe1TempCtrl= false;
	for(size_t i=0;i<rfe1TempCtrl_attr_names[bandId-1].size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(rfe1TempCtrl,rfe1TempCtrl_attr_names[bandId-1][i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe1TempCtrl_attr_names[bandId-1][i]<<" to "<<rfe1TempCtrl<<"!";
			__ERROR_LOG(dev,ss.str());	
	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set rfe1TempCtrl)",true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
		}
	}//end loop attrs per band

	//=====================================================
	//==        ENABLE CRYO-COOLERS
	//=====================================================
	Tango::DevBoolean cryocooler_enabled= true;
	if(hasCryoMotorState[bandId-1] && dev->SetDynAttrValue(cryocooler_enabled,cryoMotorState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cryoMotorState_attr_names[bandId-1]<<" to "<<cryocooler_enabled<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set cryoMotorState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_regeneration_i"));
	}
	

	//=====================================================
	//==        CLOSE VACUUM VALVE 
	//=====================================================
	vaValveState= false;
	if(hasVaValveState[bandId-1] && dev->SetSMDynAttrValue(vaValveState,vaValveState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<vaValveState_attr_names[bandId-1]<<" to "<<vaValveState<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set vaValveState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}

	//=====================================================
	//==        SWITCH-ON 2nd-stage AMP 
	//=====================================================
	//Switch on the 2nd stage amplifiers for all bands, i.e. bXAmp2HPowerState=true, bXAmp2VPowerState=true (X=1, ..., 5)
	Tango::DevBoolean amp2_enabled= true;
	if(dev->SetDynAttrValue(amp2_enabled,amp2_h_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<amp2_h_attr_names[bandId-1]<<" to "<<amp2_enabled<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set amp2HPowerState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}
	
	if(dev->SetDynAttrValue(amp2_enabled,amp2_v_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<amp2_v_attr_names[bandId-1]<<" to "<<amp2_enabled<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set amp2VPowerState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}
	
	//=====================================================
	//==         SET RFE SET POINT TO DEFAULT
	//=====================================================
	//Initialize RFE1 temp set point to default values
	/*
	Tango::DevShort rfe1SetPointDefaultValue= rfe1SetPointDefaults[bandId-1];
			
	if( dev->SetDynAttrValue<Tango::DevEnum>(rfe1SetPointDefaultValue,rfe1SetPointAttrNames[bandId-1])<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<rfe1SetPointAttrNames[bandId-1]<<" value!";
		__ERROR_LOG(dev,ss.str());
			
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set rfe1SetPoint)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i_i"));
	}	
	*/

	
	//Initialize RFE1 temp set point to default values	
	__DEBUG_LOG(dev,"Initializing SPF rfe1_tempSetPoint...");

	for(unsigned int i=0;i<rfe1SetPointAttrNames.size();i++){
		//- Get RFE default setpoint		
		Tango::DevShort rfe1SetPointDefaultValue;
		if(dev->GetDynAttrValue<Tango::DevShort>(rfe1SetPointDefaultValue,rfe1DefaultSetPointAttrNames[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<rfe1DefaultSetPointAttrNames[i]<<" value!";
			__ERROR_LOG(dev,ss.str());
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set rfe1SetPoint)",true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
		}	
	
		//- Set RFE setpoint to current default
		std::string rfe_attr_name= rfe1SetPointAttrNames[i];
		if( dev->SetDynAttrValue<Tango::DevEnum>(rfe1SetPointDefaultValue,rfe_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set rfe1SetPoint)",true);
			}
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
		}	

	}//end loop rfe setpoints


	//Initialize RFE2 temp set point to default values
	__DEBUG_LOG(dev,"Initializing SPF rfe2_tempSetPoint...");

	for(unsigned int i=0;i<rfe2SetPointAttrNames.size();i++){
		//- Get RFE default setpoint		
		Tango::DevShort rfe2SetPointDefaultValue;
		if(dev->GetDynAttrValue<Tango::DevShort>(rfe2SetPointDefaultValue,rfe2DefaultSetPointAttrNames[i])<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<rfe2DefaultSetPointAttrNames[i]<<" value!";
			__ERROR_LOG(dev,ss.str());	
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set rfe2SetPoint)",true);
			}
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
		}	
	
		//- Set RFE setpoint to current default
		std::string rfe_attr_name= rfe2SetPointAttrNames[i];
		if( dev->SetDynAttrValue<Tango::DevEnum>(rfe2SetPointDefaultValue,rfe_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe_attr_name<<" value!";
			__ERROR_LOG(dev,ss.str());
			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set rfe2SetPoint)",true);
			}
			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
		}	
	}//end loop rfe setpoints

	//=====================================================
	//==        SWITCH ON 1st stage controllers
	//=====================================================
	rfe1TempCtrl= true;
	for(size_t i=0;i<rfe1TempCtrl_attr_names[bandId-1].size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(rfe1TempCtrl,rfe1TempCtrl_attr_names[bandId-1][i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe1TempCtrl_attr_names[bandId-1][i]<<" to "<<rfe1TempCtrl<<"!";
			__ERROR_LOG(dev,ss.str());	

			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedRegenerationStatus(bandId,"FAILED","Go regeneration command failed in execution (cannot set rfe1TempCtrl)",true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
		}
	}//end loop attrs per band

	//=====================================================
	//==        SET CAPABILITY
	//=====================================================
	//Set capability to OPERATE-FULL
	if(capability_current_str!=capability_faulty_str && dev->SetSMDynAttrValue(capability,cap_attr_names[bandId-1],&capability_degraded)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cap_attr_names[bandId-1]<<" to "<<capability<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Set feed regeneration command failed in execution (cannot set capabilities)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}
	
	//=====================================================
	//==        SET FEED MODE TO OPERATE
	//=====================================================
	//Set feed mode to OPERATE and revert back to previous in case of failure
	if(dev->SetSMDynAttrValue(feed_mode,feed_mode_attr_names[bandId-1],&feed_mode_regeneration)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<feed_mode_attr_names[bandId-1]<<" to "<<feed_mode<<"!";
		__ERROR_LOG(dev,ss.str());

		///Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetFeedRegenerationStatus(bandId,"FAILED","Set feed regeneration command failed in execution (cannot set feed mode)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_regeneration_i"));
	}
	

	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to DISABLE and set status to COMPLETED
	{ //- enter critical section
		omni_mutex_lock guard(dev->m_mutex);
		__INFO_LOG(dev,"Go regeneration task completed with success ...");

		dev->SetFeedRegenerationStatus(bandId,"COMPLETED","Do regeneration completed with success",true);
	} //- leave critical section 
	


}//close execute_go_feed_regeneration_i()




// ============================================================================
// SPFEmulatorTask::execute_go_feed_standby_lp
// ============================================================================
void SPFEmulatorTask::execute_go_feed_standby_lp (const FeedGoStandbyLPMsg& msg, bool wait_cmd_reply)
  throw (Tango::DevFailed)
{

 try {
    if (wait_cmd_reply)
      this->wait_msg_handled(kEXECUTE_FEED_GO_STANDBY_LP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
    else
      this->post(kEXECUTE_FEED_GO_STANDBY_LP_CMD_MSG, msg, kDEFAULT_MSG_TMO_MSECS);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached).
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message (i.e. RxEmulatorTask::execute_go_feed_regeneration_i) 
    throw;
  }
}//close execute_go_feed_standby_lp()

// ============================================================================
// SPFEmulatorTask::execute_go_feed_standby_lp_i
// ============================================================================
void SPFEmulatorTask::execute_go_feed_standby_lp_i (const FeedGoStandbyLPMsg& msg)
  throw (Tango::DevFailed)
{

	// == COMMAND BEHAVIOR ==
	//1) Close the vacuum valve	
	//2) Switch off all the RF amplifier
	//3) Disable the cryocooler
	//4) Disable both temperature controllers
	//5) Disable LNAs 
	//6) Sleep 2s and switch off the vacuum pump
	//7) Set capability to STANDBY
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string mode_attr_name= "operatingMode";
	std::string powerState_attr_name= "powerState";
	std::vector<std::string> feed_mode_attr_names {
		"b1OperatingState",
		"b2OperatingState",	
		"b3OperatingState",
		"b4OperatingState",
		"b5OperatingState"
	};
	std::string va_state_attr_name = "vaOperatingState";
	std::string va_state_str= "STANDBY-LP";//"START";//CHECK!!!
	std::string he_state_attr_name = "heOperatingState";
	std::string he_state_str= "STANDBY-LP";//"START";//CHECK!!!		
	std::vector<std::string> cap_attr_names= {
		"b1CapabilityState",
		"b2CapabilityState",
		"b3CapabilityState",
		"b4CapabilityState",
		"b5CapabilityState",
	};
	std::vector<std::vector<std::string>> rfe1TempCtrl_attr_names= {
		{"b1LnaHTempCtrlState","b1LnaVTempCtrlState"},
		{"b2Rfe1TempCtrlState"},
		{"b3Rfe1TempCtrlState"},
		{"b4Rfe1TempCtrlState"},
		{"b5Rfe1TempCtrlState"}
	};
	std::vector<bool> hasRfe2TempCtrl {0,0,1,1,1};
	std::vector<std::string> rfe2TempCtrl_attr_names= {
		"b1Rfe2TempCtrlState",
		"b2Rfe2TempCtrlState",
		"b3Rfe2TempCtrlState",
		"b4Rfe2TempCtrlState",
		"b5Rfe2TempCtrlState"
	};
	std::vector<bool> hasVaValveState {0,1,1,1,1};
	std::vector<std::string> vaValveState_attr_names= {
		"b1VaValveState",
		"b2VaValveState",
		"b345VaValveState",
		"b345VaValveState",
		"b345VaValveState"
	};
	std::vector<std::string> amp2_h_attr_names= {
		"b1Amp2HPowerState",
		"b2Amp2HPowerState",
		"b3Amp2HPowerState",
		"b4Amp2HPowerState",
		"b5Amp2HPowerState"
	};	
	std::vector<std::string> amp2_v_attr_names= {
		"b1Amp2VPowerState",
		"b2Amp2VPowerState",
		"b3Amp2VPowerState",
		"b4Amp2VPowerState",
		"b5Amp2VPowerState"
	};
	std::vector<bool> hasCryoMotorState {0,1,1,1,1};
	std::vector<std::string> cryoMotorState_attr_names= {
		"b1CryoMotorState",
		"b2CryoMotorState",
		"b345CryoMotorState",
		"b345CryoMotorState",
		"b345CryoMotorState"
	};
	std::vector<std::string> lnaHPowerState_attr_names= {
		"b1LnaHPowerState",
		"b2LnaHPowerState",
		"b3LnaHPowerState",
		"b4LnaHPowerState",
		"b5LnaHPowerState"
	};
	std::vector<std::string> lnaVPowerState_attr_names= {
		"b1LnaVPowerState",
		"b2LnaVPowerState",
		"b3LnaVPowerState",
		"b4LnaVPowerState",
		"b5LnaVPowerState"
	};
	std::string mode_str= "OPERATE";
	std::string mode_safe_str= "STANDBY-LP";
	std::string mode_fault_str= "ERROR";
	std::string feed_mode_str= "STANDBY-LP";
	std::string powerState_str= "FULL-POWER";
	std::string capability_str= "STANDBY";
	std::string capability_degraded_str= "OPERATE-DEGRADED";
	std::string capability_safe_str= "STANDBY";	
	std::string capability_faulty_str= "UNAVAILABLE";
	//===========================================================================
	
	//## Get message info
	unsigned long int task_duration= msg.task_duration;
	bool enable_failure= msg.enable_failure;
	float failure_prob= msg.failure_probability;
	int bandId= msg.bandId;
	
	//## Get device instance
	SPFEmulator* dev= static_cast<SPFEmulator*>(m_cfg.host_device);
	if(!dev){
		//Throw exception (catchable in synchronous invocation)
		std::string errMsg("Failed to get access to device instance!");
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), errMsg.c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}

	__DEBUG_LOG(dev,"Starting set feed to STANDBY-LP mode ...");

	//## Set task status to RUNNING
	{ 	
		omni_mutex_lock guard(dev->m_mutex);
		dev->SetFeedStandbyLPStatus(bandId,"RUNNING","Set feed to STANDBY-LP mode started",true);
	}

	//=====================================================
	//==         INIT
	//=====================================================
	//Retrieve mode value corresponding to label
	Tango::DevShort feed_mode;
	Utils_ns::TangoUtils::GetEnumAttrValue(feed_mode,dev,feed_mode_attr_names[bandId-1],feed_mode_str);	
	
	//Retrieve the capabilities value corresponding to label
	Tango::DevShort capability;
	Utils_ns::TangoUtils::GetEnumAttrValue(capability,dev,cap_attr_names[bandId-1],capability_str);
		
	Tango::DevShort capability_degraded;
	Utils_ns::TangoUtils::GetEnumAttrValue(capability_degraded,dev,cap_attr_names[bandId-1],capability_degraded_str);
		
	
	//=====================================================
	//==      CHECK CURRENT FEED MODE/CAPABILITY
	//=====================================================
	//Get current feed mode
	Tango::DevShort feed_mode_current;	
	if(dev->GetDynAttrValue(feed_mode_current,feed_mode_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<feed_mode_attr_names[bandId-1]<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED",ss.str(),true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}	
	

	//Get capability values
	Tango::DevShort capability_current;
	
	if(dev->GetDynAttrValue(capability_current,cap_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<cap_attr_names[bandId-1]<<" value!";
		__ERROR_LOG(dev,ss.str());

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED",ss.str(),true);
		}

		//Throw exception
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}		
	
	std::string capability_current_str;	
	Utils_ns::TangoUtils::GetEnumAttrLabelValue(capability_current_str,dev,cap_attr_names[bandId-1],capability_current);
	
	//=====================================================
	//==        CLOSE VACUUM VALVE 
	//=====================================================
	Tango::DevBoolean vaValveState= false;
	if(hasVaValveState[bandId-1] && dev->SetSMDynAttrValue(vaValveState,vaValveState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<vaValveState_attr_names[bandId-1]<<" to "<<vaValveState<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set vaValveState)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}


	//=====================================================
	//==         Switch OFF 2nd stage amplifier
	//=====================================================
	Tango::DevBoolean amp2_power_state= false;

	if(dev->SetDynAttrValue<Tango::DevBoolean>(amp2_power_state,amp2_h_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<amp2_h_attr_names[bandId-1]<<" to "<<amp2_power_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set amp2HPowerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}
	if(dev->SetDynAttrValue<Tango::DevBoolean>(amp2_power_state,amp2_v_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<amp2_v_attr_names[bandId-1]<<" to "<<amp2_power_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set amp2VPowerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}

	

	//=====================================================
	//==         Switch OFF CRYOCOOLER
	//=====================================================
	Tango::DevBoolean cryo_motor_state= false;

	if(hasCryoMotorState[bandId-1] && dev->SetDynAttrValue<Tango::DevBoolean>(cryo_motor_state,cryoMotorState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cryoMotorState_attr_names[bandId-1]<<" to "<<cryo_motor_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set cryoMotorState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}


	//=====================================================
	//==         DISABLE 1st-stage temp controllers
	//=====================================================
	Tango::DevBoolean rfe1TempCtrl= false;
	for(size_t i=0;i<rfe1TempCtrl_attr_names[bandId-1].size();i++){
		if(dev->SetDynAttrValue<Tango::DevBoolean>(rfe1TempCtrl,rfe1TempCtrl_attr_names[bandId-1][i])<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<rfe1TempCtrl_attr_names[bandId-1][i]<<" to "<<rfe1TempCtrl<<"!";
			__ERROR_LOG(dev,ss.str());	

			//Set fail event attribute
			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set rfe1TempCtrl)",true);
			}

			THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
		}
	}//end loop attrs per band

	//=====================================================
	//==         DISABLE 2nd-state temp controllers
	//=====================================================
	Tango::DevBoolean rfe2TempCtrl= false;
	if(hasRfe2TempCtrl[bandId-1] && dev->SetDynAttrValue<Tango::DevBoolean>(rfe2TempCtrl,rfe2TempCtrl_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<rfe2TempCtrl_attr_names[bandId-1]<<" to "<<rfe2TempCtrl<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set rfe2TempCtrl)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}

	//=====================================================
	//==         Switch OFF LNAs
	//=====================================================
	Tango::DevBoolean lna_power_state= false;

	if(dev->SetDynAttrValue<Tango::DevBoolean>(lna_power_state,lnaHPowerState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<lnaHPowerState_attr_names[bandId-1]<<" to "<<lna_power_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set lnaHPowerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}
	if(dev->SetDynAttrValue<Tango::DevBoolean>(lna_power_state,lnaVPowerState_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<lnaVPowerState_attr_names[bandId-1]<<" to "<<lna_power_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set lnaVPowerState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}
	
	//=====================================================
	//==        SET CAPABILITY TO DEGRADED
	//=====================================================
	//Set capability to OPERATE-DEGRADED
	if(capability_current_str!=capability_faulty_str && dev->SetSMDynAttrValue(capability_degraded,cap_attr_names[bandId-1],&capability_current)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cap_attr_names[bandId-1]<<" to "<<capability<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set capabilities)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}


	//=====================================================
	//==         GO STANDBY_LP TIME...
	//=====================================================
	//Simulate Wait to ensure that the valve closing mechanism has been closed properly
	__DEBUG_LOG(dev,"Go standby time...");
	{ //- enter critical section
		yat::AutoMutex<> guard(m_data_lock);
		bool status= m_wait_cond->timed_wait(task_duration*1000);//in ms
		if(status){
			__INFO_LOG(dev,"Wait condition signalled, exit set feed STANDBY-LP task...");

			{ 	
				omni_mutex_lock guard(dev->m_mutex);
				dev->SetFeedStandbyLPStatus(bandId,"ABORTED","Wait condition signalled, exit set feed STANDBY-LP task...",true);
			}
			return;
		}
	} //- leave critical section 


	//=====================================================
	//==         Set Vacuum State to STOP
	//=====================================================
	Tango::DevShort va_state;
	Utils_ns::TangoUtils::GetEnumAttrValue(va_state,dev,va_state_attr_name,va_state_str);

	if(dev->SetDynAttrValue<Tango::DevEnum>(va_state,va_state_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<va_state_attr_name<<" to "<<va_state<<"!";
		__ERROR_LOG(dev,ss.str());	

		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed STANDBY-LP mode failed in execution (cannot set vaState)",true);
		}

		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}


	//=====================================================
	//==        SET CAPABILITY
	//=====================================================
	//Set capability to STANDBY
	if(capability_current_str!=capability_faulty_str && dev->SetSMDynAttrValue(capability,cap_attr_names[bandId-1],&capability_degraded)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<cap_attr_names[bandId-1]<<" to "<<capability<<"!";
		__ERROR_LOG(dev,ss.str());
				
		//Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed STANDBY-LP mode command failed in execution (cannot set capabilities)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}
	
	//=====================================================
	//==        SET FEED MODE TO STANDBY-LP
	//=====================================================
	//Set feed mode to STANDBY-LP and revert back to previous in case of failure
	if(dev->SetSMDynAttrValue(feed_mode,feed_mode_attr_names[bandId-1])<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<feed_mode_attr_names[bandId-1]<<" to "<<feed_mode<<"!";
		__ERROR_LOG(dev,ss.str());

		///Set fail event attribute
		{ 	
			omni_mutex_lock guard(dev->m_mutex);		
			dev->SetFeedStandbyLPStatus(bandId,"FAILED","Set feed to STANDBY-LP mode failed in execution (cannot set feed mode)",true);
		}
	
		THROW_DEVFAILED(_CPTC("DEVICE_ERROR"), ss.str().c_str(), _CPTC("SPFEmulatorTask::execute_go_feed_standby_lp_i"));
	}
	

	//=====================================================
	//==         UPDATE DEVICE STATE
	//=====================================================
	//## Set device state to DISABLE and set status to COMPLETED
	{ //- enter critical section
		omni_mutex_lock guard(dev->m_mutex);
		__INFO_LOG(dev,"Set feed to STANDBY-LP mode task completed with success ...");

		dev->SetFeedStandbyLPStatus(bandId,"COMPLETED","Set feed to STANDBY-LP mode completed with success",true);
	} //- leave critical section 

}//close execute_go_feed_standby_lp_i()

}//close namespace

