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

#include <TangoProxyHelper.h>

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
//***       STATIC MEMBER INIT
//*******************************************
/**
* \brief Init run state map
*/
std::map<int,std::string> InitRunStateTable() {
	//Fill map items
	std::map<int,std::string> tmp
	{
  	std::pair <int, std::string> (TangoProxyHelper::ePROXY_ONLINE, "ONLINE"),
    std::pair <int, std::string> (TangoProxyHelper::ePROXY_SOFT_OFFLINE, "SOFT_OFFLINE"),
    std::pair <int, std::string> (TangoProxyHelper::ePROXY_HARD_OFFLINE, "HARD_OFFLINE")
	};
	return tmp;
}
std::map<int,std::string> TangoProxyHelper::m_runStateMap(InitRunStateTable());

/**
* \brief Init run state string map
*/
std::map<std::string,int> InitRunStateStrTable() {
	//Fill map items
	std::map<std::string,int> tmp
	{
  	std::pair<std::string,int> ("ONLINE",TangoProxyHelper::ePROXY_ONLINE),
    std::pair<std::string,int> ("SOFT_OFFLINE",TangoProxyHelper::ePROXY_SOFT_OFFLINE),
    std::pair<std::string,int> ("HARD_OFFLINE",TangoProxyHelper::ePROXY_HARD_OFFLINE)
	};
	return tmp;
}
std::map<std::string,int> TangoProxyHelper::m_runStateStrMap(InitRunStateStrTable());


//*******************************************
//***       CONSTRUCTORS/DESTRUCTORS
//*******************************************
TangoProxyHelper::TangoProxyHelper(Tango::DeviceImpl* dev,std::string proxy_name)
	: m_dev(dev),m_proxy_name(proxy_name)
{
	//Init pars
	Init();

}//close constructor

TangoProxyHelper::~TangoProxyHelper()
{
	__DEBUG_LOG(m_dev,"Deleting proxy helper "<<m_proxy_name<<"...");

	//Delete proxy (CHECK)
	if(m_proxy){
		__DEBUG_LOG(m_dev,"Deleting proxy "<<m_proxy_name<<"...");
		delete m_proxy;
		m_proxy= 0;
	}

	//Delete timer(s)
	if(m_timer){
		__DEBUG_LOG(m_dev,"Deleting yat::timer...");
		delete m_timer;
		m_timer= 0;
	}
	if(m_downtimeActionTimeout){
		__DEBUG_LOG(m_dev,"Deleting yat::Timeout...");
		delete m_downtimeActionTimeout;
		m_downtimeActionTimeout= 0;
	}

}//close destructor


//*******************************************
//***       PUBLIC METHODS
//*******************************************
int TangoProxyHelper::InitProxy(){

	//Check if proxy is already initialized
	if(m_proxy && m_is_initialized) return 0;

	//Destroy existing proxy
	if(m_proxy){
		delete m_proxy;
		m_proxy= 0;
	}	
	
	//Create proxy
	try {
		m_proxy= new Tango::DeviceProxy(m_proxy_name);
	}
	catch(Tango::WrongNameSyntax& e){//device name is wrong and will be wrong forever unless corrected in DB
		Tango::Except::print_exception(e);
		__WARN_LOG(m_dev,"Tango WrongNameSyntax exception received, check proxy name, proxy won't be initialized!");
		if(m_proxy){
			delete m_proxy;
			m_proxy= 0;
		}
		return -1;
	}//close catch
	catch(Tango::ConnectionFailed& e){
		Tango::Except::print_exception(e);
		std::string reason(e.errors[0].reason.in());
		__WARN_LOG(m_dev,"Tango ConnectionFailed received (reason="<<reason<<")");
		if(reason=="DB_DeviceNotDefined"){//device name is not defined in DB
			if(m_proxy){
				delete m_proxy;
				m_proxy= 0;
			}
			return -1;
		}
	}//close catch
	catch(Tango::DevFailed& e){
		__WARN_LOG(m_dev,"Tango exception received, check proxy name, proxy won't be initialized!");
		if(m_proxy){
			delete m_proxy;
			m_proxy= 0;
		}
		return -1;
	}//close catch
	catch(...){
		__WARN_LOG(m_dev,"C++ exception received, check proxy name, proxy won't be initialized!");
		if(m_proxy){
			delete m_proxy;
			m_proxy= 0;
		}
		return -1;
	}//close catch

	//Check proxy pointer
	if(!m_proxy){
		__WARN_LOG(m_dev,"Failed to initialize proxy "<<m_proxy_name<<"!");
		return -1;
	}
	
	//Set initialized flag
	m_is_initialized= true;

	//Try to ping proxy and tag proxy status
	if(Monitor()<0){
		__WARN_LOG(m_dev,"Proxy "<<m_proxy_name<<" monitoring failed...");
	}

	return 0;

}//close InitProxy()


