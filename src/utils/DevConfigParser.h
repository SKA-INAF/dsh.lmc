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
* @file DevConfigParser.h
* @class DevConfigParser
* @brief Class for parsing device config file
*
* Class for parsing device config file
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _DEV_CONFIG_PARSER_H
#define _DEV_CONFIG_PARSER_H

#ifndef _DEVICE_DATA_h
	#include <DeviceData.h>
#endif

#ifndef _ALARM_DATA_h
	#include <AlarmData.h>
#endif

#ifndef _ARCHIVED_DATA_h
	#include <ArchivedData.h>
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS_h
	#include <CodeUtils.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

//# Tango headers
#ifndef _TANGO_H
	#include <tango.h>
#endif

//# XML parser
#ifndef HEADER_PUGIXML_HPP
	#include <pugixml.hpp>
#endif

//# JSON CPP
#ifndef JSON_JSON_H_INCLUDED
	#include <json/json.h>
#endif

//# Google test headers
#ifdef ENABLE_TEST
	#include <gtest/gtest.h>
	#include <gtest/gtest_prod.h>
#endif

#include <string>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <iomanip>

#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <map>



namespace Utils_ns {

//Forward declarations
#ifdef ENABLE_TEST
class DevConfigParserTest;
#endif

// ============================================================================
// class: DevConfigParser
// ============================================================================
class DevConfigParser {

	//Friend class declarations
	#ifdef ENABLE_TEST
		friend class DevConfigParserTest;
		//friend class ::DevConfigParserTest;
		FRIEND_TEST(DevConfigParserTest, TestGetAttrDataFormat);	
		FRIEND_TEST(DevConfigParserTest, TestGetAttrDataType);			
	#endif
	
	public:
		DevConfigParser();
    DevConfigParser(std::string filename);
		~DevConfigParser();

	public: 
		/** 
		\brief Read XML config file
 		*/
		static int ReadFromFile(pugi::xml_document& doc,std::string& filename);
		/** 
		\brief Read XML config file string
 		*/
		static int ReadFromString(pugi::xml_document& doc,std::string& filename);
		/** 
		\brief Parse config file and get attributes
 		*/
		static int Parse(std::vector<Utils_ns::DeviceAttr*>& attrCollection,std::string& input,bool readFromFile);
		/** 
		\brief Parse config file and get alarms
 		*/
		static int ParseAlarms(std::vector<Utils_ns::AlarmData*>& alarmCollection,std::string& input,bool readFromFile);
		/** 
		\brief Parse config file and get archived attrs
 		*/
		static int ParseArchivedAttrs(std::vector<Utils_ns::ArchivedData*>& archivedAttrCollection,std::string& input,bool readFromFile);
		
		
	protected:
		/** 
		\brief Parse attributes from XML config file
 		*/
		static void ParseAttributes(std::vector<DeviceAttr*>& attrCollection,pugi::xml_node& rootNode);
		/** 
		\brief Parse alarms from XML config file
 		*/
		static void ParseAlarmCollection(std::vector<AlarmData*>& alarmCollection,pugi::xml_node& rootNode);
		/** 
		\brief Parse archived attrs from XML config file
 		*/
		static void ParseArchivedAttrCollection(std::vector<ArchivedData*>& archivedAttrCollection,pugi::xml_node& rootNode);

		/** 
		\brief Parse attribute node from XML config file
 		*/
		static DeviceAttr* ParseAttrNode(pugi::xml_node& node);
		/** 
		\brief Parse fwd attribute node from XML config file
 		*/
		static DeviceAttr* ParseFwdAttrNode(pugi::xml_node& node);
		/** 
		\brief Get attribute data type
 		*/
		static long GetAttrDataType(std::string dataTypeTag);
		/** 
		\brief Get attr write/read type
 		*/
		static Tango::AttrWriteType GetAttrRWType(std::string dataRWTag);
		
		/** 
		\brief Get attr data format
 		*/
		static int GetAttrDataFormat(std::string dataFormatTag);

		/** 
		\brief Get formula eval mode
 		*/
		static int GetFormulaEvalMode(Tango::EventType& ev_type,std::string mode);


		/** 
		\brief Parse attr value node in config
 		*/
		template<typename T>
		static int ParseValueNode(Utils_ns::DeviceAttr* dev_attr, pugi::xml_node& node);

		/** 
		\brief Parse attr property node in config
 		*/	
		template<typename T>
		static int ParseMultiAttrProp(Tango::MultiAttrProp<T>* prop,pugi::xml_node& node);
		
		/** 
		\brief Parse formula node in config
 		*/		
		static FormulaData* ParseFormulaNode(pugi::xml_node& node);

		/** 
		\brief Parse alarm data node in config
 		*/		
		static AlarmData* ParseAlarmDataNode(pugi::xml_node& node);

		/** 
		\brief Parse archived attr data node in config
 		*/		
		static ArchivedData* ParseArchivedDataNode(pugi::xml_node& node);

	private:

};//close DevConfigParser class


#include "DevConfigParser.tpp"


}//close namespace

#endif





