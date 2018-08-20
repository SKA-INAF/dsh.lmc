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
* @file TangoUtils.cc
* @class TangoUtils
* @brief Utility functions for Tango
*
* Utility functions for Tango
* @author S. Riggi
* @date 15/01/2016
*/

#include <TangoUtils.h>

#ifndef _CODE_UTILS_h
	#include <CodeUtils.h>
#endif

#ifndef _TANGO_PROXY_MANAGER_h
	#include <TangoProxyManager.h>
#endif

#ifndef _YAT_STRING_TOKENIZER_H_
	#include <yat/utils/StringTokenizer.h>
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

StringBiMapSetPtr RuleFactory::m_cmdRulesPtr= 0;
StringBiMapSetPtr RuleFactory::m_attrRulesPtr= 0;
StringBiMapSetPtr RuleFactory::m_attrReadRulesPtr= 0;
StringBiMapSetPtr RuleFactory::m_attrWriteRulesPtr= 0;
StringMapSetPtr RuleFactory::m_attrWritePreActionPtr= 0;
StringMapSetPtr RuleFactory::m_attrWritePostActionPtr= 0;

/**
* \brief Init Tango symbol table
*/
symbol_table_t InitTangoSymbolTable() {
	//Fill Tango symbols to be used in formulas
	symbol_table_t tmp;

	//States
	tmp.add_constant("ON", (unsigned int)Tango::ON);
	tmp.add_constant("OFF", (unsigned int)Tango::OFF);
	tmp.add_constant("CLOSE", (unsigned int)Tango::CLOSE);
	tmp.add_constant("OPEN", (unsigned int)Tango::OPEN);
	tmp.add_constant("INSERT", (unsigned int)Tango::INSERT);
	tmp.add_constant("EXTRACT", (unsigned int)Tango::EXTRACT);
	tmp.add_constant("MOVING", (unsigned int)Tango::MOVING);
	tmp.add_constant("STANDBY", (unsigned int)Tango::STANDBY);
	tmp.add_constant("FAULT", (unsigned int)Tango::FAULT);
	tmp.add_constant("INIT", (unsigned int)Tango::INIT);
	tmp.add_constant("RUNNING", (unsigned int)Tango::RUNNING);
	tmp.add_constant("ALARM", (unsigned int)Tango::ALARM);
	tmp.add_constant("DISABLE", (unsigned int)Tango::DISABLE);
	tmp.add_constant("UNKNOWN", (unsigned int)Tango::UNKNOWN);

	//Quality Factors
	tmp.add_constant("ATTR_VALID", (unsigned int)Tango::ATTR_VALID);
	tmp.add_constant("ATTR_INVALID", (unsigned int)Tango::ATTR_INVALID);
	tmp.add_constant("ATTR_ALARM", (unsigned int)Tango::ATTR_ALARM);
	tmp.add_constant("ATTR_CHANGING", (unsigned int)Tango::ATTR_CHANGING);
	tmp.add_constant("ATTR_WARNING", (unsigned int)Tango::ATTR_WARNING);

	return tmp;
}//close InitTangoSymbolTable()

symbol_table_t TangoUtils::tango_symbols(InitTangoSymbolTable());

/**
* \brief Init Tango state map
*/
std::map<std::string,Tango::DevState> InitTangoStateStrMap() {

	//Fill map items
	std::map<std::string,Tango::DevState> tmp
	{
  	std::pair<std::string,Tango::DevState> ("ON",Tango::ON),
    std::pair<std::string,Tango::DevState> ("OFF",Tango::OFF),
    std::pair<std::string,Tango::DevState> ("CLOSE",Tango::CLOSE),
    std::pair<std::string,Tango::DevState> ("OPEN",Tango::OPEN),
    std::pair<std::string,Tango::DevState> ("INSERT",Tango::INSERT),
    std::pair<std::string,Tango::DevState> ("EXTRACT",Tango::EXTRACT),
    std::pair<std::string,Tango::DevState> ("MOVING",Tango::MOVING),
    std::pair<std::string,Tango::DevState> ("STANDBY",Tango::STANDBY),
    std::pair<std::string,Tango::DevState> ("FAULT",Tango::FAULT),
    std::pair<std::string,Tango::DevState> ("INIT",Tango::INIT),
    std::pair<std::string,Tango::DevState> ("RUNNING",Tango::RUNNING),
    std::pair<std::string,Tango::DevState> ("ALARM",Tango::ALARM),
    std::pair<std::string,Tango::DevState> ("DISABLE",Tango::DISABLE),
    std::pair<std::string,Tango::DevState> ("UNKNOWN",Tango::UNKNOWN)
	};
	return tmp;

}//close InitTangoStateStrMap()
std::map<std::string,Tango::DevState> TangoUtils::m_tangoStateStrMap(InitTangoStateStrMap()); 

TangoUtils::TangoUtils()
{

}//close constructor


TangoUtils::~TangoUtils()
{

}//close destructor

int TangoUtils::TangoStateStrToDevState(Tango::DevState& state,std::string state_str){
			
	if(m_tangoStateStrMap.empty()) return -1;//shoul not occur since map is initialized 
	std::map<std::string,Tango::DevState>::iterator it= m_tangoStateStrMap.find(state_str);
	if(it==m_tangoStateStrMap.end()) return -1;//Invalid State string given!
	state= it->second;
	return 0;
		
}//close TangoStateStrToDevState()


int TangoUtils::GetDevicesInServer(std::vector<std::string>& device_names, std::string server_full_name, bool prepend_tango_host){

	device_names.clear();

	//Get DB instance
	Tango::Database* db= 0;
	try{
		db= (Tango::Util::instance())->get_database();
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		ERROR_LOG("Failed to get access to Database!");
		return -1;
	}
	
	//Get class list in server
	std::vector<std::string> dev_list;
	try{
		Tango::DbDatum db_datum= db->get_device_class_list(server_full_name);
		db_datum >> dev_list;
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		ERROR_LOG("Failed to get access to class list in server "<<server_full_name<<"!");
		return -1;
	}

	//Get Tango host name
	std::string tango_host= GetTangoDBFacility();
	
	//Extract device names
	for(unsigned int i=0;i<dev_list.size()-1;i+=2){
		std::string device_name= dev_list[i];
		std::string class_name= dev_list[i+1];
		if(class_name=="DServer") continue;
		std::string full_device_name= device_name;
		if(prepend_tango_host) full_device_name= std::string("tango://") + tango_host + std::string("/") + device_name;
		device_names.push_back(full_device_name);
	}//end loop class-device list

	return 0;

}//close GetDevicesInServer()

