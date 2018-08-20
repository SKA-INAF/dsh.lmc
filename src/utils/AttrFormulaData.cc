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
// * Caesar is distributed in the hope that it will be useful, but 			    *
// * WITHOUT ANY WARRANTY; without even the implied warranty of             * 
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
// * See the GNU General Public License for more details. You should        * 
// * have received a copy of the GNU General Public License along with      * 
// * DSH.LMC. If not, see http://www.gnu.org/licenses/.                      *
// **************************************************************************

/**
* @file AttrFormulaData.cc
* @class AttrFormulaData
* @brief AttrFormulaData container
*
* AttrFormulaData container for Tango
* @author S. Riggi
* @date 15/01/2016
*/

#include <AttrFormulaData.h>
#include <AttrData.h>
#include <TangoUtils.h>

#include <exprtk.hpp>

#include <json/json.h>

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

using namespace std;

namespace Utils_ns {



// ============================================================================
// class: AttrFormulaVar
// ============================================================================
int AttrFormulaVar::SetAttrInfo(Tango::AttributeInfoEx* info,bool copy){

	//Check info			
	if(!info) return -1;

	//Destroy existing config
	ClearAttrInfo();

	//Set new info
	if(copy){
		attr_info= new Tango::AttributeInfoEx;
		*attr_info= *info;
	}
	else{	
		attr_info= info;
	}

	//Create attr data from this attr info
	if(CreateAttrData()<0){
		WARN_LOG("Failed to create attr data from info!");
		return -1;
	}

	return 0;
}//close SetAttrInfo()


int AttrFormulaVar::CreateAttrData(){

	//Check if attr info have been set
	if(!attr_info) return -1;

	//Create attr data according to info set
	Tango::AttrDataFormat data_format= attr_info->data_format;
 	int data_type=	attr_info->data_type;

	try{			
		if(data_format==Tango::SCALAR){
			switch (data_type) {
				case Tango::DEV_FLOAT: 
					CreateScalarAttrData<Tango::DevFloat>(Tango::DevFloat());
					break;
				case Tango::DEV_DOUBLE: 
					CreateScalarAttrData<Tango::DevDouble>(Tango::DevDouble());
					break;
				case Tango::DEV_LONG: 
					CreateScalarAttrData<Tango::DevLong>(Tango::DevLong());
					break;
				case Tango::DEV_SHORT: 
					CreateScalarAttrData<Tango::DevShort>(Tango::DevShort());
					break;
				case Tango::DEV_ENUM: 
					CreateScalarEnumAttrData(Tango::DevShort());
					break;
				case Tango::DEV_STRING: 
					CreateScalarAttrData<std::string>(std::string(""));
					break;
				case Tango::DEV_BOOLEAN: 
					CreateScalarAttrData<Tango::DevBoolean>(Tango::DevBoolean());
					break;
				case Tango::DEV_STATE: 
					CreateScalarAttrData<Tango::DevState>(Tango::UNKNOWN);
					break;
				default:
				{
					std::stringstream ss;
					ss<<"Unsupported data type ("<<data_type<<") for attribute "<<attr_name<<"!";
					ERROR_LOG(ss.str());
	      	_THROW_TANGO_EXCEPTION("ADD_FORMULA_ATTR_FAILED",ss.str());
        	break;
				}	
			}//close switch		
		}//close if SCALAR
			
		else if(data_format==Tango::SPECTRUM){
			switch (data_type) {
				case Tango::DEV_FLOAT: 
					CreateSpectrumAttrData<Tango::DevFloat>(Tango::DevFloat());
					break;
				case Tango::DEV_DOUBLE: 
					CreateSpectrumAttrData<Tango::DevDouble>(Tango::DevDouble());
					break;
				case Tango::DEV_LONG: 
					CreateSpectrumAttrData<Tango::DevLong>(Tango::DevLong());
					break;
				case Tango::DEV_SHORT: 
					CreateSpectrumAttrData<Tango::DevShort>(Tango::DevShort());
					break;
				case Tango::DEV_ENUM: 
					CreateSpectrumAttrData(Tango::DevShort());
					break;
				case Tango::DEV_STRING: 
					CreateSpectrumAttrData<std::string>(std::string(""));
					break;
				case Tango::DEV_BOOLEAN: 
					CreateSpectrumAttrData<Tango::DevBoolean>(Tango::DevBoolean());
					break;
				case Tango::DEV_STATE: 
					CreateSpectrumAttrData<Tango::DevState>(Tango::UNKNOWN);
					break;
				default:
				{
					std::stringstream ss;
					ss<<"Unsupported data type ("<<data_type<<") for attribute "<<attr_name<<"!";
					ERROR_LOG(ss.str());
	      	_THROW_TANGO_EXCEPTION("ADD_FORMULA_ATTR_FAILED",ss.str());
        	break;
				}	
			}//close switch
		}//close else if SPECTRUM

		else{
			std::stringstream ss;
			ss<<"Unsupported data format ("<<data_format<<") for attribute "<<attr_name<<"!";
			ERROR_LOG(ss.str());
	    _THROW_TANGO_EXCEPTION("ADD_FORMULA_ATTR_FAILED",ss.str());
		}

	}//close try 
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		ERROR_LOG("Tango exception occurred while creating and adding a formula attr to dyn attr "<<attr_name);
		return -1;
	}