int TangoProxyHelper::GetAdminDeviceName(std::string& admin_dev_name){

	admin_dev_name= "";
	try{
		admin_dev_name= m_proxy->adm_name();
	}
	catch(...){
		__WARN_LOG(m_dev,"Failed to retrieve admin dev name of device!");
		return -1;
	}

	return 0;

}//close GetAdminDeviceName()

double TangoProxyHelper::GetTimeElapsed(){

	//yat::MutexLock guard(m_lock);
	if(!m_timer) return 0;
	return m_timer->elapsed_sec();

}//close GetTimeElapsed()

long int TangoProxyHelper::GetTimeToExpiration(){
	
	//yat::MutexLock guard(m_lock);
	if(!m_downtimeActionTimeout) return -1;
	if(m_downtimeActionTimeout->enabled()) return m_downtimeActionTimeout->time_to_expiration();
	else return -1;

}//close GetTimeToExpiration()


int TangoProxyHelper::Monitor(){

	//Check dev & proxy
	if(!m_dev || !m_proxy) return -1;
 
	//Ping proxy
	std::string proxy_name= m_proxy->dev_name();
	__DEBUG_LOG(m_dev,"Pinging proxy "<<proxy_name<<"...");
	int ping_time= -1;
	bool hasResponded= false;
	bool isOffline= false;
	try{
		ping_time= m_proxy->ping();
		hasResponded= true;
	}
	catch(Tango::ConnectionFailed& e){
		//Other exception reasons (API_CorbaException+API_CantConnectToDatabase) are not indicative of proxy downtime but rather TangoDB/Corba issues
		int ne= e.errors.length();
		std::string reason(e.errors[0].reason.in());
		__DEBUG_LOG(m_dev,"Tango::ConnectionFailed exception occurred while pinging proxy "<<proxy_name<<" (reason="<<reason<<")");
		
		if( reason == "API_DeviceNotExported" ) isOffline= true;
		if(ne>=2){			
			std::string reason2(e.errors[1].reason.in());
			if(reason == "API_CorbaException" && (reason2=="API_ServerNotRunning" || reason2=="API_CantConnectToDevice")){
				isOffline= true;
			}
		}
		
	}//close catch

	catch(Tango::CommunicationFailed& e){
			
		//Other exception reasons (API_CorbaException+API_CommunicationFailed) is maybe related to syntax?
		int ne= e.errors.length();	
		std::string reason(e.errors[0].reason.in());
		__DEBUG_LOG(m_dev,"Tango::CommunicationFailed exception occurred while pinging proxy "<<proxy_name<<" (reason="<<reason<<")");
		
		if(ne>=2){			
			std::string reason2(e.errors[1].reason.in());
			if(reason == "API_CorbaException" && reason2=="API_DeviceTimedOut"){
				isOffline= true;//Interpret timeout as downtime (THINK AGAIN!!!)
			}
		}
		
	}//close catch

	//Tag proxy as offline?
	if(isOffline){
		std::stringstream ss;
		ss<<"Device proxy "<<proxy_name<<" detected offline, mark as down and will retry later...";
		__DEBUG_LOG(m_dev,ss.str());
	
		//== CRITICAL SECTION START ==
		{			
			yat::MutexLock guard(m_lock);	
			SetOffline_i();
		}
		//== CRITICAL SECTION END ==
		return 0;
	}	

	//Skip other processing if no response was obtained
	if(!hasResponded) return 0;

	//Get state
	Tango::DevState state= Tango::UNKNOWN;
	try{
		state= m_proxy->state();
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		std::stringstream ss;
		ss<<"Failed to retrieve State attr of "<<proxy_name<<" proxy device!";
		__DEBUG_LOG(m_dev,ss.str());
		return -1;
	}

	//Get status	
	std::string status= "";
	try{
		status= m_proxy->status();
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		std::stringstream ss;
		ss<<"Failed to retrieve Status attr of "<<proxy_name<<" proxy device!";
		__DEBUG_LOG(m_dev,ss.str());
		return -1;
	}

	//Set online 
	//NB: ping time to be converted in ms! 
	//== CRITICAL SECTION START ==
	{			
		yat::MutexLock guard(m_lock);	
		SetOnline_i(state,status,ping_time);
	}
	//== CRITICAL SECTION END ==

	return 0;

}//close Monitor()	


