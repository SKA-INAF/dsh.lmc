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
* @file DSEmulatorTask.h
* @class DSEmulatorTask
* @brief DSEmulatorTask
*
* DSEmulatorTask
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef _DS_EMULATOR_TASK_H_
#define _DS_EMULATOR_TASK_H_

#include "DSEmulator.h"

#include <tango.h>

#include <yat/any/GenericContainer.h>
#include <yat/memory/DataBuffer.h>
#include <yat4tango/DeviceTask.h>

#include <yat4tango/DynamicAttributeManager.h>

namespace DSEmulator_ns {

struct StartupMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
};

struct StowMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
};

struct MoveIndexerMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
	short indexer_position;
};

struct StandbyLPMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
	bool checkPreConditions;
};

struct StandbyFPMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
};

struct PointMsg {
	unsigned long int task_duration;//average duration of startup task
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault in configure)
};

struct ShutdownMsg {
	unsigned long int task_duration;//average duration of task
	unsigned long int shutdown_time;
	bool enable_failure;//enable/disable failure simulation
	float failure_probability;//fault probability [0,1] (to emulate a fault )
};


// ============================================================================
// class: DSEmulatorTask
// ============================================================================
class DSEmulatorTask : public yat4tango::DeviceTask {
	
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
		DSEmulatorTask (const Config & cfg);

		//- Destructor
		virtual ~DSEmulatorTask ();

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
  	void execute_startup (const StartupMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device stow method (with synchronous mode option)
  	void execute_stow (const StowMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device stow method (with synchronous mode option)
  	void execute_stow_simple (const StowMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device move indexer method (with synchronous mode option)
  	void execute_move_indexer (const MoveIndexerMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device set standbylp method (with synchronous mode option)
  	void execute_set_standby_lp (const StandbyLPMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device set standbyfp method (with synchronous mode option)
  	void execute_set_standby_fp (const StandbyFPMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device shutdown method (with synchronous mode option)
  	void execute_shutdown (const ShutdownMsg& msg, bool wait_cmd_reply = false)
    	throw (Tango::DevFailed);

		//- execute the device set point method (with synchronous mode option)
  	void execute_set_point (const PointMsg& msg, bool wait_cmd_reply = false)
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
  	void execute_startup_i (const StartupMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the device stow method
  	void execute_stow_i (const StowMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the device simple stow method
  	void execute_stow_simple_i (const StowMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the device move indexer method
  	void execute_move_indexer_i (const MoveIndexerMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the device set standbylp method
  	void execute_set_standby_lp_i (const StandbyLPMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the device set standbyfp method
  	void execute_set_standby_fp_i (const StandbyFPMsg& msg)
    	throw (Tango::DevFailed); 

		//- execute the device set point method
  	void execute_set_point_i (const PointMsg& msg)
    	throw (Tango::DevFailed);

		//- execute the device shutdown (power cut) method
		void execute_shutdown_i (const ShutdownMsg& msg)
    	throw (Tango::DevFailed);

	public:
		//- the task's configuration 
  	Config m_cfg;

	private:
		//- a mutex to protect some data against "race condition"
  	yat::Mutex m_data_lock; 

		//- a condition for timed wait
		yat::Condition* m_wait_cond;
  	
		
};//close DSEmulatorTask class

}//close namespace 

#endif
