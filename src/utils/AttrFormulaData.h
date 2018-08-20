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
* @file AttrFormulaData.h
* @class AttrFormulaData
* @brief AttrFormulaData container
*
* AttrFormulaData container 
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _ATTR_FORMULA_DATA_h
#define _ATTR_FORMULA_DATA_h 1

#include <Logger.h>
#include <CodeUtils.h>
#include <TangoUtils.h>
#include <AttrData.h>
#include <EvtSubscriptionData.h>

//Tango headers
#include <tango.h>
#include <log4tango.h>

//Yat headers
#include <yat/any/GenericContainer.h>
#include <yat/memory/DataBuffer.h>
#include <yat/time/Time.h>
#include <yat/time/Timer.h>

//Exprtk lib
#include <exprtk.hpp>


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

using namespace std;

namespace Utils_ns {

//Forward declaration
//...


// ============================================================================
// class: AttrFormulaVar
// ============================================================================
class AttrFormulaVar {

	public:
		enum FormulaValueType {eATTR_VALUE=0, eATTR_QUALITY_FACTOR=1};

	public:
		
		/** 
		\brief Parametric Constructor
 		*/
		AttrFormulaVar(std::string devname,std::string attrname,int value_type=eATTR_VALUE) 
			: device_name(devname), attr_name(attrname), attr_value_type(value_type)
		{
			attr_info= 0;
			attr_data= 0;		
			attr_init_value_str= "";
			has_init_value= false;
			exp_data_type= -1;
			exp_data_format= -1;
		};


		/** 
		\brief Destructor
 		*/
		~AttrFormulaVar(){
			ClearAttrInfo();
			ClearAttrData();
		};
		/** 
		\brief == operator for attr formula var
 		*/
		bool operator==(const AttrFormulaVar& rhs) const { 
			return ((this->device_name==rhs.device_name) && (this->attr_name==rhs.attr_name)); 
		}

		/** 
		\brief == operator for attr formula var ptr
 		*/
		bool operator==(const std::shared_ptr<AttrFormulaVar> rhs) const { 
			return ((this->device_name==rhs->device_name) && (this->attr_name==rhs->attr_name)); 
		}

	public:
		/** 
		\brief Set attribute info
 		*/
		int SetAttrInfo(Tango::AttributeInfoEx* info,bool copy=false);

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
		void SetExpectedDataInfo(int type,int format){
			exp_data_type= type;
			exp_data_format= format;
		}


		/** 
		\brief Has attr info
 		*/
		bool HasAttrInfo(){	
			bool hasAttrInfo= (attr_info) ? true : false; 
			return hasAttrInfo;
		}	

		/** 
		\brief Has attr data
 		*/
		bool HasAttrData(){	
			bool hasAttrData= (attr_data) ? true : false; 
			return hasAttrData;
		}		

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
		int SetAttrValue(Tango::DeviceAttribute* da){
			if(!da){
				DEBUG_LOG("Null ptr to given DeviceAttribute!");
				return -1;	
			}	
			if(!attr_data) {
				DEBUG_LOG("Null ptr to given AttrData!");
				return -1;	
			}			
			return attr_data->set_data(da);
		}
		

	protected:

		/** 
		\brief Create attr data
 		*/	
		int CreateAttrData();

		/** 
		\brief Create scalar attr data
 		*/
		template<typename T>
		int CreateScalarAttrData(T initValue);

		/** 
		\brief Create spectrum attr data
 		*/
		template<typename T>
		int CreateSpectrumAttrData(T initValue);

		/** 
		\brief Create enum scalar attr data
 		*/
		int CreateScalarEnumAttrData(Tango::DevShort initValue);

