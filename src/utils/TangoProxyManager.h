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
* @file TangoProxyManager.h
* @class TangoProxyManager
* @brief Tango proxy manager
*
* Tango proxy manager
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef _TANGO_PROXY_MANAGER_h
#define _TANGO_PROXY_MANAGER_h 1

#ifndef _TANGO_PROXY_HELPER
	#include <TangoProxyHelper.h>
#endif

#ifndef _DEVICE_DATA_h
	#include <DeviceData.h>
#endif

#ifndef _EVT_SUBSCRIPTION_DATA_h
	#include <EvtSubscriptionData.h>
#endif

#ifndef _TASK_h
	#include <Task.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS
	#include <CodeUtils.h>
#endif

//Tango headers
#ifndef _TANGO_H
	#include <tango.h>
#endif

#ifndef _LOG4TANGO_H_
	#include <log4tango.h>
#endif

//Yat headers
#ifndef _YAT_GENERIC_CONTAINER_H_
	#include <yat/any/GenericContainer.h>
#endif

#ifndef _YAT_DATA_BUFFER_H_
	#include <yat/memory/DataBuffer.h>
#endif

#ifndef __YAT_TIME_H__
	#include <yat/time/Time.h>
#endif

#ifndef _YAT_TIMER_H_
	#include <yat/time/Timer.h>
#endif


namespace Utils_ns {

// ============================================================================
// class: TangoProxyManager
// ============================================================================
class TangoProxyManager {

	public:
		/** 
		\brief Constructor
 		*/
		TangoProxyManager(Tango::DeviceImpl* dev);
			
		/** 
		\brief Destructor
 		*/
		~TangoProxyManager();

	
	public:

		/** 
		\brief Find device proxy from the map
 		*/
		int GetProxyAdminDeviceName(std::string& admin_dev_name, std::string proxy_name);

		/** 
		\brief Find device proxy from the map
 		*/
		Tango::DeviceProxy* FindProxy(std::string proxy_name);
		
		/** 
		\brief Find proxy helper from the map
 		*/
		TangoProxyHelperPtr FindProxyHelper(std::string proxy_name);


		/** 
		\brief Add a device proxy helper to map
 		*/
		int RegisterProxy(std::string proxy_name,long int timeout=20);


		/** 
		\brief Add a device proxy helper to map
 		*/
		int UnregisterProxy(std::string proxy_name);

		/** 
		\brief Initialize proxy 
 		*/
		int InitProxy(std::string proxy_name);


		/** 
		\brief Monitor proxy
 		*/
		int MonitorProxy(std::string proxy_name);

		/** 
		\brief Monitor proxy and get proxy info
 		*/
		int MonitorProxy(std::string proxy_name,std::vector<std::string>& proxy_info);

		/** 
		\brief Subscribe proxy to events
 		*/
		int SubscribeProxyToEvents(std::string proxy_name,Tango::CallBack* event_cb);

		/** 
		\brief Monitor all proxies
 		*/
		int MonitorProxies();

		/** 
		\brief Monitor all proxies
 		*/
		int InitAndSubscribeProxies(Tango::CallBack* event_cb);

		/** 
		\brief Register event subscription data to collection
 		*/
		int RegisterEvtSubscriptions(std::string proxy_name,std::string attr_name,std::vector<Tango::EventType> event_types);

		/** 
		\brief Register event subscription data to collection
 		*/
		int UnregisterEvtSubscriptions(std::string proxy_name,std::string attr_name,std::vector<Tango::EventType> event_types);

		/** 
		\brief Register event handlers to subscription (they must be previously registered)
 		*/
		int RegisterEvtHandlers(std::string proxy_name,std::string attr_name,Tango::EventType event_type,std::vector<std::string> evt_handlers,long int execution_delay=0,int priority=Task::eMEDIUM_PRIORITY);
		
		/** 
		\brief Find event subscription 
 		*/
		EvtSubscriptionDataPtr FindEvtSubscription(std::string proxy_name,std::string attr_name,Tango::EventType event_type);

		/** 
		\brief Find event subscription with stringified event type
 		*/
		EvtSubscriptionDataPtr FindEvtSubscription(std::string proxy_name,std::string attr_name,std::string evt_type_str);

		/** 
		\brief Get event subscription attr info
 		*/
		Tango::AttributeInfoEx* GetEvtSubscriptionAttrInfo(std::string proxy_name,std::string attr_name,Tango::EventType event_type);
	
		/** 
		\brief Check if proxy has evt subscription and is subscribed to event
 		*/
		bool IsSubscribedToEvent(std::string proxy_name,std::string attr_name,Tango::EventType event_type);

		/** 
		\brief Get registered proxies
 		*/
		const std::vector<std::string> GetRegisteredProxyNames();

		/** 
		\brief Get registered events
 		*/
		const std::vector<std::string> GetRegisteredEventNames();

		/** 
		\brief Execute command synchronously on proxy
 		*/
		void ExecuteCommand(Tango::DeviceData& dout,std::string proxy_name,std::string cmd_name);
		/** 
		\brief Execute command synchronously on proxy
 		*/
		void ExecuteCommand(Tango::DeviceData& dout,std::string proxy_name,std::string cmd_name,Tango::DeviceData& din);