void TangoProxyHelper::SetMaxPingTimeBufferSize(int bufsize){
	
	//yat::MutexLock guard(m_lock);
	m_max_buffer_size= bufsize;

}//close SetMaxPingTimeBufferSize()


int TangoProxyHelper::SetTimeout(long int timeout){
			
	//yat::MutexLock guard(m_lock);

	if(timeout<=0) return -1;
	if(m_downtimeActionTimeout){
		//Do not allow to reset timout while downtime is activated and timeout running
		if(!m_is_online && m_downtimeActionTimeout->enabled()){
			return -1;
		}

		//Destroy existing timeout
		delete m_downtimeActionTimeout;
		m_downtimeActionTimeout= 0;
	}

	//Create a new timeout and set disabled
	bool isEnabled= false;
	m_downtimeActionTimeout= new yat::Timeout(timeout,yat::Timeout::TMO_UNIT_SEC,isEnabled);//this starts the timeout
	m_downtimeActionTimeout->disable();
	m_action_timeout= timeout;
			
	return 0;
}//close SetTimeout()


int TangoProxyHelper::AddEventSubscription(std::string attr_name,Tango::EventType event_type){

	yat::MutexLock guard(m_lock);

	//Find if a subscription is already present in collection
	auto ptr= std::make_shared<EvtSubscriptionData>(m_proxy_name,attr_name,event_type);
	auto it = std::find_if(m_evtSubscriptions.begin(), m_evtSubscriptions.end(), [&](std::shared_ptr<EvtSubscriptionData> const& p) {
  	return *p == *ptr; // assumes has operator==
	});

	bool hasItem= (!m_evtSubscriptions.empty() && it!=m_evtSubscriptions.end());
	if(hasItem) {
		__DEBUG_LOG(m_dev,"Event subscription already present, nothing to be added!");
		return 0;
	}

	//Check if attribute actually exists by getting configuration info
	__DEBUG_LOG(m_dev,"Trying to connect to attr proxy "<<attr_name<<" to ensure attr exist...");
	if(add_attr_config_to_event_subscription(ptr,attr_name)<0){
		__WARN_LOG(m_dev,"Cannot retrieve event attr "<<attr_name<<" info from proxy "<<m_proxy_name<<" (proxy offline?), will retry later...");
	}

	//Not found, add in collection
	m_evtSubscriptions.push_back(ptr);
			
	return 0;

}//close AddEventSubscription()



