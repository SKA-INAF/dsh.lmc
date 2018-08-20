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

#include <TangoProxyManager.h>

#ifndef _TANGO_PROXY_HELPER
	#include <TangoProxyHelper.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS_h
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

//# JSON CPP
#ifndef JSON_JSON_H_INCLUDED
	#include <json/json.h>
#endif

#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <ctime>

#include <map>


namespace Utils_ns {

//*******************************************
//***       CONSTRUCTORS/DESTRUCTORS
//*******************************************
TangoProxyManager::TangoProxyManager(Tango::DeviceImpl* dev) 
	: m_dev(dev)		
{
	m_proxyHelperMap.clear();
}


TangoProxyManager::~TangoProxyManager(){

	// Cleanup device proxy map
	typedef typename std::map<std::string,TangoProxyHelperPtr>::iterator mapIter;
	for(mapIter it=m_proxyHelperMap.begin();it!=m_proxyHelperMap.end();it++){
		if(it->second){
			delete it->second;
			it->second= 0;
		}
	}//end loop items
	m_proxyHelperMap.clear();

}//close destructor


//*******************************************
//***       PUBLIC METHODS
//*******************************************
int TangoProxyManager::RegisterProxy(std::string proxy_name,long int timeout){
			
	//## Check device instance
	if(!m_dev) return -1;

	//## Find if proxy name is already present
	//## Mutex not needed here (CHECK)
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(proxy_helper){
		__DEBUG_LOG(m_dev,"Device proxy "<<proxy_name<<" already registered");
		return 0;
	} 		

	//## Create proxy helper
	proxy_helper= new TangoProxyHelper(m_dev,proxy_name);
	proxy_helper->SetTimeout(timeout);
			
	//## Try to initialize it
	__DEBUG_LOG(m_dev,"Trying to initialize proxy helper "<<proxy_name<<"...");
	if(proxy_helper->InitProxy()<0){
		__WARN_LOG(m_dev,"Failed to initialize proxy (check proxy name and/or syntax, etc)!");
		delete proxy_helper;
		proxy_helper= 0;
		return -1;
	}

	//Add proxy to map
	//== CRITICAL SECTION START ==
	{
		yat::MutexLock guard(m_lock);
		if(proxy_helper) {
			__DEBUG_LOG(m_dev,"Adding proxy helper "<<proxy_name<<" in map...");
			m_proxyHelperMap.insert( std::make_pair(proxy_name,proxy_helper) );
		}
	}
	//== CRITICAL SECTION END ==
	
	return 0;
		
}//close RegisterProxy()


int TangoProxyManager::UnregisterProxy(std::string proxy_name){
			
	//## Protect entire method with mutex
	//## Search & Delete from the proxy map
	yat::MutexLock guard(m_lock);

	//## Check dev
	if(!m_dev) return -1;

	//## Find if proxy name is present
	try{
		std::map<std::string,TangoProxyHelperPtr>::iterator it= m_proxyHelperMap.find(proxy_name);
		bool hasProxy= (!m_proxyHelperMap.empty() && it!=m_proxyHelperMap.end());	
		if(hasProxy && (it->second) ) {
			delete it->second;
			m_proxyHelperMap.erase(it);
		}
		else{
			__DEBUG_LOG(m_dev,"Proxy "<<proxy_name<<" not found in map, nothing to be removed!");
		}
	}//close try
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception occurred while removing proxy "<<proxy_name<<" from map (err="<<e.what()<<")!");		
		return -1;
	}

	return 0;
		
}//close UnregisterProxy()

int TangoProxyManager::InitProxy(std::string proxy_name){
			
	//## Check dev
	if(!m_dev) return -1;
	
	//## Find if proxy helper is present
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__DEBUG_LOG(m_dev,"Device proxy "<<proxy_name<<" not found (not registered?)");
		return -1;
	} 
		
	//## Initialize proxy (create device proxy and try to monitor it for the first time)
	//## NB: Mutex is locked inside the monitoring step (when setting state online/offline)	
	return proxy_helper->InitProxy();
		
}//close InitProxy()

