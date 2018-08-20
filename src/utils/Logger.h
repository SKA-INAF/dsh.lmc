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
* @file Logger.h
* @class Logger
* @brief Logger
*
* Logger class for managing logging msg
* @author S. Riggi
* @date 20/01/2015
*/

#ifndef _LOGGER_h
#define _LOGGER_h 1

#ifndef _SYS_UTILS_h
	#include <SysUtils.h>
#endif

//Tango headers
#ifndef _TANGO_H
	#include <tango.h>
#endif

#ifndef _LOG4TANGO_H_
	#include <log4tango.h>
#endif

//#include <syslog.h>

/*
//== LOG4CXX HEADERS ==
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/simplelayout.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/helpers/exception.h>
*/

//Boost headers
//#define BOOST_LOG_DYN_LINK 1 // necessary when linking the boost_log library dynamically
#include <boost/log/core/core.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks/sink.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <stdexcept>
#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <map>
#include <string>

using namespace std;


namespace Utils_ns {



// ============================================================================
// class: SysLogger
// ============================================================================
/** 
\brief Boost severity log level codes
*/
enum boost_severity_level {	
	boost_off= 0,
	boost_fatal= 1,
	boost_error= 2,
  boost_warn= 3,
	boost_info= 4,
  boost_debug= 5
};
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity",boost_severity_level)


class SysLogger {

	public:
		/** 
		\brief Class constructor
		*/
		SysLogger(const std::string level,const std::string tag,const std::string facility,const std::string host="localhost");

		/** 
		\brief Class destructor
		*/
		virtual ~SysLogger();

	private:
		/** 
		\brief Get syslog facility
		*/
		virtual sinks::syslog::facility GetFacility(const std::string syslog_facility) const;
			
		/** 
		\brief Get boost log level from Tango enum code
		*/
		virtual boost_severity_level GetMappedLogLevel(int level) const;
		/** 
		\brief Get boost log level from Log4Tango enum code
		*/
		virtual boost_severity_level GetMappedLogLevelFromLog4Tango(int level) const;
		
		/** 
		\brief Get boost log level from string code
		*/
		boost_severity_level GetMappedLogLevelFromString(const std::string sLevel) const;
		
	

	public:

		/** 
		\brief Initialize boost logger
		*/
		virtual int Init();

		/** 
		\brief Log a message with an enum log level
		*/
		virtual void Log(int level, const std::string msg, const std::string msg_prefix="");
		
		/** 
		\brief Log a message with an string log level
		*/
		virtual void Log(const std::string sLevel, const std::string msg, const std::string msg_prefix="");
		
		/** 
		\brief Set appender log level from enum code
		*/
		virtual void SetLogLevel(int log_level);
		
		/** 
		\brief Set appender log level from string code
		*/
		virtual void SetLogLevel(const std::string sLevel);
			
		/** 
		\brief Get syslog facility code
		*/
		virtual int GetFacilityCode(const std::string syslog_facility) const;
		
		
	protected:
		std::string m_level;
		std::string m_tag;		
		std::string m_facility;
		std::string m_host;
		
	private:
		typedef sinks::synchronous_sink< sinks::syslog_backend > sink_t;
		boost::shared_ptr<sink_t> sink;
		boost::shared_ptr<logging::core > core;
		
};//close SysLogger class



// ============================================================================
// class: LoggerManager
// ============================================================================

class LoggerManager {
	
	public:
		/** 
		\brief Return instance of LoggerManager singleton
		*/
		static LoggerManager& Instance();
 
		/** 
		\brief Copy constructor: NOT ALLOWED
		*/
    LoggerManager(LoggerManager const&) = delete;             // Copy construct

		/** 
		\brief Move constructor: NOT ALLOWED
		*/
    LoggerManager(LoggerManager&&) = delete;                  // Move construct

		/** 
		\brief Copy assignment: NOT ALLOWED
		*/
    LoggerManager& operator=(LoggerManager const&) = delete;  // Copy assign

		/** 
		\brief Move assignment: NOT ALLOWED
		*/
    LoggerManager& operator=(LoggerManager &&) = delete;      // Move assign

	public:

		/** 
		\brief Log target enumerations
		*/
		enum LogTarget{eSysLog=1,eConsole=2,eFile=3};

	public:
		/** 
		\brief Create syslog logger
		*/
		static int CreateSysLogger(const std::string level,const std::string tag="syslogger",const std::string facility="local6",const std::string host="localhost");

		/** 
		\brief Destroy syslog logger
		*/
		static int DestroySysLogger();

