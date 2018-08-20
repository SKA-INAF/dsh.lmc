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
* @file ArchivedData.cc
* @class ArchivedData
* @brief ArchivedData class
*
* ArchivedData class 
* @author S. Riggi
* @date 30/06/2017
*/

#include <ArchivedData.h>


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
// class: ArchivedData
// ============================================================================

ArchivedData::ArchivedData(std::string name,std::vector<std::string> strategies,long int ttl,bool enabled) 
	: m_name(name), 
		m_strategies(strategies),
		m_ttl(ttl),
		m_enabled(enabled)
{
	
}//close constructor



ArchivedData::~ArchivedData()
{
			
};//close destructor


bool ArchivedData::IsValid()
{
	//Check name
	if(m_name=="") return false;
	
	//Check strategies
	if(m_strategies.empty()) return false;

	//Check level
	if(m_ttl<0) return false;

	return true;

}//close IsValid()

int ArchivedData::GetEncodedStrategyStr(std::string& data)
{
	data= "";
	if(m_strategies.empty()) return -1;
	if(m_strategies.size()==1) {
		data= m_strategies[0]; 
	}
	else{
		for(size_t i=0;i<m_strategies.size()-1;i++) data+= m_strategies[i] + "|";
		data+= m_strategies[m_strategies.size()-1];
	}

	return 0;

}//close GetEncodedStrategyStr()

int ArchivedData::SerializeToTangoFormat(std::string& data)
{
	//============================================================================
	//== Serialization format example: attr_full_name;strategy=ALWAYS|RUN;ttl=0 
	//============================================================================

	//Check if valid data
	if(!IsValid()) return -1;

	//Init return value
	data= "";

	//Serialize to Tango format used in AlarmSystem
	//Encode strategy list
	std::string strategy_encoded= "";
	if(GetEncodedStrategyStr(strategy_encoded)<0) return -1;
	
	std::stringstream ss;
	ss<<m_name<<";";
	ss<<"strategy="<<strategy_encoded<<";";
	ss<<"ttl="<<m_ttl;
	
	data= ss.str();

	return 0;

}//close SerializeToTangoFormat()


}//close namespace

