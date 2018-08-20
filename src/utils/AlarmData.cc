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
* @file AlarmData.cc
* @class AlarmData
* @brief AlarmData class
*
* AlarmData class 
* @author S. Riggi
* @date 30/06/2017
*/

#include <AlarmData.h>


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
// class: AlarmData
// ============================================================================

AlarmData::AlarmData(std::string name,std::string formula, std::map<std::string,std::string> symbols, std::string level, std::vector<std::string> groups,std::string message) 
	: m_name(name), 
		m_formula(formula),
		m_symbols(symbols),
		m_level(level),
		m_groups(groups),
		m_message(message)
{
	//Initialize optional members
	InitDefaults();

}//close constructor



AlarmData::~AlarmData()
{
			
};//close destructor


void AlarmData::InitDefaults()
{
	//Init optional fields
	m_on_delay= 0;
	m_off_delay= 0;
	m_silent_time= -1;
	m_on_command= "";
	m_off_command= "";
	m_enabled= true;
	m_is_polled= false;
	m_poll_period= 0;//in ms

}//close InitDefaults()

bool AlarmData::IsValid()
{

	//Check name
	if(m_name=="") return false;
	
	//Check formula
	if(m_formula=="") return false;

	//Check formula symbols
	if(m_symbols.empty()) return false;

	//Check level
	if(m_level=="") return false;

	//Check groups
	//If empty groups is set to "none"
	//if(m_groups.empty()) return false;

	//Check message
	if(m_message=="") return false;

	//If has polling check period
	if(m_is_polled && m_poll_period<0) return false;

	return true;

}//close IsValid()

int AlarmData::ExpandFormula(std::string& formula_expander)
{

	//Init return value
	formula_expander= m_formula;

	//Check symbols
	if(m_symbols.empty())
	{
		ERROR_LOG("Empty alarm formula symbols!");
		return -1;
	}

	//Iterate over formula symbols and replace $var with corresponding map value
	typedef std::map<std::string,std::string>::iterator mapIter;
	for( mapIter it = m_symbols.begin() ; it!=m_symbols.end() ; ++it )
  {
		std::string formula_var_alias= std::string("$") + it->first;
		std::string formula_var_value= it->second;
		if(CodeUtils::StringFindAndReplace(m_formula, formula_var_alias, formula_var_value)<0)
		{
			ERROR_LOG("No formula var named "<<formula_var_alias<<" is found in formula expression (formula & symbols mismatch or parsing error)!");
			return -1;
		}
	}//end map iteration

	
	return 0;

}//close ExpandFormula()

int AlarmData::SerializeToTangoFormat(std::string& data)
{

	//Check if valid data
	if(!IsValid()) return -1;

	//Init return value
	data= "";

	//Expand formula using symbols
	std::string formula_expanded= "";
	if(ExpandFormula(formula_expanded)<0){
		ERROR_LOG("Failed to expand alarm formula using given formula symbols!");
		return -1;
	}

	//Serialize to Tango format used in AlarmSystem
	//name=nome_allarme;formula=(max(graziano/beamstatus/0/dattr1.alarm, sin(graziano/beamstatus/0/dattr2.normal)));on_delay=0;off_delay=1;level=warning;silent_time=2;group=none;message=beamstatus dattr2!;on_command=graziano/beamstatus/0/status;off_command=graziano/beamstatus/0/state;enabled=1
	
	std::stringstream ss;
	//ss<<"name="<<m_name<<";";//IN OLD ALARM HANDLER attribute name key was "name"
	ss<<"tag="<<m_name<<";";
	ss<<"formula="<<m_formula<<";";
	ss<<"on_delay="<<m_on_delay<<";";
	ss<<"off_delay="<<m_off_delay<<";";
	//ss<<"level="<<m_level<<";";//IN OLD ALARM HANDLER attribute name key was "level"
	ss<<"priority="<<m_level<<";";
	//ss<<"silent_time="<<m_silent_time<<";";//IN OLD ALARM HANDLER attribute name key was "silent_time"
	ss<<"shlvd_time="<<m_silent_time<<";";
	if(m_groups.empty()) {
		ss<<"group="<<"none"<<";";
	}	
	else {
		
		if(m_groups.size()==1) ss<<"group="<<m_groups[0]<<";";
		else{
			ss<<"group=";
			for(unsigned int i=0;i<m_groups.size()-1;i++){
				ss<<m_groups[i]<<" | ";
			}
			ss<<m_groups[m_groups.size()-1]<<";";
		}
	}//close else
	ss<<"message="<<m_message<<";";
	if(m_on_command!="") ss<<"on_command="<<m_on_command<<";";
	if(m_off_command!="") ss<<"off_command="<<m_off_command<<";";
	ss<<"enabled="<<m_enabled;//NB: Do not put a ';' at the end otherwise parsing will fail in AlarmHandler device
	
	data= ss.str();

	return 0;

}//close SerializeToTangoFormat()


}//close namespace

