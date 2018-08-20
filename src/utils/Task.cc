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
* @file Task.cc
* @class Task
* @brief Task class
*
* Task class
* @author S. Riggi
* @date 15/01/2016
*/

#include <Task.h>

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

#include <tango_const.h>

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


Task::Task()
{
	m_dev= 0;
	useUTC= false;
	priority= eMEDIUM_PRIORITY;
	cmd_name= ""; 

	Init();
	GenerateId();	
	
}


Task::Task(Tango::DeviceImpl* dev,std::string name,int priority_level, bool useUTCTime)
	: cmd_name(name), priority(priority_level), useUTC(useUTCTime), m_dev(dev)
{
	//Initialize 
	Init();
	GenerateId();

}//close constructor 



Task::~Task()
{

}//close destructor


// ============================================================================
//                 PUBLIC METHODS
// ============================================================================
void Task::SetTaskArgin(CORBA::Any& argin,int data_type,int data_format){

	//Set task argin data
	argin_type= data_type;
	argin_format= data_format;
	cmd_argin= argin;
	hasArgin= true;

}//close SetTaskArgin()

int Task::SetActivationTime(yat::Time& t_start){
			
	//Check given tstart wrt to now
	yat::Time now;
	now.set_current(useUTC);
	if(t_start<now) {
		__WARN_LOG(m_dev,"Failed to set activation time (reason: given tstart is in the past)!");
		return -1;//tstart is in the past!
	}

	//Set activation time & timeout
	activation_time= t_start;		
	SetActivationTimeout(t_start);
	hasActivationTime= true;		
	
	//Update expiration time & timeout
	expiration_time= t_start;
	expiration_time+= static_cast<double>(default_max_duration/1000);
	SetExpirationTimeout(expiration_time);
	hasExpirationTime= true;	

	return 0;
		
}//close SetActivationTime()

int Task::SetActivationTimeDelay(long int delay_time){

	//Check delay time
	if(delay_time<0) return -1;

	//Set activation time = now + delay
	activation_time.set_current(useUTC);//set to now
	activation_time+= static_cast<double>(delay_time/1000);//tstart_delay shall be given in ms
	
	return SetActivationTime(activation_time);
	
}//close SetActivationTimeDelay()


int Task::SetExpirationTime(yat::Time& t_stop){
			
	//Check given tstop wrt to now
	yat::Time now;
	now.set_current(useUTC);
	if(t_stop<now) {
		__WARN_LOG(m_dev,"Failed to set expiration time (reason: given tstop is in the past)!");	
		return -1;//tstop is in the past!
	}

	//Check tstop wrt to tstart (if present)
	if(hasActivationTime && t_stop<=activation_time) {
		__WARN_LOG(m_dev,"Failed to set expiration time (reason: given tstop is before tstart)!");	
		return -1;//tstop is before tstart
	}

	//Set expiration time	& timeout		
	expiration_time= t_stop;
	SetExpirationTimeout(t_stop);
	hasExpirationTime= true;	
	
	return 0;

}//close SetExpirationTime()

int Task::SetMaxDuration(long int duration,int time_ref_mode, yat::Time time_ref){

	//Check duration time
	if(duration<0) return -1;
	
	//Update max_duration var
	max_duration= duration;
	
	//Set expiration time
	if(hasActivationTime) expiration_time= activation_time;
	else {
		if(time_ref_mode==eTIME_NOW) expiration_time.set_current(useUTC);
		else if(time_ref_mode==eCREATION_TIME) expiration_time= creation_time;
		else if(time_ref_mode==eEXTERNAL_TIME) expiration_time= time_ref;
		else {
			__WARN_LOG(m_dev,"Invalid time ref mode ("<<time_ref_mode<<") specified!");
			return -1;
		}
	}
	expiration_time+= static_cast<double>(duration/1000);	

	SetExpirationTimeout(expiration_time);
	hasExpirationTime= true;

	return 0;

}//close SetMaxDuration()

int Task::SetTimeDelayAndDuration(long int delay_time,long int duration,int time_ref_mode, yat::Time time_ref){

	//Check delay time & duration
	if(delay_time<0 || duration<0) return -1;

	//Set activation time (= now + delay) & timeout
	if(time_ref_mode==eTIME_NOW) activation_time.set_current(useUTC);	
	else if(time_ref_mode==eCREATION_TIME) activation_time= creation_time;
	else if(time_ref_mode==eEXTERNAL_TIME) activation_time= time_ref;
	else {
		__WARN_LOG(m_dev,"Invalid time ref mode ("<<time_ref_mode<<") specified!");
		return -1;
	}
	activation_time+= static_cast<double>(delay_time/1000);//tstart_delay shall be given in ms
	SetActivationTimeout(activation_time);
	hasActivationTime= true;	

	//Set expiration time (=tstart+max_duration) & timeout
	max_duration= duration;
	expiration_time= activation_time;
	expiration_time+= static_cast<double>(duration/1000);
	SetExpirationTimeout(expiration_time);
	hasExpirationTime= true;


	return 0;

}//close SetTimeDelayAndDuration()