bool TangoUtils::IsProxyAttr(std::string url){

	yat::StringTokenizer st(url, "/");
  size_t nt = st.count_tokens();
	if (nt==3 || nt==6) return true;
  return false;

}//close IsProxyAttr()

bool TangoUtils::IsFQDNProxyAttr(std::string url){

	yat::StringTokenizer st(url, "/");
  size_t nt = st.count_tokens();
	
	DEBUG_LOG("Checking if proxy attr "<<url<<" is FQDN (nt="<<nt<<")");
	if (nt==6) return true;
	return false;

}//close IsFQDNProxyAttr()


int TangoUtils::GetFQDNDeviceNameFromAlias(std::string& fqdn_device_name,std::string alias){
	
	if(alias=="") return -1;

	//Get facility DB name 
	std::string tango_host= GetTangoDBFacility();

	//Get database info
	std::string device_name= "";
	Tango::Database* db = 0;
	try{
		db = new Tango::Database();
		db->get_device_from_alias(alias,device_name);
	}
	catch (Tango::DevFailed& e) {
		Tango::Except::print_exception(e);
		WARN_LOG("Tango exception occurred while retrieving device name from alias!");
		if(db){
			delete db;
			db= 0;
		}
		return -1;
	}
	catch(...){
		WARN_LOG("Unknown exception occurred while retrieving device name from alias!");			
		if(db){
			delete db;
			db= 0;
		}
		return -1;
	}

	//Fill FQDN device name
	fqdn_device_name= std::string("tango://") + tango_host + std::string("/") + device_name;

	//Clear Database
	if(db){
		delete db;
		db= 0;
	}

	return 0;

}//close GetFQDNDeviceNameFromAlias()



int TangoUtils::ParseFullAttrName(std::string& device_name,std::string& attr_name,std::string full_attr_name){
	
	device_name= "";
	attr_name= "";
	const char* c = full_attr_name.c_str();
	int ind = 0;
	int num_slashes= 3;	//not FQDN
						
	if(full_attr_name.find("tango://") != string::npos)	//FQDN!!
		num_slashes = 6;
						
	while (*c) {
		if (*c == '/') ind++;
		if (ind < num_slashes)
			device_name.push_back(*c);
		else if (*c != '/')
			attr_name.push_back(*c);
		c++;
	}//end loop

	if(device_name=="" || attr_name=="") return -1;
	
	return 0;

}//close ParseFullAttrName()


int TangoUtils::ParseFullAttrName(std::string& domain_name,std::string& device_name,std::string& attr_name,bool& hasQualityTag, std::string full_attr_name){

	hasQualityTag= false;
	domain_name= "";
	device_name= "";
	attr_name= "";
	
	//Detect if name is a FQDN
	std::size_t pos_slash = full_attr_name.find("tango://");
	
	if (pos_slash != std::string::npos) {//FQDN
		//Looking for first occurrence of ':' after 'tango://' prefix		
		std::string::size_type pos_colon = full_attr_name.find(":",8);	//look for first : after tango://

		//Looking for first occurrence of '/' after 'tango://' prefix
		pos_slash = full_attr_name.find('/',8);			
		
		if(pos_colon<pos_slash && pos_colon != string::npos && pos_slash != std::string::npos) {
			domain_name= full_attr_name.substr(0,pos_slash);
		}
		else {
			WARN_LOG("Unexpected full attr name given (name="<<full_attr_name<<"), cannot parse domain name!");
			return -1;
		}

		//Looking for first occurrence of '/' after domain prefix
		std::size_t pos_device_name = full_attr_name.find("/",pos_slash);

		//Looking for last occurrence of '/' after device name
		std::size_t pos_attr_name = full_attr_name.find_last_of("/");

		if(pos_device_name!=std::string::npos && pos_attr_name!=std::string::npos){
			device_name= full_attr_name.substr(pos_slash+1,pos_attr_name-pos_slash-1);
			attr_name= full_attr_name.substr(pos_attr_name+1);
		}
		else {
			WARN_LOG("Unexpected full attr name given (name="<<full_attr_name<<"), cannot parse device and attr name!");
			return -1;
		}

	}//close if
	else{//No domain name is given (domain_name will be an empty string!)

		//Check if invalid attr name (e.g. localhost:10000/dev/family/id/myattr)
		std::string::size_type pos_colon = full_attr_name.find(":");
		if(pos_colon!=std::string::npos){
			WARN_LOG("Invalid full attr name given (name="<<full_attr_name<<"), a ':' is present but given name is not FQDN!");
			return -1;
		}

		//Check if a '//' is present
		std::string::size_type pos_double_slash = full_attr_name.find("//");
		if(pos_double_slash!=std::string::npos){
			WARN_LOG("Invalid full attr name given (name="<<full_attr_name<<"), a '//' is present but given name is not FQDN!");
			return -1;
		}

		//Parse string fields
		domain_name= "";
		pos_slash= 0;

		std::size_t pos_device_name = full_attr_name.find("/",pos_slash);
		std::size_t pos_attr_name = full_attr_name.find_last_of("/");

		if(pos_device_name!=std::string::npos && pos_attr_name!=std::string::npos){
			//device_name= full_attr_name.substr(0,pos_attr_name-pos_slash-1);
			device_name= full_attr_name.substr(0,pos_attr_name);
			attr_name= full_attr_name.substr(pos_attr_name+1);
		}
		else {
			WARN_LOG("Unexpected full attr name given (name="<<full_attr_name<<"), cannot parse device and attr name!");
			return -1;
		}
	}

	//Check if quality is specified in the attr name
	std::size_t pos_quality = attr_name.find(".quality");
	if(pos_quality!=std::string::npos){	
		hasQualityTag= true;
		std::string attr_name_wquality= attr_name.substr(0,attr_name.length()-8);
		attr_name= attr_name_wquality;
	}

	return 0;

}//close ParseFullAttrName()


