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
* @file TangoUtils.h
* @class TangoUtils
* @brief Utility functions for Tango
*
* Utility functions for Tango
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _TANGO_UTILS_h
#define _TANGO_UTILS_h 1

#ifndef _CODE_UTILS_h
	#include <CodeUtils.h>
#endif

#ifndef _DEVICE_DATA_h
	#include <DeviceData.h>
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
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

//Exprtk lib
#ifndef INCLUDE_EXPRTK_HPP
	#include <exprtk.hpp>
#endif

/*
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
*/

//using namespace std;

namespace Utils_ns {


// ============================================================================
// class: RuleFactory
// ============================================================================
class RuleFactory {

	public:
		static RuleFactory& Instance() {
    	// Since it's a static variable, if the class has already been created,
      // It won't be created again.
      // And it is thread-safe in C++11.
      static RuleFactory myInstance;
 
			if(!m_cmdRulesPtr) m_cmdRulesPtr= std::make_shared<StringBiMapSet>();
			if(!m_attrRulesPtr) m_attrRulesPtr= std::make_shared<StringBiMapSet>();
			if(!m_attrReadRulesPtr) m_attrReadRulesPtr= std::make_shared<StringBiMapSet>();
			if(!m_attrWriteRulesPtr) m_attrWriteRulesPtr= std::make_shared<StringBiMapSet>();
			if(!m_attrWritePreActionPtr) m_attrWritePreActionPtr= std::make_shared<StringMapSet>();
			if(!m_attrWritePostActionPtr) m_attrWritePostActionPtr= std::make_shared<StringMapSet>();

      // Return a reference to our instance.
      return myInstance;
    }
 
    // delete copy and move constructors and assign operators
    RuleFactory(RuleFactory const&) = delete;             // Copy construct
    RuleFactory(RuleFactory&&) = delete;                  // Move construct
    RuleFactory& operator=(RuleFactory const&) = delete;  // Copy assign
    RuleFactory& operator=(RuleFactory &&) = delete;      // Move assign

	public:
		/** 
		\brief Dump cmd rules
 		*/
		static void DumpCmdRules(){
			if(!m_cmdRulesPtr) return;
			return m_cmdRulesPtr->Dump();
		}
		
		/** 
		\brief Dump attr rules
 		*/
		static void DumpAttrRules(){
			if(!m_attrRulesPtr) return;
			return m_attrRulesPtr->Dump();
		}

		/** 
		\brief Dump attr read rules
 		*/
		static void DumpAttrReadRules(){
			if(!m_attrReadRulesPtr) return;
			return m_attrReadRulesPtr->Dump();
		}

		/** 
		\brief Dump attr write rules
 		*/
		static void DumpAttrWriteRules(){
			if(!m_attrWriteRulesPtr) return;
			return m_attrWriteRulesPtr->Dump();
		}

		/** 
		\brief Dump attr write pre-actions
 		*/
		static void DumpAttrWritePreActions(){
			if(!m_attrWritePreActionPtr) return;
			return m_attrWritePreActionPtr->Dump();
		}

		/** 
		\brief Dump attr write post-actions
 		*/
		static void DumpAttrWritePostActions(){
			if(!m_attrWritePostActionPtr) return;
			return m_attrWritePostActionPtr->Dump();
		}

		/** 
		\brief Get attr names in cmd rules
 		*/
		static int GetAttrsInCmdRules(std::vector<std::string>& attr_names,std::string cmd_name){
			if(!m_cmdRulesPtr) return -1;
			return m_cmdRulesPtr->Get2ndKeyList<std::string>(attr_names,cmd_name);
		}

		/** 
		\brief Get attr names in attr read rules
 		*/
		static int GetSMAttrsInAttrReadRules(std::vector<std::string>& sm_attr_names,std::string attr_name){
			if(!m_attrReadRulesPtr) return -1;
			return m_attrReadRulesPtr->Get2ndKeyList<std::string>(sm_attr_names,attr_name);
		}

		/** 
		\brief Get attr names in attr write rules
 		*/
		static int GetSMAttrsInAttrWriteRules(std::vector<std::string>& sm_attr_names,std::string attr_name){
			if(!m_attrWriteRulesPtr) return -1;
			return m_attrWriteRulesPtr->Get2ndKeyList<std::string>(sm_attr_names,attr_name);
		}

		/** 
		\brief Get cmd actions names in attr write pre-actions
 		*/
		static int GetAttrWritePreActions(std::vector<std::string>& cmd_names,std::string attr_name){
			if(!m_attrWritePreActionPtr) return -1;
			return m_attrWritePreActionPtr->GetSetValues<std::string>(cmd_names,attr_name);
		}

		/** 
		\brief Get cmd actions names in attr write post-actions
 		*/
		static int GetAttrWritePostActions(std::vector<std::string>& cmd_names,std::string attr_name){
			if(!m_attrWritePostActionPtr) return -1;
			return m_attrWritePostActionPtr->GetSetValues<std::string>(cmd_names,attr_name);
		}

		/** 
		\brief Create a cmd rule
 		*/
		template<typename T>
	 	static int AddCmdRule(std::string cmd_name,std::string attr_name,std::vector<T> attr_values) {
			if(!m_cmdRulesPtr) return -1;	
			m_cmdRulesPtr->Add<std::string,T>(cmd_name,attr_name,attr_values);
			return 0;
		}

