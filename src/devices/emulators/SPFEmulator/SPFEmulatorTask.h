

#ifndef _SPF_EMULATOR_TASK_H_
#define _SPF_EMULATOR_TASK_H_

#include <SPFEmulator.h>

#include <tango.h>

#include <yat/any/GenericContainer.h>
#include <yat/memory/DataBuffer.h>
#include <yat4tango/DeviceTask.h>

#include <yat4tango/DynamicAttributeManager.h>

namespace SPFEmulator_ns {

struct GoOperationalMsg {
	unsigned long int task_duration;//average duration of configure task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
};

struct GoStandbyLPMsg {
	unsigned long int task_duration;//average duration of configure task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
};

struct GoMaintenanceMsg {
	unsigned long int task_duration;//average duration of configure task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
};

struct StartupMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
};

struct FeedGoOperateMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
	int bandId;//band to be set to FULL PERF (-1 means all)
};

struct FeedGoRegenerationMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
	int bandId;//band to be set to REGENERATION (-1 means all)
};

struct FeedGoStandbyLPMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
	int bandId;//band to be set to FULL PERF (-1 means all)
};

// ============================================================================
// class: SPFEmulatorTask
// ============================================================================
class SPFEmulatorTask : public yat4tango::DeviceTask {
	
	public:

		//===================================================
		//== the task's configuration data struct ==
  	typedef struct Config {
    	//-default ctor
    	Config ()
      	: task_activity_period_ms(1000), host_device(0)
    	{}

    	//- copy ctor
    	Config (const Config & src) { 
      	*this = src;
    	}

    	//- operator=
    	const Config & operator= (const Config & src) { 
      	if (&src == this)
        	return *this;
      	task_activity_period_ms = src.task_activity_period_ms;
      	host_device = src.host_device;
				enable_periodic_msg= src.enable_periodic_msg;
      	return *this;
    	}

			//- period of task's periodic activity in msecs 
    	size_t task_activity_period_ms;
    	//- the Tango device hosting this task
    	Tango::DeviceImpl * host_device;
			//- Enable periodic message in task
			bool enable_periodic_msg;
  	} Config;
		//================================================================

	public:
		//- Constructor
		SPFEmulatorTask (const Config & cfg);

		//- Destructor
		virtual ~SPFEmulatorTask ();

	public:
		//- specialization of yat4tango::DeviceTask::go
  	//---------------------------------------------------------------------------------------
  	//- TANGO THREADING CONSTRAINT: THIS METHOD IS EXPECTED TO BE CALLED FROM <INIT_DEVICE>
  	//- OR (AT LEAST) FROM A TANGO DEVICE THREAD (SEE THE TANGO DEVICE MONITOR FOR DETAILS)
  	//---------------------------------------------------------------------------------------
  	virtual void go (size_t tmo_ms = kDEFAULT_MSG_TMO_MSECS)
    	throw (Tango::DevFailed);

		//- specialization of yat4tango::DeviceTask::exit
  	//---------------------------------------------------------------------------------------
  	//- TANGO THREADING CONSTRAINT: THIS METHOD IS EXPECTED TO BE CALLED FROM <INIT_DEVICE>
  	//- OR (AT LEAST) FROM A TANGO DEVICE THREAD (SEE THE TANGO DEVICE MONITOR FOR DETAILS)
  	//---------------------------------------------------------------------------------------
  	virtual void exit ()
    	throw (Tango::DevFailed);

  	//- starts task's periodic activity (a <tmo_ms> of 0 means asynchronous exec)
  	void start_periodic_activity (size_t tmo_ms = 0)
    	throw (Tango::DevFailed);

  	//- stops task's periodic activity (a <tmo_ms> of 0 means asynchronous exec)
 	 	void stop_periodic_activity (size_t tmo_ms = 0)
    	throw (Tango::DevFailed);

  	//- execute the specified cmd on the group (with synchronous mode option)
  	void execute_cmd (const std::string& cmd_name, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed); 

		//- execute the device startup method (with synchronous mode option)
  	//void execute_startup (const unsigned long int& startupTime, bool wait_cmd_reply = false)
		void execute_startup (const StartupMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device go operational method (with synchronous mode option)
  	void execute_go_operational (const GoOperationalMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device go standbyLP method (with synchronous mode option)
  	void execute_go_standbylp (const GoStandbyLPMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device go maintenance method (with synchronous mode option)
  	void execute_go_maintenance (const GoMaintenanceMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the set feed operate task (with synchronous mode option)
		void execute_go_feed_operate (const FeedGoOperateMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the set feed regeneration task (with synchronous mode option)
		void execute_go_feed_regeneration (const FeedGoRegenerationMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);
	
		//- execute the set feed standbylp task (with synchronous mode option)
		void execute_go_feed_standby_lp (const FeedGoStandbyLPMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- signal waiting cond
		void signal_wait(){
			if(m_wait_cond) m_wait_cond->signal();
		}
	
	protected:
		//- process_message (implements yat4tango::DeviceTask pure virtual method)
		virtual void process_message (yat::Message& msg)
    	throw (Tango::DevFailed);

	private:
		//- encapsulates the periodic activity
  	void periodic_job_i ()
    	throw (Tango::DevFailed);

		//- execute the specified command on the group
  	void execute_cmd_i (const std::string& cmd_name)
    	throw (Tango::DevFailed); 

		//- execute the device startup method
  	//void execute_startup_i (const unsigned long int& startupTime)
		void execute_startup_i (const StartupMsg& startupTime)
    	throw (Tango::DevFailed); 

		//- execute the device go operational method
  	void execute_go_operational_i (const GoOperationalMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the device go standbylp method
  	void execute_go_standbylp_i (const GoStandbyLPMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the device go maintenance method
  	void execute_go_maintenance_i (const GoMaintenanceMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the set feed operate task
  	void execute_go_feed_operate_i (const FeedGoOperateMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the set feed regeneration task
  	void execute_go_feed_regeneration_i (const FeedGoRegenerationMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the set feed standbylp task
  	void execute_go_feed_standby_lp_i (const FeedGoStandbyLPMsg& msg)
    	throw (Tango::DevFailed); 

	public:
		//- the task's configuration 
  	Config m_cfg;

	private:
		//- a mutex to protect some data against "race condition"
  	yat::Mutex m_data_lock;

		//- a condition for timed wait
		yat::Condition* m_wait_cond;
  	
};//close SPFEmulatorTask class

}//close namespace 

#endif