int TangoProxyManager::GetProxyAdminDeviceName(std::string& admin_dev_name, std::string proxy_name){

	//## Check dev
	if(!m_dev) return -1;
	
	//## Find if proxy helper is present
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__DEBUG_LOG(m_dev,"Device proxy "<<proxy_name<<" not found (not registered?)");
		return -1;
	} 

	//## Get admin device name
	return proxy_helper->GetAdminDeviceName(admin_dev_name);

}//close GetProxyAdminDeviceName()


int TangoProxyManager::MonitorProxy(std::string proxy_name){
			
	//## Check dev
	if(!m_dev) return -1;

	//## Find if proxy is registered
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__DEBUG_LOG(m_dev,"Device proxy "<<proxy_name<<" not registered in manager!");
		return -1;
	} 	

	//## Monitor proxy
	//## NB: Mutex is locked inside the monitoring step (when setting state online/offline)
	if(proxy_helper->Monitor()<0){
		__WARN_LOG(m_dev,"Failed to monitor proxy "<<proxy_name<<"...");
		return -1;
	}

	return 0;
		
}//close MonitorProxy()

int TangoProxyManager::MonitorProxy(std::string proxy_name,std::vector<std::string>& proxy_info){
			
	//## Check dev
	if(!m_dev) return -1;
	proxy_info.clear();
			
	//## Find if proxy is registered
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__DEBUG_LOG(m_dev,"Device proxy "<<proxy_name<<" not registered in manager!");
		return -1;
	} 	
			
	//## Monitor proxy
	//## NB: Mutex is locked inside the monitoring step (when setting state online/offline)
	if(proxy_helper->Monitor()<0){
		__WARN_LOG(m_dev,"Failed to monitor proxy "<<proxy_name<<"...");
		return -1;
	}
			
	//## Get access to proxy info
	//## NB: Mutex is locked inside the GetProxyInfo method
	if(proxy_helper->GetProxyInfo(proxy_info)<0){
		__WARN_LOG(m_dev,"Failed to get access to proxy "<<proxy_name<<" info ...");
		return -1;
	}
			
	return 0;
		
}//close MonitorProxy()


int TangoProxyManager::SubscribeProxyToEvents(std::string proxy_name,Tango::CallBack* event_cb){
			
	//## Check dev
	if(!m_dev) return -1;

	//## Check callback
	if(event_cb) return -1;

	//## Get proxy helper
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__WARN_LOG(m_dev,"Cannot retrieve proxy helper (hint: you must register it before adding event subscriptions!)");
		return -1;
	}

	//## Subscribe to events
	//## NB: Mutex is locked internally (only when setting proxy as subscribed, not when trying subscription)
	return proxy_helper->SubscribeToEvents(event_cb);
		
}//close SubscribeProxyToEvents()


int TangoProxyManager::MonitorProxies(){
		
	//## Check dev
	if(!m_dev) return -1;

	//## Check if map empty 
	if(m_proxyHelperMap.empty()){
		__DEBUG_LOG(m_dev,"No proxy registered in manager, nothing to be done!");
		return 0;
	}

	//## Iterate over map
	typedef typename std::map<std::string,TangoProxyHelperPtr>::iterator mapIter;
	int status= 0;
	for(mapIter it=m_proxyHelperMap.begin();it!=m_proxyHelperMap.end();it++){
		std::string proxy_name= it->first;
		TangoProxyHelperPtr proxy_helper= it->second;
		if(proxy_helper && proxy_helper->Monitor()<0){
			__WARN_LOG(m_dev,"Monitoring of proxy "<<proxy_name<<" has failed, skip to other registered proxies...");
			status= -1;
			continue;
		} 
	}//end loop map items

	return status;
		
}//close MonitorProxies()

int TangoProxyManager::InitAndSubscribeProxies(Tango::CallBack* event_cb){
			
	//## Check dev
	if(!m_dev) return -1;

	//## Check if map empty 
	if(m_proxyHelperMap.empty()){
		__DEBUG_LOG(m_dev,"No proxy registered in manager, nothing to be done!");
		return 0;
	}

	//## Iterate over proxies in map
	typedef typename std::map<std::string,TangoProxyHelperPtr>::iterator mapIter;
	int status= 0;
	for(mapIter it=m_proxyHelperMap.begin();it!=m_proxyHelperMap.end();it++){

		//Get proxy helper instance
		std::string proxy_name= it->first;
		TangoProxyHelperPtr proxy_helper= it->second;

		//Initialize proxy first (if failing skip to other proxies...)
		if(proxy_helper && proxy_helper->InitProxy()<0){
			__WARN_LOG(m_dev,"Initialization of proxy "<<proxy_name<<" has failed, skip to other registered proxies...");
			status= -1;
			continue;
		} 

		//Subscribe to proxy events
		if(proxy_helper && proxy_helper->SubscribeToEvents(event_cb)<0){
			status= -1;
			continue;
		}
	}//end loop map items

	return status;
		
}//close InitAndSubscribeProxies()