int Task::EncodeToJson(Json::Value& root){

	//Set mandatory fields
	root["id"]= id;
	root["name"]= cmd_name;	
	root["priority"]= priority;
	root["status"]= status;
	root["status_info"]= status_info;
	root["is_utc"]= useUTC;
	root["creation_time"]= creation_time.to_ISO8601_ms();
	if(hasActivationTime) root["activation_time"]= activation_time.to_ISO8601_ms();
	else root["activation_time"]= std::string("");
	if(hasExpirationTime) root["expiration_time"]= expiration_time.to_ISO8601_ms();
	else root["expiration_time"]= std::string("");

	root["is_asynch"]= is_asynch;
	root["status_attr_name"]= status_attr_name;

	root["seq_id"]= sequence_id;
	root["seq_name"]= sequence_name;
	

	//Set argument field
	if(hasArgin){
		Json::Value argObj;
		argObj["type"]= argin_type;
		argObj["format"]= argin_format;
		
		if(TangoUtils::CorbaAnyToJsonObj(argObj,cmd_argin,argin_type,argin_format)<0){
			__WARN_LOG(m_dev,"Failed to convert CORBA::Any to Json object!");
			return -1;
		}
		root["arg"]= argObj;
	}

	return 0;

}//close EncodeToJson()

int Task::EncodeToJsonString(std::string& jsonString){

	Json::Value jsonObj;
	if(EncodeToJson(jsonObj)<0){
		__WARN_LOG(m_dev,"Failed to encode Task to json object!");
		return -1;
	}

	if(CodeUtils::JsonToString(jsonString,jsonObj,true)<0){
		__WARN_LOG(m_dev,"Failed to encode json object to string!");
		return -1;
	}

	return 0;

}//close EncodeToJsonString()


void Task::SetAsynchTask(std::string attr_name){

	is_asynch= true;
	status_attr_name= attr_name;

}//close SetAsynchTask()


CORBA::Any* Task::Run(){

	//## Check dev
	if(!m_dev){
		std::stringstream ss;
		ss<<"Null ptr to device (this should not occur, check if you correctly initialized this Task object)!";
		_THROW_TANGO_EXCEPTION("RUN_FAILED",ss.str());
	}

	//## Get task data encoded in json string
	std::string json_str= "";	
	if(EncodeToJsonString(json_str)<0){
		std::stringstream ss;
		ss<<"Failed to encode task data to json string!";
		__ERROR_LOG(m_dev,ss.str());
		_THROW_TANGO_EXCEPTION("RUN_FAILED",ss.str());
	}

	//## Convert json string to CORBA::Any
	CORBA::Any argin;
	try{
		argin <<= json_str.c_str();
	}
	catch(...){
		std::stringstream ss;
		ss<<"Exception while encoding json string to CORBA::Any argin!";
		__ERROR_LOG(m_dev,ss.str());
		throw;
	}

	//## Execute handler command
	__DEBUG_LOG(m_dev,"Running cmd "<<cmd_name<<" with arg data= "<<json_str);
	CORBA::Any* argout= 0;
	try {
		argout= m_dev->get_device_class()->command_handler(m_dev,cmd_name,argin);
	}	
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		__ERROR_LOG(m_dev,"Tango exception occurred while executing task "<<cmd_name<<" (check cmd name, is_allowed, argin, etc...)!");
		throw;
	}
	catch(std::exception& e){
		__ERROR_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while executing task "<<cmd_name<<" (check cmd name, is_allowed, argin, etc...)!");
		throw;
	}
	catch(...){
		__ERROR_LOG(m_dev,"Unknown exception occurred while executing task "<<cmd_name<<" (check cmd name, is_allowed, argin, etc...)!");
		throw;
	}

	return argout;

}//close Run()


// ============================================================================
//                 PRIVATE METHODS
// ============================================================================
void Task::Init(){

	//Set creation time
	SetCreationTime();
		
	//Timestamp vars
	activation_time.set_empty_unix();
	exp_max_activation_time.set_empty_unix();
	exp_max_expiration_time.set_empty_unix();
	activation_timeout= 0;
	expiration_timeout= 0;
	hasActivationTime= false;	
	
	//Set default expiration time
	default_max_duration= 30000;//in ms
	max_duration= default_max_duration;
	expiration_time= creation_time;
	expiration_time+= static_cast<double>(default_max_duration/1000);
	SetExpirationTimeout(expiration_time);//Init and start the expiration timeout
	hasExpirationTime= true;	

	//Args
	hasArgin= false; 
	argin_type= -1;
	argin_format= -1;
	
	//Main properties 
	id= "";
	
	//Cmd exec info
	status= eINIT;
	status_info= "";

	//Asynch task?
	is_asynch= false;
	status_attr_name= "";

	//Sequence pars
	is_in_sequence= false;
	sequence_id= "";
	sequence_name= "";
	sequence_index= 0;
	depend_on_sequence_tasks= false;
	
}//close Init()

void Task::GenerateId(){

	id= CodeUtils::GenerateUUID();

}//close GenerateId()


void Task::InitActivationTimeout(long int delay_time){
			
	//Destroy existing timeout
	if(activation_timeout){
		delete activation_timeout;
		activation_timeout= 0;
	}
			
	//Create a new timeout
	activation_timeout= new yat::Timeout(delay_time,yat::Timeout::TMO_UNIT_MSEC,true);//this starts the timeout
	hasActivationTime= true;

}//close InitActivationTimeout()

void Task::InitExpirationTimeout(long int delay_time){
			
	//Destroy existing timeout
	if(expiration_timeout){
		delete expiration_timeout;
		expiration_timeout= 0;
	}
			
	//Create a new timeout
	expiration_timeout= new yat::Timeout(delay_time,yat::Timeout::TMO_UNIT_MSEC,true);//this starts the timeout
	hasExpirationTime= true;

}//close InitExpirationTimeout()


