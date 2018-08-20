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
* @file NagiosListenerThread.h
* @class NagiosListenerThread
* @brief NagiosListenerThread
*
* NagiosListenerThread
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef _NAGIOS_LISTENER_THREAD_H
#define _NAGIOS_LISTENER_THREAD_H

#include <tango.h>

//#include <LMCMonitor.h>

namespace Utils_ns {
	class SocketClient;
	struct SBuffer;
}

namespace LMCMonitor_ns {
  
class LMCMonitor;
class SocketClient;

class NagiosListenerThread : public omni_thread {
  	
  public:
		/** 
		\brief Constructor
 		*/
    NagiosListenerThread(LMCMonitor* dev,std::string query_handler_path);

		/** 
		\brief Destructor
 		*/
    ~NagiosListenerThread();

	protected:
		/** 
		\brief Nagios host status code
 		*/
    enum HostStatus {
			UP= 0,
			DOWN= 1,
			UNREACHABLE= 2
		};

		/** 
		\brief Nagios service status code
 		*/
		enum ServiceStatus {
			OK= 0,
			WARNING= 1,
			CRITICAL= 2,
			UNKNOWN= 3
		};

	public:	
		/** 
		\brief Set Nagios query handler path
 		*/
		void SetNagiosQueryHandlerPath(std::string path) {m_nagiosQueryHandlerPath= path;}

		/** 
		\brief Set Nagios attr poll period
 		*/
		void SetNagiosAttrPollPeriod(Tango::DevLong period) {m_nagiosAttrPollPeriod= period;}	
		/** 
		\brief Set Nagios attr periodic event period
 		*/
		void SetNagiosAttrPeriodicEventPeriod(Tango::DevLong period) {m_nagiosAttrPeriodicEventPeriod= period;}
		/** 
		\brief Set Nagios attr archive event period
 		*/
		void SetNagiosAttrArchiveEventPeriod(Tango::DevLong period) {m_nagiosAttrArchiveEventPeriod= period;}

	protected:

		/** 
		\brief Convert host check code to string
 		*/
		int HostCheckToStr(std::string& code_str,int code);

		/** 
		\brief Convert service check code to string
 		*/
		int ServiceCheckToStr(std::string& code_str,int code);
		/** 
		\brief Create Nagios check dyn attr
 		*/
		int CreateNagiosCheckDynAttr(std::string attr_name,long poll_period,long periodic_event_period,long archive_event_period,bool isFullInfoAttr);

		/** 
		\brief Process event
 		*/
		int ProcessEvent(std::string& msg);

	private:

		/** 
		\brief Endless thread loop
 		*/
    void* run_undetached(void *arg);
    
	private:	

  	LMCMonitor* m_dev;
    Utils_ns::SocketClient* m_SocketClient;

		std::string m_nagiosQueryHandlerPath;
		Tango::DevLong m_nagiosAttrPollPeriod;
		Tango::DevLong m_nagiosAttrPeriodicEventPeriod;
		Tango::DevLong m_nagiosAttrArchiveEventPeriod;
		
	friend class LMCMonitor;

};//close class


}//close namespace

#endif

