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
* @file EvtSubscriptionData.h
* @class EvtSubscriptionData
* @brief EvtSubscriptionData container
*
* EvtSubscriptionData container 
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _EVT_SUBSCRIPTION_DATA_h
#define _EVT_SUBSCRIPTION_DATA_h 1

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS_h
	#include <CodeUtils.h>
#endif

#ifndef _TASK_h
	#include <Task.h>
#endif

//Tango headers
#ifndef _TANGO_H
	#include <tango.h>
#endif

#ifndef _LOG4TANGO_H_
	#include <log4tango.h>
#endif


#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <stdexcept>

#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <time.h>
#include <ctime>
#include <cmath>


namespace Utils_ns {


// ============================================================================
// struct: EventHandlerData
// ============================================================================
struct EventHandlerData {
	EventHandlerData(std::string _name,long int _execution_delay=0,int _priority=Task::eMEDIUM_PRIORITY)
		: name(_name), execution_delay(_execution_delay), priority(_priority)
	{};
	public:
		std::string name;//name
		long int execution_delay;//in ms
		int priority;//priority level	
};//close struct

struct MatchEventHandlerData {
	MatchEventHandlerData(const std::string& name) 
		: m_name(name) 
	{}
 	bool operator()(const EventHandlerData& obj) const {
   return obj.name == m_name;
 	}
 	private:
  	const std::string& m_name;
};

// ============================================================================
// class: EvtSubscriptionData
// ============================================================================
class EvtSubscriptionData {
	
	public:
		/** 
		\brief Constructor
 		*/
		EvtSubscriptionData(std::string _device_name,std::string _attr_name,Tango::EventType _event_type, int _subscription_id=-1) 
			: device_name(_device_name), attr_name(_attr_name), event_type(_event_type), subscription_id(_subscription_id)
		{
			is_subscribed= false;
			event_handlers.clear();
			event_type_str= Tango::EventName[_event_type];
			event_attr_info= 0;
		};

		/** 
		\brief Destructor
 		*/
		~EvtSubscriptionData(){
			event_handlers.clear();
			if(event_attr_info){
				delete event_attr_info;
				event_attr_info= 0;
			}
		};
	
		/** 
		\brief Stream operator
 		*/
		friend ostream& operator<<(ostream&, const EvtSubscriptionData&);
		/** 
		\brief == operator
 		*/
		bool operator==(const EvtSubscriptionData& rhs) const {
			//Compare without case sensitivity
			bool areEqual= ( 
				stricmp(this->attr_name.c_str(),rhs.attr_name.c_str()) == 0 &&
				stricmp(this->device_name.c_str(),rhs.device_name.c_str()) == 0 &&
				stricmp(this->event_type_str.c_str(),rhs.event_type_str.c_str()) == 0
			);
			return areEqual;
		}
		/** 
		\brief == operator for pointers
 		*/
		bool operator==(const EvtSubscriptionData* rhs) const { 
			bool areEqual= ( 
				stricmp(this->attr_name.c_str(),rhs->attr_name.c_str()) == 0 &&
				stricmp(this->device_name.c_str(),rhs->device_name.c_str()) == 0 &&
				stricmp(this->event_type_str.c_str(),rhs->event_type_str.c_str()) == 0
			);
			return areEqual;
		}

		/** 
		\brief < operator for pointers
 		*/
		bool operator < (const EvtSubscriptionData& rhs) const { 
			std::string this_attr_name= this->attr_name;
			std::string this_device_name= this->device_name;
			std::string this_event_type_str= this->event_type_str;
			std::string rhs_attr_name= rhs.attr_name;
			std::string rhs_device_name= rhs.device_name;
			std::string rhs_event_type_str= rhs.event_type_str;
			
			std::transform(this_attr_name.begin(), this_attr_name.end(), this_attr_name.begin(), ::tolower);
			std::transform(this_device_name.begin(), this_device_name.end(), this_device_name.begin(), ::tolower);
			std::transform(this_event_type_str.begin(), this_event_type_str.end(), this_event_type_str.begin(), ::tolower);
			std::transform(rhs_attr_name.begin(), rhs_attr_name.end(), rhs_attr_name.begin(), ::tolower);
			std::transform(rhs_device_name.begin(), rhs_device_name.end(), rhs_device_name.begin(), ::tolower);
			std::transform(rhs_event_type_str.begin(), rhs_event_type_str.end(), rhs_event_type_str.begin(), ::tolower);
			
			bool isLess= (std::tie(this_attr_name,this_device_name,this_event_type_str) < std::tie(rhs_attr_name,rhs_device_name,rhs_event_type_str) );
			/*
			bool isLess= ( 
				stricmp(this->attr_name.c_str(),rhs.attr_name.c_str())<0 &&
				stricmp(this->device_name.c_str(),rhs.device_name.c_str())<0 &&
				stricmp(this->event_type_str.c_str(),rhs.event_type_str.c_str())<0
			);
			*/
			return isLess;
		}//close < operator
		