int TangoProxyHelper::RemoveEventSubscription(std::string attr_name,Tango::EventType event_type){

	yat::MutexLock guard(m_lock);

	//Find if a subscription is already present in collection
	int pos= -1;
	EvtSubscriptionDataPtr event_sub_data_ptr= find_event_subscription(pos,attr_name,event_type);
	if(!event_sub_data_ptr) {
		__DEBUG_LOG(m_dev,"Subscriptions not registered for proxy "<<m_proxy_name<<", nothing to be removed!");
		return 0;
	}

	//Unsubscribe proxy from event (if subscribed)
	if(unsubscribe_event(event_sub_data_ptr)<0){
		__WARN_LOG(m_dev,"Failed to unsubscribe from event (attr="<<attr_name<<", type="<<event_type<<")!");
	}

	//Remove event from collection
	//NB: I'm not explicitly deleting the shared_ptr 
	m_evtSubscriptions.erase(m_evtSubscriptions.begin()+pos);

	return 0;

}//close RemoveEventSubscription()


EvtSubscriptionDataPtr TangoProxyHelper::FindEventSubscription(int& pos,std::string attr_name,Tango::EventType event_type){

	yat::MutexLock guard(m_lock);
	return find_event_subscription(pos,attr_name,event_type);

}//close FindEventSubscription()


int TangoProxyHelper::SubscribeToEvent(std::string attr_name,Tango::EventType event_type,Tango::CallBack* event_cb){

	//Check callback
	if(!event_cb) {
		__WARN_LOG(m_dev,"Null ptr to event callback given!");
		return -1;
	}

	//Check if event is registered in subscription collection
	//== CRITICAL SECTION START ==
	int pos= -1;
	EvtSubscriptionDataPtr event_sub_data_ptr= 0;
	{		
		yat::MutexLock guard(m_lock);
		event_sub_data_ptr= find_event_subscription(pos,attr_name,event_type);
	}
	//== CRITICAL SECTION END ==

	if(!event_sub_data_ptr) {
		__WARN_LOG(m_dev,"No subscriptions registered for proxy "<<m_proxy_name<<" (you must register and event subscription before subscribing!)");
		return -1;
	}

	//Check if already subscribed to this event
	if(event_sub_data_ptr->IsSubscribed()){
		__DEBUG_LOG(m_dev,"Proxy "<<m_proxy_name<<" is already subscribed to event (attr="<<attr_name<<", type="<<event_type<<"), nothing to be done!");
		return 0;
	}

	//Subscribe proxy to event	
	int subscriptionId= -1;
	bool stateless_flag= false;//When the stateless flag is set to false, an exception will be thrown when the event subscription encounters a problem (if true Tango retries to subscribe each 10 second)
	try {
		__DEBUG_LOG(m_dev,"Subscribing to event...");
		subscriptionId= m_proxy->subscribe_event(attr_name,event_type,event_cb,stateless_flag);
		__DEBUG_LOG(m_dev,"done!");
	}
	catch(const Tango::DevFailed& e){
		__WARN_LOG(m_dev,"Failed to subscribe to event (attr="<<attr_name<<", type="<<event_type<<") for device proxy "<<m_proxy_name<<")");
		return -1;
	}

	//Set subscribed flag & add attr config
	//== CRITICAL SECTION START ==
	{		
		yat::MutexLock guard(m_lock);
		event_sub_data_ptr->SetSubscribed(subscriptionId);
	
		if(!event_sub_data_ptr->HasEventAttrInfo() && add_attr_config_to_event_subscription(event_sub_data_ptr,attr_name)<0 ){
			__WARN_LOG(m_dev,"Failed to retrieve and set event attr "<<attr_name<<" config info, mmm strange...proxy should be online here (check syntax!)");
		}
	}
	//== CRITICAL SECTION END ==

	return 0;

}//close SubscribeToEvent()

