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
* @file TangoProxyHelper.h
* @class TangoProxyHelper
* @brief Tango proxy helper
*
* Tango proxy helper
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef _TANGO_PROXY_HELPER_h
#define _TANGO_PROXY_HELPER_h 1

#ifndef _EVT_SUBSCRIPTION_DATA_h
	#include <EvtSubscriptionData.h>
#endif

#ifndef _TASK_h
	#include <Task.h>
#endif

#ifndef _DEVICE_DATA_h
	#include <DeviceData.h>
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS_h
	#include <CodeUtils.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
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
// class: TangoProxyHelper
// ============================================================================
class TangoProxyHelper {

	public:
		/** 
		\brief Constructor
 		*/
		TangoProxyHelper(Tango::DeviceImpl* dev,std::string proxy_name);
		/** 
		\brief Destructor (do not delete DeviceImpl pointer)
 		*/
		~TangoProxyHelper();

		
		/** 
		\brief == Operator 
 		*/
		bool operator==(const TangoProxyHelper& rhs) const { 
			return rhs.m_proxy_name==this->m_proxy_name;
		}
		/** 
		\brief == Operator for pointers
 		*/
		bool operator==(const TangoProxyHelper* rhs) const { 
			return rhs->m_proxy_name==this->m_proxy_name;
		}

		/** 
		\brief proxy run status codes
 		*/
		enum ProxyRunState {
			ePROXY_ONLINE= 0,
			ePROXY_SOFT_OFFLINE= 1,
			ePROXY_HARD_OFFLINE= 2
		};

	public:
		/** 
		\brief Init proxy
 		*/
		int InitProxy();
		
		/** 
		\brief Return proxy
 		*/
		Tango::DeviceProxy* GetProxy(){return m_proxy;}

		/** 
		\brief Get admin device name
 		*/
		int GetAdminDeviceName(std::string& admin_dev_name);

		/** 
		\brief Return proxy info (to be set to device attribute)
 		*/	
		int GetProxyInfo(std::vector<std::string>& proxy_info);

		/** 
		\brief Return proxy attribute info 
 		*/	
		int GetProxyAttributeInfo(Tango::AttributeInfoEx& attr_info,std::string attr_name);	

		/** 
		\brief Execute command synchronously with argin
 		*/	
		void ExecuteCommand(Tango::DeviceData& dout,std::string cmd_name,Tango::DeviceData& din);
		/** 
		\brief Execute command synchronously without argin
 		*/	
		void ExecuteCommand(Tango::DeviceData& dout,std::string cmd_name);
		/** 
		\brief Execute asynchronous command with argin and return cmd id
 		*/	
		void ExecuteCommandAsynch(long& cmd_id,std::string cmd_name,Tango::DeviceData& din);
		/** 
		\brief Execute asynchronous command without argin and return cmd id
 		*/	
		void ExecuteCommandAsynch(long& cmd_id,std::string cmd_name);

		/** 
		\brief Get asynchronous command response
 		*/
		void GetAsynchCommandReply(Tango::DeviceData& dout,long cmd_id,long timeout);
		
		/** 
		\brief Read attributes
 		*/
		template<typename T>
		void ReadAttribute(T& attr_value,std::string attr_name,int timeout){
			//Set timeout in proxy
			if(timeout>0){
				m_proxy->set_timeout_millis(timeout);
			}

			//Read attribute from device
			Tango::DeviceAttribute da;
			try {
				da= m_proxy->read_attribute(attr_name);
			}	
			catch(...){
				throw;
			}

			//Extract data to type
			try{
				da>>attr_value;
			}
			catch(...){
				throw;
			}	
		}//close ReadAttribute()

		/** 
		\brief Write attribute
 		*/
		template<typename T>
		void WriteAttribute(std::string attr_name,T& attr_value,int timeout){
			//Set timeout in proxy
			if(timeout>0){
				m_proxy->set_timeout_millis(timeout);
			}

			//Read attribute from device
			try {
				Tango::DeviceAttribute da(attr_name, attr_value);
				m_proxy->write_attribute(da);
			}	
			catch(...){
				throw;
			}
		}//close WriteAttribute()