int TangoUtils::GetPlainProxyName(std::string& proxy_name,std::string full_proxy_name)
{
	//Init proxy name to empty string
	proxy_name= "";

	//Parse proxy name to get domain, family and device name
	std::string proxy_facility_name= "";
	std::string proxy_domain_name= "";
	std::string proxy_family_name= "";
	std::string proxy_member_name= "";
	if(Utils_ns::TangoUtils::ParseFullProxyName(proxy_facility_name,proxy_domain_name,proxy_family_name,proxy_member_name,full_proxy_name)<0){
		WARN_LOG("Failed to parse proxy name "<<proxy_name<<" (NB: will not create proxy attr!)");	
		return -1;
	}

	//Set proxy name			
	proxy_name= proxy_domain_name + std::string("/") + proxy_family_name + std::string("/") + proxy_member_name;
	
	return 0;

}//close GetPlainProxyName()

int TangoUtils::ParseFullProxyName(std::string& facility_name,std::string& domain_name,std::string& family_name,std::string& member_name,std::string full_proxy_name){

	//Detect if name is a FQDN
	std::size_t pos_slash = full_proxy_name.find("tango://");
	std::string proxy_name= "";

	if (pos_slash != std::string::npos) {//FQDN!!
		//Looking for first occurrence of ':' after 'tango://' prefix		
		std::string::size_type pos_colon = full_proxy_name.find(":",8);	//look for first : after tango://

		//Looking for first occurrence of '/' after 'tango://' prefix
		pos_slash = full_proxy_name.find('/',8);			
		
		if(pos_colon<pos_slash && pos_colon != string::npos && pos_slash != std::string::npos) {
			facility_name= full_proxy_name.substr(0,pos_slash);
		}
		else {
			WARN_LOG("Unexpected full proxy name given (name="<<full_proxy_name<<"), cannot parse domain name!");
			return -1;
		}

		//Looking for first occurrence of '/' after domain prefix
		std::size_t pos_proxy_name = full_proxy_name.find("/",pos_slash);

		if(pos_proxy_name!=std::string::npos){
			proxy_name= full_proxy_name.substr(pos_slash+1,pos_proxy_name-pos_slash-1);
		}
		else {
			WARN_LOG("Unexpected full proxy name given (name="<<full_proxy_name<<"), cannot parse proxy name without domain!");
			return -1;
		}
	}
	else {
		facility_name= "";
		proxy_name= full_proxy_name;
	}

	//Parse domain part of the proxy name
	std::size_t pos_domain_name = proxy_name.find("/");
	if(pos_domain_name==std::string::npos){
		WARN_LOG("Failed to parse domain part of proxy name "<<full_proxy_name<<"!");
		return -1;
	}
	domain_name= proxy_name.substr(0,pos_domain_name);

	//Parse family part of the proxy name
	std::size_t pos_family_name = proxy_name.find("/",pos_domain_name+1);
	if(pos_family_name==std::string::npos){
		WARN_LOG("Failed to parse family part of proxy name "<<full_proxy_name<<"!");
		return -1;
	}
	family_name= proxy_name.substr(pos_domain_name+1,pos_family_name-pos_domain_name-1);
	
	//Parse member part of the proxy name
	std::size_t pos_member_name = proxy_name.find("/",pos_family_name);
	if(pos_member_name==std::string::npos){
		WARN_LOG("Failed to parse member part of proxy name "<<full_proxy_name<<"!");
		return -1;
	}
	member_name= proxy_name.substr(pos_family_name+1);

	return 0;

}//close ParseFullProxyName()


int TangoUtils::ExtractCmdName(std::string& cmd_name){

	cmd_name= "";
	std::string fcn_name= __FUNCTION__;
	std::size_t pos_prefix = fcn_name.find("is_");
	if(pos_prefix==std::string::npos) return -1;
	std::size_t pos_subfix = fcn_name.find("_allowed",pos_prefix+1);
	if(pos_subfix==std::string::npos) return -1;

	cmd_name= fcn_name.substr(pos_prefix+1,pos_subfix-pos_prefix-1);
	
	return 0;

}//close ExtractCmdName()


int TangoUtils::GetAdminProxyName(std::string& proxy_adm_name,std::string proxy_name)
{
	proxy_adm_name= "";

	//Get Database instance
	Tango::Database* db= 0;
	try{
		db= Tango::Util::instance()->get_database();
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		WARN_LOG("Tango exception while retrieving Database instance!");
		return -1;
	}
	catch(...){
		WARN_LOG("Unknown exception while retrieving Database instance!");
		return -1;
	}

	//The  db->get_device_info() command fails with FQDN, so get the standard name
	//Parse proxy name to get domain, family and device name
	std::string proxy_facility_name= "";
	std::string proxy_domain_name= "";
	std::string proxy_family_name= "";
	std::string proxy_member_name= "";
	if(Utils_ns::TangoUtils::ParseFullProxyName(proxy_facility_name,proxy_domain_name,proxy_family_name,proxy_member_name,proxy_name)<0){
		WARN_LOG("Failed to parse proxy name "<<proxy_name<<"!");	
		return -1;
	}
	
	std::string proxy_name_no_fqdn= proxy_domain_name + std::string("/") + proxy_family_name + std::string("/") + proxy_member_name;
	
	//Find admin device name of given proxy
	Tango::DbDevFullInfo dev_info;
	try{
		dev_info= db->get_device_info(proxy_name_no_fqdn);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		WARN_LOG("Tango exception while retrieving proxy "<<proxy_name_no_fqdn<<" info from DB!");
		return -1;
	}
	catch(...){
		WARN_LOG("Unknown exception while retrieving proxy "<<proxy_name_no_fqdn<<" info from DB!");
		return -1;
	}
	
	//Get device server name
	std::string server_name= dev_info.ds_full_name;
	proxy_adm_name= "dserver/" + server_name;

	return 0;

}//close GetAdminProxyName()