		/** 
		\brief Create a cmd rule from initializer set
 		*/
		template<typename T>
		static int AddCmdRule(std::string cmd_name,std::string attr_name,std::initializer_list<T> attr_values){
			if(!m_cmdRulesPtr) return -1;	
			m_cmdRulesPtr->Add<std::string,T>(cmd_name,attr_name,attr_values);
			return 0;
		}

		/** 
		\brief Create a attr read rule
 		*/
		template<typename T>
	 	static int AddAttrReadRule(std::string attr_name,std::string sm_attr_name,std::vector<T> sm_attr_values) {
			if(!m_attrReadRulesPtr) return -1;	
			m_attrReadRulesPtr->Add<std::string,T>(attr_name,sm_attr_name,sm_attr_values);
			return 0;
		}

		/** 
		\brief Create a attr read rule from initializer set
 		*/
		template<typename T>
		static int AddAttrReadRule(std::string attr_name,std::string sm_attr_name,std::initializer_list<T> sm_attr_values){
			if(!m_attrReadRulesPtr) return -1;	
			m_attrReadRulesPtr->Add<std::string,T>(attr_name,sm_attr_name,sm_attr_values);
			return 0;
		}


		/** 
		\brief Create a attr write rule
 		*/
		template<typename T>
	 	static int AddAttrWriteRule(std::string attr_name,std::string sm_attr_name,std::vector<T> sm_attr_values) {
			if(!m_attrWriteRulesPtr) return -1;	
			m_attrWriteRulesPtr->Add<std::string,T>(attr_name,sm_attr_name,sm_attr_values);
			return 0;
		}

		/** 
		\brief Create a attr write rule from initializer set
 		*/
		template<typename T>
		static int AddAttrWriteRule(std::string attr_name,std::string sm_attr_name,std::initializer_list<T> sm_attr_values){
			if(!m_attrWriteRulesPtr) return -1;	
			m_attrWriteRulesPtr->Add<std::string,T>(attr_name,sm_attr_name,sm_attr_values);
			return 0;
		}

		/** 
		\brief Create an attr rule (REPLACE NAME WITH AddTransitionRule()!!!)
 		*/
		template<typename T>
		static int AddAttrRule(std::string attr_name,T attr_key,std::vector<T> attr_values) {
			if(!m_attrRulesPtr) return -1;	
			m_attrRulesPtr->Add<T>(attr_name,attr_key,attr_values);
			return 0;
		}

		/** 
		\brief Create an attr rule from initializer set (REPLACE NAME WITH AddTransitionRule()!!!)
 		*/
		template<typename T>
		static int AddAttrRule(std::string attr_name,T attr_key,std::initializer_list<T> attr_values){
			if(!m_attrRulesPtr) return -1;	
			m_attrRulesPtr->Add<T>(attr_name,attr_key,attr_values);
			return 0;
		}

		/** 
		\brief Create a attr write pre-action from vector
 		*/
		static int AddAttrWritePreAction(std::string attr_name,std::vector<std::string> cmd_names){
			if(!m_attrWritePreActionPtr) return -1;	
			m_attrWritePreActionPtr->Add<std::string>(attr_name,cmd_names);
			return 0;
		}

		/** 
		\brief Create a attr write pre-action from initializer set
 		*/
		static int AddAttrWritePreAction(std::string attr_name,std::initializer_list<std::string> cmd_names){
			if(!m_attrWritePreActionPtr) return -1;	
			m_attrWritePreActionPtr->Add<std::string>(attr_name,cmd_names);
			return 0;
		}

		/** 
		\brief Create a attr write post-action from vector
 		*/
		static int AddAttrWritePostAction(std::string attr_name,std::vector<std::string> cmd_names){
			if(!m_attrWritePostActionPtr) return -1;	
			m_attrWritePostActionPtr->Add<std::string>(attr_name,cmd_names);
			return 0;
		}

		/** 
		\brief Create a attr write post-action from initializer set
 		*/
		static int AddAttrWritePostAction(std::string attr_name,std::initializer_list<std::string> cmd_names){
			if(!m_attrWritePostActionPtr) return -1;	
			m_attrWritePostActionPtr->Add<std::string>(attr_name,cmd_names);
			return 0;
		}
		
		/** 
		\brief Is allowed cmd given the attr value
 		*/
		template<typename T>
		static bool IsCmdAllowed(std::string cmd_name,std::string attr_name,T attr_value){
			if(!m_cmdRulesPtr) {
				return true;//no rules created, hence all cmd allowed
			}
			bool hasAllowedValue= m_cmdRulesPtr->HasAllowedValueInSet<std::string,T>(cmd_name,attr_name,attr_value);
			return hasAllowedValue;
		}

		/** 
		\brief Is attr read allowed given a sm attr value
 		*/
		template<typename T>
		static bool IsAttrReadAllowed(std::string attr_name,std::string sm_attr_name,T sm_attr_value){
			if(!m_attrReadRulesPtr) {
				return true;//no rules created, hence read attr allowed
			}
			bool hasAllowedValue= m_attrReadRulesPtr->HasAllowedValueInSet<std::string,T>(attr_name,sm_attr_name,sm_attr_value);
			return hasAllowedValue;
		}

		/** 
		\brief Is attr write allowed given a sm attr value
 		*/
		template<typename T>
		static bool IsAttrWriteAllowed(std::string attr_name,std::string sm_attr_name,T sm_attr_value){
			if(!m_attrWriteRulesPtr) {
				return true;//no rules created, hence read attr allowed
			}
			bool hasAllowedValue= m_attrWriteRulesPtr->HasAllowedValueInSet<std::string,T>(attr_name,sm_attr_name,sm_attr_value);
			return hasAllowedValue;
		}
		
