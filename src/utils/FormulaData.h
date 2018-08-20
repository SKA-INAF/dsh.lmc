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
* @file FormulaData.h
* @class FormulaData
* @brief FormulaData container
*
* FormulaData container 
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _FORMULA_DATA_h
#define _FORMULA_DATA_h 1

#ifndef _FORMULA_VAR_h
	#include <FormulaVar.h>
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

//Exprtk lib
#ifndef INCLUDE_EXPRTK_HPP
#include <exprtk.hpp>
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


template<class T,class K>
struct FormulaVarData {	
	K attr_value;
	T attr_quality;
	long attr_timediff;
	std::string formula_var_name;
	std::string formula_attr_name;

	FormulaVarData(){
		attr_value= K();
		attr_quality= T();
		formula_var_name= "";
		formula_attr_name= "";
		attr_timediff= 0;
	}
};

template<class T>
struct FormulaDataValues {
	typedef std::vector<T> VecT;
	std::map<std::string,FormulaVarData<T,T>> scalar_values;
	std::map<std::string,FormulaVarData<T,VecT>> spectrum_values;
	std::map<std::string,FormulaVarData<T,std::string>> scalar_string_values;
};

typedef std::shared_ptr<FormulaVar> FormulaVarPtr;
typedef std::shared_ptr<ProxyAttrFormulaVar> ProxyAttrFormulaVarPtr;
typedef std::map<std::string,FormulaVarPtr> FormulaSymbols;

// ============================================================================
// class: FormulaData
// ============================================================================
class FormulaData {

	public:
		/** 
		\brief Parametric constructor
 		*/
		FormulaData(const std::string& formula,const FormulaSymbols& symbols,const std::vector<Tango::EventType> evaluation_modes);

		/** 
		\brief Standard constructor
 		*/
		FormulaData();

		/** 
		\brief Destructor
 		*/
		~FormulaData();
		
	public:

		/** 
		\brief Set device attr name 
 		*/
		void SetAttrName(std::string name){m_attrName= name;}

		/** 
		\brief Get device attr name 
 		*/
		std::string GetAttrName(){return m_attrName;}

		/** 
		\brief Set invalid quality factor flag 
 		*/
		void SetInvalidQualityIfOldFlag(bool choice){m_setInvalidQualityIfOld=choice;}
		/** 
		\brief Get invalid quality factor flag 
 		*/
		const bool GetInvalidQualityIfOldFlag() const {return m_setInvalidQualityIfOld;}

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
		const FormulaSymbols& GetFormulaSymbols() const {return m_symbols;}
		/** 
		\brief Returns formula evaluation modes
 		*/
		const std::vector<Tango::EventType>& GetFormulaEvaluationMode() const {return m_evaluation_modes;}
		/** 
		\brief Set attribute info in formula
 		*/
		//int SetAttrInfo(std::string var_name,Tango::AttributeInfoEx* attr_info,bool copy=true);

		/** 
		\brief Set formula post-set actions
 		*/
		void SetPostActions(std::vector<std::string> actions){m_postActions=actions;}

		/** 
		\brief Returns formula post-set actions
 		*/
		const std::vector<std::string>& GetPostActions() const {return m_postActions;}

		/** 
		\brief Set internal attr data (taken from device)
 		*/
		int SetInternalAttrData(std::string var_name,AttrData* attr_data);

		/** 
		\brief Set proxy attr data from AttributeInfo
 		*/
		int SetProxyAttrData(std::string var_name,Tango::AttributeInfoEx& attr_info);

		/** 
		\brief Get formula expressed in json notation
 		*/
		std::string GetFormulaInJson();
		/** 
		\brief Get formula data expressed in json notation
 		*/	
		int GetFormulaDataInJson(std::string& jsonString,bool include_formula_expr=false);

		/** 
		\brief Evaluate formula (T=data type used in formula, K=formula result)
 		*/
		template<typename T,typename K>
		int EvaluateFormula(K& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold=20);

		/** 
		\brief Evaluate formula for spectrum attr (T=data type used in formula, K=formula result)
 		*/
		template<typename T,typename K>
		int EvaluateFormula(std::vector<K>& formula_result,Tango::AttrQuality& formula_attr_quality,long data_size,long attr_timestamp_threshold=20);

		/** 
		\brief Set event value in formula
 		*/
		int SetEventValue(EvtSubscriptionDataPtr evt_sub_ptr,Tango::EventData* event_data);
		
		/** 
		\brief Fill symbol table
 		*/
		template<typename T>
		int FillSymbolTable(exprtk::symbol_table<T>& symbol_table_t,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref);

		/** 
		\brief Compute evaluated formula attr quality factor
 		*/
		template<typename T>
		int ComputeFormulaAttrQuality(Tango::AttrQuality& attr_quality,FormulaDataValues<T>& data_values,long attr_timestamp_threshold=20);

		/** 
		\brief Check if formula has a specific proxy in its formula vars
 		*/
		bool HasProxyInFormulaVars(std::string proxy_name);

	protected:
		
		/** 
		\brief Add numeric scalar var to symbol table
 		*/
		template<typename T>
		int AddNumericVarToSymbolTable(exprtk::symbol_table<T>& symbol_table_t,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		/** 
		\brief Add numeric spectrum var to symbol table
 		*/
		template<typename T>
		int AddNumericSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table_t,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		/** 
		\brief Add State scalar var to symbol table
 		*/
		template<typename T>
		int AddStateVarToSymbolTable(exprtk::symbol_table<T>& symbol_table_t,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		/** 
		\brief Add State spectrum var to symbol table
 		*/
		template<typename T>
		int AddStateSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table_t,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);


		/** 
		\brief Add string var to symbol table
 		*/
		template<typename T>
		int AddStringVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		/** 
		\brief Add string spectrum var to symbol table
 		*/
		template<typename T>
		int AddStringSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue);

		
	private:
	
		
	protected:
		
		std::string m_formula;
		FormulaSymbols m_symbols;
		std::vector<Tango::EventType> m_evaluation_modes;
		std::string m_attrName;
		bool m_is_initialized;
		std::vector<std::string> m_postActions;
		bool m_setInvalidQualityIfOld;

};//close class FormulaData

#include "FormulaData.tpp"

}//close namespace

#endif