		/** 
		\brief Write attributes 
 		*/
		void WriteAttributes(std::vector<Tango::DeviceAttribute>& da_list,int timeout);

	
		/** 
		\brief Read attributes
 		*/
		template<typename T>
		void ReadAttributeAsynch(T& attr_value,std::string attr_name,int reply_timeout,int nretries){
			 
			//Read attribute from device
			long asynch_id= -1;			
			try {
				asynch_id= m_proxy->read_attribute_asynch(attr_name);
			}	
			catch(...){
				throw;
			}

			bool replyArrived= false;
			Tango::DeviceData reply_data; 

			for(int k=0;k<nretries;k++){
			
				//Try to get response		
    		try { 
					reply_data= m_proxy->read_attribute_reply(asynch_id,reply_timeout);
    		} 
    		catch(Tango::AsynReplyNotArrived) { 
    			__DEBUG_LOG(m_dev,"Async reply (id="<<asynch_id<<") not arrived yet, retry...");
					continue;
    		} 
      	catch (Tango::DevFailed &e) {//Proxy replied with exception, re-throw to clients
      		Tango::Except::print_exception(e); 
					std::stringstream ss;
					ss<<"Tango exception occurred when retrieving read_async response, retry...";
					__DEBUG_LOG(m_dev,ss.str());
					continue;
				}
				
				replyArrived= true;
				break;
			}//end loop nretries

			//Check if response arrived
			if(!replyArrived){
				std::string errMsg("Failed to retrieve asynch command response!");
				__WARN_LOG(m_dev,errMsg);
				_THROW_TANGO_EXCEPTION("READ_ASYNCH_ATTR_FAILED",errMsg);					
			}

			//Extract data to type
			Tango::DeviceAttribute da;
			try{
				da>>attr_value;
			}
			catch(...){
				throw;
			}	
		}//close ReadAttributeAsynch()

		/** 
		\brief Write attributes
 		*/
		template<typename T>
		void WriteAttributeAsynch(std::string attr_name,T& attr_value,int reply_timeout,int nretries){
			 
			//Read attribute from device
			long asynch_id= -1;			
			try {
				Tango::DeviceAttribute da(attr_name, attr_value);
				asynch_id= m_proxy->write_attribute_asynch(da);
			}	
			catch(...){
				throw;
			}

			bool replyArrived= false;
			
			for(int k=0;k<nretries;k++){
			
				//Try to get response		
    		try { 
					m_proxy->write_attribute_reply(asynch_id,reply_timeout);
    		} 
    		catch(Tango::AsynReplyNotArrived) { 
    			__DEBUG_LOG(m_dev,"Async reply (id="<<asynch_id<<") not arrived yet, retry...");
					continue;
    		} 
      	catch (Tango::DevFailed &e) {//Proxy replied with exception, re-throw to clients
      		Tango::Except::print_exception(e); 
					std::stringstream ss;
					ss<<"Tango exception occurred when retrieving write_async response, retry...";
					__DEBUG_LOG(m_dev,ss.str());
					continue;
				}
				
				replyArrived= true;
				break;
			}//end loop nretries

			//Check if response arrived
			if(!replyArrived){
				std::string errMsg("Failed to retrieve asynch command response!");
				__WARN_LOG(m_dev,errMsg);
				_THROW_TANGO_EXCEPTION("READ_ASYNCH_ATTR_FAILED",errMsg);					
			}

		}//close WriteAttributeAsynch()

		/** 
		\brief Mark proxy as offline
 		*/
		void SetOffline();

		/** 
		\brief Monitor proxy status
 		*/
		int Monitor();

		/** 
		\brief Set timeout parameter (change the default only when timeout is disabled)
 		*/
		int SetTimeout(long int timeout);

		/** 
		\brief Set max size of ping time buffer
 		*/
		void SetMaxPingTimeBufferSize(int bufsize);
	
		/** 
		\brief Returns time elapsed since timer start (can be online or offline period)
 		*/
		double GetTimeElapsed();

		/** 
		\brief Get time to expiration of downtime timeout (to trigger actions)
 		*/
		long int GetTimeToExpiration();

		/** 
		\brief Subscribe to event
 		*/
		int SubscribeToEvent(std::string attr_name,Tango::EventType event_type,Tango::CallBack* event_cb);

		/** 
		\brief Subscribe to all registered events (if not already done)
 		*/
		int SubscribeToEvents(Tango::CallBack* event_cb);

		/** 
		\brief Unsubscribe from event by attr/event name
 		*/
		int UnsubscribeEvent(std::string attr_name,Tango::EventType event_type);
		
