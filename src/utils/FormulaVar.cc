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
* @file FormulaVar.cc
* @class FormulaVar
* @brief FormulaVar container
*
* FormulaVar container for Tango
* @author S. Riggi
* @date 15/01/2016
*/

#include <FormulaVar.h>


#ifndef _ATTR_DATA_h
	#include <AttrData.h>
#endif
#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
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

using namespace std;

namespace Utils_ns {

// ============================================================================
// class: FormulaVar
// ============================================================================

FormulaVar::FormulaVar(std::string attrname,int value_type) 
	: attr_name(attrname), attr_value_type(value_type)
{
	//attr_info= 0;
	attr_data= 0;
	is_proxy_attr= true;
};

FormulaVar::~FormulaVar() {
	//ClearAttrInfo();
	if(is_proxy_attr) ClearAttrData();
};

/*
int FormulaVar::SetAttrInfo(Tango::AttributeInfoEx* info,bool copy){

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

	return 0;

}//close SetAttrInfo()
*/

int FormulaVar::GetDataAsJsonObj(Json::Value& root){

	//Get formula var attr data in string form
	std::string attr_data_str= "";
	if(attr_data){
		if(attr_data->get_data_str(attr_data_str)<0){
			WARN_LOG("Failed to retrieve formula attr data in string form, setting empty string");
			attr_data_str= "";
		} 	
	}
	else{
		WARN_LOG("Formula var has no data stored, setting empty string...");
	}

	//Fill json object
	root["attr_name"]= attr_name;
	root["attr_value_type"]= attr_value_type;
	root["is_proxy_attr"]= is_proxy_attr;
	root["attr_data"]= attr_data_str;

	return 0;

}//close GetDataAsJsonObj()

// ============================================================================
// class: ProxyAttrFormulaVar
// ============================================================================

ProxyAttrFormulaVar::ProxyAttrFormulaVar(std::string devname,std::string attrname,int value_type) 
	: FormulaVar(attrname,value_type), device_name(devname)
{
	is_proxy_attr= true;
	attr_init_value_str= "";
	has_init_value= false;
	exp_data_type= -1;
	exp_data_format= -1;
}//close constructor

ProxyAttrFormulaVar::~ProxyAttrFormulaVar()
{

}//close destructor

/*
int ProxyAttrFormulaVar::SetAttrInfo(Tango::AttributeInfoEx* info,bool copy){

	//Call base method		
	if(FormulaVar::SetAttrInfo(info,copy)<0){
		return -1;
	}

	//Create attr data from this attr info
	if(CreateAttrData()<0){
		WARN_LOG("Failed to create attr data from info!");
		return -1;
	}

	return 0;

}//close SetAttrInfo()
*/

int ProxyAttrFormulaVar::CreateAttrData(Tango::AttributeInfoEx& attr_info){

	//Destroy existing attr data
	ClearAttrData();
	
	//Create attr data according to info set
	Tango::AttrDataFormat data_format= attr_info.data_format;
 	int data_type=	attr_info.data_type;
	int data_size= attr_info.max_dim_x;

	try{			
		if(data_format==Tango::SCALAR){
			switch (data_type) {
				case Tango::DEV_FLOAT: 
					attr_data= new Utils_ns::ScalarAttrData<Tango::DevFloat>(attr_name,data_type);
					break;
				case Tango::DEV_DOUBLE: 
					attr_data= new Utils_ns::ScalarAttrData<Tango::DevDouble>(attr_name,data_type);
					break;
				case Tango::DEV_LONG: 
					attr_data= new Utils_ns::ScalarAttrData<Tango::DevLong>(attr_name,data_type);
					break;
				case Tango::DEV_SHORT: 
					attr_data= new Utils_ns::ScalarAttrData<Tango::DevShort>(attr_name,data_type);
					break;
				#if TANGO_VERSION_MAJOR >= 9
				case Tango::DEV_ENUM: 
				{
					std::vector<std::string> enum_labels= attr_info.enum_labels;
					if(enum_labels.empty()){
						std::stringstream ss;
						ss<<"Empty enum labels for attribute "<<attr_name<<"!";
						WARN_LOG(ss.str());
	      		_THROW_TANGO_EXCEPTION("CREATE_ATTR_DATA_FAILED",ss.str());
					}
					//attr_data= new Utils_ns::ScalarEnumAttrData(attr_name,data_type,enum_labels);
					attr_data= new Utils_ns::ScalarAttrData<Tango::DevEnum>(attr_name,data_type);
					attr_data->set_labels(enum_labels);
					break;
				}
				#endif
				case Tango::DEV_STRING: 
					attr_data= new Utils_ns::ScalarAttrData<std::string>(attr_name,data_type);
					break;
				case Tango::DEV_BOOLEAN: 
					attr_data= new Utils_ns::ScalarAttrData<Tango::DevBoolean>(attr_name,data_type);
					break;
				case Tango::DEV_STATE: 
					attr_data= new Utils_ns::ScalarAttrData<Tango::DevState>(attr_name,data_type,Tango::UNKNOWN);
					break;
				default:
				{
					std::stringstream ss;
					ss<<"Unsupported data type ("<<data_type<<") for attribute "<<attr_name<<"!";
					ERROR_LOG(ss.str());
	      	_THROW_TANGO_EXCEPTION("CREATE_ATTR_DATA_FAILED",ss.str());
        	break;
				}	
			}//close switch		
		}//close if SCALAR
			
		else if(data_format==Tango::SPECTRUM){
			switch (data_type) {
				case Tango::DEV_FLOAT: 
					attr_data = new Utils_ns::SpectrumAttrData<Tango::DevFloat>(attr_name,data_type,data_size);
					break;
				case Tango::DEV_DOUBLE: 
					attr_data = new Utils_ns::SpectrumAttrData<Tango::DevDouble>(attr_name,data_type,data_size);
					break;
				case Tango::DEV_LONG: 
					attr_data = new Utils_ns::SpectrumAttrData<Tango::DevLong>(attr_name,data_type,data_size);
					break;
				case Tango::DEV_SHORT: 
					attr_data = new Utils_ns::SpectrumAttrData<Tango::DevShort>(attr_name,data_type,data_size);
					break;
				#if TANGO_VERSION_MAJOR >= 9
				case Tango::DEV_ENUM: 
					attr_data = new Utils_ns::SpectrumAttrData<Tango::DevShort>(attr_name,data_type,data_size);
					break;
				#endif
				case Tango::DEV_STRING: 
					attr_data = new Utils_ns::SpectrumAttrData<std::string>(attr_name,data_type,data_size);
					break;
				case Tango::DEV_BOOLEAN: 
					attr_data = new Utils_ns::SpectrumAttrData<Tango::DevBoolean>(attr_name,data_type,data_size);
					break;
				case Tango::DEV_STATE: 
					attr_data = new Utils_ns::SpectrumAttrData<Tango::DevState>(attr_name,data_type,data_size,Tango::UNKNOWN);
					break;
				default:
				{
					std::stringstream ss;
					ss<<"Unsupported data type ("<<data_type<<") for attribute "<<attr_name<<"!";
					ERROR_LOG(ss.str());
	      	_THROW_TANGO_EXCEPTION("CREATE_ATTR_DATA_FAILED",ss.str());
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



int ProxyAttrFormulaVar::SetAttrValue(Tango::DeviceAttribute* da){
			
	if(!da){
		DEBUG_LOG("Null ptr to given DeviceAttribute!");
		return -1;	
	}	
			
	if(!attr_data) {
		DEBUG_LOG("Null ptr to given AttrData!");
		return -1;	
	}			
	
	return attr_data->set_data(da);
		
}//close SetAttrValue()

// ============================================================================
// class: InternalAttrFormulaVar
// ============================================================================
InternalAttrFormulaVar::InternalAttrFormulaVar(std::string attrname,int value_type) 
	: FormulaVar(attrname,value_type)
{
	is_proxy_attr= false;
	
}//close constructor

InternalAttrFormulaVar::~InternalAttrFormulaVar()
{

}//close destructor


}//close namespace
