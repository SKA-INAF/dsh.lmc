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
* @file AlarmData.h
* @class AlarmData
* @brief AlarmData class
*
* AlarmData class 
* @author S. Riggi
* @date 30/06/2017
*/

#ifndef _ALARM_DATA_h
#define _ALARM_DATA_h 1

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
// class: AlarmData
// ============================================================================
class AlarmData {

	public:
		
		/** 
		\brief Constructor
 		*/
		AlarmData(
			std::string name,
			std::string formula,
			std::map<std::string,std::string> symbols,
			std::string level,
			std::vector<std::string> groups,
			std::string message
		);

		/** 
		\brief Destructor
 		*/
		~AlarmData();
		
	protected:

		//Alarm severity level codes
		//enum SeverityLevel {eFAULT= 0, eWARNING= 1,	eLOG= 2};
	
	public:
		/** 
		\brief Check if alarm data (formula, fields) are valid
 		*/
		bool IsValid();	
		/** 
		\brief Serialize to Tango string
 		*/
		int SerializeToTangoFormat(std::string& data);
		/** 
		\brief Expand alarm formula replacing $var with corresponding value set in symbol map
 		*/
		int ExpandFormula(std::string& formula_expander);
		
		/** 
		\brief Set alarm name
 		*/
		void SetName(std::string s){ m_name = s;}
		/** 
		\brief Get alarm name
 		*/
		std::string GetName() {return m_name;}

		/** 
		\brief Set alarm formula
 		*/
		void SetFormula(std::string s) {m_formula = s;}
		/** 
		\brief Get alarm formula
 		*/	
		std::string GetFormula() {return m_formula;}

		/** 
		\brief Set alarm symbols
 		*/
		void SetSymbols(std::map<std::string,std::string>& m){m_symbols = m;}
		/** 
		\brief Get alarm symbols
 		*/
		std::map<std::string,std::string>& GetSymbols() {return m_symbols;}
				
		/** 
		\brief Set on delay par
 		*/
		void SetOnDelay(int i) {m_on_delay= i;} 
		/** 
		\brief Get on delay par
 		*/
		int GetOnDelay() {return m_on_delay;}
		/** 
		\brief Set off delay par
 		*/
		void SetOffDelay(int i) {m_off_delay= i;}
		/** 
		\brief Get off delay par
 		*/
		int GetOffDelay() {return m_off_delay;} 
		
		/** 
		\brief Set alarm level
 		*/
		void SetLevel(std::string s) {m_level= s;}
		/** 
		\brief Get alarm level
 		*/	
		std::string GetLevel() {return m_level;}
		/** 
		\brief Set silent time
 		*/
		void SetSilentTime(int i) {m_silent_time= i;}
		/** 
		\brief Get silent time par
 		*/
		int GetSilentTime() {return m_silent_time;}

  	/** 
		\brief Set alarm groups
 		*/	
		void SetGroups(std::vector<std::string>& v ){ m_groups= v; }  
		/** 
		\brief Get alarm groups
 		*/
		std::vector<std::string>& GetGroups() {return m_groups;}
		/** 
		\brief Set message
 		*/
		void SetMessage(std::string s){ m_message= s; }  
		/** 
		\brief Get message
 		*/
		std::string GetMessage() {return m_message;}

		/** 
		\brief Set on command
 		*/
		void SetOnCommand(std::string s){ m_on_command= s; } 
		/** 
		\brief Get on command
 		*/
		std::string GetOnCommand() {return m_on_command;} 

		/** 
		\brief Set on command
 		*/
		void SetOffCommand(std::string s){ m_off_command= s; } 
		/** 
		\brief Get off command
 		*/
		std::string GetOffCommand() {return m_off_command;} 

		/** 
		\brief Set enabled
 		*/
		void SetEnabled(bool value){ m_enabled= value; }  
		/** 
		\brief Get enabled
 		*/
		bool GetEnabled() {return m_enabled;}  