		/** 
		\brief Is allowed attr given the attr value (REPLACE NAME WITH IsAttrTransitionAllowed()!!!)
 		*/
		template<typename T>
		static bool IsAttrAllowed(std::string attr_name,T attr_key,T attr_value){
			if(!m_attrRulesPtr) return true;//no rules created, hence all cmd allowed
			bool hasAllowedValue= m_attrRulesPtr->HasAllowedValueInSet<T,T>(attr_name,attr_key,attr_value);
			return hasAllowedValue;
		}

	protected:
		RuleFactory(){};
		virtual ~RuleFactory(){
			//Deleting map with options
			//...
		};

	private:
		static StringBiMapSetPtr m_cmdRulesPtr;		
		static StringBiMapSetPtr m_attrRulesPtr;
		static StringBiMapSetPtr m_attrReadRulesPtr;
		static StringBiMapSetPtr m_attrWriteRulesPtr;
		static StringMapSetPtr m_attrWritePreActionPtr;
		static StringMapSetPtr m_attrWritePostActionPtr;
};


// ============================================================================
// class: CorbaAnyDataConverter
// ============================================================================
template<typename T,typename K>
struct CorbaAnyDataConverter {

  static void convert_scalar_to_json(Json::Value& root,CORBA::Any& any){	
		try{
			T value;	
			if ((any>>=value) == false) {
				std::stringstream ss;
				ss<<"Failed to extract Corba::Any to desired type!";
				Tango::Except::throw_exception( 
					(const char *)"EXTRACT_CORBA_ANY_TO_JSON_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"TangoUtils::ExtractCorbaAnyToJson"
				);
			}
			root["value"]= static_cast<K>(value);
		}
		catch(...){
			throw;
		}
	}//close convert_scalar_to_json()	

	static void convert_spectrum_to_json(Json::Value& root,CORBA::Any& any){	
		try{
			const T* values;	
			if ((any>>=values)== false) {
				std::stringstream ss;
				ss<<"Failed to extract Corba::Any to desired type!";
				Tango::Except::throw_exception( 
					(const char *)"EXTRACT_CORBA_ANY_TO_JSON_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"TangoUtils::ExtractCorbaAnyToJson"
				);
			}
			for(unsigned int i=0;i<values->length();i++){
				K item= static_cast<K>( (*values)[i] );
				root["value"].append(Json::Value(item));
			}
		}
		catch(...){
			throw;
		}
	}//close convert_spectrum_to_json()	

	
};//close CorbaAnyDataConverter struct




template<>
struct CorbaAnyDataConverter<Tango::DevBoolean,Tango::DevBoolean> {

	static void convert_scalar_to_json(Json::Value& root,CORBA::Any& any){	
		try{
			Tango::DevBoolean value;	
			if ((any>>=CORBA::Any::to_boolean(value))== false) {
				std::stringstream ss;
				ss<<"Failed to extract Corba::Any to desired type!";
				Tango::Except::throw_exception( 
					(const char *)"EXTRACT_CORBA_ANY_TO_JSON_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"TangoUtils::ExtractCorbaAnyToJson"
				);
			}
			root["value"]= value;
		}
		catch(...){
			throw;
		}
	}//close convert_scalar_to_json()	

};//close CorbaAnyDataConverter struct specialization for bool

template<>
struct CorbaAnyDataConverter<Tango::DevString,Tango::DevString> {

	static void convert_scalar_to_json(Json::Value& root,CORBA::Any& any){	
		try{
			Tango::DevString value;	
			if ((any>>=const_cast<const char *&>(value))== false) {
				std::stringstream ss;
				ss<<"Failed to extract Corba::Any to desired type!";
				Tango::Except::throw_exception( 
					(const char *)"EXTRACT_CORBA_ANY_TO_JSON_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"TangoUtils::ExtractCorbaAnyToJson"
				);
			}
			root["value"]= value;
		}
		catch(...){
			throw;
		}
	}//close convert_scalar_to_json()	

};//close CorbaAnyDataConverter struct specialization for DevString


template<>
struct CorbaAnyDataConverter<Tango::DevVarStringArray,Tango::DevString> {

	static void convert_spectrum_to_json(Json::Value& root,CORBA::Any& any){	
		try{
			const Tango::DevVarStringArray* values;	
			if ((any>>=values)== false) {
				std::stringstream ss;
				ss<<"Failed to extract Corba::Any to desired type!";
				Tango::Except::throw_exception( 
					(const char *)"EXTRACT_CORBA_ANY_TO_JSON_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"TangoUtils::ExtractCorbaAnyToJson"
				);
			}
			for(unsigned int i=0;i<values->length();i++){
				root["value"].append(Json::Value( ((std::string)(*values)[i]).c_str() ));
			}
		}
		catch(...){
			throw;
		}
	}//close convert_spectrum_to_json()	
	
};



// ============================================================================
// struct: CaseInsensitiveStringComp
// ============================================================================
struct CaseInsensitiveStringComp { 
	bool operator() (const std::string& lhs, const std::string& rhs) const {
  	return stricmp(lhs.c_str(), rhs.c_str()) < 0;
  }
};

// ============================================================================
// class: TangoUtils
// ============================================================================
typedef exprtk::symbol_table<double> symbol_table_t;

class TangoUtils {