int Task::SetActivationTimeout(yat::Time& tstart){

	//Compute the timeout value wrt to now
	double timeout= TangoUtils::ComputeYatTimeoutMSec(tstart,useUTC);
	if(timeout<=0){
		return -1;
	}

	//Create a new timeout
	InitActivationTimeout(timeout);

	return 0;

}//close SetActivationTimeout()



int Task::SetExpirationTimeout(yat::Time& tstop){

	//Compute the timeout value wrt to now
	double timeout= TangoUtils::ComputeYatTimeoutMSec(tstop,useUTC);
	if(timeout<=0){
		return -1;
	}

	//Create a new timeout
	InitExpirationTimeout(timeout);

	return 0;

}//close SetExpirationTimeout()


void Task::SetStatus(int _status,std::string _status_info){

	status= _status;
	status_info= _status_info;

}//close SetStatus()

void Task::SetSequenceInfo(std::string seq_id,int seq_index,std::vector<int> dep_task_indexes){

	is_in_sequence= true;
	sequence_id= seq_id;
	sequence_index= seq_index;
	if(dep_task_indexes.empty()){
		depend_on_sequence_tasks= false;
		dep_task_seq_indexes.clear();
	}
	else{
		depend_on_sequence_tasks= true;
		dep_task_seq_indexes= dep_task_indexes;
	}

}//close SetSequenceInfo()


bool Task::IsActivationReached(){

	//If no timeout is present return true
	if(!activation_timeout) return true;

	bool isActivationReached= (
		activation_timeout && 
		activation_timeout->enabled() &&		
		activation_timeout->expired()
	);

	return isActivationReached;

}//close IsActivationReached()

bool Task::IsWaitingForActivation(){

	//If no timeout is present return true
	if(!activation_timeout) return false;

	bool isWaitingActivation= (
		activation_timeout && 
		activation_timeout->enabled() &&		
		!activation_timeout->expired()
	);

	return isWaitingActivation;
	
}//close IsWaitingForActivation()

bool Task::IsExpirationReached(){
	
	//If no timeout is present return true
	if(!expiration_timeout) return false;

	bool isExpirationReached= (
		expiration_timeout && 
		expiration_timeout->enabled() && 
		expiration_timeout->expired()
	);

	return isExpirationReached;

}//close IsExpirationReached()


bool Task::IsRunnable(){

	bool isActivationReached= IsActivationReached();
	bool isExpirationReached= IsExpirationReached();
	bool isQueued= (status == Task::eIDLE);
	bool isRunnable= (
		isQueued &&
		isActivationReached &&
		!isExpirationReached
	);
	DEBUG_LOG("Task "<<cmd_name<<": isActivationReached? "<<isActivationReached<<", isExpirationReached? "<<isExpirationReached<<", isQueued? "<<isQueued<<", isRunnable? "<<isRunnable);

	return isRunnable;

}//close IsRunnable()


bool Task::DependOnTask(TaskPtr aTask){
	
	//Return false if task is not marked to depend or if tasks are in different sequences
	if(!aTask) return false;
	if(!depend_on_sequence_tasks) return false;
	if(sequence_id!= aTask->sequence_id || sequence_name!= aTask->sequence_name) return false;

	//Check if given task index is present in dep index
	int pos= -1;
	return CodeUtils::FindItem(dep_task_seq_indexes, aTask->sequence_index,pos);

}//close DependOnTask()

// ============================================================================
//                 TASK UTILS CLASS
// ============================================================================

int TaskUtils::ParseFromJson(TaskData& task_data,Tango::DeviceImpl* dev,Json::Value& root){

	if(!dev) return -1;

	//Check if empty json object
	if(root.isNull()) {
		__WARN_LOG(dev,"Empty/null json tree given!");
		return -1;
	}

	//## Extract fields
	//--> id
	if(!root.isMember("id")){
		__WARN_LOG(dev,"Missing 'id' field in parsed data!");
		return -1;
	}
	task_data.id= root["id"].asString();

	//--> name
	if(!root.isMember("name")){
		__WARN_LOG(dev,"Missing 'name' field in parsed data!");
		return -1;
	}
	task_data.name= root["name"].asString();

	//--> priority
	if(!root.isMember("priority")){
		__WARN_LOG(dev,"Missing 'priority' field in parsed data!");
		return -1;
	}
	task_data.priority= root["priority"].asInt();
	
	//--> status
	if(!root.isMember("status")){
		__WARN_LOG(dev,"Missing 'status' field in parsed data!");
		return -1;
	}
	task_data.status= root["status"].asInt();

	
	//--> is_utc
	if(!root.isMember("is_utc")){
		__WARN_LOG(dev,"Missing 'is_utc' field in parsed data!");
		return -1;
	}
	task_data.is_utc= root["is_utc"].asBool();
	
	//--> creation_time
	if(!root.isMember("creation_time")){
		__WARN_LOG(dev,"Missing 'creation_time' field in parsed data!");
		return -1;
	}
	task_data.creation_time= root["creation_time"].asString();
	
	//--> activation_time
	if(root.isMember("activation_time")){
		task_data.has_activation_time= true;
		task_data.activation_time= root["activation_time"].asString();
	}
	else {
		task_data.has_activation_time= false;
		task_data.activation_time= "";
	}

	//--> expiration_time
	if(root.isMember("expiration_time")){
		task_data.has_expiration_time= true;
		task_data.expiration_time= root["expiration_time"].asString();
	}
	else {
		task_data.has_expiration_time= false;
		task_data.expiration_time= "";
	}

	//--> data
	if(ParseDataFieldFromJson(task_data,dev,root)<0){
		__WARN_LOG(dev,"Failed to parse data field from json!");
		return -1;
	}

	return 0;

}//close ParseFromJson()



