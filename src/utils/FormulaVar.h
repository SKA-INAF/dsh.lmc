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
* @file FormulaVar.h
* @class FormulaVar
* @brief FormulaVar container
*
* FormulaVar container 
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _FORMULA_VAR_h
#define _FORMULA_VAR_h 1

#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS
	#include <CodeUtils.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

#ifndef _ATTR_DATA_h
	#include <AttrData.h>
#endif

#ifndef _EVT_SUBSCRIPTION_DATA_h
	#include <EvtSubscriptionData.h>
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

//Forward declaration
//...

// ============================================================================
// class: FormulaVar
// ============================================================================
class FormulaVar {

	public:
		enum FormulaValueType {eATTR_VALUE=0, eATTR_QUALITY_FACTOR=1};

	public:
		
		/** 
		\brief Constructor
 		*/
		FormulaVar(std::string attrname,int value_type=eATTR_VALUE);

		/** 
		\brief Destructor
 		*/
		virtual ~FormulaVar();

		/** 
		\brief == operator for attr formula var
 		*/
		bool operator==(const FormulaVar& rhs) const { 
			return ( (typeid(*this)==typeid(rhs)) && isEqual(rhs) );
		}

		/** 
		\brief == operator for attr formula var ptr
 		*/
		bool operator==(const std::shared_ptr<FormulaVar> rhs) const { 
			return ( (typeid(*this)==typeid(rhs)) && isEqual(rhs) );
		}

		/** 
		\brief Is equal (to be overridden in derived)
 		*/
		virtual bool isEqual(const FormulaVar& obj) const { 
			return (this->attr_name==obj.attr_name);
		}

		/** 
		\brief Is equal (to be overridden in derived)
 		*/
		virtual bool isEqual(const std::shared_ptr<FormulaVar> obj) const { 
			return (this->attr_name==obj->attr_name);
		}

	public:
		
		/** 
		\brief Has attr data
 		*/
		int SetAttrData(AttrData* data){	
			if(!data) return -1;
			ClearAttrData();
			attr_data= data;
			return 0;
		}	

		/** 
		\brief Has attr info
 		*/
		/*
		bool HasAttrInfo(){	
			bool hasAttrInfo= (attr_info) ? true : false; 
			return hasAttrInfo;
		}	
		*/

		/** 
		\brief Has attr data
 		*/
		bool HasAttrData(){	
			bool hasAttrData= (attr_data) ? true : false; 
			return hasAttrData;
		}		

		/** 
		\brief Check if formula var is initialized
 		*/
		virtual bool IsInitialized(){
			return HasAttrData();
		}

		/** 
		\brief Set attribute info
 		*/
		//virtual int SetAttrInfo(Tango::AttributeInfoEx* info,bool copy=false);

		/** 
		\brief Set attribute value
 		*/
		virtual int SetAttrValue(Tango::DeviceAttribute* da){return 0;}


		/** 
		\brief Get data for scalar attr casted to desired template type
 		*/	
		template<typename T>
		int GetScalarAttrData(T& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp);

		/** 
		\brief Get data for scalar attr casted to desired template type
 		*/	
		template<typename T>
		int GetSpectrumAttrData(std::vector<T>& value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp);


		/** 
		\brief Get formula data json object
 		*/
		virtual int GetDataAsJsonObj(Json::Value& jsonObj);

		/** 
		\brief Get formula data in string form
 		*/
		virtual int GetDataAsString(std::string& attr_data_str){
			attr_data_str= "";
			if(!attr_data) return -1;
			return attr_data->get_data_str(attr_data_str);
		}

	protected:

		/** 
		\brief Clear attr info
 		*/
		/*
		void ClearAttrInfo(){
			if(attr_info){
				delete attr_info;
				attr_info= 0;
			}
		}//close ClearAttrInfo()
		*/