		/** 
		\brief Is alarm attribute to be polled?
 		*/
		bool IsPolled() const {return m_is_polled;}

		/** 
		\brief Set alarm attribute as to be polled
 		*/
		void SetPolled(bool value){m_is_polled=value;}
		/** 
		\brief Get poll period in ms
 		*/
		long int GetPollPeriod(){return m_poll_period;}
		/** 
		\brief Set poll period in ms
 		*/
		void SetPollPeriod(long int period){m_poll_period= period;}

	private:
		/** 
		\brief Init optional field values to defaults
 		*/
		void InitDefaults();
	
	private:	
		//- Alarm name (MANDATORY)
		std::string m_name;

		//- Alarm formula expression with variable references (e.g. $x) (MANDATORY)
		std::string m_formula;

		//- Alarm formula var map: key=label (e.g. x), value=attr name (e.g. tango://.../dev/mydevice/id1/myattr (MANDATORY)
		std::map<std::string,std::string> m_symbols;

		//- Delay in seconds before the alarm become active after the formula result change from false to true (OPTIONAL, default=0)
		int m_on_delay;
	
		//- Delay in seconds before the alarm become inactive after the formula result change from true to false
		int m_off_delay;

		//- Severity of the alarm. ‘fault’, ‘warning’ and ‘log’ are allowed (MANDATORY)
		std::string m_level;
		
		//- Silent time: Time in minutes this alarm will be silenced or shelved with the Silence and Shelve commands. If -1 this alarm cannot be silenced nor shelved.
		int m_silent_time;

		//- Alarm group (MANDATORY)
		std::vector<std::string> m_groups;

		//- Alarm message (MANDATORY)
		//  Additional message carried with the alarm
		std::string m_message;

		//- On/off commands (OPTIONAL, default=empty string)
		std::string m_on_command;
		std::string m_off_command;

		//- Enable flag: If true alarm is enabled, otherwise it is set to the OOSRV - out of service alarm state (OPTIONAL, default=true)
		bool m_enabled;

		//- Enable polling
		bool m_is_polled;
		long int m_poll_period;
		
};//close class AlarmData


// ============================================================================
// class: AlarmDataBuilder
// ============================================================================
/*
class AlarmData::AlarmDataBuilder {

	private:
		// Variables needed for construction of AlarmData class
		std::string m_name;
		std::string m_formula;
		std::map<std::string,std::string> m_symbols;	
		int m_on_delay;

	public:
		// default values for variables
		static const constexpr int on_delay_default = 0;
		static const constexpr int off_delay_default = 0;
		static const constexpr int silent_time_default = -1;
		static const constexpr bool enabled_default = true;
		static const constexpr std::string empty_str= "";

		//Create Builder with default values assigned
		AlarmDataBuilder() 
		{
			//Init default fields 
			InitDefaults();
		}

		//Set alarm name
		AlarmDataBuilder& SetName( const std::string s ){ this->m_name = s; return *this; }
		AlarmDataBuilder& SetFormula( const std::string s ){ this->m_formula = f; return *this; }
		AlarmDataBuilder& setSymbols( const std::map<std::string,std::string> m ){ this->m_symbols = m; return *this; }


		// produce desired Product
		AlarmDataBuilder build(){
			// Check variable consistency
			//...

			return AlarmData( 
				this->m_name, 
				this->m_formula, 
				this->m_symbols 
			);
		}//close build()

	private:
	
		//Initialize default fields
		void InitDefaults()
		{			
			m_name= empty_str;//M
			n_formula= empty_str;//M
			m_symbols.clear();//M
			m_on_delay= on_delay_default;//O
			m_off_delay= off_delay_default;//O
			m_level= empty_str;//M
			m_silent_time= silent_time_default;//O
			m_groups.clear();//M
			m_message= empty_str;//M
			m_on_command= empty_str;//O
			m_off_command= empty_str;//O
			m_enabled= enabled_default;//O
		}
		
		
};//close class AlarmDataBuilder
*/

}//close namespace

#endif