		/** 
		\brief Execute command asynchronously on proxy and return cmd id
 		*/
		void ExecuteCommandAsynch(long& cmd_id,std::string proxy_name,std::string cmd_name);

		/** 
		\brief Execute command asynchronously on proxy and return cmd id
 		*/
		void ExecuteCommandAsynch(long& cmd_id,std::string proxy_name,std::string cmd_name,Tango::DeviceData& din);

		
		/** 
		\brief Get asynchronous command response
 		*/
		void GetAsynchCommandReply(Tango::DeviceData& dout,std::string proxy_name,long cmd_id,long timeout);
	
		/** 
		\brief Execute asynchronous commands on proxy and collect response with timeout
 		*/
		void ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::vector<std::string> proxy_names,std::vector<std::string> cmd_names,std::vector<Tango::DeviceData>& din,int reply_timeout=1000,int nretries=3);

		/** 
		\brief Execute asynchronous commands on proxy list and collect response with timeout
 		*/
		void ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::vector<std::string> proxy_names,std::string cmd_name,Tango::DeviceData& din,int reply_timeout=1000,int nretries=3);

		/** 
		\brief Execute asynchronous commands on proxy list without argin and collect response with timeout
 		*/
		void ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::vector<std::string> proxy_names,std::string cmd_name,int reply_timeout=1000,int nretries=3);

		/** 
		\brief Execute asynchronous commands on proxy list without argin and collect response with timeout
 		*/
		void ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::vector<std::string> proxy_names,std::vector<std::string> cmd_names,int reply_timeout=1000,int nretries=3);


		/** 
		\brief Execute asynchronous commands on proxy and collect response with timeout
 		*/
		void ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::string proxy_name,std::vector<std::string> cmd_names,std::vector<Tango::DeviceData>& din,int reply_timeout=1000,int nretries=3);


		/** 
		\brief Get proxy attribute info
 		*/
		int GetProxyAttributeInfo(Tango::AttributeInfoEx& attr_info,std::string proxy_name,std::string attr_name);

		/** 
		\brief Read attribute from proxy
 		*/
		template<typename T>
		void ReadAttribute(T& attr_value,std::string proxy_name,std::string attr_name,int timeout=3000){
			//Find proxy
			TangoProxyHelperPtr proxy_helper= FindProxyHelper(proxy_name);
			if(!proxy_helper){
				std::stringstream ss;
				ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
				__WARN_LOG(m_dev,ss.str());
				_THROW_TANGO_EXCEPTION("READ_ATTRIBUTE_FAILED",ss.str())
			}

			//Read attribute from proxy
			try{
				proxy_helper->ReadAttribute(attr_value,attr_name,timeout);
			}
			catch(...){
				throw;
			}
		}//close ReadAttribute()
		
		/** 
		\brief Write attribute from proxy
 		*/
		template<typename T>
		void WriteAttribute(std::string proxy_name,std::string attr_name,T& attr_value,int timeout=3000){
			//Find proxy
			TangoProxyHelperPtr proxy_helper= FindProxyHelper(proxy_name);
			if(!proxy_helper){
				std::stringstream ss;
				ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
				__WARN_LOG(m_dev,ss.str());
				_THROW_TANGO_EXCEPTION("WRITE_ATTRIBUTE_FAILED",ss.str())
			}

			//Read attribute from proxy
			try{
				proxy_helper->WriteAttribute(attr_value,attr_name,timeout);
			}
			catch(...){
				throw;
			}
		}//close WriteAttribute()

		/** 
		\brief Write attributes from proxy
 		*/
		void WriteAttributes(std::string proxy_name,std::vector<Tango::DeviceAttribute>& da_list,int timeout=3000);

		/** 
		\brief Write attribute from proxy
 		*/
		template<typename T>
		void WriteAttributeAsynch(T& attr_value,std::string proxy_name,std::string attr_name,int reply_timeout=1000,int nretries=3){
			//Find proxy
			TangoProxyHelperPtr proxy_helper= FindProxyHelper(proxy_name);
			if(!proxy_helper){
				std::stringstream ss;
				ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
				__WARN_LOG(m_dev,ss.str());
				_THROW_TANGO_EXCEPTION("WRITE_ATTRIBUTE_ASYNCH_FAILED",ss.str())
			}

			//Read attribute from proxy
			try{
				proxy_helper->WriteAttributeAsynch(attr_value,attr_name,reply_timeout,nretries);
			}
			catch(...){
				throw;
			}
		}//close WriteAttributeAsynch()

		/** 
		\brief Mark proxy offline
 		*/
		int SetProxyOffline(std::string proxy_name);

	private:		
		/** 
		\brief Find proxy helper from the map (internal method)
 		*/
		TangoProxyHelperPtr FindProxyHelper_i(std::string proxy_name);

		/** 
		\brief Find proxy from the map (internal method)
 		*/
		Tango::DeviceProxy* FindProxy_i(std::string proxy_name);

	private:
		//Mutex & timers
		yat::Mutex m_lock;

		//Device & proxy helper map
		Tango::DeviceImpl* m_dev;
		TangoProxyHelperMap m_proxyHelperMap;
};


}//close namespace

#endif

