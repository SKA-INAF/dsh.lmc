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
* @file ArchivedData.h
* @class ArchivedData
* @brief ArchivedData class
*
* ArchivedData class 
* @author S. Riggi
* @date 30/06/2017
*/

#ifndef _ARCHIVED_DATA_h
#define _ARCHIVED_DATA_h 1

#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS
	#include <CodeUtils.h>
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
// class: ArchivedData
// ============================================================================
class ArchivedData {

	public:
		
		/** 
		\brief Constructor
 		*/
		ArchivedData(
			std::string name,
			std::vector<std::string> strategies,
			long int ttl= 0,
			bool enabled= true
		);

		/** 
		\brief Destructor
 		*/
		~ArchivedData();
		
	protected:

	public:
		/** 
		\brief Check if archived data are valid
 		*/
		bool IsValid();	
		/** 
		\brief Serialize to Tango string
 		*/
		int SerializeToTangoFormat(std::string& data);

		/** 
		\brief Serialize strategies to Tango string
 		*/
		int GetEncodedStrategyStr(std::string& data);
	
		/** 
		\brief Set alarm name
 		*/
		void SetName(std::string s){ m_name = s;}
		/** 
		\brief Get alarm name
 		*/
		std::string GetName() {return m_name;}

  	/** 
		\brief Set archiving strategies
 		*/	
		void SetStrategies(std::vector<std::string>& v ){ m_strategies= v; }  
		/** 
		\brief Get archiving strategies
 		*/
		std::vector<std::string>& GetStrategies() {return m_strategies;}
		
		/** 
		\brief Set enabled
 		*/
		void SetEnabled(bool value){ m_enabled= value; }  
		/** 
		\brief Get enabled
 		*/
		bool GetEnabled() {return m_enabled;}  
		
		/** 
		\brief Get ttl in hours
 		*/
		long int GetTTL(){return m_ttl;}
		/** 
		\brief Set ttl in hours
 		*/
		void SetTTL(long int ttl){m_ttl= ttl;}

	private:
		
	
	private:	
		//- Attribute name (MANDATORY)
		std::string m_name;

		//- Archiving strategy (MANDATORY)
		std::vector<std::string> m_strategies;

		//- Time-to-live
		long int m_ttl;

		//- Enable at startup
		bool m_enabled;

		
};//close class ArchivedData

}//close namespace

#endif