		/** 
		\brief Return logger instance
		*/
		SysLogger* GetLogger(){return m_logger;}

		/** 
		\brief Return logger target enumeration
		*/
		int GetLoggerTarget(){return m_target;}

		/** 
		\brief Set log level
		*/
		int SetLogLevel(int log_level){
			if(!m_logger) return -1;
			m_logger->SetLogLevel(log_level);
			return 0;
		}

	protected:
		/** 
		\brief Private constructor
		*/
		LoggerManager();

		/** 
		\brief Private destructor
		*/
		virtual ~LoggerManager();

	private:
		static int m_target;
		static SysLogger* m_logger;
	
};//close LoggerManager()

// ============================================================================
// class: ScopedLogger
// ============================================================================
class ScopedLogger {

	public:
		/** 
		\brief Class constructor
		*/
		ScopedLogger(std::string level,std::string prefix="",std::string device_name="");
			
		/** 
		\brief Class destructor
		*/
  	~ScopedLogger();

	public:
		/** 
		\brief Get string stream message
		*/
  	std::stringstream& stream(){ return m_sstream; }	
	
	private:
		std::stringstream m_sstream;
  	std::string m_level;
		std::string m_msgprefix;
		std::string m_deviceName;

};//close ScopedLogger

// ============================================================================
// class: DevScopedLogger
// ============================================================================
class DevScopedLogger {

	public:

		/** 
		\brief Class constructor
		*/
		DevScopedLogger(std::string level,std::string prefix="",Tango::DeviceImpl* ds_impl=nullptr);

		/** 
		\brief Class destructor
		*/
  	~DevScopedLogger();

	public:
		//Get stream
  	std::stringstream& stream(){ 	
			return m_sstream; 
		}	
	
	private:
		std::stringstream m_sstream;
  	std::string m_level;
		std::string m_msgprefix;
		std::string m_deviceName;
		Tango::DeviceImpl* m_ds_impl;

};//close DevScopedLogger

/** 
\brief Return device name 
*/
inline std::string GetDeviceName(){	
	std::string dev_name= "";
	try {
		//dev_name= Tango::Util::instance(false)->get_ds_name();
		std::vector<Tango::DeviceImpl*> dev_list= Tango::Util::instance(false)->get_device_list("*");	
		if(dev_list.empty()) dev_name= "";
		else dev_name= dev_list[0]->get_name();
	}
	catch(Tango::DevFailed& e){
		dev_name= "";
	}
	return dev_name;
}

/** 
\brief Check if run instance is inside a Tango device
*/
inline bool areInTangoServer(){
	try {
		std::vector<Tango::DeviceImpl*> dev_list= Tango::Util::instance(false)->get_device_list("*");	
		if(!dev_list.empty()) return true;
	}
	catch(Tango::DevFailed& e){
		return false;
	}
	return false;
}

/** 
\brief Get current class name
*/
inline std::string getClassName(std::string fullFuncName,std::string funcName){

	//Init pattern to be searched
	std::string result= "";
	//std::string pattern("::(.*)::");//FIX ME!!!
	std::string pattern("([-A-Za-z0-9_]+)::");
	pattern+= funcName;

	//Create regex
	boost::regex expression;
	try {
  	expression = pattern;
  }
  catch (boost::regex_error& e) {
  	return result;
  }

	//Find match
	boost::smatch matches;
	if (boost::regex_search(fullFuncName, matches, expression) && matches.size()>1) {
		result= std::string(matches[1].first, matches[1].second);
		//result= std::string(matches[matches.size()-1].first, matches[matches.size()-1].second);
		//for(int i=0;i<matches.size();i++) cout<<"match no. "<<i<<"="<<matches[i]<<endl;
  }//close if
	
	return result;

}//close function

/** 
\brief Get current class name prefix
*/
inline std::string getClassNamePrefix(std::string fullFuncName,std::string funcName){
	std::string className= getClassName(fullFuncName,funcName);
	std::string sprefix= "::";
	if(className=="") return className;
	return className+sprefix;
}

}//close namespace 

// ============================================================================
//              UTILITY MACROS
// ============================================================================
/** 
\brief Macro to get current class name
*/
#ifndef __CLASS__
	#define __CLASS__ Utils_ns::getClassName(__PRETTY_FUNCTION__,__FUNCTION__)
#endif

/** 
\brief Macro to get current class function name prefix
*/
#ifndef __CLASS_PREFIX__
	#define __CLASS_PREFIX__ Utils_ns::getClassNamePrefix(__PRETTY_FUNCTION__,__FUNCTION__)