int TangoProxyManager::RegisterEvtSubscriptions(std::string proxy_name,std::string attr_name,std::vector<Tango::EventType> event_types) {
			
	//## Check dev
	if(!m_dev) return -1;

	//## Get proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__WARN_LOG(m_dev,"Cannon retrieve proxy helper (you must register it before adding event subscriptions!)");
		return -1;
	}

	//## Iterate over events and register them
	for(unsigned int i=0;i<event_types.size();i++){
		if(proxy_helper->AddEventSubscription(attr_name,event_types[i])<0){
			__WARN_LOG(m_dev,"Failed to register event (name="<<attr_name<<", type="<<event_types[i]<<") for proxy helper "<<proxy_name<<"!");
			return -1;
		}
	}
	
	return 0;
		
}//close RegisterEvtSubscriptions()


int TangoProxyManager::UnregisterEvtSubscriptions(std::string proxy_name,std::string attr_name,std::vector<Tango::EventType> event_types) {
			
	//## Check dev
	if(!m_dev) return -1;

	//## Get proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__WARN_LOG(m_dev,"Cannon retrieve proxy helper (you must register it before adding event subscriptions!)");
		return -1;
	}

	//## Iterate over events and unregister them
	for(unsigned int i=0;i<event_types.size();i++){
		if(proxy_helper->RemoveEventSubscription(attr_name,event_types[i])<0){
			__WARN_LOG(m_dev,"Failed to remove event (name="<<attr_name<<", type="<<event_types[i]<<") for proxy helper "<<proxy_name<<"!");
			return -1;
		}
	}
			
	return 0;
		
}//close UnregisterEvtSubscriptions()

int TangoProxyManager::RegisterEvtHandlers(std::string proxy_name,std::string attr_name,Tango::EventType event_type,std::vector<std::string> evt_handlers,long int execution_delay,int priority) {
			
	//## Check dev
	if(!m_dev) return -1;

	//## Get proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__WARN_LOG(m_dev,"Cannon retrieve proxy helper (you must register it before adding event subscriptions!)");
		return -1;
	}

	//## Add event handlers
	if(proxy_helper->AddEventHandlers(attr_name,event_type,evt_handlers,execution_delay,priority)<0){
		__WARN_LOG(m_dev,"Failed to register handler for event (name="<<attr_name<<", type="<<event_type<<") in proxy helper "<<proxy_name<<"!");
		return -1;
	}
			
	return 0;

}//close RegisterEvtHandlers()


Tango::DeviceProxy* TangoProxyManager::FindProxy(std::string proxy_name){
			
	//## Find device proxy (version for clients)
	//## NB: Class methods shall call the internal method that do not use mutex
	yat::MutexLock guard(m_lock);
	return FindProxy_i(proxy_name);
		
}//close FindProxy()

TangoProxyHelperPtr TangoProxyManager::FindProxyHelper(std::string proxy_name){
			
	//## Find proxy helper (version for clients)
	//## NB: Class methods shall call the internal method that do not use mutex
	yat::MutexLock guard(m_lock);
	return FindProxyHelper_i(proxy_name);
		
}//close FindProxyHelper()


EvtSubscriptionDataPtr TangoProxyManager::FindEvtSubscription(std::string proxy_name,std::string attr_name,Tango::EventType event_type) {
			
	//Check dev
	if(!m_dev) return nullptr;

	//Get proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__WARN_LOG(m_dev,"Cannon retrieve proxy helper (you must register it before adding event subscriptions!)");
		return nullptr;
	}
	
	int pos= -1;	
	return proxy_helper->FindEventSubscription(pos,attr_name,event_type);
		
}//close FindEvtSubscription()

