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
* @file FormulaManager.cc
* @class FormulaManager
* @brief Formula manager
*
* Formula manager
* @author S. Riggi
* @date 15/01/2017
*/

#include <FormulaManager.h>

#ifndef _FORMULA_DATA_h
	#include <FormulaData.h>
#endif

#ifndef _TANGO_PROXY_MANAGER_h
	#include <TangoProxyManager.h>
#endif

#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS
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

FormulaManager::FormulaManager(Tango::DeviceImpl* dev,TangoProxyManager* proxy_mgr)
	: m_dev(dev), m_proxyManager(proxy_mgr)	
{
	//Clear formula map
	ClearFormulaMap();
}

FormulaManager::~FormulaManager(){

	//Clear formula map
	ClearFormulaMap();

}//close destructor



FormulaData* FormulaManager::FindFormula(std::string attr_name){
			
	//yat::MutexLock guard(m_lock);
	return FindFormula_i(attr_name);

}//close FindFormula()

int FormulaManager::GetFormulaDataAsJsonString(std::string& jsonString,std::string attr_name,bool include_formula_expr){

	//Init string
	jsonString= "";

	//Find formula
	FormulaData* formula_data= FindFormula_i(attr_name);
	if(!formula_data){
		__WARN_LOG(m_dev,"Formula attr "<<attr_name<<" not found in registered formulas (returning empty string)!");
		return -1;
	}

	//Get formula data
	return formula_data->GetFormulaDataInJson(jsonString,include_formula_expr);
	
}//close GetFormulaDataAsJsonString()