#endif

/** 
\brief Macro to get current device class name
*/
#ifndef __DEVICE_CLASS
	#define __DEVICE_CLASS(deviceInstance) deviceInstance->get_device_class()->get_name()
#endif

/** 
\brief Macro to get current device instance name
*/
#ifndef __DEVICE_NAME
	#define __DEVICE_NAME(deviceInstance) deviceInstance->get_name()
#endif

// ============================================================================
//              LOG UTIL MACROS
// ============================================================================
/** 
\brief Macro to get current log message prefix given current class function
*/
#ifndef LOG_PREFIX
	#define LOG_PREFIX \
		__CLASS_PREFIX__ + __FUNCTION__ + std::string("() - ")
#endif

/** 
\brief Macro to get current device name
*/
#ifndef DEVICE_NAME
	#define DEVICE_NAME GetDeviceName()
#endif

/** 
\brief Macro to check if class function is inside a Tango device server
*/
#ifndef __INSIDE_TANGO_SERVER
	#define __INSIDE_TANGO_SERVER Utils_ns::areInTangoServer()
#endif

// ============================================================================
//              LOG MACROS
// ============================================================================
/** 
\brief Macro to send a log to a device and at desired log level
*/
#ifndef LMC_LOG
	#define LMC_LOG(DeviceName, Level, What) \
		Utils_ns::ScopedLogger(Level,LOG_PREFIX,DeviceName).stream() << What
#endif

/** 
\brief Macro to send a log to current device and at desired log level
*/
#ifndef LOG
	#define LOG(Level, What) LMC_LOG(DEVICE_NAME,Level,What)
#endif

/** 
\brief Macro to send an info log to current device 
*/
#ifndef INFO_LOG
	#define INFO_LOG(What) LOG("INFO",What)
#endif

/** 
\brief Macro to send a warn log to current device 
*/
#ifndef WARN_LOG
	#define WARN_LOG(What) LOG("WARN",What)
#endif

/** 
\brief Macro to send a debug log to current device 
*/
#ifndef DEBUG_LOG
	#define DEBUG_LOG(What) LOG("DEBUG",What)
#endif

/** 
\brief Macro to send an error log to current device 
*/
#ifndef ERROR_LOG
	#define ERROR_LOG(What) LOG("ERROR",What)
#endif

/** 
\brief Macro to send a fatal log to current device 
*/
#ifndef FATAL_LOG
	#define FATAL_LOG(What) LOG("FATAL",What)
#endif

// ============================================================================
//            DEVICE LOG MACROS
// ============================================================================
/** 
\brief Macro to send log from a device instance and at desired log level
*/
#ifndef DEV_LOG
	#define DEV_LOG(deviceInstance,Level, What) \
		Utils_ns::DevScopedLogger(Level,LOG_PREFIX,deviceInstance).stream() << What
#endif

#ifndef _DEV_LOG
	#define _DEV_LOG(Level, What) DEV_LOG(this,Level,What)
#endif

#ifndef _INFO_LOG
	#define _INFO_LOG(What) DEV_LOG(this,"INFO",What)
#endif

#ifndef _WARN_LOG
	#define _WARN_LOG(What) DEV_LOG(this,"WARN",What)
#endif

#ifndef _DEBUG_LOG
	#define _DEBUG_LOG(What) DEV_LOG(this,"DEBUG",What)
#endif

#ifndef _ERROR_LOG
	#define _ERROR_LOG(What) DEV_LOG(this,"ERROR",What)
#endif

#ifndef _FATAL_LOG
	#define _FATAL_LOG(What) DEV_LOG(this,"FATAL",What)
#endif

#ifndef __INFO_LOG
	#define __INFO_LOG(deviceInstance,What) DEV_LOG(deviceInstance,"INFO",What)
#endif

#ifndef __WARN_LOG
	#define __WARN_LOG(deviceInstance,What) DEV_LOG(deviceInstance,"WARN",What)
#endif

#ifndef __DEBUG_LOG
	#define __DEBUG_LOG(deviceInstance,What) DEV_LOG(deviceInstance,"DEBUG",What)
#endif

#ifndef __ERROR_LOG
	#define __ERROR_LOG(deviceInstance,What) DEV_LOG(deviceInstance,"ERROR",What)
#endif

#ifndef __FATAL_LOG
	#define __FATAL_LOG(deviceInstance,What) DEV_LOG(deviceInstance,"FATAL",What)
#endif

#endif