EvtSubscriptionDataPtr TangoProxyManager::FindEvtSubscription(std::string proxy_name,std::string attr_name,std::string evt_type_str) {
			
	Tango::EventType event_type;
	if(TangoUtils::GetEventTypeFromString(event_type,evt_type_str)<0) return nullptr;
	return FindEvtSubscription(proxy_name,attr_name,event_type);
		
}//close FindEvtSubscription()


Tango::AttributeInfoEx* TangoProxyManager::GetEvtSubscriptionAttrInfo(std::string proxy_name,std::string attr_name,Tango::EventType event_type){
			
	//Check dev
	if(!m_dev) return nullptr;
			
	//Find event subscription
	EvtSubscriptionDataPtr evt_sub_ptr= FindEvtSubscription(proxy_name,attr_name,event_type);
	if(!evt_sub_ptr){
		__WARN_LOG(m_dev,"Cannot find evt subscription (proxy="<<proxy_name<<", attr="<<attr_name<<" evt_type="<<event_type<<"), did you register this subscription?");
		return nullptr;
	}

	//Check if subscription has attr info
	if(!evt_sub_ptr->HasEventAttrInfo()){
		__WARN_LOG(m_dev,"Evt subscription (proxy="<<proxy_name<<", attr="<<attr_name<<" evt_type="<<event_type<<") has no attr info stored yet, did you actually subscribe to the event (check if proxy is offline)?");
		return nullptr;
	}
			
	return evt_sub_ptr->event_attr_info;
		
}//close GetEvtSubscriptionAttrInfo()

bool TangoProxyManager::IsSubscribedToEvent(std::string proxy_name,std::string attr_name,Tango::EventType event_type){
			
	//Check dev
	if(!m_dev) return -1;

	//Get proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__WARN_LOG(m_dev,"Cannon retrieve proxy helper (you must register it before adding checking event subscriptions!)");
		return -1;
	}

	//Check if subscribed
	if(proxy_helper->IsSubscribedToEvent(attr_name,event_type)<0){
		__WARN_LOG(m_dev,"Failed to check event subscription to event (name="<<attr_name<<", type="<<event_type<<") for proxy helper "<<proxy_name<<"!");
		return -1;
	}
			
	return 0;
		
}//close IsSubscribedToEvent()


const std::vector<std::string> TangoProxyManager::GetRegisteredProxyNames(){
			
	//__DEBUG_LOG(m_dev,"#"<<m_proxyHelperMap.size()<<" registered proxies...");
	std::vector<std::string> proxy_names;
	typedef typename std::map<std::string,TangoProxyHelperPtr>::iterator mapIter;
	for(mapIter it=m_proxyHelperMap.begin();it!=m_proxyHelperMap.end();it++){
		std::string proxy_name= it->first;
		proxy_names.push_back(proxy_name);
	}		
			
	return proxy_names;
		
}//close GetRegisteredProxyNames()

const std::vector<std::string> TangoProxyManager::GetRegisteredEventNames(){
			
	std::vector<std::string> event_names;
	typedef typename std::map<std::string,TangoProxyHelperPtr>::iterator mapIter;
	for(mapIter it=m_proxyHelperMap.begin();it!=m_proxyHelperMap.end();it++){
		std::string proxy_name= it->first;
		TangoProxyHelperPtr proxy_helper= it->second;
		std::vector<EvtSubscriptionDataPtr> subscriptions= proxy_helper->GetEventSubscriptions();
		for(unsigned int i=0;i<subscriptions.size();i++){
			std::stringstream ss;
			ss<<proxy_name<<"/"<<subscriptions[i]->attr_name<<" (type="<<subscriptions[i]->event_type<<")";
			event_names.push_back(ss.str());
		}//end loop events
	}//end loop proxies
			
	return event_names;
		
}//close GetRegisteredEventNames()


int TangoProxyManager::GetProxyAttributeInfo(Tango::AttributeInfoEx& attr_info,std::string proxy_name,std::string attr_name){

	//Check dev
	if(!m_dev) return -1;

	//Get proxy helper
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__WARN_LOG(m_dev,"Cannot retrieve proxy helper!");
		return -1;
	}

	//Get attr info
	if(proxy_helper->GetProxyAttributeInfo(attr_info,attr_name)<0){
		__WARN_LOG(m_dev,"Cannot retrieve proxy attr "<<attr_name<<" info!");
		return -1;
	}

	return 0;

}//close GetProxyAttributeInfo()