int TangoProxyHelper::SubscribeToEvents(Tango::CallBack* event_cb){

	//Check callback
	if(!event_cb) {
		__WARN_LOG(m_dev,"Null ptr to event callback given!");
		return -1;
	}

	//Return if no events are registered
	if(m_evtSubscriptions.empty()) {
		__DEBUG_LOG(m_dev,"No events registsred in proxy "<<m_proxy_name<<", nothing to be subscribed!");
		return 0;
	}

	//Iterate over all registered events and subscribe to each one
	for(unsigned int i=0;i<m_evtSubscriptions.size();i++){
		std::string attr_name= m_evtSubscriptions[i]->attr_name;
		Tango::EventType evt_type= m_evtSubscriptions[i]->event_type;
		if(SubscribeToEvent(attr_name,evt_type,event_cb)<0){
			__WARN_LOG(m_dev,"Failed to subscribe to event (attr="<<attr_name<<", type="<<evt_type<<") for device proxy "<<m_proxy_name<<"), skip to next event...");
			continue;
		}
	}//end loop events

	return 0;

}//close SubscribeToEvents()


int TangoProxyHelper::UnsubscribeEvent(std::string attr_name,Tango::EventType event_type){

	yat::MutexLock guard(m_lock);

	//Check if event is registered in subscription collection
	int pos= -1;
	EvtSubscriptionDataPtr event_sub_data_ptr= find_event_subscription(pos,attr_name,event_type);
	if(!event_sub_data_ptr) {
		__DEBUG_LOG(m_dev,"Subscriptions not registered for proxy "<<m_proxy_name<<", nothing to be unsubscribed!");
		return 0;
	}

	//Unsubscribe to event
	if(unsubscribe_event(event_sub_data_ptr)<0){
		__WARN_LOG(m_dev,"Failed to unsubscribe from event (attr="<<attr_name<<", type="<<event_type<<")!");
		return 0;
	}

	
	return 0;

}//close UnsubscribeEvent()





bool TangoProxyHelper::IsSubscribedToEvent(std::string attr_name,Tango::EventType event_type){
			
	yat::MutexLock guard(m_lock);

	//Find subscription
	int pos= -1;
	EvtSubscriptionDataPtr item= find_event_subscription(pos,attr_name,event_type);
	if(!item) return false;
	return item->IsSubscribed();

}//close IsSubscribedToEvent()

int TangoProxyHelper::SetSubscribedToEvent(std::string attr_name,Tango::EventType event_type,int ev_id){
	
	yat::MutexLock guard(m_lock);
	
	//Find subscription
	int pos= -1;
	EvtSubscriptionDataPtr item= find_event_subscription(pos,attr_name,event_type);
	if(!item) return -1;
			
	//Set subscription id
	item->SetSubscribed(ev_id);		

	return 0;

}//close SetSubscribedToEvent()

int TangoProxyHelper::AddEventHandlers(std::string attr_name,Tango::EventType event_type,std::vector<std::string> event_handlers,long int execution_delay,int priority){
			
	yat::MutexLock guard(m_lock);
	
	//Find subscription
	int pos= -1;
	EvtSubscriptionDataPtr item= find_event_subscription(pos,attr_name,event_type);
	if(!item) {
		__WARN_LOG(m_dev,"No event subscription (attr="<<attr_name<<", evt_type="<<event_type<<") found!");
		return -1;
	}

	//Check if specified handlers are valid (e.g. they are existing cmd in device)
	for(unsigned int i=0;i<event_handlers.size();i++){
		std::string cmd_handler_name= event_handlers[i];
		if(Utils_ns::TangoUtils::HasCommand(m_dev,cmd_handler_name)<0){
			__WARN_LOG(m_dev,"Given cmd handler (cmd="<<cmd_handler_name<<") is invalid (not found in device), no handlers will be registered!");
			return -1;
		}
	}

	//Add handlers in evt subscription data
	//item->SetEvtHandlers(event_handlers,execution_delay,priority);
	item->AddEvtHandlers(event_handlers,execution_delay,priority);

	return 0;

}//close AddHandlers()