		/** 
		\brief Clear attr data
 		*/
		void ClearAttrData(){
			if(attr_data){
				delete attr_data;
				attr_data= 0;
			}
		}//close ClearAttrData()

	public:
		std::string attr_name;
		//Tango::AttributeInfoEx* attr_info;
		AttrData* attr_data;
		int attr_value_type;
		bool is_proxy_attr;

};//close FormulaVar class


// ============================================================================
// class: ProxyAttrFormulaVar
// ============================================================================
class ProxyAttrFormulaVar : public FormulaVar {

	public:
		
		/** 
		\brief Parametric Constructor
 		*/
		ProxyAttrFormulaVar(std::string devname,std::string attrname,int value_type=eATTR_VALUE); 

		/** 
		\brief Destructor
 		*/
		virtual ~ProxyAttrFormulaVar();


		/** 
		\brief Is equal (to be overridden in derived)
 		*/
		virtual bool isEqual(const FormulaVar& obj) const override {
        auto v = static_cast<const ProxyAttrFormulaVar&>(obj);
        return FormulaVar::isEqual(v) && (this->device_name==v.device_name);
    }

		/** 
		\brief Is equal (to be overridden in derived)
 		*/
		virtual bool isEqual(const std::shared_ptr<FormulaVar> obj) const override {
        auto v = std::dynamic_pointer_cast<ProxyAttrFormulaVar>(obj);
        return FormulaVar::isEqual(v) && (this->device_name==v->device_name);
    }
	

	public:

		/** 
		\brief Set attribute info
 		*/
		//virtual int SetAttrInfo(Tango::AttributeInfoEx* info,bool copy=false);

		
		/** 
		\brief Set attribute info
 		*/
		void SetInitDataValue(std::string value){
			attr_init_value_str= value;
			has_init_value= true;
		}

		/** 
		\brief Set expected attribute data type info (used when attr type is not yet detected from remote proxy)
 		*/
		void SetExpectedDataInfo(int type,int format,long int size){
			exp_data_type= type;
			exp_data_format= format;
			exp_data_size= size;
		}

		
		/** 
		\brief Get data from init value for scalar attr casted to desired template type
 		*/	
		template<typename T>
		int GetScalarAttrDataFromInitValue(T& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp);

		/** 
		\brief Get data from init value for scalar attr casted to desired template type
 		*/	
		template<typename T>
		int GetSpectrumAttrDataFromInitValue(std::vector<T>& value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp);

		/** 
		\brief Set data from Tango::DeviceAttribute
 		*/
		virtual int SetAttrValue(Tango::DeviceAttribute* da);
		
		/** 
		\brief Create attr data
 		*/	
		int CreateAttrData(Tango::AttributeInfoEx& attr_info);


	protected:

		
		/** 
		\brief Create scalar attr data
 		*/
		//template<typename T>
		//int CreateScalarAttrData(T initValue);

		/** 
		\brief Create spectrum attr data
 		*/
		//template<typename T>
		//int CreateSpectrumAttrData(T initValue);

		/** 
		\brief Create enum scalar attr data
 		*/
		//int CreateScalarEnumAttrData(Tango::DevShort initValue,std::vector<std::string>& enum_labels);

	public:

		std::string device_name;
		std::string attr_init_value_str;
		int exp_data_type;
		int exp_data_format;
		long int exp_data_size;
		bool has_init_value;

};//close ProxyAttrFormulaVar class



// ============================================================================
// class: InternalAttrFormulaVar
// ============================================================================
class InternalAttrFormulaVar : public FormulaVar {

	public:
		
		/** 
		\brief Parametric Constructor
 		*/
		InternalAttrFormulaVar(std::string attrname,int value_type=eATTR_VALUE);

		/** 
		\brief Destructor
 		*/
		~InternalAttrFormulaVar();

	public:
		
};//close InternalAttrFormulaVar class


#include "FormulaVar.tpp"


}//close namespace 

#endif