int FormulaManager::RegisterFormula(std::string attr_name,FormulaData* formula_data){
			
	//## Check args
	if(!m_dev || !m_proxyManager) return -1;
	if(!formula_data) return -1;

	//## Find if formula is already registered
	if(FindFormula_i(attr_name)){
		__DEBUG_LOG(m_dev,"Formula for attr "<<attr_name<<" is already registered, nothing to be done");
		return 0;
	} 

	//## Get formula data info
	Utils_ns::FormulaSymbols formula_symbols= formula_data->GetFormulaSymbols();
	std::vector<Tango::EventType> evt_types= formula_data->GetFormulaEvaluationMode();

	//## Register proxies and events involved in the formula
	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;	
	std::vector<std::string> registered_proxies;
	struct RegisteredEvent {
		std::string _proxy_name;
		std::string _attr_name;		
		std::vector<Tango::EventType> _evt_types;
	};

	std::vector<RegisteredEvent> registered_events;
	std::vector<RegisteredEvent> formula_attr_events;
	int status= 0;

	for(mapIter it=formula_symbols.begin();it!=formula_symbols.end();it++){
		//Get formula var info
		FormulaVarPtr formula_var= it->second;
		bool is_proxy_attr= formula_var->is_proxy_attr;
		if(!is_proxy_attr){//Skip internal attr
			__DEBUG_LOG(m_dev,"Formula var "<<it->first<<" is an internal attr, skip to next var...");
			continue;
		}

		std::string proxy_name= std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->device_name;
		std::string proxy_attr_name= formula_var->attr_name;
		__DEBUG_LOG(m_dev,"Registering proxy "<<proxy_name<<" and events (attr_name="<<proxy_attr_name<<")...");

		RegisteredEvent aFormulaEvent;
		aFormulaEvent._proxy_name= proxy_name;
		aFormulaEvent._attr_name= proxy_attr_name;
		aFormulaEvent._evt_types= evt_types;
		formula_attr_events.push_back(aFormulaEvent);

		//Check if proxy is already registered (e.g. done manually in the device)
		//If present, do not insert this in the list of registered proxy that can be removed in case of errors
		if(m_proxyManager->FindProxyHelper(proxy_name)){
			__DEBUG_LOG(m_dev,"Proxy "<<proxy_name<<" is already registered in device, check for events ...");
		}
		else {
			//Register proxy
			if(m_proxyManager->RegisterProxy(proxy_name)<0){
				__WARN_LOG(m_dev,"Failed to register proxy "<<proxy_name<<" present in formula (check proxy syntax)!");
				status= -1;
				break;
			}
			registered_proxies.push_back(proxy_name);
			
		}

		//Check if events are already registered
		std::vector<Tango::EventType> events_to_be_registered;
		for(unsigned int i=0;i<evt_types.size();i++){
			Tango::EventType event_type= evt_types[i];
			if(!m_proxyManager->FindEvtSubscription(proxy_name,attr_name,event_type)) {
				events_to_be_registered.push_back(event_type);
			}
		}

		//Register event subscriptions
		if(m_proxyManager->RegisterEvtSubscriptions(proxy_name,proxy_attr_name,events_to_be_registered)<0){
			__WARN_LOG(m_dev,"Failed to register events for attr "<<proxy_attr_name<<" of proxy "<<proxy_name<<" present in formula (check syntax/attr name)!");
			status= -1;
			break;
		}
		
		RegisteredEvent aRegisteredEvent;
		aRegisteredEvent._proxy_name= proxy_name;
		aRegisteredEvent._attr_name= proxy_attr_name;
		aRegisteredEvent._evt_types= events_to_be_registered;
		registered_events.push_back(aRegisteredEvent);

	}//end loop formula symbol vars
	
	//## Check for errors 
	//## NB: If any error is found all registered proxies and events for this formula will be unregistered from the device!
	if(status==-1){
		__WARN_LOG(m_dev,"Errors occurred while registering formula for attr "<<attr_name<<", unregistering all proxies and events ...");

		//Unregister all events first
		for(unsigned int i=0;i<registered_events.size();i++){
			if(m_proxyManager->UnregisterEvtSubscriptions(registered_events[i]._proxy_name,registered_events[i]._attr_name,registered_events[i]._evt_types)<0){
				__WARN_LOG(m_dev,"Failed to unsubscribe event (attr_name="<<registered_events[i]._attr_name<<") for proxy "<<registered_events[i]._proxy_name);
				continue;
			}
		}//end loop registered events

		//Unregister proxies previously registered in formula
		for(unsigned int i=0;i<registered_proxies.size();i++){
			if(m_proxyManager->UnregisterProxy(registered_proxies[i])<0){
				__WARN_LOG(m_dev,"Failed to unsubscribe proxy "<<registered_proxies[i]);
				continue;
			}
		}//end loop proxies

		return -1;
	}//close if
		
	//## If everything went fine add formula to map
	__DEBUG_LOG(m_dev,"Adding formula for attr "<<attr_name<<" in map...");
	FormulaData* aNewFormulaData= new FormulaData(formula_data->GetFormula(),formula_data->GetFormulaSymbols(),formula_data->GetFormulaEvaluationMode());
	aNewFormulaData->SetAttrName(attr_name);
	aNewFormulaData->SetPostActions(formula_data->GetPostActions());
	aNewFormulaData->SetInvalidQualityIfOldFlag(formula_data->GetInvalidQualityIfOldFlag());
	m_attrFormulaMap.insert( std::make_pair(attr_name,aNewFormulaData) );

	for(unsigned int i=0;i<formula_attr_events.size();i++){
		for(unsigned int j=0;j<(formula_attr_events[i]._evt_types).size();j++){
			if(FillEventAttrMap(attr_name,formula_attr_events[i]._proxy_name,formula_attr_events[i]._attr_name,(formula_attr_events[i]._evt_types)[j])<0){
				__WARN_LOG(m_dev,"Failed to add attr event to map, skip to next item...");
				continue;
			}	
		}//end loop events per attr
	}//end loop formula attr

	//DEBUG======
	typedef std::map<EvtSubscriptionData,std::vector<std::string>>::iterator mit;
	for(mit it=m_eventAttrsMap.begin();it!=m_eventAttrsMap.end();it++){
		std::stringstream ss;
		ss<<"proxy="<<(it->first).device_name<<", proxy_attr="<<(it->first).attr_name<<", evttype="<<(it->first).event_type_str<<" formula_attr{";
		for(unsigned int i=0;i<(it->second).size();i++){
			ss<<(it->second)[i]<<", ";
		}		
		ss<<"}";
		__DEBUG_LOG(m_dev,ss.str());
	}
	//==========

	/*
	//## Try also to initialize formula (if some attr proxy is offline this will fail, but it will be retried later in device)
	if(InitializeFormula(aNewFormulaData)<0){
		__DEBUG_LOG(m_dev,"Cannot initialize formula for attr "<<attr_name<<" (some proxy are offline?), will retry later...");
	}
	*/
	
	return 0;

}//close RegisterFormula()



int FormulaManager::GetRegisteredFormulasInJson(std::vector<std::string>& formula_list){
	
	formula_list.clear();

	//## Iterate over all registered formulas 
	typedef std::map<std::string,FormulaData*>::iterator mapIter; 
	for(mapIter it=m_attrFormulaMap.begin();it!=m_attrFormulaMap.end();it++){
		FormulaData* formula_data= it->second;
		std::string formula_json= formula_data->GetFormulaInJson();
		formula_list.push_back(formula_json);
	}//end loop registered formulas

	return 0;

}//close GetRegisteredFormulasInJson()


