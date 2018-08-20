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
* @file ProxyMonitorTask.h
* @class ProxyMonitorTask
* @brief ProxyMonitorTask
*
* ProxyMonitorTask
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef _PROXY_MONITOR_TASK_H_
#define _PROXY_MONITOR_TASK_H_

#ifndef LMCDevice_H
	#include <LMCDevice.h>
#endif

#ifndef _TANGO_H
	#include <tango.h>
#endif

//Yat headers
#ifndef _YAT_GENERIC_CONTAINER_H_
	#include <yat/any/GenericContainer.h>
#endif

#ifndef _YAT_DATA_BUFFER_H_
	#include <yat/memory/DataBuffer.h>
#endif

#ifndef _DEVICE_TASK_H_
	#include <yat4tango/DeviceTask.h>
#endif

//Forward declarations
namespace Utils_ns {
class TangoProxyManager;
}


namespace LMCDevice_ns {

//Define messages
//...

// ============================================================================
// class: ProxyMonitorTask
// ============================================================================
class ProxyMonitorTask : public yat4tango::DeviceTask {
	
	public:

		//===================================================
		//== the task's configuration data struct ==
  	typedef struct Config {
    	//-default ctor
    	Config ()
      	: task_activity_period_ms(1000), host_device(0), proxy_manager(0)
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
				proxy_manager= src.proxy_manager;
      	return *this;
    	}

			//- period of task's periodic activity in msecs 
    	size_t task_activity_period_ms;
    	//- the Tango device hosting this task
    	//Tango::DeviceImpl * host_device;
			LMCDevice* host_device;
	
			//- Enable periodic message in task
			bool enable_periodic_msg;
			//- Device proxy manager
			Utils_ns::TangoProxyManager* proxy_manager;
  	} Config;
		//================================================================

	public:
		//- Constructor
		ProxyMonitorTask (const Config & cfg);

		//- Destructor
		virtual ~ProxyMonitorTask ();

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

		

	public:
		//- the task's configuration 
  	Config m_cfg;

	private:
		//- a mutex to protect some data against "race condition"
  	yat::Mutex m_data_lock; 

		//- a condition for timed wait
		yat::Condition* m_wait_cond;
  	
		
};//close ProxyMonitorTask class

}//close namespace 

#endif