void TangoProxyHelper::ExecuteCommand(Tango::DeviceData& dout,std::string cmd_name,Tango::DeviceData& din){

	try {
		dout= m_proxy->command_inout(cmd_name,din);
	}	
	catch(...){
		throw;
	}

}//close ExecuteCommand()

void TangoProxyHelper::ExecuteCommand(Tango::DeviceData& dout,std::string cmd_name){

	try {
		dout= m_proxy->command_inout(cmd_name);
	}	
	catch(...){
		throw;
	}

}//close ExecuteCommand() (no argin)



void TangoProxyHelper::ExecuteCommandAsynch(long& cmd_id,std::string cmd_name,Tango::DeviceData& din){

	cmd_id= -1;
	bool forget = false;
	try {
		cmd_id= m_proxy->command_inout_asynch(cmd_name,din,forget);
	}	
	catch(...){
		throw;
	}
	
}//close ExecuteCommandAsync()

void TangoProxyHelper::ExecuteCommandAsynch(long& cmd_id,std::string cmd_name){

	cmd_id= -1;
	bool forget = false;
	try {
		cmd_id= m_proxy->command_inout_asynch(cmd_name,forget);
	}	
	catch(...){
		throw;
	}

}//close ExecuteCommandAsync() (no argin)



void TangoProxyHelper::GetAsynchCommandReply(Tango::DeviceData& dout,long cmd_id,long timeout){
	
	try {
		dout= m_proxy->command_inout_reply(cmd_id,timeout);
	}	
	catch(...){
		throw;
	}

}//close GetAsynchCommandReply()


void TangoProxyHelper::WriteAttributes(std::vector<Tango::DeviceAttribute>& da_list,int timeout){
			
	//Set timeout in proxy
	if(timeout>0){
		m_proxy->set_timeout_millis(timeout);
	}

	//Check args
	if(da_list.empty()){
		_THROW_TANGO_EXCEPTION("WRITE_ASYNCH_ATTR_FAILED","Empty device attribute list given!");			
	}

	//Write attributes to device
	try {
		m_proxy->write_attributes(da_list);
	}	
	catch(...){
		throw;
	}
		
}//close WriteAttributes()

int TangoProxyHelper::GetProxyAttributeInfo(Tango::AttributeInfoEx& attr_info,std::string attr_name){

	if(!m_proxy) return -1;

	//## Connect to proxy to get attr config info
	try{
		attr_info= m_proxy->get_attribute_config(attr_name);
	}
	catch(Tango::DevFailed& e){//ConnectionFailed,CommunicationFailed,DevFailed
		__WARN_LOG(m_dev,"Failed to connect to proxy attr "<<attr_name<<" for proxy "<<m_proxy_name<<" to get config info");
		return -1;
	}	

	return 0;

}//close GetProxyAttributeInfo()

int TangoProxyHelper::GetProxyInfo(std::vector<std::string>& proxy_info){

	yat::MutexLock guard(m_lock);

	proxy_info.clear();
	int runStatus= GetProxyRunStatus();
	std::string runStatusStr= m_runStateMap[runStatus];
	std::string json_proxy_info= "";
	if(GetProxyInfoInJson(json_proxy_info)<0){
		__WARN_LOG(m_dev,"Failed to get proxy info in json format!");
		return -1;
	}

	proxy_info.push_back(runStatusStr);
	proxy_info.push_back(json_proxy_info);

	return 0;

}//close GetProxyInfo()


void TangoProxyHelper::SetOffline(){
	
	yat::MutexLock guard(m_lock);
	SetOffline_i();
	
}//close SetOffline()