  public:
		
		/** 
		\brief Class constructor: initialize structures.
 		*/
    TangoUtils();
		/**
		* \brief Class destructor: free allocated memory
		*/
   	virtual ~TangoUtils();

		
	public:

		/**
		* \brief Convert Tango State to string
		*/
		static std::string TangoStateToString(Tango::DevState state){
			unsigned int index= static_cast<unsigned int>(state);
			return std::string(Tango::DevStateName[index]);
		}

		/**
		* \brief Convert string to Tango State 
		*/
		static int StringToTangoState(Tango::DevState& state,std::string state_str){
			std::map<std::string,Tango::DevState>::iterator it= m_tangoStateStrMap.find(state_str);
			if(m_tangoStateStrMap.empty() || it==m_tangoStateStrMap.end()) return -1;
			state= it->second;
			return 0;
		}

		/**
		* \brief Convert Tango timestamp to timeval
		*/
		static void TangoTimestampToTimeval(timeval &t,Tango::TimeVal& timestamp){
			t.tv_sec= timestamp.tv_sec;
			t.tv_usec= timestamp.tv_usec + timestamp.tv_nsec/1000;
		}
		
		/**
		* \brief Convert Tango timestamp to timeval
		*/
		static void TimevalToTangoTimestamp(Tango::TimeVal& timestamp,timeval &t){
			timestamp.tv_sec= t.tv_sec;
			timestamp.tv_usec= t.tv_usec;
			timestamp.tv_nsec= 0;
		}

		/**
		* \brief Returns difference between two Tango timestamp in microseconds
		*/
		static long GetTimeDiffUSec(Tango::TimeVal before, Tango::TimeVal after) {
			return ((after.tv_sec-before.tv_sec)*1000000 + after.tv_usec - before.tv_usec);
		}	
	
		/**
		* \brief Returns difference between two Tango timestamp in seconds
		*/
		static long GetTimeDiffSec(Tango::TimeVal before, Tango::TimeVal after) {
			return GetTimeDiffUSec(before,after)/1000000;
		}

		/**
		* \brief Returns difference between two Tango timestamp in milliseconds
		*/
		static long GetTimeDiffMSec(Tango::TimeVal before, Tango::TimeVal after) {
			return GetTimeDiffUSec(before,after)/1000;
		}

		/**
		* \brief Returns current timestamp in Tango format
		*/
		static Tango::TimeVal GetTimestampNow(){
			struct timezone tz;
			struct timeval tv;
			gettimeofday(&tv,&tz);

			Tango::TimeVal timestamp;
			timestamp.tv_sec = (CORBA::Long)tv.tv_sec;
			timestamp.tv_usec = (CORBA::Long)tv.tv_usec;
			timestamp.tv_nsec = 0;
			
			return timestamp;
		}//close GetTimestampNow()

		/**
		* \brief Returns current timestamp 
		*/
		static timeval GetTimeNow(){
			struct timezone tz;
			struct timeval tv;
			gettimeofday(&tv,&tz);
			return tv;
		}

		/**
		* \brief Returns difference between two yat::Time in microsec
		*/
		static double GetYatTimeDiffUSec(yat::Time before, yat::Time after) {
			//return (after.double_unix()-before.double_unix());
			return (after.double_unix()-before.double_unix())*1000000;
		}

		/**
		* \brief Returns difference between two yat::Time in milliseconds
		*/
		static double GetYatTimeDiffMSec(yat::Time before, yat::Time after) {
			return GetYatTimeDiffUSec(before,after)/1000;
		}

		/**
		* \brief Returns difference between two yat::Time in seconds
		*/
		static double GetYatTimeDiffSec(yat::Time before, yat::Time after) {
			return GetYatTimeDiffUSec(before,after)/1000000;
		}

		/**
		* \brief Returns timeout wrt to now in milliseconds
		*/
		static double ComputeYatTimeoutMSec(yat::Time t,bool isUTC=false) {
			yat::Time now;
			now.set_current(isUTC);
			return GetYatTimeDiffMSec(now,t);
		}

		/**
		* \brief Returns timeout wrt to now in microseconds
		*/
		static double ComputeYatTimeoutUSec(yat::Time t,bool isUTC=false) {
			yat::Time now;
			now.set_current(isUTC);
			return GetYatTimeDiffUSec(now,t);
		}

		/**
		* \brief Returns timeout wrt to now in seconds
		*/
		static double ComputeYatTimeoutSec(yat::Time t,bool isUTC=false) {
			yat::Time now;
			now.set_current(isUTC);
			return GetYatTimeDiffSec(now,t);
		}

		/**
		* \brief Sum timestamp
		*/
		/*
		static timespec SumTimeStamp (timespec time1, timespec time2) {
			timespec  result ;
			result.tv_sec = time1.tv_sec + time2.tv_sec ;
    	result.tv_nsec = time1.tv_nsec + time2.tv_nsec ;
    	if (result.tv_nsec >= 1000000000L) {
      	result.tv_sec++ ;  result.tv_nsec = result.tv_nsec - 1000000000L ;
    	}
    	return (result) ;
		}
		*/


		/**
		* \brief Get control system facility name
		*/
		static std::string GetTangoDBFacility(){
			return Utils_ns::SysUtils::GetEnvVar("TANGO_HOST");
		}
		
		/**
		* \brief Get FQDN device name from alias
		*/
		static int GetFQDNDeviceNameFromAlias(std::string& fqdn_device_name,std::string alias);

