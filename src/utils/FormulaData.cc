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
* @file FormulaData.cc
* @class FormulaData
* @brief FormulaData container
*
* FormulaData container for Tango
* @author S. Riggi
* @date 15/01/2016
*/

#include <FormulaData.h>

#ifndef _FORMULA_VAR_h
	#include <FormulaVar.h>
#endif
#ifndef _ATTR_DATA_h
	#include <AttrData.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

#ifndef INCLUDE_EXPRTK_HPP
#include <exprtk.hpp>
#endif

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

// ============================================================================
// class: FormulaData
// ============================================================================

FormulaData::FormulaData(const std::string& formula,const FormulaSymbols& symbols,const std::vector<Tango::EventType> evaluation_modes)
	: m_formula(formula), m_symbols(symbols), m_evaluation_modes(evaluation_modes)
{
	m_attrName= "";	
	m_postActions.clear();
	m_setInvalidQualityIfOld= false;
}

FormulaData::FormulaData() {
	m_formula= "";
	m_attrName= "";
	m_postActions.clear();
	m_setInvalidQualityIfOld= false;
}

FormulaData::~FormulaData(){
			
};

int FormulaData::GetFormulaDataInJson(std::string& jsonString,bool include_formula_expr){

	//Init string 
	jsonString= "";

	//## Get json object representation
	Json::Value root;

	//Add expression formula?
	if(include_formula_expr) root["expression"]= m_formula;

	/*
	{
	"formula_expr": "a+b+c",
	"formula_vars": [
     {"var_name": "alias1","attr_name": "name1"},
		 {"var_name": "alias2","attr_name": "name1"}
	 ]
	}
	*/

	//Iterate over variables
	Json::Value var_list;
	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;
	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;	
		FormulaVarPtr formula_var= it->second;
		bool is_proxy_attr= formula_var->is_proxy_attr;
		std::string proxy_attr_name= formula_var->attr_name;
		std::string formula_attr_name= proxy_attr_name;
		if(is_proxy_attr) {
			std::string proxy_name= std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->device_name;
			formula_attr_name= proxy_name + std::string("/") + proxy_attr_name;	
		} 
		std::string attr_data_str= "";
		if(formula_var->GetDataAsString(attr_data_str)<0){
			WARN_LOG("Failed to get formula var "<<var_name<<" data as string, set empty string (hint: check if formula var is initialized and has data stored!");
		}

		//Create formula var json and append to list
		Json::Value thisVar;
		thisVar["var_name"]= var_name;
		thisVar["attr_name"]= formula_attr_name;
		thisVar["is_proxy_attr"]= is_proxy_attr;
		thisVar["attr_data"]= attr_data_str;

		root["formula_vars"].append(thisVar);
	}//end loop map items		

	for(unsigned int i=0;i<m_evaluation_modes.size();i++){	
		root["formula_eval_modes"].append( Json::Value(m_evaluation_modes[i]) );
	}

	//Convert to string
	if(CodeUtils::JsonToString(jsonString,root,true)<0){
		WARN_LOG("Failed to convert formula json representation in string!");
		return -1;
	}
			
	return 0;

}//close GetFormulaDataInJson()


std::string FormulaData::GetFormulaInJson(){

	//Get json object representation
	Json::Value root;
	root["expression"]= m_formula;

	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;
	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;	
		FormulaVarPtr formula_var= it->second;
		bool is_proxy_attr= formula_var->is_proxy_attr;
		std::string proxy_attr_name= formula_var->attr_name;

		Json::Value thisVar;
		if(is_proxy_attr){
			std::string proxy_name= std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->device_name;
			thisVar[var_name.c_str()]= proxy_name + std::string("/") + proxy_attr_name;	
		}
		else {
			thisVar[var_name.c_str()]= proxy_attr_name;	
		}
		root["vars"].append(thisVar);
	}//end loop map items
		
	for(unsigned int i=0;i<m_evaluation_modes.size();i++){	
		root["evaluation"].append( Json::Value(m_evaluation_modes[i]) );
	}

	//Convert to string
	std::string jsonString= "";
	if(CodeUtils::JsonToString(jsonString,root,true)<0){
		WARN_LOG("Failed to convert formula json representation in string!");
		return "";
	}
			
	return jsonString;

};//close GetFormulaInJson()


bool FormulaData::IsInitialized(){

	//Iterate over attr to check if they have attr info stored
	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;

	bool isInitialized= true;
	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;
		FormulaVarPtr formula_var= it->second;
		
		if( !formula_var->IsInitialized() ){
			DEBUG_LOG("Formula attr var "<<var_name<<" is not initialized, formula marked as not initialized!");
			isInitialized= false;
			break;
		}
	}//end loop map items
	
	return isInitialized;

}//close IsInitialized()