int TangoUtils::DeviceAttributeToCorbaAny(CORBA::Any& any,Tango::DeviceAttribute* da,Tango::DeviceImpl* dev){

	if(!da || !dev) return -1;

	std::string attr_name= da->name;
	Tango::AttrDataFormat	attr_format= da->data_format;
	int	data_type= da->data_type;

	try {
		if(attr_format==Tango::SCALAR){
			
			switch (data_type) {
				case Tango::DEV_STATE: 
				{
					Tango::DevState attr_value;
					(*da) >> attr_value;
					any <<= attr_value;
					break;
				}
				case Tango::DEV_FLOAT: 
				{
					float attr_value;
					(*da) >> attr_value; 
					any <<= attr_value;
					break;
				}
				case Tango::DEV_DOUBLE: 
				{
					double attr_value;
					(*da) >> attr_value; 
					any <<= attr_value;
					break;
				}
				case Tango::DEV_LONG: 
				{
					Tango::DevLong attr_value;
					(*da) >> attr_value; 
					any <<= attr_value;
					break;
				}
				case Tango::DEV_SHORT: 
				{
					Tango::DevShort attr_value;
					(*da) >> attr_value; 
					any <<= attr_value;
					break;
				}
				#if TANGO_VERSION_MAJOR >= 9
				case Tango::DEV_ENUM: 
				{
					Tango::DevShort attr_value;
					(*da) >> attr_value; 
					any <<= attr_value;
					break;
				}
				#endif
				case Tango::DEV_STRING: 
				{
					std::string attr_value;
					(*da) >> attr_value; 
					any <<= attr_value.c_str();
					break;
				}
				case Tango::DEV_BOOLEAN: 
				{
					Tango::DevBoolean attr_value;
					(*da) >> attr_value; 
					any <<= CORBA::Any::from_boolean(attr_value);
					break;
				}
				default:
				{
					std::stringstream ss;
					ss<<"Unsupported data type ("<<data_type<<") for attribute "<<attr_name<<"!";
	      	_THROW_TANGO_EXCEPTION("DEVATTR_TO_CORBA_ANY_FAILED",ss.str());
       	 	break;
				}	
			}//close switch
		}//SCALAR 

		else if(attr_format==Tango::SPECTRUM){
			switch (data_type) {
				case Tango::DEV_STATE: 
				{
					Tango::DevVarStateArray* attr_values;
					(*da) >> attr_values;
					any <<= (*attr_values);//makes a deep copy
					if(attr_values) delete attr_values;
					break;
				}
				case Tango::DEV_FLOAT: 
				{
					Tango::DevVarFloatArray* attr_values;
					(*da) >> attr_values;
					any <<= *attr_values;//makes a deep copy
					if(attr_values) delete attr_values;
					break;
				}
				case Tango::DEV_DOUBLE: 
				{
					Tango::DevVarDoubleArray* attr_values;
					(*da) >> attr_values;
					any <<= *attr_values;//makes a deep copy
					if(attr_values) delete attr_values;
					break;
				}
				case Tango::DEV_LONG: 
				{
					Tango::DevVarLongArray* attr_values;
					(*da) >> attr_values;
					any <<= *attr_values;//makes a deep copy
					if(attr_values) delete attr_values;
					break;
				}
				case Tango::DEV_SHORT: 
				{
					Tango::DevVarShortArray* attr_values;
					(*da) >> attr_values;
					any <<= *attr_values;//makes a deep copy
					if(attr_values) delete attr_values;
					break;
				}
				#if TANGO_VERSION_MAJOR >= 9
				case Tango::DEV_ENUM: 
				{
					Tango::DevVarShortArray* attr_values;
					(*da) >> attr_values;
					any <<= *attr_values;//makes a deep copy
					if(attr_values) delete attr_values;
					break;
				}
				#endif
				case Tango::DEV_STRING: 
				{
					Tango::DevVarStringArray* attr_values;
					(*da) >> attr_values;
					any <<= *attr_values;//makes a deep copy
					if(attr_values) delete attr_values;
					break;
				}
				case Tango::DEV_BOOLEAN: 
				{
					Tango::DevVarBooleanArray* attr_values;
					(*da) >> attr_values;
					any <<= *attr_values;//makes a deep copy
					if(attr_values) delete attr_values;
					break;
				}
				default:
				{
					std::stringstream ss;
					ss<<"Unsupported data type ("<<data_type<<") for attribute "<<attr_name<<"!";
	      	_THROW_TANGO_EXCEPTION("DEVATTR_TO_CORBA_ANY_FAILED",ss.str());
       	 	break;
				}	
			}//close switch

		}//SPECTRUM

		else{
			std::stringstream ss;
			ss<<"Unsupported/invalid data format ("<<attr_format<<")!";
			_THROW_TANGO_EXCEPTION("DEVATTR_TO_CORBA_ANY_FAILED",ss.str());
			return -1;
		}
	}//close try block
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		__ERROR_LOG(dev,"Tango exception occurred while extracting DeviceAttribute data!");
		return -1;	
	}
	catch(std::exception& e){
		__ERROR_LOG(dev,"C++ exception occurred while extracting DeviceAttribute data (err="<<e.what()<<")!");
		return -1;	
	}
	catch(...){
		__ERROR_LOG(dev,"Unknown exception occurred while extracting DeviceAttribute data!");
		return -1;
	}

	return 0;

}//close DeviceAttributeToCorbaAny()