		/**
		* \brief Parse a full attr name and return plain device & attr name
		*/
		static int ParseFullAttrName(std::string& device_name,std::string& attr_name,std::string full_attr_name);
		/**
		* \brief Parse a full attr name and return plain domain & device & attr name
		*/
		static int ParseFullAttrName(std::string& domain_name,std::string& device_name,std::string& attr_name,bool& hasQualityTag, std::string full_attr_name);

		/**
		* \brief Parse a full proxy name and return plain facility/domain/family/member name
		*/
		static int ParseFullProxyName(std::string& facility_name,std::string& domain_name,std::string& family_name,std::string& member_name,std::string full_proxy_name);
		
		/**
		* \brief Parse a full proxy name and return plain proxy name
		*/
		static int GetPlainProxyName(std::string& proxy_name,std::string full_proxy_name);

		/**
		* \brief Get admin device name from proxy name
		*/
		static int GetAdminProxyName(std::string& proxy_adm_name,std::string proxy_name);

		/**
		* \brief Parse a DeviceAttribute data and convert to CORBA any
		*/
		static int DeviceAttributeToCorbaAny(CORBA::Any& any,Tango::DeviceAttribute* da,Tango::DeviceImpl* dev);

		/**
		* \brief Convert CORBA any to json object
		*/
		static int CorbaAnyToJsonObj(Json::Value& root,CORBA::Any& any,int data_type,int data_format);

		/**
		* \brief Convert string to DevState
		*/	
		static int TangoStateStrToDevState(Tango::DevState& state,std::string state_str);

		/**
		* \brief Convert DevState string to desired type
		*/	
		template <typename T>
		static int TangoStateStrToValue(T& state_value,std::string state_str);

		/**
		* \brief Convert DevState string list to desired type 
		*/	
		template <typename T>
		static int TangoStateStrToValue(std::vector<T>& state_value,std::vector<std::string>& state_str);

		/**
		* \brief Extract list of device names present in server (NB: server is given in full format, e.g. <Server_Name>/<Instance>)
		*/
		static int GetDevicesInServer(std::vector<std::string>& device_names, std::string server_full_name,bool prepend_tango_host= true);

		/**
		* \brief Check if given url can be a remote proxy attr name (checking for '/' occurrences)
		*/
		static bool IsProxyAttr(std::string url);

		/**
		* \brief Check if given url can be a remote FQDN proxy attr name (checking for '/' occurrences)
		*/
		static bool IsFQDNProxyAttr(std::string url);

		/**
		* \brief Returns log appender type in TANGO
		*/
		static int GetLogAppenderType(log4tango::Appender* appender);
		/**
		* \brief Set log level of appender in logger
		*/
		static int SetAppenderLogLevel(Tango::DeviceImpl* dev,std::string appender_name,Tango::DevLong log_level);

		/** 
			\brief Get stringified log level from Tango log level.
		*	\details Convert input Tango log level in Log4Cxx equivalent
		*	@param[in] level Tango log level
		*	@return String log level	
 		*/
		static log4tango::Level::Value GetTangoLogLevelFromString(std::string sLevel);

		/** 
			\brief Get stringified log level from Tango log level.
		*	\details Convert input Tango log level to string
		*	@param[in] level Tango log level
		*	@return String log level	
 		*/
		static std::string GetStringMappedLogLevel(int level);

		/**
		* \brief Put attribute property in DB
		*/
		template <typename T> 
		static int PutAttrProp(Tango::DeviceImpl* dev,std::string& attr_name,std::string prop_name,T& prop_value);
	
		/**
		* \brief Retrieve desired attr property from DB and return them as DbData
		*/
		static int GetAttrProps(Tango::DeviceImpl* dev,Tango::DbData& db_data,std::string& attr_name,std::vector<std::string> prop_names);


		/**
		* \brief Retrieve attr property value from DB
		*/
		template <typename T> 
		static int ExtractPropValue(Tango::DeviceImpl* dev,T& prop_value,std::string& prop_name);

		/**
		* \brief Has attribute a given property stored in DB?
		*/	
		static bool HasAttrProp(Tango::DeviceImpl* dev,std::string attr_name,std::string prop_name);

		/**
		* \brief Has device property stored in DB?
		*/	
		static bool HasDevProp(Tango::DeviceImpl* dev,std::string prop_name);

	
		/**
		* \brief Has attribute a memorized value stored in DB?
		*/	
		static bool HasMemorizedValue(Tango::DeviceImpl* dev,std::string attr_name);

		/**
		* \brief Extract sub-group from Group
		*/
		static Tango::Group* GetSubGroup(Tango::DeviceImpl* dev,Tango::Group* grp,std::vector<std::string>& device_names,bool requireDevOnline=false);

		/**
		* \brief Write an attribute to a Group
		*/
		template <typename T> 
			static int WriteAttrToGroup(Tango::DeviceImpl* dev,Tango::Group* grp,std::string attr_name,T& attr_value);
		
		#if TANGO_VERSION_MAJOR >= 9
		/**
		* \brief Get enum attr value from label
		*/
		static int GetEnumAttrValue(Tango::DevShort& enumValue,Tango::DeviceImpl* dev,std::string attr_name,std::string enumLabel);
		#endif

		#if TANGO_VERSION_MAJOR >= 9
		/**
		* \brief Get enum string label from enum attr value
		*/	
		static int GetEnumAttrLabelValue(std::string& enum_value_str,Tango::DeviceImpl* dev,std::string attr_name,Tango::DevShort enum_value);
		#endif