		/** 
		\brief Clear attr info
 		*/
		void ClearAttrInfo(){
			if(attr_info){
				delete attr_info;
				attr_info= 0;
			}
		}//close ClearAttrInfo()

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
		std::string device_name;
		std::string attr_name;
		Tango::AttributeInfoEx* attr_info;
		AttrData* attr_data;
		int attr_value_type;
		std::string attr_init_value_str;
		int exp_data_type;
		int exp_data_format;
		bool has_init_value;

};//close AttrFormulaVar class

template<class T,class K>
struct AttrFormulaVarData {
	K attr_value;
	T attr_quality;
	long attr_timediff;
};

template<class T>
struct AttrFormulaDataValues {
	typedef std::vector<T> VecT;
	std::map<std::string,AttrFormulaVarData<T,T>> scalar_values;
	std::map<std::string,AttrFormulaVarData<T,VecT>> spectrum_values;
	std::map<std::string,AttrFormulaVarData<T,std::string>> scalar_string_values;
};

typedef std::shared_ptr<AttrFormulaVar> AttrFormulaVarPtr;
typedef std::map<std::string,AttrFormulaVarPtr> AttrFormulaSymbols;

// ============================================================================
// class: AttrFormulaData
// ============================================================================
class AttrFormulaData {

	public:
		/** 
		\brief Parametric constructor
 		*/
		AttrFormulaData(const std::string& formula,const AttrFormulaSymbols& symbols,const std::vector<Tango::EventType> evaluation_modes)
			: m_formula(formula), m_symbols(symbols), m_evaluation_modes(evaluation_modes)
		{
		
		};


		/** 
		\brief Standard constructor
 		*/
		AttrFormulaData() {
			m_formula= "";	
		};

		/** 
		\brief Destructor
 		*/
		~AttrFormulaData(){
			
		};

	public:
		/** 
		\brief Return true if formula was initialized (e.g. all formula vars have attr info)
 		*/
		bool IsInitialized();
			
		/** 
		\brief Returns formula text
 		*/
		const std::string& GetFormula() const {return m_formula;}		
		/** 
		\brief Returns formula symbols
 		*/
		const AttrFormulaSymbols& GetFormulaSymbols() const {return m_symbols;}
		/** 
		\brief Returns formula evaluation modes
 		*/
		const std::vector<Tango::EventType>& GetFormulaEvaluationMode() const {return m_evaluation_modes;}
		/** 
		\brief Set attribute info in formula
 		*/
		int SetAttrInfo(std::string var_name,Tango::AttributeInfoEx* attr_info,bool copy=true);
		/** 
		\brief Get formula expressed in json notation
 		*/
		std::string GetFormulaInJson();
		/** 
		\brief Evaluate formula (T=data type used in formula, K=formula result)
 		*/
		template<typename T,typename K>
		int EvaluateFormula(K& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold);

		/** 
		\brief Evaluate formula for spectrum attr (T=data type used in formula, K=formula result)
 		*/
		template<typename T,typename K>
		int EvaluateFormula(std::vector<K>& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold);

		/** 
		\brief Set event value in formula
 		*/
		int SetEventValue(EvtSubscriptionDataPtr evt_sub_ptr,Tango::EventData* event_data);
		
		/** 
		\brief Fill symbol table
 		*/
		template<typename T>
		int FillSymbolTable(exprtk::symbol_table<T>& symbol_table_t,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref);

		/** 
		\brief Compute evaluated formula attr quality factor
 		*/
		template<typename T>
		int ComputeFormulaAttrQuality(Tango::AttrQuality& attr_quality,AttrFormulaDataValues<T>& data_values,long attr_timestamp_threshold=20);

		/** 
		\brief Check if formula has a specific proxy in its formula vars
 		*/
		bool HasProxyInFormulaVars(std::string proxy_name);

	protected:
		
		/** 
		\brief Add numeric scalar var to symbol table
 		*/
		template<typename T>
		int AddNumericVarToSymbolTable(exprtk::symbol_table<T>& symbol_table_t,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		/** 
		\brief Add numeric spectrum var to symbol table
 		*/
		template<typename T>
		int AddNumericSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table_t,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		/** 
		\brief Add State scalar var to symbol table
 		*/
		template<typename T>
		int AddStateVarToSymbolTable(exprtk::symbol_table<T>& symbol_table_t,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		/** 
		\brief Add State spectrum var to symbol table
 		*/
		template<typename T>
		int AddStateSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table_t,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);


		/** 
		\brief Add string var to symbol table
 		*/
		template<typename T>
		int AddStringVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		
	private:
	
		
	protected:
		std::string m_formula;
		AttrFormulaSymbols m_symbols;
		std::vector<Tango::EventType> m_evaluation_modes;
		bool m_is_initialized;

};//close class AttrFormulaData

#include "AttrFormulaData.tpp"

}//close namespace

#endif