int FormulaData::SetInternalAttrData(std::string var_name,AttrData* attr_data){

	//Check attr data arg
	if(!attr_data) {
		WARN_LOG("Null ptr to AttrData given, nothing will be set!");		
		return -1;
	}

	//Get map iterator
	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;
	mapIter it= m_symbols.find(var_name);
	if(m_symbols.empty() || it==m_symbols.end()) {
		WARN_LOG("Attr var "<<var_name<<" not found in formula (did you register it?)!");
		return -1;
	}

	
	//Check if formula var is indeed an internal attr
	if((it->second)->is_proxy_attr){
		WARN_LOG("Attr var "<<var_name<<" is a proxy attr var and not an internal as it should be!");
		return -1;
	}
	
	//Set internal attr data (if succesful this will make var initialized)
	if( (it->second)->SetAttrData(attr_data)<0) {
		WARN_LOG("Failed to set attr data in formula var "<<var_name<<"!");
		return -1;
	}
	DEBUG_LOG("Formula var "<<var_name<<" initialized? "<<(it->second)->IsInitialized());

	return 0;

}//close SetInternalAttrData()

int FormulaData::SetProxyAttrData(std::string var_name,Tango::AttributeInfoEx& attr_info){

	//Get map iterator
	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;
	mapIter it= m_symbols.find(var_name);
	if(m_symbols.empty() || it==m_symbols.end()) {
		WARN_LOG("Attr var "<<var_name<<" not found in formula (did you register it?)!");
		return -1;
	}

	//Check supported data type & format
	int data_type= attr_info.data_type;
	Tango::AttrDataFormat data_format= attr_info.data_format;

	#if TANGO_VERSION_MAJOR >= 9
	if(data_format==Tango::SPECTRUM && data_type==Tango::DEV_ENUM){
		WARN_LOG("Attr var "<<var_name<<" (type="<<data_type<<", format="<<data_format<<") is not supported in formula!");
		return -1;
	}
	#endif

	if(data_format!=Tango::SCALAR && data_format!=Tango::SPECTRUM){
		WARN_LOG("Attr var "<<var_name<<" format (format="<<data_format<<") is not supported in formula!");
		return -1;
	}
			
	//Check if formula var is indeed a proxy attr
	if(!(it->second)->is_proxy_attr){
		WARN_LOG("Attr var "<<var_name<<" is not a proxy attr var!");
		return -1;
	}
	
	//Set proxy attr data (if succesful this will make var initialized)
	if( std::dynamic_pointer_cast<ProxyAttrFormulaVar>(it->second)->CreateAttrData(attr_info)<0) {
		WARN_LOG("Failed to set attr data in formula var "<<var_name<<"!");
		return -1;
	}

	return 0;

}//close SetProxyAttrData()


int FormulaData::SetEventValue(EvtSubscriptionDataPtr evt_sub_ptr,Tango::EventData* event_data){

	//Check args
	if(!evt_sub_ptr) {
		WARN_LOG("Null ptr to evt subscription data given!");
		return -1;
	}
	if(!event_data){
		WARN_LOG("Null ptr to event data given!");
		return -1;
	}

	std::string proxy_name= evt_sub_ptr->device_name;
	std::string proxy_attr_name= evt_sub_ptr->attr_name;
	
	//Find attr var in map by device & attr name
	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;
	mapIter item= m_symbols.begin();
	bool isFound= false;

	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;	
		bool is_proxy_attr= (it->second)->is_proxy_attr;
		if(!is_proxy_attr) continue;//skip internal attr in formula (events are received from proxy attr)
		std::string this_proxy_name= (std::dynamic_pointer_cast<ProxyAttrFormulaVar>(it->second))->device_name;
		std::string this_proxy_attr_name= (it->second)->attr_name;

		//Compare with case insensitivity
		DEBUG_LOG("this_proxy_name="<<this_proxy_name.c_str()<<", proxy_name="<<proxy_name.c_str());
		DEBUG_LOG("this_proxy_attr_name="<<this_proxy_attr_name.c_str()<<", proxy_attr_name="<<proxy_attr_name.c_str());
		if(stricmp(this_proxy_name.c_str(),proxy_name.c_str())==0 && stricmp(this_proxy_attr_name.c_str(),proxy_attr_name.c_str())==0){
			item= it;
			isFound= true;
			break;
		}
	}//end loop map items

	if(!isFound){
		WARN_LOG("Failed to retrieve formula attr var with proxy="<<proxy_name<<" and attr="<<proxy_attr_name<<"!");
		return -1;
	}

	//Set value in var
	if( (item->second)->SetAttrValue(event_data->attr_value)<0){
		WARN_LOG("Failed to set attr "<<(item->second)->attr_name<<" value in formula defined for attr "<<m_attrName<<"!");
		return -1;
	}
	
	return 0;

}//close SetEventValue()

bool FormulaData::HasProxyInFormulaVars(std::string proxy_name){

	//Check arg
	if(proxy_name=="") return false;

	//Iterate over symbols and find if they contains the proxy name
	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;
	bool isFound= false;

	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;	
		bool is_proxy_attr= (it->second)->is_proxy_attr;
		if(!is_proxy_attr) continue;//skip internal attr in formula (events are received from proxy attr)
		std::string this_proxy_name= (std::dynamic_pointer_cast<ProxyAttrFormulaVar>(it->second))->device_name;
		
		if(this_proxy_name==proxy_name){
			isFound= true;
			break;
		}
	}//end loop map items

	return isFound;

}//close HasProxyInFormulaVars()


}//close namespace