int FormulaManager::GetInitializedFormulasInJson(std::vector<std::string>& formula_list){
	
	formula_list.clear();

	//## Iterate over all registered formulas and select the initialized
	typedef std::map<std::string,FormulaData*>::iterator mapIter; 
	for(mapIter it=m_attrFormulaMap.begin();it!=m_attrFormulaMap.end();it++){
		FormulaData* formula_data= it->second;
		if(!formula_data->IsInitialized()) continue;
		std::string formula_json= formula_data->GetFormulaInJson();
		formula_list.push_back(formula_json);
	}//end loop registered formulas

	return 0;

}//close GetInitializedFormulasInJson()


int FormulaManager::GetFormulasLinkedToEvent(std::vector<std::string>& attr_name_list,std::vector<FormulaData*>& formula_data_list,Utils_ns::EvtSubscriptionDataPtr evt_sub_ptr){

	attr_name_list.clear();
	formula_data_list.clear();
	if(!evt_sub_ptr) return -1;

	//DEBUG======
	/*
	typedef std::map<EvtSubscriptionData,std::vector<std::string>>::iterator mapIter;
	for(mapIter it=m_eventAttrsMap.begin();it!=m_eventAttrsMap.end();it++){
		std::stringstream ss;
		ss<<"proxy="<<(it->first).device_name<<", proxy_attr="<<(it->first).attr_name<<", evttype="<<(it->first).event_type_str<<" formula_attr{";
		for(unsigned int i=0;i<(it->second).size();i++){
			ss<<(it->second)[i]<<", ";
		}		
		ss<<"}";
		__DEBUG_LOG(m_dev,ss.str());
	}
	//==========
	*/

	//Find evt subscription in map
	std::map<EvtSubscriptionData,std::vector<std::string>>::iterator it= m_eventAttrsMap.find(*evt_sub_ptr);
	bool hasData= (!m_eventAttrsMap.empty() && it!=m_eventAttrsMap.end());	
	if(!hasData) {
		__DEBUG_LOG(m_dev,"Given event data are not present in formula map (check if formula is registered?)!");
		return -1;
	}

	//Get list of attr names
	attr_name_list= it->second;
	__DEBUG_LOG(m_dev,"#"<<attr_name_list.size()<<" formula attrs linked to given event...");

	//Get list of formula data
	for(unsigned int i=0;i<attr_name_list.size();i++){
		FormulaData* formula_data= FindFormula(attr_name_list[i]);
		if(!formula_data){
			__WARN_LOG(m_dev,"Cannot find formula data associated to attr "<<attr_name_list[i]<<" (check for bugs, this should not occur)!");	
			attr_name_list.clear();
			formula_data_list.clear();
			return -1;
		}
		//__DEBUG_LOG(m_dev,"Is formula initialized? "<<formula_data->IsInitialized());

		formula_data_list.push_back(formula_data);
	}//end loop matching attr

	return 0;

}//close GetFormulasLinkedToEvent()

int FormulaManager::GetFormulasLinkedToProxy(std::vector<std::string>& attr_name_list,std::vector<FormulaData*>& formula_data_list,std::string proxy_name){

	//Check args
	attr_name_list.clear();
	formula_data_list.clear();
	if(proxy_name=="") return -1;

	//Iterate over registered formulas and get all containing proxy_name in their vars
	typedef std::map<std::string,FormulaData*>::iterator mapIter; 
	for(mapIter it=m_attrFormulaMap.begin();it!=m_attrFormulaMap.end();it++){
		std::string attr_name= it->first;
		FormulaData* formula_data= it->second;
		if(formula_data && formula_data->HasProxyInFormulaVars(proxy_name)){
			attr_name_list.push_back(attr_name);
			formula_data_list.push_back(formula_data);
		}
	}//end loop map items

	return 0;

}//close GetFormulasLinkedToProxy()


int FormulaManager::GetFormulaDataList(std::vector<std::string>& attr_name_list,std::vector<FormulaData*>& formula_data_list){

	typedef std::map<std::string,FormulaData*>::iterator mapIter; 
	for(mapIter it=m_attrFormulaMap.begin();it!=m_attrFormulaMap.end();it++){
		FormulaData* formula_data= it->second;
		attr_name_list.push_back(it->first);
		formula_data_list.push_back(formula_data);
	}//end loop map items

	return 0;

}//close GetFormulaDataList()



