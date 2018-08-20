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

#include <Logger.h>
#include <TangoUtils.h>

namespace Utils_ns {

int LoggerManager::m_target;
SysLogger* LoggerManager::m_logger= 0;


//=========================================
//==         SYSLOGGER CLASS
//=========================================
SysLogger::SysLogger(const std::string level,const std::string tag,const std::string facility,const std::string host)
	: m_level(level), m_tag(tag), m_facility(facility), m_host(host)
{

};

SysLogger::~SysLogger()
{

};

sinks::syslog::facility SysLogger::GetFacility(const std::string syslog_facility) const {
			
	sinks::syslog::facility syslog_facility_code= sinks::syslog::local6;

	if(syslog_facility=="local0") syslog_facility_code= sinks::syslog::local0;
	else if(syslog_facility=="local1") syslog_facility_code= sinks::syslog::local1;
	else if(syslog_facility=="local2") syslog_facility_code= sinks::syslog::local2;
	else if(syslog_facility=="local3") syslog_facility_code= sinks::syslog::local3;
	else if(syslog_facility=="local4") syslog_facility_code= sinks::syslog::local4;
	else if(syslog_facility=="local5") syslog_facility_code= sinks::syslog::local5;
	else if(syslog_facility=="local6") syslog_facility_code= sinks::syslog::local6;
	else if(syslog_facility=="local7") syslog_facility_code= sinks::syslog::local7;
	else if(syslog_facility=="syslog") syslog_facility_code= sinks::syslog::syslogd;
	else if(syslog_facility=="user") syslog_facility_code= sinks::syslog::user;
	else syslog_facility_code= sinks::syslog::local6;
	
	return syslog_facility_code;
		
}//close GetFacility()



boost_severity_level SysLogger::GetMappedLogLevelFromLog4Tango(int level) const 
{	
	//Get boost level corresponding to log4tango level			
	if(level==log4tango::Level::DEBUG) return boost_debug;
	else if(level==log4tango::Level::INFO) return boost_info;
	else if(level==log4tango::Level::WARN) return boost_warn;
	else if(level==log4tango::Level::ERROR) return boost_error;
	else if(level==log4tango::Level::FATAL) return boost_fatal;
	else return boost_off;
		
	return boost_off;
		
}//close GetMappedLogLevelFromLog4Tango()

boost_severity_level SysLogger::GetMappedLogLevel(int level) const 
{	
	//Get boost level corresponding to Tango level			
	if(level==Tango::LOG_OFF) return boost_off;
	else if(level==Tango::LOG_INFO) return boost_info;
	else if(level==Tango::LOG_WARN) return boost_warn;
	else if(level==Tango::LOG_ERROR) return boost_error;
	else if(level==Tango::LOG_FATAL) return boost_fatal;
	else if(level==Tango::LOG_DEBUG) return boost_debug;

	return boost_off;
		
}//close GetMappedLogLevel()

boost_severity_level SysLogger::GetMappedLogLevelFromString(const std::string sLevel) const {
	 
	if (sLevel == "DEBUG") return boost_debug;
	else if (sLevel == "INFO") return boost_info;
	else if (sLevel == "WARN") return boost_warn;
	else if (sLevel == "ERROR") return boost_error;
	else if (sLevel == "FATAL") return boost_fatal;
	else if (sLevel == "OFF") return boost_off;
	else return boost_off;
 			
	return boost_off;
		
}//close GetMappedLogLevelFromString()

int SysLogger::Init(){	
			
	//Set attributes
	//m_facility= facility;
	//m_tag= tag;
	//m_level= sLevel;

	try {
		//Init boost core
		core = boost::log::core::get();

		//Get host & facility code	
		//std::string syslog_host= Utils_ns::SysUtils::GetHost();
		std::string syslog_host= m_host;
		sinks::syslog::facility syslog_facility_code= GetFacility(m_facility);

		// Create a syslog sink ==
		//LIST OF SUPPORTED KEYWORDS
		//facility - Specifies the facility code. If not specified, syslog::user will be used.
		//use_impl - Specifies the backend implementation. Can be one of:
		//native - Use the native syslog API, if available. If no native API is available, it is equivalent to udp_socket_based.
		//udp_socket_based - Use the UDP socket-based implementation, conforming to RFC3164 protocol specification. This is the default.
		//ip_version - Specifies IP protocol version to use, in case if socket-based implementation is used. Can be either v4 (the default one) or v6.
		//ident - Process identification string. This parameter is only supported by native syslog implementation.
		//=================================
		sink = boost::make_shared<sink_t>(
			keywords::use_impl = sinks::syslog::native,
      keywords::facility = syslog_facility_code,
			keywords::ident = m_tag
		);

		//Set message format
		sink->set_formatter(
    	logging::expressions::format("%1%")
      	% logging::expressions::smessage
    );

		// Set the straightforward level translator for the "Severity" attribute of type int
		sinks::syslog::custom_severity_mapping< boost_severity_level > mapping("Severity");
    mapping[boost_info] = sinks::syslog::info;
    mapping[boost_warn] = sinks::syslog::warning;
		mapping[boost_debug] = sinks::syslog::debug;
    mapping[boost_error] = sinks::syslog::error;
		mapping[boost_fatal] = sinks::syslog::critical;
		sink->locked_backend()->set_severity_mapper(mapping);

		//Set host address
		sink->locked_backend()->set_local_address(syslog_host);
		sink->locked_backend()->set_target_address(syslog_host);
		
		//Set log threshold
		boost_severity_level filter_level= GetMappedLogLevelFromString(m_level);
		sink->set_filter(severity <= filter_level);

    // Add the sink to the core
		core.get()->add_sink(sink);

    // Add some attributes too
		//core->add_global_attribute("RecordID", boost::log::attributes::counter< unsigned int >());
		//core->add_global_attribute("DeviceName", boost::log::attributes::constant<std::string>(dev_name));

		//Add common attributes
		//LineID - logging records counter with value type unsigned int
		//TimeStamp - local time generator with value type boost::posix_time::ptime
		//ProcessID - current process identifier with value type attributes::current_process_id::value_type
		//ThreadID - in multithreaded builds, current thread identifier with value type attributes::current_thread_id::value_type
		logging::add_common_attributes();

	}//close try block
  		
	catch (std::exception& e) {
		cerr<<"C++ exception occurred while setting up boost logger (err="<<e.what()<<")"<<endl;
  	return -1;
 	}

	return 0;
		
}//close Init()


void SysLogger::Log(int level, const std::string msg, const std::string msg_prefix)
{
	//Retrieve logger			
	logging::core_ptr syslogger= logging::core::get();
	//boost_severity_level syslevel= GetMappedLogLevel(level);
	boost_severity_level syslevel= GetMappedLogLevelFromLog4Tango(level);
					
	//Log message if logger is found and logging enabled
	if(syslogger && syslogger->get_logging_enabled() ){
		src::severity_logger_mt<boost_severity_level> lg(keywords::severity = boost_off);
		BOOST_LOG_SEV(lg, syslevel) << msg_prefix << msg;
	}
		
}//close Log()

void SysLogger::Log(const std::string sLevel, const std::string msg, const std::string msg_prefix)
{
	//Get Tango log levels from string 
	log4tango::Level::Value level= TangoUtils::GetTangoLogLevelFromString(sLevel);

	//Log message
	Log(level,msg,msg_prefix);
		
}//close Log()

void SysLogger::SetLogLevel(int log_level)
{
	//Get boost log level from Tango level code
	boost_severity_level level= GetMappedLogLevel(log_level);
	cout<<"SysLogger::SetLogLevel() - log_level="<<log_level<<", mapped_level="<<level<<endl;	
	if(level==boost_off) core->set_logging_enabled(false);
	else core->set_logging_enabled(true);
	core->set_filter( severity <= level );
		
}//close SetLogLevel()

void SysLogger::SetLogLevel(const std::string sLevel)
{
	//Get boost log level from string			
	boost_severity_level level= GetMappedLogLevelFromString(sLevel);
	if(level==boost_off) core->set_logging_enabled(false);
	else core->set_logging_enabled(true);
	core->set_filter( severity <= level );
		
}//close SetLogLevel()

int SysLogger::GetFacilityCode(const std::string syslog_facility) const 
{
	//Get syslog facility from string 			
	sinks::syslog::facility fac= GetFacility(syslog_facility);
	return static_cast<int>(fac);
		
}//close GetFacilityCode()


//=========================================
//==        LOGGER MANAGER CLASS
//=========================================
LoggerManager::LoggerManager()
{
	//- No op
}

LoggerManager::~LoggerManager()
{			
	/*
	if(m_logger){
		delete m_logger;
		m_logger= 0;
	}	
	*/	
}


LoggerManager& LoggerManager::Instance() {
    	
	// Since it's a static variable, if the class has already been created,
  // It won't be created again.
  // And it is thread-safe in C++11.
  static LoggerManager myInstance;
 
  // Return a reference to our instance.
  return myInstance;
    
}//close Instance()

int LoggerManager::CreateSysLogger(const std::string level,const std::string tag,const std::string facility,const std::string host) {
			
	//Skip if already created
	if(m_logger) {	
		//cout<<"LoggerManager::CreateSysLogger(): WARN: Already created...skip!"<<endl;
		return 0;
	}

	//Create syslogger
	//cout<<"LoggerManager::CreateSysLogger(): INFO: Allocate syslogger!"<<endl;			
	m_target= eSysLog;
	m_logger= new SysLogger(level,tag,facility,host);
	//cout<<"LoggerManager::CreateSysLogger(): INFO: Initialize syslogger!"<<endl;
	if(m_logger->Init()<0){
		//cout<<"LoggerManager::CreateSysLogger(): ERROR: Failed to init syslogger!"<<endl;
		return -1;
	}

	return 0;
		
}//close CreateSysLogger()


int LoggerManager::DestroySysLogger(){

	//Check if logger was created, if not return
	if(!m_logger){
		return 0;
	}

	//Delete logger
	delete m_logger;
	m_logger= 0;

	return 0;

}//close DestroySysLogger()




//=========================================
//==        SCOPED LOGGER CLASS
//=========================================
ScopedLogger::ScopedLogger(std::string level,std::string prefix,std::string device_name)
	: m_level(level), m_msgprefix(prefix), m_deviceName(device_name) 
{
	//- No op
}

ScopedLogger::~ScopedLogger()
{ 
	//Logging command
	SysLogger* logger= LoggerManager::Instance().GetLogger();
	if(logger) {	
		//cout<<"ScopedLogger::~ScopedLogger(): INFO: Sending log ("<<m_sstream.str()<<" with level "<<m_level<<" and prefix="<<m_msgprefix<<endl;
		logger->Log(m_level, m_sstream.str(), m_msgprefix);			
	}

	//Tango logging
	try {
		Tango::DeviceImpl* ds_impl= Tango::Util::instance(false)->get_device_by_name(m_deviceName);
		log4tango::Level::Value tango_level= TangoUtils::GetTangoLogLevelFromString(m_level);
		if(ds_impl && ds_impl->get_logger()->is_level_enabled(tango_level) ) {
			ds_impl->get_logger()->log(tango_level, m_msgprefix+m_sstream.str());
		}
	}
	catch(Tango::DevFailed& e){
		//cerr<<"ScopedLogger::~ScopedLogger(): ERROR: Failed to log with Tango logger!"<<endl;
		//Tango::Except::print_exception(e);
	}
		
}//close destructor

//=========================================
//==        DEVICE SCOPED LOGGER CLASS
//=========================================
DevScopedLogger::DevScopedLogger(std::string level,std::string prefix,Tango::DeviceImpl* ds_impl)
	: m_level(level), m_msgprefix(prefix)
{
	m_ds_impl= ds_impl;
	if(m_ds_impl) m_deviceName= m_ds_impl->get_name();
	else m_deviceName= "";
}

DevScopedLogger::~DevScopedLogger()
{ 			
	//SysLog logging command
	SysLogger* logger= LoggerManager::Instance().GetLogger();
	if(logger) {
		//cout<<"DevScopedLogger::~DevScopedLogger(): INFO: Sending log ("<<m_sstream.str()<<" with level "<<m_level<<" and prefix="<<m_msgprefix<<endl;			
		logger->Log(m_level, m_sstream.str(), m_msgprefix);			
	}
	else{
		//cout<<"DevScopedLogger::~DevScopedLogger(): ERROR: No logger instance can be retrieved!"<<endl;
	}
	
	//Tango logging
	log4tango::Level::Value tango_level= TangoUtils::GetTangoLogLevelFromString(m_level);
	if(m_ds_impl && m_ds_impl->get_logger()->is_level_enabled(tango_level) ) {
		m_ds_impl->get_logger()->log(tango_level, m_msgprefix+m_sstream.str());
	}
		
}//close destructor

}//close namespace