int TangoUtils::CorbaAnyToJsonObj(Json::Value& root,CORBA::Any& any,int data_type,int data_format){

	try {
		if(data_format==Tango::SCALAR){
			
			switch (data_type) {
				case Tango::DEV_STATE: 
				{
					CorbaAnyDataConverter<Tango::DevState,unsigned int>::convert_scalar_to_json(root,any);
					break;
				}
				case Tango::DEV_FLOAT: 
				{
					CorbaAnyDataConverter<Tango::DevFloat,Tango::DevFloat>::convert_scalar_to_json(root,any);
					break;
				}
				case Tango::DEV_DOUBLE: 
				{
					CorbaAnyDataConverter<Tango::DevDouble,Tango::DevDouble>::convert_scalar_to_json(root,any);
					break;
				}
				case Tango::DEV_LONG: 
				{
					CorbaAnyDataConverter<Tango::DevLong,Tango::DevLong>::convert_scalar_to_json(root,any);
					break;
				}
				case Tango::DEV_SHORT: 
				{
					CorbaAnyDataConverter<Tango::DevShort,Tango::DevShort>::convert_scalar_to_json(root,any);
					break;
				}
				#if TANGO_VERSION_MAJOR >= 9
				case Tango::DEV_ENUM: 
				{
					CorbaAnyDataConverter<Tango::DevShort,Tango::DevShort>::convert_scalar_to_json(root,any);
					break;
				}
				#endif
				case Tango::DEV_STRING: 
				{
					CorbaAnyDataConverter<Tango::DevString,Tango::DevString>::convert_scalar_to_json(root,any);				
					break;
				}
				case Tango::DEV_BOOLEAN: 
				{
					CorbaAnyDataConverter<Tango::DevBoolean,Tango::DevBoolean>::convert_scalar_to_json(root,any);				
					break;
				}
				default:
				{
					std::stringstream ss;
					ss<<"Unsupported data type ("<<data_type<<") given for conversion!";
	      	_THROW_TANGO_EXCEPTION("CORBA_ANY_TO_JSON_FAILED",ss.str());
       	 	break;
				}	
			}//close switch
		}//SCALAR 

		
		else if(data_format==Tango::SPECTRUM){
			switch (data_type) {
				case Tango::DEV_STATE: 
				{
					CorbaAnyDataConverter<Tango::DevVarStateArray,unsigned int>::convert_spectrum_to_json(root,any);							
					break;
				}			
				case Tango::DEV_FLOAT: 
				{
					CorbaAnyDataConverter<Tango::DevVarFloatArray,Tango::DevFloat>::convert_spectrum_to_json(root,any);
					break;
				}
				case Tango::DEV_DOUBLE: 
				{
					CorbaAnyDataConverter<Tango::DevVarDoubleArray,Tango::DevDouble>::convert_spectrum_to_json(root,any);
					break;
				}
				case Tango::DEV_LONG: 
				{
					CorbaAnyDataConverter<Tango::DevVarLongArray,Tango::DevLong>::convert_spectrum_to_json(root,any);
					break;
				}
				case Tango::DEV_SHORT: 
				{
					CorbaAnyDataConverter<Tango::DevVarShortArray,Tango::DevShort>::convert_spectrum_to_json(root,any);
					break;
				}
				#if TANGO_VERSION_MAJOR >= 9
				case Tango::DEV_ENUM: 
				{
					CorbaAnyDataConverter<Tango::DevVarShortArray,Tango::DevShort>::convert_spectrum_to_json(root,any);
					break;
				}
				#endif
				case Tango::DEV_STRING: 
				{
					CorbaAnyDataConverter<Tango::DevVarStringArray,Tango::DevString>::convert_spectrum_to_json(root,any);
					break;
				}
				case Tango::DEV_BOOLEAN: 
				{
					CorbaAnyDataConverter<Tango::DevVarBooleanArray,Tango::DevBoolean>::convert_spectrum_to_json(root,any);
					break;
				}
				default:
				{
					std::stringstream ss;
					ss<<"Unsupported data type ("<<data_type<<") given for conversion!";
	      	_THROW_TANGO_EXCEPTION("CORBA_ANY_TO_JSON_FAILED",ss.str());
       	 	break;
				}	
			}//close switch

		}//SPECTRUM
		

		else{
			std::stringstream ss;
			ss<<"Unsupported data type ("<<data_type<<") given for conversion!";
	    _THROW_TANGO_EXCEPTION("CORBA_ANY_TO_JSON_FAILED",ss.str());
			return -1;
		}

	}//close try block
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		return -1;	
	}
	catch(std::exception& e){
		return -1;	
	}
	catch(...){
		return -1;
	}

	return 0;

}//close CorbaAnyToJsonObj()

#if TANGO_VERSION_MAJOR >= 9
int TangoUtils::GetEnumAttrLabelValueMap(std::map<int,std::string>& attrValLabelMap,std::map<std::string,int>& attrLabelValMap,Tango::DeviceImpl* dev,std::string attr_name){

	//Clear existing entries
	attrValLabelMap.clear();
	attrLabelValMap.clear();

	//Check device ptr
	if(!dev) return -1;

	//Check if attribute exists
	long data_type;
	Tango::MultiAttrProp<Tango::DevEnum> multi_attr_prop;
	try{
		Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_type= attr.get_data_type();
		attr.get_properties(multi_attr_prop);
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		__ERROR_LOG(dev,"Attribute "<<attr_name<<" does not exist in device!");
		return -1;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		__ERROR_LOG(dev,"Attribute "<<attr_name<<" does not exist in device (unknown exception)!");
		return -1;
	}

	//Check this is an enum
	if(data_type!=Tango::DEV_ENUM){
		__ERROR_LOG(dev,"Invoked method is only for enum attribute!");
		return -1;
	}

	//Retrieve enum labels
	std::vector<std::string> enum_labels= multi_attr_prop.enum_labels;
	if(enum_labels.size()<=0){
		__ERROR_LOG(dev,"No labels are present for enum attribute "<<attr_name<<"!");
		return -1;
	}

	//Fill maps
	for(int i=0;i<(int)(enum_labels.size());i++){
		int id= i;
		std::string label= enum_labels[i];
		attrValLabelMap.insert( make_pair<int&,std::string&>(id,label) );
		attrLabelValMap.insert( make_pair<std::string&,int&>(label,id) );
	}//end loop labels
	
	return 0;

}//close GetEnumAttrLabelValueMap()
#endif

#if TANGO_VERSION_MAJOR >= 9
int TangoUtils::GetEnumAttrValue(Tango::DevShort& enumValue,Tango::DeviceImpl* dev,std::string attr_name,std::string enumLabel){

	//Check device ptr
	if(!dev) return -1;
	enumValue= -1;

	//Check if attribute exists
	long data_type;
	Tango::MultiAttrProp<Tango::DevEnum> multi_attr_prop;
	try{
		Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_type= attr.get_data_type();
		attr.get_properties(multi_attr_prop);
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		__ERROR_LOG(dev,"Attribute "<<attr_name<<" does not exist in device!");
		return -1;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		__ERROR_LOG(dev,"Attribute "<<attr_name<<" does not exist in device (unknown exception)!");
		return -1;
	}

	//Check this is an enum
	if(data_type!=Tango::DEV_ENUM){
		__ERROR_LOG(dev,"Invoked method is only for enum attribute!");
		return -1;
	}

	//Retrieve enum labels
	std::vector<std::string> enum_labels= multi_attr_prop.enum_labels;
	if(enum_labels.size()<=0){
		__ERROR_LOG(dev,"No labels are present for enum attribute "<<attr_name<<", cannot retrieve enum code!");
		return -1;
	}

	//Find if given enum string is present in label list
	std::vector<std::string>::iterator it= std::find(enum_labels.begin(),enum_labels.end(),enumLabel);
	if(it==enum_labels.end()){
		__ERROR_LOG(dev,"Given label ("<<enumLabel<<") not found among enum label list, cannot retrieve enum code!");
		return -1;
	}
	
	//Set enum value	
	enumValue= static_cast<Tango::DevShort>(it-enum_labels.begin());
	__DEBUG_LOG(dev,"Given label ("<<enumLabel<<") corresponds to enum code="<<enumValue);
	
	return 0;

}//close GetEnumAttrValue()
#endif