int TaskUtils::ParseFromJsonString(TaskData& task_data,Tango::DeviceImpl* dev,std::string& json_string){

	
	//First parse input string msg into Json object
	Json::Value root;//root of json
	Json::Reader reader;
	try {
		reader.parse(json_string,root);
	}
	catch(...){
		__WARN_LOG(dev,"Failed to parse input string to json object!");
		return -1;
	}
	
	return ParseFromJson(task_data,dev,root);

}//close ParseFromJsonString()

int TaskUtils::ParseDataFieldFromJson(TaskData& task_data,Tango::DeviceImpl* dev,Json::Value& root){

	Json::Value arg_root= root["arg"];
	if(arg_root.empty()){
		task_data.has_data= false;
		task_data.data= 0;
		return 0;		
	}

	//Parse data fields
	if(!arg_root.isMember("type") || !arg_root.isMember("format") || !arg_root.isMember("value")){
		__WARN_LOG(dev,"Missing 'type'/'format'/'value' field(s) in parsed data!");
		return -1;
	}
	
	//Extract data
	int data_type= arg_root["type"].asInt();
	//int data_format= arg_root["format"].asInt();
	
	Json::Value data_root= arg_root["value"];	
	DataValuePtr data_ptr= nullptr;
	switch (data_type) {
		case Tango::DEV_FLOAT: 
			data_ptr= CreateFloatDataFromJson(data_root,data_type);
			break;
		case Tango::DEV_DOUBLE: 
			data_ptr= CreateFloatDataFromJson(data_root,data_type);
			break;
		case Tango::DEV_LONG: 
			data_ptr= CreateIntDataFromJson(data_root,data_type);
			break;
		case Tango::DEV_SHORT: 
			data_ptr= CreateIntDataFromJson(data_root,data_type);
			break;
		#if TANGO_VERSION_MAJOR >= 9
		case Tango::DEV_ENUM: 
			data_ptr= CreateIntDataFromJson(data_root,data_type);
			break;
		#endif
		case Tango::DEV_STRING: 
			data_ptr= CreateStringDataFromJson(data_root,data_type);
			break;
		case Tango::DEV_BOOLEAN: 
			data_ptr= CreateBoolDataFromJson(data_root,data_type);
			break;
		case Tango::DEV_STATE: 
			data_ptr= CreateIntDataFromJson(data_root,data_type);
			break;
		default:
		{
			std::stringstream ss;
			ss<<"Unsupported data type ("<<data_type<<")!";
			__ERROR_LOG(dev,ss.str());
	    _THROW_TANGO_EXCEPTION("PARSE_DATA_FIELD_FROM_JSON_FAILED",ss.str());
      break;
		}			
	}//close switch
		
	if(!data_ptr){
		__WARN_LOG(dev,"Failed to extract data from json object!");
		return -1;
	}

	task_data.has_data= true;
	task_data.data= data_ptr;

	return 0;

}//close ParseDataFieldFromJson()


DataValuePtr TaskUtils::CreateBoolDataFromJson(Json::Value& root,int data_type){

	if(root.empty()) return 0;

	DataValuePtr data_ptr= 0;
	if(root.isArray()){
		std::vector<Tango::DevBoolean> values;
		for(unsigned int i=0;i<root.size();i++){
			if(!root[i].isBool()) return 0;
			values.push_back( root[i].asBool() );
		}
		data_ptr= std::make_shared<SpectrumDataValue<Tango::DevBoolean>>(data_type,values);
	}
	else {
		if(!root.isBool()) return 0;
		Tango::DevBoolean value= root.asBool();
		data_ptr= std::make_shared<ScalarDataValue<Tango::DevBoolean>>(data_type,value);
	}
	
	return data_ptr;

}//close CreateBoolDataFromJson()


DataValuePtr TaskUtils::CreateFloatDataFromJson(Json::Value& root,int data_type){

	if(root.empty()) return 0;

	DataValuePtr data_ptr= 0;
	if(root.isArray()){
		std::vector<Tango::DevFloat> values;
		for(unsigned int i=0;i<root.size();i++){
			if(!root[i].isNumeric()) return 0;
			values.push_back( root[i].asFloat() );
		}
		data_ptr= std::make_shared<SpectrumDataValue<Tango::DevFloat>>(data_type,values);
	}
	else {
		if(!root.isNumeric()) return 0;
		Tango::DevFloat value= root.asFloat();
		data_ptr= std::make_shared<ScalarDataValue<Tango::DevFloat>>(data_type,value);
	}
	
	return data_ptr;

}//close CreateFloatDataFromJson()

DataValuePtr TaskUtils::CreateDoubleDataFromJson(Json::Value& root,int data_type){

	if(root.empty()) return 0;

	DataValuePtr data_ptr= 0;
	if(root.isArray()){
		std::vector<Tango::DevDouble> values;
		for(unsigned int i=0;i<root.size();i++){
			if(!root[i].isNumeric()) return 0;
			values.push_back( root[i].asDouble() );
		}
		data_ptr= std::make_shared<SpectrumDataValue<Tango::DevDouble>>(data_type,values);
	}
	else {
		if(!root.isNumeric()) return 0;
		Tango::DevDouble value= root.asDouble();
		data_ptr= std::make_shared<ScalarDataValue<Tango::DevDouble>>(data_type,value);
	}
	
	return data_ptr;

}//close CreateDoubleDataFromJson()