void TangoProxyManager::ExecuteCommand(Tango::DeviceData& dout,std::string proxy_name,std::string cmd_name){
			
	//Find proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		std::stringstream ss;
		ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_COMMAND_FAILED",ss.str())
	}

	//Execute command
	try{
		proxy_helper->ExecuteCommand(dout,cmd_name);
	}
	catch(...){
		throw;
	}
		
}//close ExecuteCommand()


void TangoProxyManager::ExecuteCommand(Tango::DeviceData& dout,std::string proxy_name,std::string cmd_name,Tango::DeviceData& din){
			
	//Find proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		std::stringstream ss;
		ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_COMMAND_FAILED",ss.str())
	}
			
	//Execute command
	try{
		proxy_helper->ExecuteCommand(dout,cmd_name,din);
	}
	catch(...){
		throw;
	}
		
}//close ExecuteCommand()


void TangoProxyManager::ExecuteCommandAsynch(long& cmd_id,std::string proxy_name,std::string cmd_name){
			
	//Find proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){				
		std::stringstream ss;
		ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_COMMAND_ASYNCH_FAILED",ss.str())			
	}

	//Execute command
	try{
		proxy_helper->ExecuteCommandAsynch(cmd_id,cmd_name);
	}
	catch(...){
		throw;
	}
		
}//close ExecuteCommandAsynch()


void TangoProxyManager::ExecuteCommandAsynch(long& cmd_id,std::string proxy_name,std::string cmd_name,Tango::DeviceData& din){
			
	//Find proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		std::stringstream ss;
		ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_COMMAND_FAILED",ss.str())
	}

	//Execute command
	try{
		proxy_helper->ExecuteCommandAsynch(cmd_id,cmd_name,din);
	}
	catch(...){
		throw;
	}
		
}//close ExecuteCommandAsynch()


void TangoProxyManager::GetAsynchCommandReply(Tango::DeviceData& dout,std::string proxy_name,long cmd_id,long timeout){
			
	//Find proxy
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		std::stringstream ss;
		ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_COMMAND_FAILED",ss.str())
	}
			
	//Get async reply
	try{
		proxy_helper->GetAsynchCommandReply(dout,cmd_id,timeout);
	}
	catch(...){
		throw;
	}
		
}//close GetAsynchCommandReply()


void TangoProxyManager::ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::string proxy_name,std::vector<std::string> cmd_names,std::vector<Tango::DeviceData>& din,int reply_timeout,int nretries){
			
	//Check args
	//If cmd name size is different wrt to proxy size throw error
	if(cmd_names.size()!=din.size()){
		std::string errMsg("Command argin list size different from proxy list size!");
		_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",errMsg);
	}

	//Find proxy helper
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		std::stringstream ss;
		ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",ss.str())
	}
	
	//Execute command asynchronously on proxy list
	std::vector<long> async_cmd_ids; 

	for(unsigned int i=0;i<cmd_names.size();i++){
		//Execute async command (argin, no argout expected) on each proxy
		long cmd_id;
		try {
			proxy_helper->ExecuteCommandAsynch(cmd_id,cmd_names[i],din[i]);
			__DEBUG_LOG(m_dev,"Executed async cmd (id="<<cmd_id<<") on proxy "<<proxy_name<<"...");
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			std::stringstream ss;
			ss<<"Tango exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_name<<" (check proxy registered/syntax/proxy online)!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}
		catch(std::exception& e){
			std::stringstream ss;
			ss<<"C++ exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_name<<" (err="<<e.what()<<")!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}
		catch(...){
			std::stringstream ss;
			ss<<"Unknown exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_name<<"!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}

		//Fill cmd ids
		async_cmd_ids.push_back(cmd_id);

	}//end loop proxies()

	//Collect async cmd responses
	std::vector<bool> is_reply_arrived(async_cmd_ids.size(),false);
	size_t nreplies= 0;

	for(int k=0;k<nretries;k++){
		//All responses arrived, nothing to be done
		if(nreplies==async_cmd_ids.size()){
			break;
		}

		//Loop and get async cmd responses if they arrived
		for(unsigned int i=0;i<async_cmd_ids.size();i++){
			if(is_reply_arrived[i]) continue;
			Tango::DeviceData reply_data; 
    	try { 
				proxy_helper->GetAsynchCommandReply(reply_data,async_cmd_ids[i],reply_timeout);
    	} 
    	catch(Tango::AsynReplyNotArrived) { 
    		__DEBUG_LOG(m_dev,"Async reply (id="<<async_cmd_ids[i]<<") not arrived yet, skip to next...");
				continue;
    	} 
      catch (Tango::DevFailed &e) {//Proxy replied with exception, re-throw to clients
      	Tango::Except::print_exception(e); 
				std::stringstream ss;
				ss<<"Proxy "<<proxy_name<<" replied with an exception!";
				__WARN_LOG(m_dev,ss.str());
				nreplies++;

				//Cancel other pending requests
				//...

				//Rethrow exception to client
				throw;
      } 
			is_reply_arrived[i]= true;
			dout.push_back(reply_data);
			nreplies++;
			
		}//end loop async requests
	}//end loop nretries

	//Check if all replies arrived
	if(nreplies!=async_cmd_ids.size()){
		std::stringstream ss;
		ss<<"Failed to retrieve responses (timeout) from proxies (nreplies="<<nreplies<<"!= async_cmd_ids="<<async_cmd_ids.size()<<")!";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",ss.str());	
	}
		
}//close ExecuteGroupCommandsAsynch()








