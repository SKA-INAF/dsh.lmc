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
* @file NagiosListenerThread.cc
* @class NagiosListenerThread
* @brief NagiosListenerThread
*
* NagiosListenerThread
* @author S. Riggi
* @date 15/01/2017
*/

#include <NagiosListenerThread.h>
#include <LMCMonitor.h>

#include <SocketUtils.h>
#include <Logger.h>
#include <CodeUtils.h>

#include <tango.h>

//## Boost
#include <boost/regex.hpp>

//## Standard headers
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctime>
#include <stdexcept>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

#include <map>
#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <chrono>
#include <regex>

using namespace std;


namespace LMCMonitor_ns {

NagiosListenerThread::NagiosListenerThread (LMCMonitor* dev,std::string query_handler_path) 
	: omni_thread() {

  m_dev = dev;
	m_nagiosQueryHandlerPath= query_handler_path;
	m_nagiosAttrPollPeriod= 60000;//ms
	m_nagiosAttrPeriodicEventPeriod= 60000;//ms
	m_nagiosAttrArchiveEventPeriod= 60000;//ms
	m_SocketClient= 0;

}//close constructor
  

NagiosListenerThread::~NagiosListenerThread(){
	
	//## Deleting socket client
	if(m_SocketClient){
		__DEBUG_LOG(m_dev,"Deleting unix socket client...");
		delete m_SocketClient;
		m_SocketClient= 0;
	}

}//close destructor


//+------------------------------------------------------------------
/**
  *	method:	run()
  *
  *	description:	
  *
  */
//+------------------------------------------------------------------
void* NagiosListenerThread::run_undetached(void*) {

  __INFO_LOG(m_dev,"Starting Nagios listener thread...");

	//## Create the UNIX socket client
	__INFO_LOG(m_dev,"Creating client to UNIX socket "<<m_nagiosQueryHandlerPath<<" ...");
	if(!m_SocketClient) m_SocketClient= new Utils_ns::SocketClient(m_dev,m_nagiosQueryHandlerPath,false);
	
	//## Init socket client 
	int nInitAttempts= 5;
	bool isSocketConnected= false;
	for(int i=0;i<nInitAttempts;i++){

		if(m_SocketClient->Init()<0){
			__ERROR_LOG(m_dev,"Failed to connect to UNIX socket "<<m_nagiosQueryHandlerPath<<" (attempt no. "<<i+1<<")...");

			//Close and retry
			m_SocketClient->Close();
			continue;
		}
		isSocketConnected= true;
		break;
	}//end loop

	if(!isSocketConnected){
		__ERROR_LOG(m_dev,"Failed to initialize socket after "<<nInitAttempts<<" attempts, terminate Nagios listener thread!");
		return 0;
	}

	//## Init buffer
	int bufsize= 8192;
	char buffer[8192];
	int timeout= 2;
	int sleep_time_after_failure= 3; 

	bool isSubscribedToHostChecks= false;
	bool isSubscribedToServiceChecks= false;
	Utils_ns::SBuffer serviceCheckSubscribeCmd (
		"@nerd subscribe servicechecks\0",
		30
	);
	Utils_ns::SBuffer hostCheckSubscribeCmd (
		"@nerd subscribe hostchecks\0",
		27
	);
	

	//## Read loop
	while(1) {

		//================================================
		//==  Check if thread stop flag is set
		//================================================
		if(m_dev->IsNagiosThreadStopped()) {
      __INFO_LOG(m_dev,"Stop thread signal received, ending Nagios listener thread!");
			break; 
    }

		//=================================================
		//==   Subscribe to Nagios host & service checks
		//=================================================
		//## Write to UNIX socket the host subscribe command 
		if(!isSubscribedToHostChecks){
			__INFO_LOG(m_dev,"Subscribing to host checks in Nagios NERD ...");
			ssize_t recvBytes= m_SocketClient->Write((hostCheckSubscribeCmd.data).c_str(),hostCheckSubscribeCmd.size);
		
			if(recvBytes>0){
				isSubscribedToHostChecks= true;
				__INFO_LOG(m_dev,"Subscribed to Nagios host checks");
			}
			else{
				__ERROR_LOG(m_dev,"Failed to subscribe to Nagios host checks, will retry later...");
				if(recvBytes!=Utils_ns::SocketClient::eConnectionTimedOut){
					__ERROR_LOG(m_dev,"Failed to write to socket when subscribing to host check, close and reopening socket...");
					isSubscribedToServiceChecks= false;
					isSubscribedToHostChecks= false;
					m_SocketClient->Reset();
				}
			}//close if 

			//Sleep a bit and retry 
			omni_thread::sleep(sleep_time_after_failure);
			continue;
		
		}//close if !isSubscribedToHostChecks
	
		//## Write to UNIX socket the service subscribe command 
		if(!isSubscribedToServiceChecks){
			__INFO_LOG(m_dev,"Subscribing to service checks in Nagios NERD ...");
			ssize_t recvBytes= m_SocketClient->Write((serviceCheckSubscribeCmd.data).c_str(),serviceCheckSubscribeCmd.size);
		
			if(recvBytes>0){
				isSubscribedToServiceChecks= true;
				__INFO_LOG(m_dev,"Subscribed to Nagios service checks");
			}
			else{
				if(recvBytes!=Utils_ns::SocketClient::eConnectionTimedOut){
					__ERROR_LOG(m_dev,"Failed to write to socket when subscribing to service check, close and reopening socket...");
					isSubscribedToServiceChecks= false;
					isSubscribedToHostChecks= false;
					m_SocketClient->Reset();
				}
			}//close if 

			//Sleep a bit and retry 
			omni_thread::sleep(sleep_time_after_failure);
			continue;

		}//close if !isSubscribedToHostChecks


		//=========================================
		//===      Read Unix Nagios socket
		//=========================================
		ssize_t nbytes= m_SocketClient->Read(buffer,bufsize,timeout);
		if(nbytes<0){
			
			if(nbytes!=Utils_ns::SocketClient::eConnectionTimedOut){
				__ERROR_LOG(m_dev,"Failed to read from socket, close and reopening socket...");
				isSubscribedToServiceChecks= false;
				isSubscribedToHostChecks= false;
				m_SocketClient->Reset();
			}
			else{
				__DEBUG_LOG(m_dev,"Timeout reached while reading from socket, no data available ...");
			}
			continue;
		}//close if

		__DEBUG_LOG(m_dev,"Received message (n="<<nbytes<<"): "<< buffer);
		
		//=========================================
		//===    Process received message 
		//=========================================
		std::string msg= "";
		try{
			msg= std::string(buffer,nbytes);
		}
		catch (std::exception const &e) {
  		__ERROR_LOG(m_dev,"C++ exception occurred while setting event string data (e="<<e.what()<<")!");
			continue;
  	}
  	catch (...) {
    	__ERROR_LOG(m_dev,"Unknown exception occurred while setting event string data!");
			continue;
 	 	}
		
		if(ProcessEvent(msg)<0){
			__ERROR_LOG(m_dev,"Failed to process event received from Nagios NERD system...");
			continue;
		}

	}//end foreever loop
	
	      
	__INFO_LOG(m_dev,"End Nagios listener thread...");


  return 0;

}//close run()


int NagiosListenerThread::ProcessEvent(std::string& msg){

	//localhost from 0 -> 0: PING OK - Packet loss = 0%, RTA = 0.03 ms|rta=0.034000ms;3000.000000;5000.000000;0.000000 pl=0%;80;100;0

	long poll_period= m_nagiosAttrPollPeriod;
	long periodic_event_period= m_nagiosAttrPeriodicEventPeriod;
	long archive_event_period= m_nagiosAttrArchiveEventPeriod;

	__DEBUG_LOG(m_dev,"Processing Nagios msg: "<<msg);
	boost::regex serviceCheckExpr("(.*?);(.*?) from ([0-9]+) -> ([0-9]+): (.*)$");
	boost::regex hostCheckExpr("(.*?) from ([0-9]+) -> ([0-9]+): (.*)$");
	boost::regex cmdErrorExpr("([0-9]+): (.*)$");
	boost::match_results<std::string::const_iterator> results;

	//=========================================
	//===    SERVICE CHECK DATA? 
	//=========================================
	if( boost::regex_match(msg, results, serviceCheckExpr) ){//Service Check data received
		__DEBUG_LOG(m_dev,"Service check data received, parsing data fields...");
		std::string parsedData= results[0].str();
		std::string serviceHost= results[1].str();
		std::string serviceName= results[2].str();
		std::string serviceState_old= results[3].str();
		std::string serviceState= results[4].str();
		std::string serviceCheckData= results[5].str();
		bool hasChangedState= (serviceState != serviceState_old);	
		int serviceStateCode= std::stoi(serviceState); 		
		std::string serviceStateStringCode= "";
		ServiceCheckToStr(serviceStateStringCode,serviceStateCode);
		
		std::vector<std::string> serviceCheckDynAttrValues {
			serviceStateStringCode,
			serviceHost,
			serviceCheckData
		};

		__INFO_LOG(m_dev,"Service Check data: {host="<<serviceHost<<", service="<<serviceName<<", state("<<serviceState_old<<"-->"<<serviceState<<"), info="<<serviceCheckData<<"}");
		
		//If state changed, create and update dyn attr
		//if(hasChangedState){
			std::string serviceHost_name= serviceHost;
			Utils_ns::CodeUtils::TransformToCamelCase(serviceHost_name);

			std::string dyn_attr_name= serviceName;
			//Utils_ns::CodeUtils::TransformToCamelCase(dyn_attr_name);
			//std::string dyn_attr_full_name= std::string("serviceStatus_") + dyn_attr_name;
			std::string dyn_attr_full_name= serviceHost_name + dyn_attr_name; //+ std::string("Status");

			//Create and fill service status attr
			if(CreateNagiosCheckDynAttr(dyn_attr_full_name,poll_period,periodic_event_period,archive_event_period,false)==0){
				
				//Update dyn attr value
				__INFO_LOG(m_dev,"Updating Nagios dyn attr "<<dyn_attr_full_name<<" value...");
				bool emit_event= false;
				if(hasChangedState) emit_event= true;
				Tango::AttrQuality attr_quality= Tango::ATTR_VALID;
				if(m_dev->SetDynAttrValue<std::string>(serviceStateStringCode,dyn_attr_full_name,attr_quality,emit_event)<0){
					__WARN_LOG(m_dev,"Failed to update attr "<<dyn_attr_full_name<<"!");
				}
			
			}//close if
			else{
				__WARN_LOG(m_dev,"Failed to create Nagios check dyn attr "<<dyn_attr_full_name<<"!");
			} 

			//Create and fill service status with full info
			std::string dyn_attr_full_name_fullinfo= dyn_attr_full_name + std::string("Info");
			if(CreateNagiosCheckDynAttr(dyn_attr_full_name_fullinfo,poll_period,periodic_event_period,archive_event_period,true)==0){
				
				//Update dyn attr value
				__INFO_LOG(m_dev,"Updating Nagios dyn attr "<<dyn_attr_full_name_fullinfo<<" value...");
				bool emit_event= false;
				if(hasChangedState) emit_event= true;
				Tango::AttrQuality attr_quality= Tango::ATTR_VALID;
				if(m_dev->SetDynAttrValue<std::string>(serviceCheckDynAttrValues,dyn_attr_full_name_fullinfo,attr_quality,emit_event)<0){
					__WARN_LOG(m_dev,"Failed to update attr "<<dyn_attr_full_name<<"!");
				}
			
			}//close if
			else{
				__WARN_LOG(m_dev,"Failed to create Nagios check dyn attr "<<dyn_attr_full_name_fullinfo<<"!");
			} 	

		//}//close if state changed

	}//close if service check

	//=========================================
	//===    HOST CHECK DATA? 
	//=========================================

	else if( boost::regex_match(msg, results, hostCheckExpr) ){//Host Check data received
		__DEBUG_LOG(m_dev,"Host check data received, parsing data fields...");
		std::string parsedData= results[0].str();
		std::string Host= results[1].str();	
		std::string hostState_old= results[2].str();
		std::string hostState= results[3].str();
		std::string hostCheckData= results[4].str();		
		bool hasChangedState= (hostState != hostState_old);
		int hostStateCode= std::stoi(hostState);
		std::string hostStateStringCode= "";
		HostCheckToStr(hostStateStringCode,hostStateCode);

		std::vector<std::string> hostCheckDynAttrValues {
			hostStateStringCode,
			Host,
			hostCheckData
		};

		__INFO_LOG(m_dev,"Host check data: {host="<<Host<<", state("<<hostState_old<<"-->"<<hostState<<"), info="<<hostCheckData<<"}");
		
		//If state changed, create and update dyn attr
		//if(hasChangedState){
			std::string dyn_attr_name= Host;
			Utils_ns::CodeUtils::TransformToCamelCase(dyn_attr_name);
			//std::string dyn_attr_full_name= std::string("hostStatus_") + dyn_attr_name;
			std::string dyn_attr_full_name= dyn_attr_name + std::string("HostStatus");

			//Create and fill Nagios status attr
			__DEBUG_LOG(m_dev,"Creating Nagios dyn attr "<<dyn_attr_full_name<<"...");
			if(CreateNagiosCheckDynAttr(dyn_attr_full_name,poll_period,periodic_event_period,archive_event_period,false)==0){			
				//Update dyn attr value
				__DEBUG_LOG(m_dev,"Updating Nagios dyn attr "<<dyn_attr_full_name<<" value...");
				bool emit_event= false;
				if(hasChangedState) emit_event= true;
				Tango::AttrQuality attr_quality= Tango::ATTR_VALID;
				if(m_dev->SetDynAttrValue<std::string>(hostStateStringCode,dyn_attr_full_name,attr_quality,emit_event)<0){
					__WARN_LOG(m_dev,"Failed to update attr "<<dyn_attr_full_name<<"!");
				}
			}//close if
			else{
				__WARN_LOG(m_dev,"Failed to create Nagios check dyn attr "<<dyn_attr_full_name<<"!");
			} 

			//Create Nagios attr with full status info
			std::string dyn_attr_full_name_fullinfo= dyn_attr_full_name + std::string("Info");
			__DEBUG_LOG(m_dev,"Creating Nagios dyn attr "<<dyn_attr_full_name_fullinfo<<"...");
			if(CreateNagiosCheckDynAttr(dyn_attr_full_name_fullinfo,poll_period,periodic_event_period,archive_event_period,true)==0){			
				//Update dyn attr value
				__DEBUG_LOG(m_dev,"Updating Nagios dyn attr "<<dyn_attr_full_name_fullinfo<<" value...");
				bool emit_event= false;
				if(hasChangedState) emit_event= true;
				Tango::AttrQuality attr_quality= Tango::ATTR_VALID;
				if(m_dev->SetDynAttrValue<std::string>(hostCheckDynAttrValues,dyn_attr_full_name_fullinfo,attr_quality,emit_event)<0){
					__WARN_LOG(m_dev,"Failed to update attr "<<dyn_attr_full_name_fullinfo<<"!");
				}
			}//close if
			else{
				__WARN_LOG(m_dev,"Failed to create Nagios check dyn attr "<<dyn_attr_full_name_fullinfo<<"!");
			} 

		//}//close if state changed

	}//close else if Host check

	//=========================================
	//===    ERROR MESSAGE 
	//=========================================

	else if(boost::regex_match(msg, results, cmdErrorExpr)){
		//__WARN_LOG(m_dev,"Error message received, parsing data fields...");
		std::string parsedData= results[0].str();
		int ErrorCode= atoi(results[1].str().c_str());
		std::string ErrorMsg= results[2].str();
		__WARN_LOG(m_dev,"Error message data: {err_code="<<ErrorCode<<", msg="<<ErrorMsg<<"}");
		
	}//close else if Error

	//=========================================
	//===    UNKNOWN MESSAGE 
	//=========================================

	else{
		//Other data received, send them back to dispatcher
		__ERROR_LOG(m_dev,"Unknown message received from Nagios NERD (msg="<<msg<<")");
		return -1;
	}


	return 0;

}//close ProcessEvent()


int NagiosListenerThread::CreateNagiosCheckDynAttr(std::string attr_name,long poll_period,long periodic_event_period,long archive_event_period,bool isFullInfoAttr){

	//Check if dynamic attribute manager is instantiated
	if(!m_dev->HasDynAttrManagerInstance()){
		__ERROR_LOG(m_dev,"Dynamic attribute manager was not instantiated yet!");
		return -1;
	}

	//Check if attribute already exist
	if(Utils_ns::TangoUtils::HasAttribute(m_dev,attr_name)){
		__DEBUG_LOG(m_dev,"Attribute "<<attr_name<<" already present in device, nothing to be done...");
		return 0;
	}
	__DEBUG_LOG(m_dev,"Creating Nagios dyn attr "<<attr_name<<"...");

	//## If full info attr create a spectrum attr
	Tango::Attr* attr= 0;
	if(isFullInfoAttr){
		//Create a Tango attribute: format=SPECTRUM, type=STRING, rwtype=READ
		int maxDimX= 4;
		attr= new Tango::SpectrumAttr(attr_name.c_str(),Tango::DEV_STRING,Tango::READ,maxDimX,Tango::EXPERT);
	}
	else{
		//Create a Tango attribute: format=SCALAR, type=STRING, rwtype=READ
		attr= new Tango::Attr(attr_name.c_str(),Tango::DEV_STRING,Tango::OPERATOR,Tango::READ);
	}

	//Set polling?
	if(poll_period>0) attr->set_polling_period(poll_period);

	//Create attr properties
	Tango::MultiAttrProp<Tango::DevString>* prop= new Tango::MultiAttrProp<Tango::DevString>;	
	prop->min_value= Tango::AlrmValueNotSpec;
	prop->max_value= Tango::AlrmValueNotSpec;
	prop->min_alarm= Tango::AlrmValueNotSpec;
	prop->max_alarm= Tango::AlrmValueNotSpec;
	prop->min_warning= Tango::AlrmValueNotSpec;
	prop->max_warning= Tango::AlrmValueNotSpec;
	prop->delta_t= Tango::AlrmValueNotSpec;
	prop->delta_val= Tango::AlrmValueNotSpec;
	prop->rel_change= Tango::AlrmValueNotSpec;
	prop->abs_change= Tango::AlrmValueNotSpec;
	prop->archive_rel_change= Tango::AlrmValueNotSpec;
	prop->archive_abs_change= Tango::AlrmValueNotSpec;
	
	if(periodic_event_period>0) prop->event_period= periodic_event_period;
	if(archive_event_period>0) prop->archive_period= archive_event_period;
	
	//Create a device attr
	Utils_ns::DeviceAttr* device_attr= new Utils_ns::DeviceAttrImpl<Tango::DevString>(attr,prop);
	if(poll_period>0) device_attr->SetPolled(true);
	device_attr->SetGeneratorModel( new Utils_ns::AttrConstModel<std::string>(std::string()) );

	std::string initValue("");
	dynamic_cast<Utils_ns::DeviceAttrImpl<Tango::DevString>*>(device_attr)->SetInitValue(initValue);

	//Add dyn attribute in device
	try{	
		m_dev->CreateAndInitDynAttr<std::string,Tango::DevString>(device_attr);
	}
	catch(Tango::DevFailed &e){
		Tango::Except::print_exception(e);
		__ERROR_LOG(m_dev,"Adding attr "<<attr_name<<" failed!");
		return -1;
	}
	catch(std::exception& e){
		__ERROR_LOG(m_dev,"C++ exception occurred while adding attr "<<attr_name<<" (err="<<e.what()<<")!");
		return -1;
	}
	catch(...){
		__ERROR_LOG(m_dev,"Unknown exception occurred while adding attr "<<attr_name<<"!");
		return -1;
	}

	return 0;

}//close CreateNagiosCheckDynAttr()

int NagiosListenerThread::HostCheckToStr(std::string& code_str,int code){
	
	code_str= "";
	switch(code){
		case UP:
			code_str= "UP";
			break;
		case DOWN:
			code_str= "DOWN";
			break;
		case UNREACHABLE:
			code_str= "UNREACHABLE";
			break;
		default:
			return -1;
			break;
	}//close switch

	
	return 0;

}//close HostCheckToStr()

		
int NagiosListenerThread::ServiceCheckToStr(std::string& code_str,int code){

	code_str= "";
	switch(code){
		case OK:
			code_str= "OK";
			break;
		case WARNING:
			code_str= "WARNING";
			break;
		case CRITICAL:
			code_str= "CRITICAL";
			break;
		default:
			code_str= "UNKNOWN";
			break;
	}//close switch

	return 0;

}//close ServiceCheckToStr()

}//close namespace