		#if TANGO_VERSION_MAJOR >= 9
		/**
		* \brief Get enum label-value maps
		*/	
		static int GetEnumAttrLabelValueMap(std::map<int,std::string>& attrValLabelMap,std::map<std::string,int>& attrLabelValMap,Tango::DeviceImpl* dev,std::string attr_name);
		#endif

		/**
		* \brief Extract command name from StateMachine is allowed command
		*/
		static int ExtractCmdName(std::string& cmd_name);
		
		/**
		* \brief Check if attribute with given name is present in device
		*/	
		static bool HasAttribute(Tango::DeviceImpl* dev,std::string attr_name);
	
		/**
		* \brief Check if command with given name is present in device
		*/
		static bool HasCommand(Tango::DeviceImpl* dev,std::string cmd_name);

		/**
		* \brief Get attr names in cmd rule
		*/
		static int GetAttrsInCmdRules(std::vector<std::string>& attr_names,std::string cmd_name){
			return Utils_ns::RuleFactory::Instance().GetAttrsInCmdRules(attr_names,cmd_name);
		}

		/**
		* \brief Get state machine attr names in attr read rule
		*/
		static int GetSMAttrsInAttrReadRules(std::vector<std::string>& sm_attr_names,std::string attr_name){
			return Utils_ns::RuleFactory::Instance().GetSMAttrsInAttrReadRules(sm_attr_names,attr_name);
		}

		/**
		* \brief Get state machine attr names in attr write rule
		*/
		static int GetSMAttrsInAttrWriteRules(std::vector<std::string>& sm_attr_names,std::string attr_name){
			return Utils_ns::RuleFactory::Instance().GetSMAttrsInAttrWriteRules(sm_attr_names,attr_name);
		}

		/**
		* \brief Get registered write pre-action cmd names for a device attr
		*/
		static int GetAttrWritePreActions(std::vector<std::string>& cmd_names,std::string attr_name){
			return Utils_ns::RuleFactory::Instance().GetAttrWritePreActions(cmd_names,attr_name);
		}

		/**
		* \brief Get registered write post-action cmd names for a device attr
		*/
		static int GetAttrWritePostActions(std::vector<std::string>& cmd_names,std::string attr_name){
			return Utils_ns::RuleFactory::Instance().GetAttrWritePostActions(cmd_names,attr_name);
		}

		/**
		* \brief Check if a command is allowed when an attribute has a given value
		*/
		template<typename T>
		static bool IsCmdAllowed(Tango::DeviceImpl* dev,std::string cmd_name,std::string attr_name,T attr_value,bool check=true);

		/**
		* \brief Check if an attribute READ is allowed when a state machine attribute has a given value
		*/
		template<typename T>
		static bool IsAttrReadAllowed(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,T sm_attr_value,bool check=true);

		/**
		* \brief Check if an attribute WRITE is allowed when a state machine attribute has a given value
		*/
		template<typename T>
		static bool IsAttrWriteAllowed(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,T sm_attr_value,bool check=true);

		/**
		* \brief Check if a command is allowed when an attribute has a given value
		*/
		template<typename T> 
		static bool IsAttrAllowed(Tango::DeviceImpl* dev,std::string attr_name,T attr_value_current,T attr_value_new,bool check=true);

		/**
		* \brief Add a cmd rule to given device
		*/
		template<typename T>
	 	static int AddCmdRule(Tango::DeviceImpl* dev,std::string cmd_name,std::string attr_name,std::vector<T> attr_values,bool check=true);
		
		/**
		* \brief Add a cmd rule to given device
		*/
		template<typename T>
	 	static int AddCmdRule(Tango::DeviceImpl* dev,std::string cmd_name,std::string attr_name,std::initializer_list<T> attr_set,bool check=true){
			std::vector<T> attr_values= attr_set;
			return AddCmdRule<T>(dev,cmd_name,attr_name,attr_values,check);
		}


		/**
		* \brief Add a attr READ rule to given device
		*/
		template<typename T>
	 	static int AddAttrReadRule(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,std::vector<T> sm_attr_values,bool check=true);
		
		/**
		* \brief Add a attr READ rule to given device
		*/
		template<typename T>
	 	static int AddAttrReadRule(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,std::initializer_list<T> sm_attr_set,bool check=true){
			std::vector<T> sm_attr_values= sm_attr_set;
			return AddAttrReadRule<T>(dev,attr_name,sm_attr_name,sm_attr_values,check);
		}

		/**
		* \brief Add a attr WRITE rule to given device
		*/
		template<typename T>
	 	static int AddAttrWriteRule(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,std::vector<T> sm_attr_values,bool check=true);
		
		/**
		* \brief Add a attr WRITE rule to given device
		*/
		template<typename T>
	 	static int AddAttrWriteRule(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,std::initializer_list<T> sm_attr_set,bool check=true){
			std::vector<T> sm_attr_values= sm_attr_set;
			return AddAttrWriteRule<T>(dev,attr_name,sm_attr_name,sm_attr_values,check);
		}

		/**
		* \brief Add a cmd rule to given device (REPLACE NAME WITH AddAttrTransitionRule()!!!)
		*/
		template<typename T>
	 	static int AddAttrRule(Tango::DeviceImpl* dev,std::string attr_name,T attr_value,std::vector<T> attr_values,bool check=true);
		