void TangoProxyManager::ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::vector<std::string> proxy_names,std::vector<std::string> cmd_names,std::vector<Tango::DeviceData>& din,int reply_timeout,int nretries){
			
	//Check args
	//If cmd name size is different wrt to proxy size throw error
	if(cmd_names.size()!=proxy_names.size()){
		std::string errMsg("Command list size different from proxy list size!");
		_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",errMsg);
	}
	if(din.size()!=proxy_names.size()){
		std::string errMsg("Command argument data list size different from proxy list size!");
		_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",errMsg);
	}

	//Find proxy helpers
	std::vector<TangoProxyHelperPtr> proxy_helpers;
	for(unsigned int i=0;i<proxy_names.size();i++){
		TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_names[i]);
		if(!proxy_helper){
			std::stringstream ss;
			ss<<"Cannot retrieve proxy helper "<<proxy_names[i]<<" (check if registered or if valid name is given (should be FQDN)!)";
			__WARN_LOG(m_dev,ss.str());
			_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",ss.str())
		}
		proxy_helpers.push_back(proxy_helper);
	}//end loop proxy


	//Execute command asynchronously on proxy list
	std::vector<long> async_cmd_ids; 

	for(unsigned int i=0;i<proxy_names.size();i++){
		//Execute async command (argin, no argout expected) on each proxy
		long cmd_id;
		try {
			proxy_helpers[i]->ExecuteCommandAsynch(cmd_id,cmd_names[i],din[i]);
			__DEBUG_LOG(m_dev,"Executed async cmd (id="<<cmd_id<<") on proxy "<<proxy_names[i]<<"...");
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			std::stringstream ss;
			ss<<"Tango exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_names[i]<<" (check proxy registered/syntax/proxy online)!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}
		catch(std::exception& e){
			std::stringstream ss;
			ss<<"C++ exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_names[i]<<" (err="<<e.what()<<")!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}
		catch(...){
			std::stringstream ss;
			ss<<"Unknown exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_names[i]<<"!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}

		//Fill cmd ids
		async_cmd_ids.push_back(cmd_id);

	}//end loop proxies()

	//Collect async cmd responses
	std::vector<bool> is_reply_arrived(async_cmd_ids.size(),false);
	size_t nreplies= 0;

	for(int k=0;k<nretries;k++){
		//All responses arrived, nothing to be done
		if(nreplies==async_cmd_ids.size()){
			break;
		}

		//Loop and get async cmd responses if they arrived
		for(unsigned int i=0;i<async_cmd_ids.size();i++){
			if(is_reply_arrived[i]) continue;
			Tango::DeviceData reply_data; 
    	try { 
				proxy_helpers[i]->GetAsynchCommandReply(reply_data,async_cmd_ids[i],reply_timeout);
    	} 
    	catch(Tango::AsynReplyNotArrived) { 
    		__DEBUG_LOG(m_dev,"Async reply (id="<<async_cmd_ids[i]<<") not arrived yet, skip to next...");
				continue;
    	} 
      catch (Tango::DevFailed &e) {//Proxy replied with exception, re-throw to clients
      	Tango::Except::print_exception(e); 
				std::stringstream ss;
				ss<<"Proxy "<<proxy_names[i]<<" replied with an exception!";
				__WARN_LOG(m_dev,ss.str());
				nreplies++;

				//Cancel other pending requests
				//...

				//Rethrow exception to client
				throw;
      } 
			is_reply_arrived[i]= true;
			dout.push_back(reply_data);
			nreplies++;
			
		}//end loop async requests
	}//end loop nretries

	//Check if all replies arrived
	if(nreplies!=async_cmd_ids.size()){
		std::stringstream ss;
		ss<<"Failed to retrieve responses (timeout) from proxies (nreplies="<<nreplies<<"!= async_cmd_ids="<<async_cmd_ids.size()<<")!";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",ss.str());	
	}
		
}//close ExecuteGroupCommandsAsynch()