#if TANGO_VERSION_MAJOR >= 9
int TangoUtils::GetEnumAttrLabelValue(std::string& enumLabel,Tango::DeviceImpl* dev,std::string attr_name,Tango::DevShort enumValue){

	//Check device ptr
	if(!dev) return -1;
	enumLabel= "";

	//Check if attribute exists
	long data_type;
	Tango::MultiAttrProp<Tango::DevEnum> multi_attr_prop;
	try{
		Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_type= attr.get_data_type();
		attr.get_properties(multi_attr_prop);
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		__ERROR_LOG(dev,"Attribute "<<attr_name<<" does not exist in device!");
		return -1;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		__ERROR_LOG(dev,"Attribute "<<attr_name<<" does not exist in device (unknown exception)!");
		return -1;
	}

	//Check this is an enum
	if(data_type!=Tango::DEV_ENUM){
		__ERROR_LOG(dev,"Invoked method is only for enum attribute!");
		return -1;
	}

	//Retrieve enum labels
	std::vector<std::string> enum_labels= multi_attr_prop.enum_labels;
	if(enum_labels.size()<=0){
		__ERROR_LOG(dev,"No labels are present for enum attribute "<<attr_name<<", cannot retrieve enum code!");
		return -1;
	}

	//Check if given enum code exceeds number of labels
	if(enumValue<0 || enumValue>(int)(enum_labels.size()-1)){
		std::stringstream ss;
		ss<<"Invalid enum argument value given ("<<enumValue<<"), valid values are {0,"<<enum_labels.size()-1<<"]";
		__ERROR_LOG(dev,ss.str());
		return -1;
	}

	//Set string enum attr from labels
	enumLabel= enum_labels[enumValue];

	return 0;

}//close GetEnumAttrLabelValue()
#endif

Tango::Group* TangoUtils::GetSubGroup(Tango::DeviceImpl* dev,Tango::Group* grp,std::vector<std::string>& device_names,bool requireDevOnline){

	if(!dev) return 0;

	//Check input group and device names
	if(!grp) {
		__ERROR_LOG(dev,"Null ptr to input group given, returning null ptr!");
		return 0;
	}
	if(device_names.empty()) {
		__ERROR_LOG(dev,"Empty list of device names given, returning null ptr!");
		return 0;
	}

	//Extract sub-group from main group
	Tango::Group* subgrp= 0;
	bool isFailed= false;
	bool hasDevOffline= false;
	for(unsigned int i=0;i<device_names.size();i++){
		std::string dev_name= device_names[i];

		//Get i-th device from group
		//If requireDevOnline is true, do not add device to the group
		Tango::DeviceProxy* dp= 0;
		try{
			dp= grp->get_device(dev_name);
		}
		catch(Tango::DevFailed& e){
			hasDevOffline= true;			
			if(requireDevOnline) {
				__WARN_LOG(dev,"Device "<<dev_name<<" exists in group but is found offline, do not add to subgroup...");
				continue;
			}
		}

		//Check if device does not exist in the group
		if (!dp) {//device does not belong to the group, exit loop
			__WARN_LOG(dev,"Device "<<dev_name<<" in list does not belong to the group!");
      isFailed= true;
			break;
   	}

		//Add device to subgroup			
		if(!subgrp) subgrp= new Tango::Group("subgrp");
		try {
			subgrp->add(dev_name);
		}	
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			__WARN_LOG(dev,"Failed to add device "<<dev_name<<" in subgroup!");
      isFailed= true;
			break;
		}
	}//end loop device names

	//If at least one device is not belonging to the group, return failure
	if(isFailed){
		__WARN_LOG(dev,"Extract subgroup failed (at least one device is not belonging to the group or cannot be added to subgroup)!");
		if(subgrp){
			delete subgrp;
			subgrp= 0;
		}
		return 0;
	}

	//If at least one device is offline and requireDevOnline is true, return failure
	if(requireDevOnline && hasDevOffline){
		__WARN_LOG(dev,"Failed to extract subgroup (at least one device is offline wilst requested all online)!");
		if(subgrp){
			delete subgrp;
			subgrp= 0;
		}
		return 0;
	}

	//Check subgrp size
	if(subgrp && subgrp->get_size()<=0){
		__WARN_LOG(dev,"Created subgrp is empty!");
		if(subgrp){
			delete subgrp;
			subgrp= 0;
		}
		return 0;
	}

	return subgrp;

}//close GetSubGroup()



int TangoUtils::SetAppenderLogLevel(Tango::DeviceImpl* dev,std::string appender_name,Tango::DevLong log_level) {
	
	if(!dev) return -1;

	#ifdef APPENDERS_HAVE_LEVEL_THRESHOLD
	try {
		//Convert given Tango log level in log4 code
		log4tango::Level::Value log4_level= Tango::Logging::tango_to_log4tango_level(static_cast<Tango::LogLevel>(log_level), true);

		//Get appender from logger
		log4tango::Logger* logger= dev->get_logger();
		log4tango::Appender* appender= logger->get_appender(appender_name);
		if(!appender){
			__ERROR_LOG(dev,"No appender with name "<<appender_name<<" found in the appender list!");
			return -1;
		}
		appender->set_level(log4_level);

	}//close try
	catch(Tango::DevFailed& e){
		__ERROR_LOG(dev,"Failed to set appender log level!");
		return -1;
	}
	#else
		__WARN_LOG(dev,"APPENDERS_HAVE_LEVEL_THRESHOLD flag was disabled when TANGO was built, cannot set log level for separate appenders!");
		return -1;
	#endif

	return 0;

}//close SetAppenderLogLevel()