//*******************************************
//***       PRIVATE METHODS
//*******************************************
void TangoProxyHelper::SetOnline_i(const Tango::DevState& state,const std::string& status,const int& ping_time){
			
	//Create the timer if not done
	m_is_online= true;
	if(!m_timer) m_timer= new yat::Timer();//this starts the online timer if not started before

	//Disable downtime timeout (if enabled)
	if(m_downtimeActionTimeout) m_downtimeActionTimeout->disable();
		
	//Update proxy info
	m_status= status;
	m_state= state;
	m_ping_time= ping_time;
	m_timestamp.set_current();

	//Update time buffer (remove oldest item if buffer is full)
	int buffer_size= static_cast<int>(m_ping_time_buffer.size());
	if(buffer_size>=m_max_buffer_size){
		m_ping_time_buffer.pop_back();
	}
	m_ping_time_buffer.push_front(ping_time);
	
	//Update time avg 
	ComputePingTimeMean();
	
	//Other stuff?
	//...

}//close SetOnline_i()

void TangoProxyHelper::SetOffline_i(){

	//Create the timer if not done			
	if(!m_timer) {
		m_timer= new yat::Timer();//this starts the downtime timer
	}
	else{//Reset the timer if exists and proxy was online	
		if(m_is_online) m_timer->restart();
	}
			
	//Enable and start downtime action counter
	bool isEnabled= true;
	if(!m_downtimeActionTimeout) {	
		m_downtimeActionTimeout= new yat::Timeout(m_action_timeout,yat::Timeout::TMO_UNIT_SEC,isEnabled);//this starts the timeout
	}
	else {//Reset the timeout if exists and proxy was online
		if(!m_downtimeActionTimeout->enabled() || m_is_online){	
			m_downtimeActionTimeout->enable(true);//same of restart()
			m_downtimeActionTimeout->restart();
		}	
	}
			
	//Set offline flag
	m_is_online= false;

	//Clear buffer
	m_ping_time_buffer.clear();

}//close SetOffline_i()



void TangoProxyHelper::Init(){
	
	//Init proxy
	m_proxy= 0;
	m_is_initialized= false;

	//Performance pars
	m_state= Tango::UNKNOWN;
	m_status= "";
	m_ping_time= -1;
	m_ping_time_avg= -1;
	m_max_buffer_size= 10;
	m_is_online= false;
	m_ping_time_buffer.clear();

	//Init timers & timeout
	m_timer= 0;
	m_action_timeout= 10;//seconds
	m_downtimeActionTimeout= 0;

}//close Init()

void TangoProxyHelper::ComputePingTimeMean(){

	//Check buffer size
	size_t bufsize= m_ping_time_buffer.size();
	if(bufsize<=0) {
		m_ping_time_avg= 0;
		return;
	}

	//Compute mean
	m_ping_time_avg= 0; 
	for(unsigned int i=0;i<bufsize;i++){
		m_ping_time_avg+= m_ping_time_buffer[i];
	}
	m_ping_time_avg/= static_cast<float>(bufsize); 

}//close ComputePingAverage()


int TangoProxyHelper::GetProxyInfoInJson(std::string& jsonString){

	//Get json object representation
	Json::Value root;
	root["timestamp_local"]= m_timestamp.to_ISO8601_ms();//	m_timestamp.to_local_ISO8601_ms(); 
	root["timestamp_utc"]= m_timestamp.to_ISO8601_ms_TU();
	root["status"]= m_status;
	root["state"]= m_state;
	root["ping_time"]= m_ping_time;
	root["ping_time_avg"]= m_ping_time_avg;

	double timeElapsed= 0;
	if(m_timer) timeElapsed= m_timer->elapsed_sec();
	root["timeElapsedInRunStatus"]= timeElapsed;

	long int offlineExpirationTime= -1;
	if(m_downtimeActionTimeout && m_downtimeActionTimeout->enabled()) offlineExpirationTime= m_downtimeActionTimeout->time_to_expiration();
	root["offlineExpirationTime"]= static_cast<Json::Int64>(offlineExpirationTime);


	//Convert to string
	jsonString= "";
	if(CodeUtils::JsonToString(jsonString,root,true)<0){
		WARN_LOG("Failed to convert formula json representation in string!");
		return -1;
	}
	
	return 0;

}//close GetProxyInfoInJson()