DataValuePtr TaskUtils::CreateIntDataFromJson(Json::Value& root,int data_type){

	if(root.empty()) return 0;

	DataValuePtr data_ptr= 0;
	if(root.isArray()){
		if(data_type==Tango::DEV_LONG){
			std::vector<Tango::DevLong> values;
			for(unsigned int i=0;i<root.size();i++){
				if(!root[i].isNumeric()) return 0;
				values.push_back( root[i].asInt() );
			}
			data_ptr= std::make_shared<SpectrumDataValue<Tango::DevLong>>(data_type,values);
		}
		else if(data_type==Tango::DEV_SHORT){
			std::vector<Tango::DevShort> values;
			for(unsigned int i=0;i<root.size();i++){
				if(!root[i].isNumeric()) return 0;
				values.push_back( root[i].asInt() );
			}
			data_ptr= std::make_shared<SpectrumDataValue<Tango::DevShort>>(data_type,values);	
		}
		#if TANGO_VERSION_MAJOR >= 9
		else if(data_type==Tango::DEV_ENUM){
			std::vector<Tango::DevShort> values;
			for(unsigned int i=0;i<root.size();i++){
				if(!root[i].isNumeric()) return 0;
				values.push_back( root[i].asInt() );
			}
			data_ptr= std::make_shared<SpectrumDataValue<Tango::DevShort>>(data_type,values);	
		}
		#endif
		else if(data_type==Tango::DEV_STATE){
			std::vector<Tango::DevState> values;
			for(unsigned int i=0;i<root.size();i++){
				if(!root[i].isNumeric()) return 0;
				values.push_back( static_cast<Tango::DevState>(root[i].asUInt()) );
			}
			data_ptr= std::make_shared<SpectrumDataValue<Tango::DevState>>(data_type,values);	
		}
		else {//invalid type given
			return 0;
		}

	}//close if SPECTRUM
	else {
		if(!root.isNumeric()) return 0;
		if(data_type==Tango::DEV_LONG){
			Tango::DevLong value= root.asInt();
			data_ptr= std::make_shared<ScalarDataValue<Tango::DevLong>>(data_type,value);
		}
		else if(data_type==Tango::DEV_SHORT){
			Tango::DevShort value= root.asInt();
			data_ptr= std::make_shared<ScalarDataValue<Tango::DevShort>>(data_type,value);	
		}
		#if TANGO_VERSION_MAJOR >= 9
		else if(data_type==Tango::DEV_ENUM){
			Tango::DevShort value= root.asInt();
			data_ptr= std::make_shared<ScalarDataValue<Tango::DevShort>>(data_type,value);	
		}
		#endif
		else if(data_type==Tango::DEV_STATE){
			Tango::DevState value= static_cast<Tango::DevState>(root.asUInt());
			data_ptr= std::make_shared<ScalarDataValue<Tango::DevState>>(data_type,value);	
		}
		else{
			return 0;
		}
	}
	
	return data_ptr;

}//close CreateIntDataFromJson()


DataValuePtr TaskUtils::CreateStringDataFromJson(Json::Value& root,int data_type){

	if(root.empty()) return 0;

	DataValuePtr data_ptr= 0;
	if(root.isArray()){
		std::vector<std::string> values;
		for(unsigned int i=0;i<root.size();i++){
			if(!root[i].isString()) return 0;
			values.push_back( root[i].asString() );
		}
		data_ptr= std::make_shared<SpectrumDataValue<std::string>>(data_type,values);
	}
	else {
		if(!root.isString()) return 0;
		std::string value= root.asString();
		data_ptr= std::make_shared<ScalarDataValue<std::string>>(data_type,value);
	}
	
	return data_ptr;

}//close CreateStringDataFromJson()


// ============================================================================
// class: TaskSequence
// ============================================================================
TaskSequence::TaskSequence()
{
	m_dev= 0;
	useUTC= false;
	seq_name= "";
	tasks.clear();
	status= Task::eINIT;
	status_info= "";
	dependency_matrix.clear(); 
	has_device_status_attr= false;
	status_attr_name= "";
	GenerateId();	
	SetCreationTime();
}

TaskSequence::TaskSequence(Tango::DeviceImpl* dev,std::string _sequence_name,bool useUTCTime) 
	: seq_name(_sequence_name), useUTC(useUTCTime), m_dev(dev)
{
	status= Task::eINIT;
	status_info= "";
	has_device_status_attr= false;
	status_attr_name= "";
	tasks.clear(); 
	dependency_matrix.clear(); 
	GenerateId();
	SetCreationTime();
}//close constructor

TaskSequence::~TaskSequence(){

}//close destructor

int TaskSequence::AddTask(TaskPtr task){

	if(!task) return -1;
	if(seq_id=="" || seq_name=="") return -1;
	
	//Add to collection
	tasks.push_back(task);

	//Set sequence name in task
	task->is_in_sequence= true;
	task->sequence_id= seq_id;
	task->sequence_name= seq_name;
	task->sequence_index= tasks.size()-1;

	return 0;

}//close AddTask()

int TaskSequence::SetDevStatusAttr(std::string attr_name){

	if(attr_name=="") return -1;

	//Check if attr name exists in device
	if(!TangoUtils::HasAttribute(m_dev,attr_name)){
		__WARN_LOG(m_dev,"Attribute "<<attr_name<<" does not exist in device, cannot set it as output dev name for task sequence!");
		return -1;
	}	

	has_device_status_attr= true;
	status_attr_name= attr_name;

	return 0;

}//close SetDevStatusAttr()