int TangoUtils::GetLogAppenderType(log4tango::Appender* appender){
	
	if(!appender) return -1;
	std::string appender_name= appender->get_name();

	//Check if file appender
	std::size_t found = appender_name.find("file::");
	if (found!=std::string::npos) return Tango::LogTarget::LOG_FILE;
		
	//Check if console appender
	found = appender_name.find("console::");
	if (found!=std::string::npos) return Tango::LogTarget::LOG_CONSOLE;
	
	//Check if device appender
	found = appender_name.find("device::");
	if (found!=std::string::npos) return Tango::LogTarget::LOG_DEVICE;
	
	return -1;

}//close GetLogAppenderType()


log4tango::Level::Value TangoUtils::GetTangoLogLevelFromString(std::string sLevel) 
{
	//Return log4tango from string  		
	if (sLevel == "DEBUG") return log4tango::Level::DEBUG;
	else if (sLevel == "INFO") return log4tango::Level::INFO; 
	else if (sLevel == "WARN") return log4tango::Level::WARN;
	else if (sLevel == "ERROR") return log4tango::Level::ERROR;
	else if (sLevel == "FATAL") return log4tango::Level::FATAL;
	else if (sLevel == "OFF") return log4tango::Level::OFF;
	else return log4tango::Level::OFF;
	
	return log4tango::Level::OFF;
		
}//close GetTangoLogLevelFromString()


std::string TangoUtils::GetStringMappedLogLevel(int level) 
{	
	//Returns string log level from Tango code			
	std::string sLevel= "OFF";
	if(level==Tango::LOG_OFF) sLevel= "OFF";
	else if(level==Tango::LOG_INFO) sLevel= "INFO";
	else if(level==Tango::LOG_WARN) sLevel= "WARN";
	else if(level==Tango::LOG_ERROR) sLevel= "ERROR";
	else if(level==Tango::LOG_FATAL) sLevel= "FATAL";
	else if(level==Tango::LOG_DEBUG) sLevel= "DEBUG";
	else sLevel= "OFF";
			
	return sLevel;
		
}//close GetStringMappedLogLevel()


bool TangoUtils::HasAttribute(Tango::DeviceImpl* dev,std::string attr_name){
	
	//Check device 
	if(!dev){
		return false;
	}

	//Check attribute
	bool hasAttr= false;
	try{
		dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		hasAttr= true;
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		hasAttr= false;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		hasAttr= false;
	}

	return hasAttr;

}//close HasAttribute()


bool TangoUtils::HasCommand(Tango::DeviceImpl* dev,std::string cmd_name){
	
	//Check device 
	if(!dev){
		return false;
	}

	//Check attribute
	bool hasCmd= false;
	try{
		dev->get_device_class()->get_cmd_by_name(cmd_name);
		hasCmd= true;
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the cmd is not existing
		hasCmd= false;
	}
	catch(...){//an exception is thrown if the cmd is not existing
		hasCmd= false;
	}

	return hasCmd;

}//close HasCommand()


bool TangoUtils::HasMemorizedValue(Tango::DeviceImpl* dev,std::string attr_name){

	return HasAttrProp(dev,attr_name,std::string("__value"));

}//close HasMemorizedValue()


bool TangoUtils::HasDevProp(Tango::DeviceImpl* dev,std::string prop_name)
{
	//Check device 
	if(!dev){
		return -1;
	}

	//Get property from DB
	bool hasProperty= false;
	try{
		Tango::DbData db_data;
		dev->get_db_device()->get_property(db_data);
	}
	catch(Tango::DevFailed& e){
		__DEBUG_LOG(dev,"Property "<<prop_name<<" not found in device "<<dev->get_name()<<" (hint: property not defined or not registered in DB), returning false.");
		hasProperty= false;
	}

	return hasProperty;

}//close HasDevProp()

bool TangoUtils::HasAttrProp(Tango::DeviceImpl* dev,std::string attr_name,std::string prop_name){

	//Check device 
	if(!dev){
		return -1;
	}

	//Get attr property from DB
	bool hasProperty= false;
	try {
		Tango::DbData db_data;
		db_data.push_back(Tango::DbDatum(attr_name.c_str()));
		dev->get_db_device()->get_attribute_property(db_data);
	
		long nb_prop;
		db_data[0] >> nb_prop;

		for (int k=0;k<nb_prop;k++) {
    	std::string& this_prop_name = db_data[k+1].name;
			if (this_prop_name == prop_name) {					
				hasProperty= true;
				break;
			}	
		}//end loop attr properties 

	}//close try 
	catch(Tango::DevFailed& e){
		__DEBUG_LOG(dev,"Property "<<prop_name<<" not found in attribute "<<attr_name<<" (hint: either attribute does not exist on device or property is not found), returning false.");
		hasProperty= false;
	}

	return hasProperty;

}//close HasAttrProperty()

int TangoUtils::GetAttrProps(Tango::DeviceImpl* dev,Tango::DbData& prop_db_data,std::string& attr_name,std::vector<string> prop_names){

	//Check device 
	if(!dev){
		return -1;
	}

	//Get attr properties from DB
	try {
		Tango::DbData db_data;
		db_data.push_back(Tango::DbDatum(attr_name.c_str()));
		dev->get_db_device()->get_attribute_property(db_data);
	
		long nb_prop;
		db_data[0] >> nb_prop;

		for(unsigned int i=0;i<prop_names.size();i++){		
			std::string desired_prop_name= prop_names[i];
			bool hasProperty= false;
			for (int k=0;k<nb_prop;k++) {
    		std::string& prop_name = db_data[k+1].name;
				if (prop_name == desired_prop_name) {					
					prop_db_data.push_back(db_data[k+1]);
					hasProperty= true;
					break;
				}	
			}//end loop attr properties 

			if(!hasProperty){
				std::stringstream ss;
				ss<<"Property "<<desired_prop_name<<" not found!";
				__ERROR_LOG(dev,ss.str());
				Tango::Except::throw_exception( 
		 			(const char *)"GET_ATTR_PROPERTIES_FAILED",
	       	ss.str().c_str(),
	       	(const char *)"LMCDevice::GetAttrProps"
	 			);
			}
		}//end loop desired properties
		
	}//close try 
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		__ERROR_LOG(dev,"Failed to get desired properties in attribute "<<attr_name<<"!");
		return -1;
	}

	return 0;

}//close GetAttrProps()