void TangoProxyManager::ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::vector<std::string> proxy_names,std::string cmd_name,Tango::DeviceData& din,int reply_timeout,int nretries){

	std::vector<std::string> cmd_names;
	std::vector<Tango::DeviceData> din_list;
	for(unsigned int i=0;i<proxy_names.size();i++){
		cmd_names.push_back(cmd_name);
		din_list.push_back(din);
	}
	return ExecuteGroupCommandsAsynch(dout,proxy_names,cmd_names,din_list,reply_timeout,nretries);

}//close ExecuteGroupCommandsAsynch()






void TangoProxyManager::ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::vector<std::string> proxy_names,std::vector<std::string> cmd_names,int reply_timeout,int nretries){
			
	//Check args
	//If cmd name size is different wrt to proxy size throw error
	if(cmd_names.size()!=proxy_names.size()){
		std::string errMsg("Command list size different from proxy list size!");
		_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",errMsg);
	}

	//Find proxy helpers
	std::vector<TangoProxyHelperPtr> proxy_helpers;
	for(unsigned int i=0;i<proxy_names.size();i++){
		TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_names[i]);
		if(!proxy_helper){
			std::stringstream ss;
			ss<<"Cannot retrieve proxy helper "<<proxy_names[i]<<" (check if registered or if valid name is given (should be FQDN)!)";
			__WARN_LOG(m_dev,ss.str());
			_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",ss.str())
		}
		proxy_helpers.push_back(proxy_helper);
	}//end loop proxy


	//Execute command asynchronously on proxy list
	std::vector<long> async_cmd_ids; 

	for(unsigned int i=0;i<proxy_names.size();i++){
		//Execute async command (argin, no argout expected) on each proxy
		long cmd_id;
		try {
			proxy_helpers[i]->ExecuteCommandAsynch(cmd_id,cmd_names[i]);
			__DEBUG_LOG(m_dev,"Executed async cmd (id="<<cmd_id<<") on proxy "<<proxy_names[i]<<"...");
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			std::stringstream ss;
			ss<<"Tango exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_names[i]<<" (check proxy registered/syntax/proxy online)!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}
		catch(std::exception& e){
			std::stringstream ss;
			ss<<"C++ exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_names[i]<<" (err="<<e.what()<<")!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}
		catch(...){
			std::stringstream ss;
			ss<<"Unknown exception occurred while executing command "<<cmd_names[i]<<" on proxy "<<proxy_names[i]<<"!";
			__WARN_LOG(m_dev,ss.str());
			throw;
		}

		//Fill cmd ids
		async_cmd_ids.push_back(cmd_id);

	}//end loop proxies()

	//Collect async cmd responses
	std::vector<bool> is_reply_arrived(async_cmd_ids.size(),false);
	size_t nreplies= 0;

	for(int k=0;k<nretries;k++){
		//All responses arrived, nothing to be done
		if(nreplies==async_cmd_ids.size()){
			break;
		}

		//Loop and get async cmd responses if they arrived
		for(unsigned int i=0;i<async_cmd_ids.size();i++){
			if(is_reply_arrived[i]) continue;
			Tango::DeviceData reply_data; 
    	try { 
				proxy_helpers[i]->GetAsynchCommandReply(reply_data,async_cmd_ids[i],reply_timeout);
    	} 
    	catch(Tango::AsynReplyNotArrived) { 
    		__DEBUG_LOG(m_dev,"Async reply (id="<<async_cmd_ids[i]<<") not arrived yet, skip to next...");
				continue;
    	} 
      catch (Tango::DevFailed &e) {//Proxy replied with exception, re-throw to clients
      	Tango::Except::print_exception(e); 
				std::stringstream ss;
				ss<<"Proxy "<<proxy_names[i]<<" replied with an exception!";
				__WARN_LOG(m_dev,ss.str());
				nreplies++;

				//Cancel other pending requests
				//...

				//Rethrow exception to client
				throw;
      } 
			is_reply_arrived[i]= true;
			dout.push_back(reply_data);
			nreplies++;
			
		}//end loop async requests
	}//end loop nretries

	//Check if all replies arrived
	if(nreplies!=async_cmd_ids.size()){
		std::stringstream ss;
		ss<<"Failed to retrieve responses (timeout) from proxies (nreplies="<<nreplies<<"!= async_cmd_ids="<<async_cmd_ids.size()<<")!";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("EXECUTE_GROUP_COMMANDS_ASYNCH_FAILED",ss.str());	
	}
		
}//close ExecuteGroupCommandsAsynch()