int FormulaManager::FillEventAttrMap(std::string& attr_name,std::string& proxy_name,std::string& proxy_attr_name,Tango::EventType& evt_type){

	__DEBUG_LOG(m_dev,"Filling event-attr map (attr="<<attr_name<<",proxy="<<proxy_name<<",proxy_attr="<<proxy_attr_name<<",evt_type="<<evt_type<<")");

	//Create a EvtSubscriptionData with args given
	EvtSubscriptionData evt_sub_data(proxy_name,proxy_attr_name,evt_type);

	//Find if data are present in map
	std::map<EvtSubscriptionData,std::vector<std::string>>::iterator it= m_eventAttrsMap.find(evt_sub_data);
	bool hasData= (!m_eventAttrsMap.empty() && it!=m_eventAttrsMap.end());	

	if(hasData) {
		//Check if attr name is already inserted in map, if not add value to list
		__DEBUG_LOG(m_dev,"Event already present in map, check if attr "<<attr_name<<" is present");
		int pos= -1;
		if(!CodeUtils::FindItem(it->second, attr_name, pos)) {
			__DEBUG_LOG(m_dev,"Attr "<<attr_name<<" not present in map, adding it...");	
			(it->second).push_back(attr_name);
		}
	}
	else{
		//Create an entry in map
		__DEBUG_LOG(m_dev,"Creating entry in map for attr "<<attr_name<<" ...");	
		std::vector<std::string> attr_list{attr_name};
		m_eventAttrsMap.insert( std::make_pair(evt_sub_data,attr_list) );
	}
	
	return 0;

}//close FillEventAttrMap()


int FormulaManager::UpdateEventValueInFormulas(EvtSubscriptionDataPtr evt_sub_ptr,Tango::EventData* event_data){

	//## Check args
	if(!evt_sub_ptr || !event_data) return -1;

	//DEBUG======
	/*
	typedef std::map<EvtSubscriptionData,std::vector<std::string>>::iterator mapIter;
	for(mapIter it=m_eventAttrsMap.begin();it!=m_eventAttrsMap.end();it++){
		std::stringstream ss;
		ss<<"proxy="<<(it->first).device_name<<", proxy_attr="<<(it->first).attr_name<<", evttype="<<(it->first).event_type_str<<" formula_attr{";
		for(unsigned int i=0;i<(it->second).size();i++){
			ss<<(it->second)[i]<<", ";
		}		
		ss<<"}";
		__DEBUG_LOG(m_dev,ss.str());
	}
	//==========
	*/

	//## Find all formulas linked to this event
	std::vector<std::string> attr_name_list;
	std::vector<Utils_ns::FormulaData*> formula_data_list;
	if(GetFormulasLinkedToEvent(attr_name_list,formula_data_list,evt_sub_ptr)<0){
		__WARN_LOG(m_dev,"Failed to retrieve formula associated to received event (proxy="<<evt_sub_ptr->device_name<<", attr="<<evt_sub_ptr->attr_name<<", event_type="<<evt_sub_ptr->event_type_str<<")...");
		return -1;
	}

	__DEBUG_LOG(m_dev,"#"<<formula_data_list.size()<<" formulas associated to received event (proxy="<<evt_sub_ptr->device_name<<", attr="<<evt_sub_ptr->attr_name<<", event_type="<<evt_sub_ptr->event_type_str<<")...");

	//## Iterate over formula data and set event value 
	int status= 0;

	for(unsigned int i=0;i<formula_data_list.size();i++){
		if(formula_data_list[i]->SetEventValue(evt_sub_ptr,event_data)<0){
			__WARN_LOG(m_dev,"Failed to set value in formula ");
			status= 1;
			continue;
		}
	}//end loop formulas

	return status;

}//close UpdateEventValueInFormulas()


//*******************************************
//***       PRIVATE METHODS
//*******************************************
void FormulaManager::ClearFormulaMap(){

	//Clear attr formula map
	typedef std::map<std::string,FormulaData*>::iterator mapIter;
	for(mapIter it=m_attrFormulaMap.begin();it!=m_attrFormulaMap.end();it++){
		if(it->second){
			delete it->second;
			it->second= 0;
		}
	}//end loop map items
	m_attrFormulaMap.clear();

	//Clear event attr map
	m_eventAttrsMap.clear();
	
}//close ClearFormulaMap()


FormulaData* FormulaManager::FindFormula_i(std::string attr_name){
			
	//Check dev
	if(!m_dev) return nullptr;

	FormulaData* formula_data= nullptr;
	std::map<std::string,FormulaData*>::iterator it= m_attrFormulaMap.find(attr_name);
	bool hasFormula= (!m_attrFormulaMap.empty() && it!=m_attrFormulaMap.end());	
	if(hasFormula) formula_data= it->second;

	return formula_data;

}//close FindFormula_i()

}//close namespace