	return 0;

}//close CreateAttrData()

// ============================================================================
// class: AttrFormulaData
// ============================================================================


std::string AttrFormulaData::GetFormulaInJson(){

	//Get json object representation
	Json::Value root;
	root["expression"]= m_formula;

	typedef std::map<std::string,AttrFormulaVarPtr>::iterator mapIter;
	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;	
		AttrFormulaVarPtr formula_var= it->second;
		std::string proxy_name= formula_var->device_name;
		std::string proxy_attr_name= formula_var->attr_name;

		Json::Value thisVar;
		thisVar[var_name.c_str()]= proxy_name + std::string("/") + proxy_attr_name;	
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


bool AttrFormulaData::IsInitialized(){

	//Iterate over attr to check if they have attr info stored
	typedef std::map<std::string,AttrFormulaVarPtr>::iterator mapIter;

	bool isInitialized= true;
	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;
		AttrFormulaVarPtr formula_var= it->second;
		
		if( !formula_var->HasAttrInfo() ){
			DEBUG_LOG("Formula attr var "<<var_name<<" is not initialized, formula marked as not initialized!");
			isInitialized= false;
			break;
		}
	}//end loop map items
	
	return isInitialized;

}//close IsInitialized()


int AttrFormulaData::SetAttrInfo(std::string var_name,Tango::AttributeInfoEx* attr_info,bool copy){

	//Check attr info arg
	if(!attr_info) {
		WARN_LOG("Null ptr to AttributeInfoEx given, nothing will be set!");		
		return -1;
	}

	//Get map iterator
	typedef std::map<std::string,AttrFormulaVarPtr>::iterator mapIter;
	mapIter it= m_symbols.find(var_name);
	if(m_symbols.empty() || it==m_symbols.end()) {
		WARN_LOG("Attr var "<<var_name<<" not found in formula (did you register it?)!");
		return -1;
	}

	//Check supported data type & format
	int data_type= attr_info->data_type;
	Tango::AttrDataFormat data_format= attr_info->data_format;
	if(data_format==Tango::SPECTRUM && data_type==Tango::DEV_STRING){
		WARN_LOG("Attr var "<<var_name<<" (type="<<data_type<<", format="<<data_format<<") is not supported in formula!");
		return -1;
	}
	if(data_format!=Tango::SCALAR && data_format!=Tango::SPECTRUM){
		WARN_LOG("Attr var "<<var_name<<" format (format="<<data_format<<") is not supported in formula!");
		return -1;
	}
			
	//Set attr info
	if( (it->second)->SetAttrInfo(attr_info,copy)<0) {
		WARN_LOG("Failed to set attr info in formula var "<<var_name<<"!");
		return -1;
	}

	return 0;

}//close SetAttrInfo()



int AttrFormulaData::SetEventValue(EvtSubscriptionDataPtr evt_sub_ptr,Tango::EventData* event_data){

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
	typedef std::map<std::string,AttrFormulaVarPtr>::iterator mapIter;
	mapIter item= m_symbols.begin();
	bool isFound= false;

	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;	
		std::string this_proxy_name= (it->second)->device_name;
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
		WARN_LOG("Failed to set attr value in formula var!");
		return -1;
	}
	
	return 0;

}//close SetEventValue()

bool AttrFormulaData::HasProxyInFormulaVars(std::string proxy_name){

	//Check arg
	if(proxy_name=="") return false;

	//Iterate over symbols and find if they contains the proxy name
	typedef std::map<std::string,AttrFormulaVarPtr>::iterator mapIter;
	bool isFound= false;

	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		std::string var_name= it->first;	
		std::string this_proxy_name= (it->second)->device_name;
		if(this_proxy_name==proxy_name){
			isFound= true;
			break;
		}
	}//end loop map items

	return isFound;

}//close HasProxyInFormulaVars()


}//close namespace