int TangoProxyHelper::GetProxyRunStatus(){
			
	int proxyRunStatus= ePROXY_ONLINE;
	if(!m_is_online) {
		if(m_downtimeActionTimeout->enabled() && m_downtimeActionTimeout->expired()) proxyRunStatus= ePROXY_HARD_OFFLINE;
		else proxyRunStatus= ePROXY_SOFT_OFFLINE;
	}
	return proxyRunStatus;

}//close GetProxyRunStatus()

int TangoProxyHelper::add_attr_config_to_event_subscription(EvtSubscriptionDataPtr event_sub_data_ptr,std::string attr_name){

	//## Check input data
	if(!event_sub_data_ptr){
		__WARN_LOG(m_dev,"Null ptr to event sub data given!");
		return -1;
	}

	//## Connect to proxy to get attr config info
	__DEBUG_LOG(m_dev,"Trying to connect to attr proxy "<<attr_name<<" to get config info...");
	bool isConnectionFailed= false;
	Tango::AttributeInfoEx* attr_info= new Tango::AttributeInfoEx;
	try{
		*attr_info= m_proxy->get_attribute_config(attr_name);
	}
	catch(Tango::DevFailed& e){
		__WARN_LOG(m_dev,"Failed to connect to proxy "<<m_proxy_name<<" to get attr "<<attr_name<<" config info, will retry later...");
		if(attr_info){
			delete attr_info;
			attr_info= 0;
		}
		isConnectionFailed= true;
	}	

	//## If connection was established add attr config to event sub
	if(attr_info && !isConnectionFailed){
		__DEBUG_LOG(m_dev,"Adding attr "<<attr_name<<" config info to event subscription data of proxy "<<m_proxy_name<<" ...");
		event_sub_data_ptr->SetEventAttrInfo(attr_info);
	}

	return 0;

}//close add_attr_config_to_event_subscription()


EvtSubscriptionDataPtr TangoProxyHelper::find_event_subscription(int& pos,std::string attr_name,Tango::EventType event_type){

	//Find subscription in collection
	pos= -1;
	if(m_evtSubscriptions.empty()) return nullptr;
	EvtSubscriptionData data(m_proxy_name,attr_name,event_type);
	auto it = std::find_if(m_evtSubscriptions.begin(), m_evtSubscriptions.end(), [&](std::shared_ptr<EvtSubscriptionData> const& p) {
  	return *p == data; // assumes has operator==
	});
	if(it==m_evtSubscriptions.end()) return nullptr;

	//Return found item
	pos= (int)(it-m_evtSubscriptions.begin());
	return m_evtSubscriptions.at(pos);

}//close find_event_subscription()


int TangoProxyHelper::unsubscribe_event(EvtSubscriptionDataPtr event_sub_data_ptr){

	//Check arg
	if(!event_sub_data_ptr){
		__WARN_LOG(m_dev,"Null ptr to input subscription data given!");
		return -1;
	}

	//Check if already subscribed to this event
	if(!event_sub_data_ptr->IsSubscribed()){
		__DEBUG_LOG(m_dev,"Proxy "<<m_proxy_name<<" is not subscribed to event, nothing to be unsubscribed!");
		return 0;
	}
	
	//Unsubscribe event
	try{
		m_proxy->unsubscribe_event(event_sub_data_ptr->subscription_id);	
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		__WARN_LOG(m_dev,"Tango exception occurred when unsubscribing from event (id="<<event_sub_data_ptr->subscription_id<<")!");
		return -1;
	}

	//Set unsubscribed 
	event_sub_data_ptr->SetUnsubscribed();
	
	return 0;

}//close unsubscribe_event()

}//close namespace