int TaskSequence::SetTaskStatus(std::string task_name,int status,std::string status_info){

	//Find task with given name
	int index= -1;
	TaskPtr task= FindTask(index,task_name);
	if(!task){
		__WARN_LOG(m_dev,"Cannot find task with name "<<task_name<<" in sequence!");
		return -1;
	}

	//Set task status
	task->SetStatus(status,status_info);

	//Update status of whole sequence
	ComputeStatus();
				
	return 0;

}//close SetTaskStatus()


int TaskSequence::SetAsynchTaskStatus(std::string attr_name,int status,std::string status_info){

	//Find task with given name
	int index= -1;
	TaskPtr task= FindAsynchTaskByWatchedAttrName(index,attr_name);
	if(!task){
		__DEBUG_LOG(m_dev,"No task in sequence matching watched attr "<<attr_name<<", nothing to be done.");
		return 0;
	}

	//Set task status
	task->SetStatus(status,status_info);

	//Update status of whole sequence
	ComputeStatus();
				
	return 0;

}//close SetAsynchTaskStatus()

void TaskSequence::ComputeStatus(){

	bool oneTaskRunning= false;
	bool oneTaskFailed= false;	
	bool oneTaskTimedOut= false;
	bool oneTaskCanceled= false;
	bool oneTaskIdle= false;
	bool oneTaskCompleted= false;
	bool allTaskIdle= true;
	bool allTaskCompleted= true;
	bool allTaskCanceled= true;
	
	__DEBUG_LOG(m_dev,"Computing task sequence status (seq_name="<<seq_name<<" ntasks="<<tasks.size()<<")");

	std::vector<std::string> failedTasks;
	std::vector<std::string> canceledTasks;
	std::vector<std::string> expiredTasks;

	for(unsigned int i=0;i<tasks.size();i++){
		int thisTaskStatus= tasks[i]->status;
		std::string thisTaskName= tasks[i]->cmd_name;
		__DEBUG_LOG(m_dev,"Task no. "<<i+1<<" name="<<thisTaskName<<", status="<<thisTaskStatus);

		if(thisTaskStatus==Task::eIDLE) oneTaskIdle= true;
		else allTaskIdle= false;
		
		if(thisTaskStatus==Task::eRUNNING) oneTaskRunning= true;

		if(thisTaskStatus==Task::eCOMPLETED) oneTaskCompleted= true;	
		else allTaskCompleted= false;
		
		if(thisTaskStatus==Task::eABORTED || thisTaskStatus==Task::eFAILED) {
			oneTaskFailed= true;		
			failedTasks.push_back(thisTaskName);
		}
		if(thisTaskStatus==Task::eEXPIRED) {
			oneTaskTimedOut= true;
			expiredTasks.push_back(thisTaskName);
		}
		if(thisTaskStatus==Task::eCANCELED) {
			oneTaskCanceled= true;
			canceledTasks.push_back(thisTaskName);
		}
		else{
			allTaskCanceled= false;
		}

	}//end loop tasks in sequence

	__DEBUG_LOG(m_dev,"oneTaskRunning="<<oneTaskRunning<<", oneTaskFailed="<<oneTaskFailed<<", oneTaskTimedOut="<<oneTaskTimedOut<<", allTaskIdle="<<allTaskIdle<<", oneTaskIdle="<<oneTaskIdle<<", oneTaskCanceled="<<oneTaskCanceled<<", allTaskCompleted="<<allTaskCompleted<<", oneTaskCompleted="<<oneTaskCompleted);


	//Set sequence status
	status= Task::eUNKNOWN;
	status_info= "Cannot determine task sequence status as one/more task in UNKNOWN state or cannot map global sequence status";

	std::stringstream info_msg;
	info_msg<<status_info;
	if(allTaskCompleted){
		status= Task::eCOMPLETED;
		info_msg.str(std::string());
		info_msg<<"Task sequence "<<seq_name<<" (id="<<seq_id<<") completed with success";
	}
	if(allTaskIdle){
		status= Task::eIDLE;
		info_msg.str(std::string());
		info_msg<<"All tasks in sequence "<<seq_name<<" (id="<<seq_id<<") are queued";
	}
	if(allTaskCanceled){
		status= Task::eCANCELED;
		info_msg.str(std::string());
		info_msg<<"All tasks in sequence "<<seq_name<<" (id="<<seq_id<<") were cancelled/revoked";
	}

	if(oneTaskRunning && !oneTaskFailed && !oneTaskTimedOut){
		status= Task::eRUNNING;
		info_msg.str(std::string());	
		info_msg<<"One/more task in sequence "<<seq_name<<" (id="<<seq_id<<") are running";
	}

	if(oneTaskCompleted && oneTaskIdle && !oneTaskFailed && !oneTaskTimedOut){
		status= Task::eRUNNING;
		info_msg.str(std::string());	
		info_msg<<"One/more task in sequence "<<seq_name<<" (id="<<seq_id<<") are running";
	}
	
	if(oneTaskFailed){
		status= Task::eFAILED;
		std::stringstream ss;
		ss<<"{";
		for(unsigned int i=0;i<failedTasks.size()-1;i++){
			ss<<failedTasks[i]<<", ";
		}
		ss<<failedTasks[failedTasks.size()-1]<<"}";
		info_msg.str(std::string());	
		info_msg<<"One/more tasks "<<ss.str()<<" in sequence "<<seq_name<<" (id="<<seq_id<<") failed in execution";
	}	
	if(oneTaskCanceled){
		status= Task::eCANCELED;
		std::stringstream ss;
		ss<<"{";
		for(unsigned int i=0;i<canceledTasks.size()-1;i++){
			ss<<canceledTasks[i]<<", ";
		}
		ss<<canceledTasks[canceledTasks.size()-1]<<"}";
		info_msg.str(std::string());
		info_msg<<"One/more tasks "<<ss.str()<<" in sequence "<<seq_name<<" (id="<<seq_id<<") were canceled";
	}
	if(oneTaskTimedOut && !oneTaskFailed){
		status= Task::eEXPIRED;
		std::stringstream ss;
		ss<<"{";
		for(unsigned int i=0;i<expiredTasks.size()-1;i++){
			ss<<expiredTasks[i]<<", ";
		}
		ss<<expiredTasks[expiredTasks.size()-1]<<"}";
		info_msg.str(std::string());
		info_msg<<"One/more task "<<ss.str()<<" in sequence "<<seq_name<<" (id="<<seq_id<<") timed-out before execution";
	}

	status_info= info_msg.str();

	__DEBUG_LOG(m_dev,"Task sequence "<<seq_name<<": status="<<status<<", info="<<status_info);

}//close ComputeStatus()