		/**
		* \brief Add a cmd rule to given device (REPLACE NAME WITH AddAttrTransitionRule()!!!)
		*/ 
		template<typename T>
	 	static int AddAttrRule(Tango::DeviceImpl* dev,std::string attr_name,T attr_value,std::initializer_list<T> attr_set,bool check=true){
			std::vector<T> attr_values= attr_set;
			return AddAttrRule<T>(dev,attr_name,attr_value,attr_values,check);
		}

		/**
		* \brief Add an attr write action
		*/
		static int AddAttrWriteAction(Tango::DeviceImpl* dev,std::string attr_name,std::vector<std::string> cmd_names,bool preaction,bool check=true);

		/**
		* \brief Add an attr write aaction
		*/
		static int AddAttrWriteAction(Tango::DeviceImpl* dev,std::string attr_name,std::initializer_list<std::string> cmd_names_set,bool preaction,bool check=true){
			std::vector<std::string> cmd_names= cmd_names_set;
			return AddAttrWriteAction(dev,attr_name,cmd_names,preaction,check);
		}


		/**
		* \brief Get event type from string
		*/
		static int GetEventTypeFromString(Tango::EventType& evt_type,std::string evt_type_str);


		/**
		* \brief Register a device proxy in current device server
		*/
		static int RegisterProxy(Tango::DeviceImpl* dev,std::string proxy_name);


		/**
		* \brief Register a proxy event subscription
		*/
		static int RegisterEvtSubscription(Tango::DeviceImpl* dev,std::string proxy_name,std::string attr_name,std::vector<Tango::EventType> evt_types);

		/**
		* \brief Register a proxy event subscription
		*/
		static int RegisterEvtSubscription(Tango::DeviceImpl* dev,std::string proxy_name,std::string attr_name,std::initializer_list<Tango::EventType> evt_set){
			std::vector<Tango::EventType> evt_types= evt_set;
			return RegisterEvtSubscription(dev,proxy_name,attr_name,evt_types);
		}


		/**
		* \brief Register event handlers
		*/
		static int RegisterEvtHandlers(Tango::DeviceImpl* dev,std::string proxy_name,std::string attr_name,Tango::EventType evt_type,std::vector<std::string> evt_handlers);

		/**
		* \brief Register event handlers
		*/
		static int RegisterEvtHandlers(Tango::DeviceImpl* dev,std::string proxy_name,std::string attr_name,Tango::EventType evt_type,std::initializer_list<std::string> evt_handlers_set){
			std::vector<std::string> evt_handlers= evt_handlers_set;
			return RegisterEvtHandlers(dev,proxy_name,attr_name,evt_type,evt_handlers);
		}


		/**
		* \brief Throw exception with string message
		*/
		static void ThrowTangoException(std::string name,std::string msg,std::string origin){
			Tango::Except::throw_exception(name.c_str(),msg.c_str(),origin.c_str());
		}

		/**
		* \brief Throw exception with 
		*/
		static void ThrowTangoExceptionStream(std::string name,std::stringstream msgStream,std::string origin){
			ThrowTangoException(name,msgStream.str(),origin);
		}

	public:
		static symbol_table_t tango_symbols;
		static std::map<std::string,Tango::DevState> m_tangoStateStrMap; 

	private:
		
};

#include "TangoUtils.tpp"

}//close namespace


//########################
//##  Custom macros
//########################
//Custom macro
#ifndef BRACED_INIT_LIST
	#define BRACED_INIT_LIST(...) {__VA_ARGS__}
#endif

