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
* @file FormulaManager.h
* @class FormulaManager
* @brief Formula manager
*
* Formula manager
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef _FORMULA_MANAGER_h
#define _FORMULA_MANAGER_h 1

#ifndef _FORMULA_DATA_h
	#include <FormulaData.h>
#endif

#ifndef _EVT_SUBSCRIPTION_DATA_h
	#include <EvtSubscriptionData.h>
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

namespace Utils_ns {

class TangoProxyManager; 

// ============================================================================
// class: FormulaManager
// ============================================================================
class FormulaManager {

	public:
		/** 
		\brief Constructor
 		*/
		FormulaManager(Tango::DeviceImpl* dev,TangoProxyManager* proxy_mgr);
		
		/** 
		\brief Destructor
 		*/
		~FormulaManager();

	public:
		/** 
		\brief Has formulas in map
		*/
		bool HasFormulas(){
			return !m_attrFormulaMap.empty();
		}	
 		
		/** 
		\brief Find formula data in map
 		*/
		FormulaData* FindFormula(std::string attr_name);
		
		/** 
		\brief Register formula
 		*/
		int RegisterFormula(std::string attr_name,FormulaData* formula_data);
	
		/** 
		\brief Get formula attr data in json string
 		*/
		int GetFormulaDataAsJsonString(std::string& jsonString,std::string attr_name,bool include_formula_expr=false);

		/** 
		\brief Returns all registered formulas 
 		*/
		int GetRegisteredFormulasInJson(std::vector<std::string>&);

		/** 
		\brief Returns all registered AND initialized formulas 
 		*/
		int GetInitializedFormulasInJson(std::vector<std::string>&);

		/** 
		\brief Returns all formulas linked to input event
 		*/
		int GetFormulasLinkedToEvent(std::vector<std::string>& attr_name_list,std::vector<FormulaData*>& formula_data_list,Utils_ns::EvtSubscriptionDataPtr evt_sub_ptr);

		/** 
		\brief Returns all formulas linked to a specific proxy
 		*/
		int GetFormulasLinkedToProxy(std::vector<std::string>& attr_name_list,std::vector<FormulaData*>& formula_data_list,std::string proxy_name);

		/** 
		\brief Update event value in formulas
 		*/
		int UpdateEventValueInFormulas(Utils_ns::EvtSubscriptionDataPtr evt_sub_ptr,Tango::EventData* event_data);

		/** 
		\brief Get formula data list
 		*/
		int GetFormulaDataList(std::vector<std::string>& attr_name_list,std::vector<FormulaData*>& formula_data_list);


	private:
		/** 
		\brief Clear formula data in map
 		*/
		void ClearFormulaMap();
		
		/** 
		\brief Fill event-attrs map
 		*/
		int FillEventAttrMap(std::string& attr_name,std::string& proxy_name,std::string& proxy_attr_name,Tango::EventType& evt_type);

		/** 
		\brief Find formula data in map
 		*/
		FormulaData* FindFormula_i(std::string attr_name);

	private:

		//Mutex & timers
		yat::Mutex m_lock;

		//Device
		Tango::DeviceImpl* m_dev;
		
		//Proxy manager
		TangoProxyManager* m_proxyManager;

		//Formula map
		std::map<std::string,FormulaData*> m_attrFormulaMap;
		
		//Events-->Attr name list
		std::map<EvtSubscriptionData,std::vector<std::string>> m_eventAttrsMap;

};//close class FormulaManager

}//close namespace


#endif