	public:
		/** 
		\brief Get printable string
 		*/
		std::string GetPrintable(){
			std::stringstream ss;
			ss<<"{device_name="<<device_name<<", attr_name="<<attr_name<<", type="<<event_type_str<<"}";
			return ss.str();
		}

		/** 
		\brief Return list of event handlers associated to this event subscription
 		*/
		//const std::vector<std::string>& GetEventHandlers() const {return event_handlers;} 
		const std::vector<EventHandlerData>& GetEventHandlers() const {return event_handlers;} 
	
		/** 
		\brief Set event as subscribed and store event id
 		*/
		void SetSubscribed(int subEvId){
			is_subscribed= true;
			subscription_id= subEvId;
		}
		/** 
		\brief Check if event is subscribed or not
 		*/
		bool IsSubscribed(){return is_subscribed;}
		/** 
		\brief Set event as not subscribed
 		*/	
		void SetUnsubscribed(){
			is_subscribed= false;
			subscription_id= -1;
		}

		/** 
		\brief Add list of event handlers associated to this event
 		*/
		void AddEvtHandlers(std::vector<std::string>& handlers,long int execution_delay=0,int priority=Task::eMEDIUM_PRIORITY){
			for(unsigned int i=0;i<handlers.size();i++){
				//Find if item is already present
				std::vector<EventHandlerData>::iterator it = find_if(event_handlers.begin(), event_handlers.end(), MatchEventHandlerData(handlers[i]));
				if(!event_handlers.empty() && it!=event_handlers.end()) continue;//already present
				EventHandlerData evt_handler_data(handlers[i],execution_delay,priority);
				event_handlers.push_back(evt_handler_data);
			}//end loop handlers
		}

		/** 
		\brief Set list of event handlers associated to this event
 		*/
		void SetEvtHandlers(std::vector<std::string>& handlers,long int execution_delay=0,int priority=Task::eMEDIUM_PRIORITY){
			event_handlers.clear();

			for(unsigned int i=0;i<handlers.size();i++){
				EventHandlerData evt_handler_data(handlers[i],execution_delay,priority);
				event_handlers.push_back(evt_handler_data);
			}//end loop handlers
			//event_handlers= handlers;
		}
		/** 
		\brief Set event attribute info
 		*/
		int SetEventAttrInfo(Tango::AttributeInfoEx* info){
			if(!info) return -1;

			//Destroy existing config
			if(event_attr_info){
				delete event_attr_info;
				event_attr_info= 0;
			}

			//Set new info
			event_attr_info= info;
			return 0;
		}
		/** 
		\brief Has event attr info
 		*/
		bool HasEventAttrInfo(){	
			if(event_attr_info) return true;
			return false;
		}		

	public:
		std::string device_name;
		std::string attr_name;
		Tango::EventType event_type; 
		std::string event_type_str;
		int subscription_id;
		bool is_subscribed;
		//std::vector<std::string> event_handlers;
		std::vector<EventHandlerData> event_handlers;

		Tango::AttributeInfoEx* event_attr_info;

};//close EvtSubscriptionData class

inline ostream& operator<<(ostream& os, const EvtSubscriptionData& obj) {
	return os << "{device_name="<<obj.device_name<<", attr_name="<<obj.attr_name<<", type="<<obj.event_type_str<<", subscription_id="<<obj.subscription_id<<"}";
}
struct MatchSubscription {
	MatchSubscription(const EvtSubscriptionData& subscriptionData) 
		: m_subscriptionData(subscriptionData) {}
 	bool operator()(const EvtSubscriptionData* obj) const {
		bool areEqual= ( obj->device_name==m_subscriptionData.device_name && 
										 obj->attr_name==m_subscriptionData.attr_name &&
										 obj->event_type_str==m_subscriptionData.event_type_str);
  	return areEqual;
 	}
 	private:
  	const EvtSubscriptionData& m_subscriptionData;
};
typedef std::shared_ptr<EvtSubscriptionData> EvtSubscriptionDataPtr;
typedef std::vector<EvtSubscriptionData*> EvtSubscriptions;


}//close namespace Utils_ns


#endif