TaskPtr TaskSequence::FindTask(int& index,std::string name){

	index= -1;
	if(tasks.empty()) return nullptr;
	std::vector<TaskPtr>::iterator it = std::find_if(tasks.begin(), tasks.end(), MatchTaskByName(name));
	if (it==tasks.end()) return nullptr;//not found in collection
	index = static_cast<int>(it-tasks.begin());
	return tasks[index];

}//close FindTask()


TaskPtr TaskSequence::FindTask(int& index,std::string id,std::string name){

	index= -1;
	if(tasks.empty()) return nullptr;
	std::vector<TaskPtr>::iterator it = std::find_if(tasks.begin(), tasks.end(), MatchTask(id,name));
	if (it==tasks.end()) return nullptr;//not found in collection
	index = static_cast<int>(it-tasks.begin());
	return tasks[index];

}//close FindTask()


TaskPtr TaskSequence::FindAsynchTaskByWatchedAttrName(int& index,std::string attr_name){
	
	index= -1;
	if(tasks.empty()) return nullptr;
	std::vector<TaskPtr>::iterator it = std::find_if(tasks.begin(), tasks.end(), MatchAsynchTaskByWatchedAttrName(attr_name));
	if (it==tasks.end()) return nullptr;//not found in collection
	index = static_cast<int>(it-tasks.begin());
	return tasks[index];

}//close FindAsynchTaskByWatchedAttrName()


int TaskSequence::SetTaskDependency(std::vector<std::vector<size_t>>& dep_indexes){

	//## Check given index list
	if(dep_indexes.empty() || dep_indexes.size()!=tasks.size()) {
		__WARN_LOG(m_dev,"Invalid size or empty dependency matrix given!");
		return -1;
	}

	//## Check if dependency matrix is correct, e.g. it does not create cyclic dependencies among tasks
	if(HasCyclicDependencies(dep_indexes)){
		__ERROR_LOG(m_dev,"Given dependency matrix is not correct because it has cyclic dependencies among tasks!!!");
		return -1;
	}
	
	//## Set dependency matrix
	dependency_matrix= dep_indexes;

	//Loop over indexes
	for(unsigned int i=0;i<dep_indexes.size();i++){

		//Set task as dependent on others if we have dependency index
		if(dep_indexes[i].size()>0) {
			tasks[i]->depend_on_sequence_tasks= true;
		}
	
		//Reset list of dependent tasks
		(tasks[i]->dep_task_seq_indexes).clear();

		for(unsigned int j=0;j<dep_indexes[i].size();j++){
			size_t dep_index= dep_indexes[i][j];
			if(dep_index==i || dep_index<0 || dep_index>=tasks.size() ){
				__WARN_LOG(m_dev,"Invalid task dependency index given (dep_index="<<dep_index<<"), it cannot be equal to the same task nor exceed number of tasks in sequence!");
				return -1;
			}
			else{
				(tasks[i]->dep_task_seq_indexes).push_back(dep_index);		
			}
		}//end loop task indexes
	}//end loop tasks

	//## Get sorting index of tasks according to given dependencies
	std::vector<size_t> sorting_index;
	if(FindSortingIndexes(sorting_index)<0){
		__ERROR_LOG(m_dev,"Failed to find sorting index according to given dependency matrix!!!");
		return -1;
	}
	

	//Compute maximum activation/expiration time
	//for(unsigned int i=0;i<tasks.size();i++){
	for(unsigned int i=0;i<sorting_index.size();i++){
		size_t index= sorting_index[i];

		bool dependOnOtherTasks= tasks[index]->depend_on_sequence_tasks;
		if(!dependOnOtherTasks) continue;
		
		//Get the maximum expiration time	
		//NB: If dependent task depend also on another task, consider its max exp expiration time!
		
		yat::Time max_expiration_time;
		max_expiration_time.set_empty_unix();//set to epoch origin
		for(unsigned int j=0;j<(tasks[index]->dep_task_seq_indexes).size();j++){
			int dep_index= (tasks[index]->dep_task_seq_indexes)[j];
			bool depend_on_tasks= tasks[dep_index]->depend_on_sequence_tasks;

			yat::Time dep_task_exp_time= tasks[dep_index]->expiration_time;
			if(depend_on_tasks) dep_task_exp_time= tasks[dep_index]->exp_max_expiration_time;

			if(dep_task_exp_time>max_expiration_time) {
				max_expiration_time= dep_task_exp_time;
			}
		}//end loop index dep tasks

		//Set maximum activation time to the max expiration time of dependant task
		bool hasActivation= tasks[index]->hasActivationTime;
		if(hasActivation && max_expiration_time<tasks[index]->activation_time) tasks[index]->exp_max_activation_time= tasks[index]->activation_time;
		else tasks[index]->exp_max_activation_time= max_expiration_time;

		//Set maximum expiration time and update timeout
		tasks[index]->exp_max_expiration_time= tasks[index]->exp_max_activation_time;
		tasks[index]->exp_max_expiration_time+= static_cast<double>(tasks[index]->max_duration/1000);
		tasks[index]->SetExpirationTime(tasks[index]->exp_max_expiration_time);	

	}//end loop tasks in sequence
	
	return 0;

}//close SetTaskDependency()