int TangoUtils::GetEventTypeFromString(Tango::EventType& evt_type,std::string evt_type_str){
	
	if(evt_type_str=="") return -1;

	if(evt_type_str=="change") evt_type= Tango::CHANGE_EVENT;
	else if(evt_type_str=="quality") evt_type= Tango::QUALITY_EVENT;
	else if(evt_type_str=="periodic") evt_type= Tango::PERIODIC_EVENT;
	else if(evt_type_str=="archive") evt_type= Tango::ARCHIVE_EVENT;
	else if(evt_type_str=="user_event") evt_type= Tango::USER_EVENT;
	else if(evt_type_str=="attr_conf") evt_type= Tango::ATTR_CONF_EVENT;
	else if(evt_type_str=="data_ready") evt_type= Tango::DATA_READY_EVENT;
	else if(evt_type_str=="intr_change") evt_type= Tango::INTERFACE_CHANGE_EVENT;
	else if(evt_type_str=="pipe") evt_type= Tango::PIPE_EVENT;
	else{
		return -1;
	}

	return 0;

}//close GetEventTypeFromString()

int TangoUtils::RegisterProxy(Tango::DeviceImpl* dev,std::string proxy_name){
	/*
	//Check device 
	if(!dev){
		return -1;
	}

	//Register proxy in manager
	//if(Utils_ns::TangoProxyManager::Instance().RegisterProxy(dev,proxy_name)<0){
	if(Utils_ns::TangoProxyManager::RegisterProxy(dev,proxy_name)<0){
		__WARN_LOG(dev,"Failed to register proxy "<<proxy_name<<" in device (NB: all event subscriptions registration will fail if you attempt to add them later)!");
		return -1;
	}
	*/
	return 0;

}//close RegisterProxy()

int TangoUtils::RegisterEvtSubscription(Tango::DeviceImpl* dev,std::string proxy_name,std::string attr_name,std::vector<Tango::EventType> evt_types){

	/*
	//Check device 
	if(!dev){
		return -1;
	}
	if(evt_types.empty()) return -1;

	//Convert Tango enum EventType in string
	std::vector<std::string> evt_types_str;
	for(unsigned int i=0;i<evt_types.size();i++){
		std::string ev_name = Tango::EventName[evt_types[i]];
		evt_types_str.push_back(ev_name);
	}

	//Register subscription in manager
	//if(Utils_ns::TangoProxyManager::Instance().RegisterEvtSubscriptions(dev,proxy_name,attr_name,evt_types)<0){
	if(Utils_ns::TangoProxyManager::RegisterEvtSubscriptions(dev,proxy_name,attr_name,evt_types)<0){
		__WARN_LOG(dev,"Failed to register subscription to event (attr="<<attr_name<<") in proxy "<<proxy_name<<", no subscriptions will be added!");
		return -1;
	}
	
	//if(Utils_ns::EvtSubscriptionManager::Instance().AddEvtSubscription(proxy_name,attr_name,evt_types)<0){
	//	__WARN_LOG(dev,"Failed to register rule for attr "<<attr_name<<" in device, no rule will be added!");
	//	return -1;
	//}
	*/
	return 0;

}//close RegisterEvtSubscription()


int TangoUtils::RegisterEvtHandlers(Tango::DeviceImpl* dev,std::string proxy_name,std::string attr_name,Tango::EventType evt_type,std::vector<std::string> evt_handlers){

	/*
	//Check device 
	if(!dev){
		return -1;
	}
	if(evt_handlers.empty()){
		return -1;
	}
	
	//Register handlers in manager
	//if(Utils_ns::TangoProxyManager::Instance().RegisterEvtHandlers(dev,proxy_name,attr_name,evt_type,evt_handlers)<0){
	if(Utils_ns::TangoProxyManager::RegisterEvtHandlers(dev,proxy_name,attr_name,evt_type,evt_handlers)<0){
		__WARN_LOG(dev,"Failed to register handlers for event (attr="<<attr_name<<", type="<<evt_type<<") in proxy "<<proxy_name<<", no handlers will be added!");
		return -1;
	}
	
	//if(Utils_ns::EvtSubscriptionManager::Instance().AddEvtHandlers(proxy_name,attr_name,evt_type,evt_handlers)<0){
	//	__WARN_LOG(dev,"Failed to register handlers for event attr "<<attr_name<<" in device, no handlers will be added!");
	//	return -1;
	//}
	*/
	return 0;

}//close RegisterEvtHandlers()


int TangoUtils::AddAttrWriteAction(Tango::DeviceImpl* dev,std::string attr_name,std::vector<std::string> cmd_names,bool preaction,bool check)
{		
	//Check device 
	if(!dev){
		return -1;
	}

	//Find if attribute exists in device
	if(check){

		//Check if attribute exists in device and if it is a writable attr
		bool hasWriteAssociation= false;
		try{
			Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
			hasWriteAssociation= attr.is_writ_associated();
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if attribute is writable
		if(!hasWriteAssociation){
			__WARN_LOG(dev,"Attr "<<attr_name<<" is not a writable attribute, no WRITE rule will be added!");
			return -1;
		}
	}//close if check

	//Register action in factory
	if(preaction){
		if(Utils_ns::RuleFactory::Instance().AddAttrWritePreAction(attr_name,cmd_names)<0){
			__WARN_LOG(dev,"Failed to register WRITE pre-action for attr "<<attr_name<<" in device, no action will be added!");
			return -1;
		}
	}
	else{
		if(Utils_ns::RuleFactory::Instance().AddAttrWritePostAction(attr_name,cmd_names)<0){
			__WARN_LOG(dev,"Failed to register WRITE post-action for attr "<<attr_name<<" in device, no action will be added!");
			return -1;
		}
	}

	return 0;

}//close AddAttrWriteAction()

}//close namespace