void TangoProxyManager::ExecuteGroupCommandsAsynch(std::vector<Tango::DeviceData>& dout,std::vector<std::string> proxy_names,std::string cmd_name,int reply_timeout,int nretries){
			
	std::vector<std::string> cmd_names;
	for(unsigned int i=0;i<proxy_names.size();i++){
		cmd_names.push_back(cmd_name);
	}
	return ExecuteGroupCommandsAsynch(dout,proxy_names,cmd_names,reply_timeout,nretries);
		
}//close ExecuteGroupCommandsAsynch()


void TangoProxyManager::WriteAttributes(std::string proxy_name,std::vector<Tango::DeviceAttribute>& da_list,int timeout){
			
	//Find proxy helper
	TangoProxyHelperPtr proxy_helper= FindProxyHelper(proxy_name);
	if(!proxy_helper){
		std::stringstream ss;
		ss<<"Cannot retrieve proxy helper "<<proxy_name<<" (check if registered or if valid name is given (should be FQDN)!)";
		__WARN_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("WRITE_ATTRIBUTES_FAILED",ss.str())
	}

	//Read attribute via helper
	try{
		proxy_helper->WriteAttributes(da_list,timeout);
	}
	catch(...){
		throw;
	}

}//close WriteAttributes()



int TangoProxyManager::SetProxyOffline(std::string proxy_name){

	yat::MutexLock guard(m_lock);
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		std::stringstream ss;
		ss<<"Failed to retrieve proxy helper "<<proxy_name<<" (check syntax or if registered)!";
		__WARN_LOG(m_dev,ss.str());
		return -1;
	}

	proxy_helper->SetOffline();

	return 0;

}//close MarkProxyOffline()


//*******************************************
//***       PRIVATE METHODS
//*******************************************

TangoProxyHelperPtr TangoProxyManager::FindProxyHelper_i(std::string proxy_name){
			
	//Check dev
	if(!m_dev) return nullptr;

	__DEBUG_LOG(m_dev,"Searching proxy helper "<<proxy_name<<"...");

	TangoProxyHelperPtr proxy_helper= nullptr;
	std::map<std::string,TangoProxyHelperPtr>::iterator it= m_proxyHelperMap.find(proxy_name);
	bool hasProxy= (!m_proxyHelperMap.empty() && it!=m_proxyHelperMap.end());	
	if(hasProxy) proxy_helper= it->second;

	return proxy_helper;
		
}//close FindProxyHelper_i()


Tango::DeviceProxy* TangoProxyManager::FindProxy_i(std::string proxy_name){
			
	//Check dev
	if(!m_dev) return nullptr;
	
	//Find if proxy helper is present
	TangoProxyHelperPtr proxy_helper= FindProxyHelper_i(proxy_name);
	if(!proxy_helper){
		__DEBUG_LOG(m_dev,"Device proxy "<<proxy_name<<" not found (not registered?)");
		return nullptr;
	} 
			
	return proxy_helper->GetProxy();
		
}//close FindProxy_i()

}//close namespace

