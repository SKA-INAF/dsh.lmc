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
* @file AttrFormulaManager.h
* @class AttrFormulaManager
* @brief Attr formula manager
*
* Attr formula manager
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef AttrFormulaManager_h
#define AttrFormulaManager_h 1

#include <AttrFormulaData.h>
#include <EvtSubscriptionData.h>

#include <TangoUtils.h>
#include <Logger.h>
#include <CodeUtils.h>
#include <tango.h>
#include <log4tango.h>

#include <yat/any/GenericContainer.h>
#include <yat/memory/DataBuffer.h>
#include <yat/time/Time.h>
#include <yat/time/Timer.h>

namespace Utils_ns {

class TangoProxyManager; 

// ============================================================================
// class: AttrFormulaManager
// ============================================================================
class AttrFormulaManager {

	public:
		/** 
		\brief Constructor
 		*/
		AttrFormulaManager(Tango::DeviceImpl* dev,TangoProxyManager* proxy_mgr);
		
		/** 
		\brief Destructor
 		*/
		~AttrFormulaManager();

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
		AttrFormulaData* FindFormula(std::string attr_name);
		/** 
		\brief Register formula
 		*/
		int RegisterFormula(std::string attr_name,AttrFormulaData* formula_data);
	
		/** 
		\brief Initialize a registered formula (e.g. filling attr info for all formula entries)
 		*/
		int InitializeFormula(std::string attr_name);

		/** 
		\brief Initialize all registered formulas 
 		*/
		int InitializeFormulas();

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
		int GetFormulasLinkedToEvent(std::vector<std::string>& attr_name_list,std::vector<AttrFormulaData*>& formula_data_list,Utils_ns::EvtSubscriptionDataPtr evt_sub_ptr);

		/** 
		\brief Returns all formulas linked to a specific proxy
 		*/
		int GetFormulasLinkedToProxy(std::vector<std::string>& attr_name_list,std::vector<AttrFormulaData*>& formula_data_list,std::string proxy_name);

		/** 
		\brief Update event value in formulas
 		*/
		int UpdateEventValueInFormulas(Utils_ns::EvtSubscriptionDataPtr evt_sub_ptr,Tango::EventData* event_data);

	private:
		/** 
		\brief Clear formula data in map
 		*/
		void ClearFormulaMap();
		/** 
		\brief Initialize formula data
 		*/
		int InitializeFormula(AttrFormulaData* formula_data);

		/** 
		\brief Get formula var attribute info from proxy
 		*/
		Tango::AttributeInfoEx* GetFormulaVarInfo(std::string proxy_name,std::string proxy_attr_name);

		/** 
		\brief Fill event-attrs map
 		*/
		int FillEventAttrMap(std::string& attr_name,std::string& proxy_name,std::string& proxy_attr_name,Tango::EventType& evt_type);

	private:

		//Mutex & timers
		yat::Mutex m_lock;

		//Device
		Tango::DeviceImpl* m_dev;
		
		//Proxy manager
		TangoProxyManager* m_proxyManager;

		//Formula map
		std::map<std::string,AttrFormulaData*> m_attrFormulaMap;
		
		//Events-->Attr name list
		std::map<EvtSubscriptionData,std::vector<std::string>> m_eventAttrsMap;

};//close class AttrFormulaManager

}//close namespace


#endif