		/** 
		\brief Add event subscriptions
 		*/
		int AddEventSubscription(std::string attr_name,Tango::EventType event_type);
		/** 
		\brief Remove event subscriptions
 		*/
		int RemoveEventSubscription(std::string attr_name,Tango::EventType event_type);
		/** 
		\brief Find event subscriptions
 		*/
		EvtSubscriptionDataPtr FindEventSubscription(int& pos,std::string attr_name,Tango::EventType event_type);
		/** 
		\brief Check if proxy is subscribed to event
 		*/
		bool IsSubscribedToEvent(std::string attr_name,Tango::EventType event_type);
		/** 
		\brief Check if proxy is subscribed to event
 		*/
		int SetSubscribedToEvent(std::string attr_name,Tango::EventType event_type,int ev_id);
		/** 
		\brief Add event handlers to event subscription
 		*/
		int AddEventHandlers(std::string attr_name,Tango::EventType event_type,std::vector<std::string> event_handlers,long int execution_delay=0,int priority=Task::eMEDIUM_PRIORITY);
		/** 
		\brief Return list of event subscription data
 		*/
		const std::vector<EvtSubscriptionDataPtr>& GetEventSubscriptions(){return m_evtSubscriptions;}
		/** 
		\brief Return proxy status from string
 		*/
		static int GetProxyRunStatusFromString(std::string proxy_status_str){
			return m_runStateStrMap[proxy_status_str];
		}
		
	private:
		/** 
		\brief Set proxy as offline
 		*/
		void SetOffline_i();
		/** 
		\brief Set proxy as online
 		*/
		void SetOnline_i(const Tango::DevState& state,const std::string& status,const int& ping_time);

		/** 
		\brief Initialize pars
 		*/
		void Init();
		/** 
		\brief Compute ping avg
 		*/
		void ComputePingTimeMean();

		/** 
		\brief Unsubscribe from event 
 		*/	
		int unsubscribe_event(EvtSubscriptionDataPtr event_sub_data_ptr);
		/** 
		\brief Add attr config to event subscription data
 		*/				
		int add_attr_config_to_event_subscription(EvtSubscriptionDataPtr event_sub_data_ptr,std::string attr_name);
		
		/** 
		\brief Get proxy status info expressed in json notation
 		*/
		int GetProxyInfoInJson(std::string& jsonString);

		/** 
		\brief Get proxy run status
 		*/
		int GetProxyRunStatus();

		/** 
		\brief Find event subscriptions (internal method)
 		*/
		EvtSubscriptionDataPtr find_event_subscription(int& pos,std::string attr_name,Tango::EventType event_type);

	private:
		//Dev proxy
		Tango::DeviceProxy* m_proxy;
		Tango::DeviceImpl* m_dev;//Tango device (for logging) (DO NOT DESTROY!)
		std::string m_proxy_name;
		bool m_is_initialized;

		//Event subscriptions
		std::vector<EvtSubscriptionDataPtr> m_evtSubscriptions;

		//Proxy info		
		yat::Time m_timestamp;//last timestamp measured
		std::string m_status;//last status measured
		Tango::DevState m_state;//last state measured
		int m_ping_time;//last ping measured (in ms)
		std::deque<int> m_ping_time_buffer;//ping time buffer
		int m_max_buffer_size;//max ping buffer size
		float m_ping_time_avg;//ping time averaged over latest buffer samples
		float m_ping_time_dvt;//derivative of ping time computed over latest buffer samples
		bool m_is_online;
		long int m_action_timeout;//timeout for downtime action (in seconds)

		//Mutex & timers
		yat::Mutex m_lock;
		yat::Mutex m_data_lock;
		yat::Timer* m_timer;
		yat::Timeout* m_downtimeActionTimeout;

		//std::map<std::string,yat::Timeout*> m_attrValueWatcherMap;

		static std::map<int,std::string> m_runStateMap;
		static std::map<std::string,int> m_runStateStrMap;

};//close class TangoProxyHelper()


typedef TangoProxyHelper* TangoProxyHelperPtr; 
typedef std::map<std::string,TangoProxyHelperPtr,CaseInsensitiveStringComp> TangoProxyHelperMap;
typedef std::shared_ptr<TangoProxyHelperMap> TangoProxyHelperMapPtr;

}//close namespace 

#endif