void TaskSequence::Sorter(int index, std::vector<bool>& visited, std::stack<int>& Stack) {
    	
	// Mark the current node as visited.
  visited[index] = true;
 
  // Recur for all the vertices adjacent to this vertex
  std::vector<size_t>::iterator it;
  for (it= dependency_matrix[index].begin(); it!= dependency_matrix[index].end(); ++it){
  	if (!visited[*it]) Sorter(*it, visited, Stack);
	}//end loop 
 
  // Push current vertex to stack which stores result
  Stack.push(index);

}//close Sorter()

int TaskSequence::Sort() {

	//#######################################################################################
	//## Performs a topological task sorting (TaskSequence is basically a directed graph)
  //#######################################################################################
	//## Compute sorting indexes
	std::vector<size_t> order_index;
	if(FindSortingIndexes(order_index)<0){
		__ERROR_LOG(m_dev,"Failed to compute task sorting index given current dep matrix!");
		return -1;
	}

	//## Reorder tasks according to given index
	//First fill the ordered task list
	std::vector<TaskPtr> tasks_ordered;
	for(unsigned int i=0;i<order_index.size();i++){
		tasks_ordered.push_back( tasks[order_index[i]] );
	}

	//Empty task lists and refill with ordered items
	tasks.clear();
	for(unsigned int i=0;i<tasks_ordered.size();i++){
		tasks.push_back(tasks_ordered[i]);
	}

	return 0;

}//close Sort()

int TaskSequence::FindSortingIndexes(std::vector<size_t>& order_index) {
	
	//#######################################################################################
	//## Performs a topological task sorting (TaskSequence is basically a directed graph)
  //#######################################################################################
	order_index.clear();
	for(unsigned int i=0; i<tasks.size(); i++){
		order_index.push_back(i);
	}

	//Nothing to be done if sequence is empty
	if(tasks.empty()){
		__DEBUG_LOG(m_dev,"Task list is empty, nothing to be sorted...");
		return 0;
	}
	
	//Nothing to be done if no dependency info are set
	if(dependency_matrix.empty()){
		__DEBUG_LOG(m_dev,"No dependency matrix is set among tasks, nothing to be sorted...");
		return 0;
	}

	//## Init data
	std::stack<int> Stack;
	std::vector<bool> visited(tasks.size(),false);//mark all tasks as non visited

  //## Call the recursive helper function to store Topological
  //## Sort starting from all vertices one by one
  for(unsigned int i=0; i<tasks.size(); i++){
  	if(!visited[i]) Sorter(i, visited, Stack);
 	}

  //## Record order indexes from stack content
	size_t counter= order_index.size()-1;
  while (!Stack.empty()) {
		order_index[counter--]= Stack.top();
    Stack.pop();
  }

	return 0;

}//close ComputeSortingIndexes()



bool TaskSequence::CheckCyclicDependencies(int index, std::vector<std::vector<size_t>>& dep_matrix, std::vector<bool>& visited, std::vector<bool>& recStack) {
    	
	//## This function is a variation of DFSUytil() in http://www.geeksforgeeks.org/archives/18212
	if(visited[index] == false) {
  	// Mark the current node as visited and part of recursion stack
    visited[index] = true;
    recStack[index] = true;
 
    // Recur for all the vertices adjacent to this vertex
    std::vector<size_t>::iterator it;
    for(it = dep_matrix[index].begin(); it!=dep_matrix[index].end(); ++it){
    	if ( !visited[*it] && CheckCyclicDependencies(*it, dep_matrix, visited, recStack) )
      	return true;
      else if (recStack[*it])
        return true;
    }//end loop
 	}//close if
    
	recStack[index] = false;  // remove the vertex from recursion stack

  return false;

}//close CheckCyclicDependencies()
 

bool TaskSequence::HasCyclicDependencies(std::vector<std::vector<size_t>>& dep_matrix) {
    
	//####################################################################################
	//##     Returns true if the graph contains a cycle, else false.
	//###################################################################################
	//Return false if sequence is empty
	if(tasks.empty()){
		__DEBUG_LOG(m_dev,"Task list is empty, nothing to be checked...");
		return false;
	}
	
	//Return false if no dependency info are set
	if(dep_matrix.empty()){
		__DEBUG_LOG(m_dev,"No cyclic dependency as given dependency matrix is empty.");
		return false;
	}

	// Mark all the vertices as not visited and not part of recursion stack
  std::vector<bool> visited(tasks.size(),false);//mark all tasks as non visited
	std::vector<bool> recStack(tasks.size(),false);
  
  // Call the recursive helper function to detect cycle in different DFS trees
  for(unsigned int i=0;i<tasks.size();i++){
  	if (CheckCyclicDependencies(i, dep_matrix, visited, recStack)) return true;
	}
 
  return false;

}//close HasCyclicDependencies()


}//close namespace 