#ifndef REGISTER_CMD_RULE
	#define REGISTER_CMD_RULE(type,cmd_name,attr_name,attr_values) \
		(Utils_ns::RuleFactory::Instance().AddCmdRule<type>(#cmd_name,#attr_name,BRACED_INIT_LIST attr_values))
#endif

#ifndef REGISTER_ATTR_READ_RULE
	#define REGISTER_ATTR_READ_RULE(type,attr_name,sm_attr_name,sm_attr_values) \
		(Utils_ns::RuleFactory::Instance().AddAttrReadRule<type>(#attr_name,#sm_attr_name,BRACED_INIT_LIST sm_attr_values))
#endif

#ifndef REGISTER_ATTR_WRITE_RULE
	#define REGISTER_ATTR_WRITE_RULE(type,attr_name,sm_attr_name,sm_attr_values) \
		(Utils_ns::RuleFactory::Instance().AddAttrWriteRule<type>(#attr_name,#sm_attr_name,BRACED_INIT_LIST sm_attr_values))
#endif

#ifndef REGISTER_ATTR_RULE
	#define REGISTER_ATTR_RULE(type,attr_name,attr_key,attr_values) \
		(Utils_ns::RuleFactory::Instance().AddAttrRule<type>(#attr_name,attr_key,BRACED_INIT_LIST attr_values))
#endif

#ifndef REGISTER_ATTR_WRITE_PRE_ACTION
	#define REGISTER_ATTR_WRITE_PRE_ACTION(attr_name,cmd_names) \
		(Utils_ns::RuleFactory::Instance().AddAttrWriteAction(#attr_name,BRACED_INIT_LIST cmd_names,true))
#endif

#ifndef REGISTER_ATTR_WRITE_POST_ACTION
	#define REGISTER_ATTR_WRITE_POST_ACTION(attr_name,cmd_names) \
		(Utils_ns::RuleFactory::Instance().AddAttrWriteAction(#attr_name,BRACED_INIT_LIST cmd_names,false))
#endif


#ifndef DUMP_CMD_RULES
	#define DUMP_CMD_RULES() (Utils_ns::RuleFactory::Instance().DumpCmdRules())
#endif

#ifndef DUMP_ATTR_READ_RULES
	#define DUMP_ATTR_READ_RULES() (Utils_ns::RuleFactory::Instance().DumpAttrReadRules())
#endif

#ifndef DUMP_ATTR_WRITE_RULES
	#define DUMP_ATTR_WRITE_RULES() (Utils_ns::RuleFactory::Instance().DumpAttrWriteRules())
#endif

#ifndef DUMP_ATTR_RULES
	#define DUMP_ATTR_RULES() (Utils_ns::RuleFactory::Instance().DumpAttrRules())
#endif

#ifndef DUMP_ATTR_WRITE_PRE_ACTIONS
	#define DUMP_ATTR_WRITE_PRE_ACTIONS() (Utils_ns::RuleFactory::Instance().DumpAttrWritePreActions())
#endif

//########################
//##  Device macros
//########################
//## CMD RULES
#ifndef __REGISTER_CMD_RULE
	#define __REGISTER_CMD_RULE(deviceInstance,type,cmd_name,attr_name,attr_values) \
		(Utils_ns::TangoUtils::AddCmdRule<type>(deviceInstance,cmd_name,attr_name,BRACED_INIT_LIST attr_values))
#endif

#ifndef _REGISTER_CMD_RULE
	#define _REGISTER_CMD_RULE(type,cmd_name,attr_name,attr_values) \
		(Utils_ns::TangoUtils::AddCmdRule<type>(this,cmd_name,attr_name,BRACED_INIT_LIST attr_values))
#endif

//## ATTR READ RULES
#ifndef __REGISTER_ATTR_READ_RULE
	#define __REGISTER_ATTR_READ_RULE(deviceInstance,type,attr_name,sm_attr_name,sm_attr_values) \
		(Utils_ns::TangoUtils::AddAttrReadRule<type>(deviceInstance,attr_name,sm_attr_name,BRACED_INIT_LIST sm_attr_values))
#endif

#ifndef _REGISTER_ATTR_READ_RULE
	#define _REGISTER_ATTR_READ_RULE(type,attr_name,sm_attr_name,sm_attr_values) \
		(Utils_ns::TangoUtils::AddAttrReadRule<type>(this,attr_name,sm_attr_name,BRACED_INIT_LIST sm_attr_values))
#endif

//## ATTR WRITE RULES
#ifndef __REGISTER_ATTR_WRITE_RULE
	#define __REGISTER_ATTR_WRITE_RULE(deviceInstance,type,attr_name,sm_attr_name,sm_attr_values) \
		(Utils_ns::TangoUtils::AddAttrWriteRule<type>(deviceInstance,attr_name,sm_attr_name,BRACED_INIT_LIST sm_attr_values))
#endif

#ifndef _REGISTER_ATTR_WRITE_RULE
	#define _REGISTER_ATTR_WRITE_RULE(type,attr_name,sm_attr_name,sm_attr_values) \
		(Utils_ns::TangoUtils::AddAttrWriteRule<type>(this,attr_name,sm_attr_name,BRACED_INIT_LIST sm_attr_values))
#endif

//## ATTR RULES
#ifndef __REGISTER_ATTR_RULE
	#define __REGISTER_ATTR_RULE(deviceInstance,type,attr_name,attr_key,attr_values) \
		(Utils_ns::TangoUtils::AddAttrRule<type>(deviceInstance,attr_name,attr_key,BRACED_INIT_LIST attr_values))
#endif

#ifndef _REGISTER_ATTR_RULE
	#define _REGISTER_ATTR_RULE(type,attr_name,attr_key,attr_values) \
		(Utils_ns::TangoUtils::AddAttrRule<type>(this,attr_name,attr_key,BRACED_INIT_LIST attr_values))
#endif

//## ATTR WRITE PRE-ACTIONS
#ifndef _REGISTER_ATTR_WRITE_PRE_ACTION
	#define _REGISTER_ATTR_WRITE_PRE_ACTION(attr_name,cmd_names) \
		(Utils_ns::TangoUtils::AddAttrWriteAction(this,attr_name,BRACED_INIT_LIST cmd_names,true))
#endif

//## ATTR WRITE POST-ACTIONS
#ifndef _REGISTER_ATTR_WRITE_POST_ACTION
	#define _REGISTER_ATTR_WRITE_POST_ACTION(attr_name,cmd_names) \
		(Utils_ns::TangoUtils::AddAttrWriteAction(this,attr_name,BRACED_INIT_LIST cmd_names,false))
#endif



//########################
//##  Exception macros
//########################
#ifndef _EXCEPTION_SOURCE
	#define _EXCEPTION_SOURCE \
		__CLASS_PREFIX__ + __FUNCTION__
#endif

#ifndef _THROW_TANGO_EXCEPTION
	#define _THROW_TANGO_EXCEPTION(name,msg) \
		(Utils_ns::TangoUtils::ThrowTangoException(name,msg,_EXCEPTION_SOURCE));
#endif

#ifndef _THROW_TANGO_STREAMED_EXCEPTION_
	#define _THROW_TANGO_STREAMED_EXCEPTION_(name,msgstream) \
		(Utils_ns::TangoUtils::ThrowTangoException(name,msgstream.str(),_EXCEPTION_SOURCE));
#endif

#endif 
