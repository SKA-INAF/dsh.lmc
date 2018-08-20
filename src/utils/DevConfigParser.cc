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
// * Caesar. If not, see http://www.gnu.org/licenses/.                      *
// **************************************************************************

/**
* @file DevConfigParser.cc
* @class DevConfigParser
* @brief Class for parsing device config file
*
* Class for parsing device config file
* @author S. Riggi
* @date 15/01/2016
*/

#include <DevConfigParser.h>

#ifndef _CODE_UTILS_h
	#include <CodeUtils.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

#include <FormulaData.h>

#include <string>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <sstream>

#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <map>

#include <chrono>
#include <algorithm>


using namespace std;

namespace Utils_ns 
{


DevConfigParser::DevConfigParser(){

}//close constructor

DevConfigParser::DevConfigParser(std::string filename) 
{

}//close constructor


DevConfigParser::~DevConfigParser(){

}//close destructor


int DevConfigParser::GetAttrDataFormat(std::string dataFormatTag){

	long data_format= 1;
	if(dataFormatTag=="SCALAR"){
		data_format= Tango::SCALAR;
	}	
	else if(dataFormatTag=="SPECTRUM"){
		data_format= Tango::SPECTRUM;
	}
	else if(dataFormatTag=="IMAGE"){
		data_format= Tango::IMAGE;
	}
	else{
		ERROR_LOG("Invalid attr format specified in config (valid values are {SCALAR,SPECTRUM,IMAGE}!");
		return -1;
	}
	
	return data_format;

}//close GetAttrDataFormat()


Tango::AttrWriteType DevConfigParser::GetAttrRWType(std::string dataRWTag){
		
	Tango::AttrWriteType data_rw_type= Tango::READ;
	if(dataRWTag=="READ"){
		data_rw_type= Tango::READ;
	}	
	else if(dataRWTag=="WRITE"){
		data_rw_type= Tango::WRITE;
	}
	else if(dataRWTag=="READ_WRITE"){
		data_rw_type= Tango::READ_WRITE;
	}
	else{
		ERROR_LOG("Invalid AttrWriteType specified in config (valid values are {READ,WRITE,READ_WRITE}, returning READ-ONLY attr!");
	}

	return data_rw_type;

}//close GetAttrRWType()

long DevConfigParser::GetAttrDataType(std::string dataTypeTag){

	long data_type= -1;
	if(dataTypeTag=="DOUBLE"){
		data_type= static_cast<long>(Tango::DEV_DOUBLE);
	}	
	else if(dataTypeTag=="FLOAT"){
		data_type= static_cast<long>(Tango::DEV_FLOAT);
	}
	else if(dataTypeTag=="SHORT"){
		data_type= static_cast<long>(Tango::DEV_SHORT);
	}
	else if(dataTypeTag=="LONG"){
		data_type= static_cast<long>(Tango::DEV_LONG);
	}	
	#if TANGO_VERSION_MAJOR >= 9
	else if(dataTypeTag=="ENUM"){
		data_type= static_cast<long>(Tango::DEV_ENUM);
	}
	#endif
	else if(dataTypeTag=="STRING"){
		data_type= static_cast<long>(Tango::DEV_STRING);
	}
	else if(dataTypeTag=="BOOL"){
		data_type= static_cast<long>(Tango::DEV_BOOLEAN);
	}
	else if(dataTypeTag=="STATE"){
		data_type= static_cast<long>(Tango::DEV_STATE);
	}
	else {
		return -1;
	}
	return data_type;

}//close SetAttrDataType()

int DevConfigParser::ReadFromFile(pugi::xml_document& doc,std::string& filename){

	//## Open file and load to doc object
	//pugi::xml_parse_result result= doc.load_file(filename.c_str(),pugi::parse_trim_pcdata & pugi::parse_escapes);
	pugi::xml_parse_result result= doc.load_file(filename.c_str(),pugi::parse_escapes);
	if (!result) {
		ERROR_LOG("Reading input file "<<filename<<" failed (status="<<result.description()<<")");
		return -1;
	}
	
	return 0;

}//close ReadFromFile()

int DevConfigParser::ReadFromString(pugi::xml_document& doc,std::string& input_string){
	
	std::ifstream sstream(input_string);	
	pugi::xml_parse_result result= doc.load(sstream);
	if (!result) {
		ERROR_LOG("Reading input file failed (status="<<result.description()<<")");
		return -1;
	}
	return 0;

}//close ReadFromString()


int DevConfigParser::Parse(std::vector<DeviceAttr*>& attrCollection,std::string& input,bool readFromFile){

	//===========================================
	//==     READ
	//===========================================
	pugi::xml_document doc;
	int status= -1;
	if(readFromFile){//read as file
		status= ReadFromFile(doc,input);
	}
	else{//Read as string
		status= ReadFromString(doc,input);
	}
	if(status<0){
		ERROR_LOG("Failed to read XML tree!");
		return -1;
	}

	//===========================================
	//==    GET TOP NODE
	//===========================================
	std::string topTag= "DeviceConfiguration";

	pugi::xml_node rootNode= doc.child(topTag.c_str());
	if(rootNode.empty()){
		ERROR_LOG("Failed to parse document (missing "<<topTag<<" tag)!");
		return -1;
	}

	//===========================================
	//==    LOAD ATTRIBUTES
	//===========================================
	ParseAttributes(attrCollection,rootNode);

	/*
	//--> Check if Attributes tag is present
	std::string mpListRootTag= "Attributes";
	std::string mpRootTag= "Attribute";
	std::string mpFwdRootTag= "FwdAttribute";

	pugi::xml_node mpRoot = rootNode.child(mpListRootTag.c_str());
	if(mpRoot.empty()){
		WARN_LOG("Missing "<<mpListRootTag<<" tag, no attributes will be configured...");
		return 0;
	}

	//--> Iterate over attribute list
	int nMPs= 0;
	
	for (pugi::xml_node mp = mpRoot.child(mpRootTag.c_str()); mp; mp = mp.next_sibling(mpRootTag.c_str())){

		//Parse attr
		DeviceAttr* aDeviceAttr= ParseAttrNode(mp);
		if(!aDeviceAttr){
			WARN_LOG("Failed to parse moni point from XML node...skip it!");
			continue;
		}

		//Add to collection
		attrCollection.push_back(aDeviceAttr);
		
		nMPs++;   
	}//end loop monitoring points

	//--> Iterate over fwd attributes
	for (pugi::xml_node mp = mpRoot.child(mpFwdRootTag.c_str()); mp; mp = mp.next_sibling(mpFwdRootTag.c_str())){

		//Parse attr
		DeviceAttr* aDeviceAttr= ParseFwdAttrNode(mp);
		if(!aDeviceAttr){
			WARN_LOG("Failed to parse fwd moni point from XML node...skip it!");
			continue;
		}

		//Add to collection
		attrCollection.push_back(aDeviceAttr);
		
		nMPs++;   
	}//end loop monitoring points
	*/

 	return 0;

}//close Parse()

void DevConfigParser::ParseAttributes(std::vector<DeviceAttr*>& attrCollection,pugi::xml_node& rootNode){

	//===========================================
	//==    LOAD ATTRIBUTES
	//===========================================
	//--> Check if Attributes tag is present
	std::string mpListRootTag= "Attributes";
	std::string mpRootTag= "Attribute";
	std::string mpFwdRootTag= "FwdAttribute";
	
	pugi::xml_node mpRoot = rootNode.child(mpListRootTag.c_str());
	if(mpRoot.empty()){
		WARN_LOG("Missing "<<mpListRootTag<<" tag, no attributes will be configured...");
		return;
	}

	//--> Iterate over attribute list
	int nMPs= 0;
	
	for (pugi::xml_node mp = mpRoot.child(mpRootTag.c_str()); mp; mp = mp.next_sibling(mpRootTag.c_str())){

		//Parse attr
		DeviceAttr* aDeviceAttr= ParseAttrNode(mp);
		if(!aDeviceAttr){
			WARN_LOG("Failed to parse moni point from XML node...skip it!");
			continue;
		}

		//Add to collection
		attrCollection.push_back(aDeviceAttr);
		
		nMPs++;   
	}//end loop monitoring points

	//--> Iterate over fwd attributes
	for (pugi::xml_node mp = mpRoot.child(mpFwdRootTag.c_str()); mp; mp = mp.next_sibling(mpFwdRootTag.c_str())){

		//Parse attr
		DeviceAttr* aDeviceAttr= ParseFwdAttrNode(mp);
		if(!aDeviceAttr){
			WARN_LOG("Failed to parse fwd moni point from XML node...skip it!");
			continue;
		}

		//Add to collection
		attrCollection.push_back(aDeviceAttr);
		
		nMPs++;   
	}//end loop monitoring points

}//close ParseAttributes()

int DevConfigParser::ParseAlarms(std::vector<AlarmData*>& alarmCollection,std::string& input,bool readFromFile){

	//===========================================
	//==     READ
	//===========================================
	pugi::xml_document doc;
	int status= -1;
	if(readFromFile){//read as file
		status= ReadFromFile(doc,input);
	}
	else{//Read as string
		status= ReadFromString(doc,input);
	}
	if(status<0){
		ERROR_LOG("Failed to read XML tree!");
		return -1;
	}

	//===========================================
	//==    GET TOP NODE
	//===========================================
	std::string topTag= "DeviceConfiguration";

	pugi::xml_node rootNode= doc.child(topTag.c_str());
	if(rootNode.empty()){
		ERROR_LOG("Failed to parse document (missing "<<topTag<<" tag)!");
		return -1;
	}

	//===========================================
	//==    LOAD ALARMS
	//===========================================
	ParseAlarmCollection(alarmCollection,rootNode);

	return 0;

}//close ParseAlarms()


void DevConfigParser::ParseAlarmCollection(std::vector<AlarmData*>& alarmCollection,pugi::xml_node& rootNode){

	//===========================================
	//==    LOAD ALARMS
	//===========================================
	//--> Check if Alarms tag is present
	std::string alarmListRootTag= "Alarms";
	std::string alarmRootTag= "Alarm";
	
	pugi::xml_node alarmRoot = rootNode.child(alarmListRootTag.c_str());
	if(alarmRoot.empty()){
		WARN_LOG("Missing "<<alarmListRootTag<<" tag, no alarms will be configured...");
		return;
	}

	//--> Iterate over alarms list
	for (pugi::xml_node alarm_node = alarmRoot.child(alarmRootTag.c_str()); alarm_node; alarm_node = alarm_node.next_sibling(alarmRootTag.c_str())){

		//Parse alarm data
		AlarmData* alarm_data= ParseAlarmDataNode(alarm_node);
		if(!alarm_data){
			WARN_LOG("Failed to parse alarm data from XML node...skip it!");
			continue;
		}

		//Add to collection
		alarmCollection.push_back(alarm_data);
		
	}//end loop alarms

}//close ParseAlarmCollection()


int DevConfigParser::ParseArchivedAttrs(std::vector<ArchivedData*>& archivedAttrCollection,std::string& input,bool readFromFile){

	//===========================================
	//==     READ
	//===========================================
	pugi::xml_document doc;
	int status= -1;
	if(readFromFile){//read as file
		status= ReadFromFile(doc,input);
	}
	else{//Read as string
		status= ReadFromString(doc,input);
	}
	if(status<0){
		ERROR_LOG("Failed to read XML tree!");
		return -1;
	}

	//===========================================
	//==    GET TOP NODE
	//===========================================
	std::string topTag= "DeviceConfiguration";

	pugi::xml_node rootNode= doc.child(topTag.c_str());
	if(rootNode.empty()){
		ERROR_LOG("Failed to parse document (missing "<<topTag<<" tag)!");
		return -1;
	}

	//===========================================
	//==    LOAD ALARMS
	//===========================================
	ParseArchivedAttrCollection(archivedAttrCollection,rootNode);

	return 0;

}//close ParseArchivedAttrs()


void DevConfigParser::ParseArchivedAttrCollection(std::vector<ArchivedData*>& archivedAttrCollection,pugi::xml_node& rootNode){

	//===========================================
	//==    LOAD ARCHIVED ATTR
	//===========================================
	//--> Check if ArchivedAttributes tag is present
	std::string archivedAttrListRootTag= "ArchivedAttributes";
	std::string archivedAttrRootTag= "ArchivedAttribute";
	
	pugi::xml_node archivedAttrRoot = rootNode.child(archivedAttrListRootTag.c_str());
	if(archivedAttrRoot.empty()){
		WARN_LOG("Missing "<<archivedAttrListRootTag<<" tag, no alarms will be configured...");
		return;
	}

	//--> Iterate over archived attr list
	for (pugi::xml_node archived_attr_node = archivedAttrRoot.child(archivedAttrRootTag.c_str()); archived_attr_node; archived_attr_node = archived_attr_node.next_sibling(archivedAttrRootTag.c_str())){

		//Parse alarm data
		ArchivedData* archived_data= ParseArchivedDataNode(archived_attr_node);
		if(!archived_data){
			WARN_LOG("Failed to parse archived attr data from XML node...skip it!");
			continue;
		}

		//Add to collection
		archivedAttrCollection.push_back(archived_data);
		
	}//end loop alarms

}//close ParseArchivedAttrCollection()

DeviceAttr* DevConfigParser::ParseFwdAttrNode(pugi::xml_node& node){

	//Get attr name
	pugi::xml_attribute name_attr= node.attribute("name");
	if(name_attr.empty()){
		WARN_LOG("Invalid monitoring point detected (missing name attr), skip it!");
		return 0;
	}	
	std::string name= name_attr.value();

	//Get attr alias
	pugi::xml_attribute alias_attr= node.attribute("alias");
	std::string alias_name= name;
	if(!alias_attr.empty()){
		alias_name= alias_attr.value();
	}

	//Get url 
	pugi::xml_node urlTag= node.child("Url");
	if(urlTag.empty()){
		WARN_LOG("Missing url info ...skip it!");
		return 0;
	}
	std::string url= urlTag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(url);
	DEBUG_LOG("url= "<<url);

	//Get label
	pugi::xml_node labelTag= node.child("Label");
	std::string label= "";
	if(!labelTag.empty()){
		label= labelTag.text().as_string();
		//Utils_ns::CodeUtils::StripBlankSpaces(label);
	}

	//Create prop
	Tango::UserDefaultFwdAttrProp prop;
	prop.set_label(label);

	
	//Set polling info
	pugi::xml_node pollingTag= node.child("Polling");
	bool isPolled= false;	
	long int poll_period= -1;
	if(!pollingTag.empty()){
		pugi::xml_node pollingPeriodTag= pollingTag.child("Period");
		if(!pollingPeriodTag.empty()){
			isPolled= true;
			poll_period= pollingPeriodTag.text().as_llong();
		}
		else {
			WARN_LOG("Missing polling period for fwd attr "<<name<<"!");
		}
	}//close if


	//Create attribute
	Tango::FwdAttr* attr= new Tango::FwdAttr(alias_name,url);
	attr->set_default_properties(prop);

	//Set polling?
	if(isPolled) {
		attr->set_polling_period(poll_period);
	}

	//Create device attr
	DeviceAttr* device_attr= new DeviceFwdAttrImpl(attr);
	device_attr->SetPolled(isPolled);

	return device_attr;

}//close ParseFwdAttrNode()

DeviceAttr* DevConfigParser::ParseAttrNode(pugi::xml_node& node){
	
	//Get attr name
	pugi::xml_attribute name_attr= node.attribute("name");
	if(name_attr.empty()){
		WARN_LOG("Invalid monitoring point detected (missing name attr), skip it!");
		return 0;
	}	
	std::string name= name_attr.value();

	//Get attr alias
	pugi::xml_attribute alias_attr= node.attribute("alias");
	std::string alias_name= name;
	if(!alias_attr.empty()){
		alias_name= alias_attr.value();
	}
	
	//Get data type & format
	pugi::xml_node dataTypeTag= node.child("DataType");
	pugi::xml_node dataFormatTag= node.child("DataFormat");
	pugi::xml_node dataRWTypeTag= node.child("RWType");
	if(dataTypeTag.empty()){
		WARN_LOG("Missing data type info ...skip it!");
		return 0;
	}
	std::string str_data_type= dataTypeTag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(str_data_type);
	long int data_type= GetAttrDataType(str_data_type);
	if(data_type<0){
		WARN_LOG("Failed to determine data type (type="<<str_data_type<<")...skip it!");
		return 0;
	}
	
	if(dataFormatTag.empty()){
		WARN_LOG("Missing data format info ...skip it!");
		return 0;
	}
	std::string str_data_format= dataFormatTag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(str_data_format);
	int data_format= GetAttrDataFormat(str_data_format);
		
	if(dataRWTypeTag.empty()){
		WARN_LOG("Missing data rw type info ...skip it!");
		return 0;
	}
	std::string str_rw_type= dataRWTypeTag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(str_rw_type);	
	Tango::AttrWriteType rw_type= GetAttrRWType(str_rw_type);
			
	//Get display level
	pugi::xml_node displayLevelTag= node.child("DisplayLevel");
	Tango::DispLevel dispLevel= Tango::OPERATOR;
	if(!displayLevelTag.empty()){
		std::string displayLevelStr= displayLevelTag.text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(displayLevelStr);
		if(displayLevelStr=="OPERATOR") dispLevel= Tango::OPERATOR;
		else if(displayLevelStr=="EXPERT") dispLevel= Tango::EXPERT;
		else{
			WARN_LOG("Invalid display level given ("<<displayLevelStr<<") ...skip it!");
			return nullptr;
		}
	}

	//Get memorized flag
	pugi::xml_node isMemorizedTag= node.child("Memorized");	
	bool hasMemorized= !isMemorizedTag.empty();
	if( (str_rw_type=="READ" || str_data_format!="SCALAR") && hasMemorized){
		WARN_LOG("Attr "<<name<<" (rw="<<str_rw_type <<", format="<<str_data_format<<") cannot be set as memorized (only available for scalar & writable attr) ...skip it!");
		return 0;
	}

	//Get memorized default prop
	std::string memorizedDefaultPropName= "";
	if(hasMemorized){
		pugi::xml_node memorizedDefaultPropTag= node.child("MemorizedDefaultProp");
		bool hasMemorizedDefaultProp= !memorizedDefaultPropTag.empty();
		if(hasMemorizedDefaultProp){
			memorizedDefaultPropName= memorizedDefaultPropTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(memorizedDefaultPropName);
		}
		else{
			std::string attr_name_capitalized= name;
			attr_name_capitalized[0] = std::toupper(attr_name_capitalized[0]);
			memorizedDefaultPropName= attr_name_capitalized + std::string("Default");
		}
	}
	 
	//Get data max dim
	long int maxDimX= 1;
	long int maxDimY= 0;
	if(data_format==Tango::SPECTRUM){
		pugi::xml_node dimXTag= node.child("DimX");
		if(!dimXTag.empty()){
			maxDimX= dimXTag.text().as_llong();
		}
		else{
			WARN_LOG("Missing data dimension info ...skip it!");
			return 0;
		}
	}
	if(data_format==Tango::IMAGE){
		pugi::xml_node dimXTag= node.child("DimX");	
		pugi::xml_node dimYTag= node.child("DimY");
		if(!dimXTag.empty() && !dimYTag.empty()){
			maxDimX= dimXTag.text().as_llong();
			maxDimY= dimYTag.text().as_llong();
		}
		else{
			WARN_LOG("Missing data dimension info ...skip it!");
			return 0;
		}
	}	

	//Set polling info
	pugi::xml_node pollingTag= node.child("Polling");
	bool isPolled= false;	
	long int poll_period= -1;
	DEBUG_LOG("hasPollingTag? "<<!pollingTag.empty());
	if(!pollingTag.empty()){
		pugi::xml_node pollingPeriodTag= pollingTag.child("Period");
		DEBUG_LOG("hasPollingPeriodTag? "<<!pollingPeriodTag.empty());
		if(!pollingPeriodTag.empty()){
			isPolled= true;
			poll_period= pollingPeriodTag.text().as_llong();
		}
		else {
			WARN_LOG("Missing polling period!");
		}
	}//close if
	//DEBUG_LOG("isPolled? "<<isPolled<<", poll_period="<<poll_period);

	
	//Create attribute
	Tango::Attr* attr= 0;

	if(data_format==Tango::SCALAR){
		attr= new Tango::Attr(alias_name.c_str(),data_type,dispLevel,rw_type);
	}
	else if(data_format==Tango::SPECTRUM){
		attr= new Tango::SpectrumAttr(alias_name.c_str(),data_type,rw_type,maxDimX,dispLevel);
	}
	else if(data_format==Tango::IMAGE){
		attr= new Tango::ImageAttr(alias_name.c_str(),data_type,rw_type,maxDimX,maxDimY,dispLevel);
	}
	else{
		WARN_LOG("Invalid attr format given!");
		return 0;
	}

	//Set memorized
	//DEBUG_LOG("isMemorized? "<<hasMemorized);
	if(hasMemorized) {
		attr->set_memorized();
		attr->set_memorized_init(false);
	}
	
	//Set polling
	if(isPolled) {
		//DEBUG_LOG("Set polling period in attr...");
		attr->set_polling_period(poll_period);
	}
	
	//Get value field node
	pugi::xml_node dataValueTag= node.child("Value");
	
	//Set attr attr and properties
	int status= -1;
	DeviceAttr* device_attr= 0;

	if(data_type==Tango::DEV_DOUBLE){
		Tango::MultiAttrProp<Tango::DevDouble>* prop= new Tango::MultiAttrProp<Tango::DevDouble>;
		status= ParseMultiAttrProp(prop,node);

		device_attr= new DeviceAttrImpl<Tango::DevDouble>(attr,prop);
		device_attr->SetPolled(isPolled);
		if(hasMemorized) {
			device_attr->SetMemorized(true);
			device_attr->SetDefaultPropName(memorizedDefaultPropName);
		}

		if(ParseValueNode<Tango::DevDouble>(device_attr,dataValueTag)<0){
			WARN_LOG("Failed to parse value node!");
			delete device_attr;
			device_attr= 0;
			return 0;
		}
	}
	else if(data_type==Tango::DEV_FLOAT){
		Tango::MultiAttrProp<Tango::DevFloat>* prop= new Tango::MultiAttrProp<Tango::DevFloat>;
		status= ParseMultiAttrProp(prop,node);

		device_attr= new DeviceAttrImpl<Tango::DevFloat>(attr,prop);
		device_attr->SetPolled(isPolled);
		if(hasMemorized) {
			device_attr->SetMemorized(true);
			device_attr->SetDefaultPropName(memorizedDefaultPropName);
		}
		if(ParseValueNode<Tango::DevFloat>(device_attr,dataValueTag)<0){
			WARN_LOG("Failed to parse value node!");
			delete device_attr;
			device_attr= 0;
			return 0;
		}
	}
	else if(data_type==Tango::DEV_LONG){
		Tango::MultiAttrProp<Tango::DevLong>* prop= new Tango::MultiAttrProp<Tango::DevLong>;
		status= ParseMultiAttrProp(prop,node);
		
		device_attr= new DeviceAttrImpl<Tango::DevLong>(attr,prop);
		device_attr->SetPolled(isPolled);
		if(hasMemorized) {
			device_attr->SetMemorized(true);
			device_attr->SetDefaultPropName(memorizedDefaultPropName);
		}
		if(ParseValueNode<Tango::DevLong>(device_attr,dataValueTag)<0){
			WARN_LOG("Failed to parse value node!");
			delete device_attr;
			device_attr= 0;
			return 0;
		}
	}
	else if(data_type==Tango::DEV_SHORT){
		Tango::MultiAttrProp<Tango::DevShort>* prop= new Tango::MultiAttrProp<Tango::DevShort>;
		status= ParseMultiAttrProp(prop,node);
		
		device_attr= new DeviceAttrImpl<Tango::DevShort>(attr,prop);	
		device_attr->SetPolled(isPolled);
		if(hasMemorized) {
			device_attr->SetMemorized(true);
			device_attr->SetDefaultPropName(memorizedDefaultPropName);
		}

		if(ParseValueNode<Tango::DevShort>(device_attr,dataValueTag)<0){
			WARN_LOG("Failed to parse value node!");
			delete device_attr;
			device_attr= 0;
			return 0;
		}
	}
	else if(data_type==Tango::DEV_BOOLEAN){
		Tango::MultiAttrProp<Tango::DevBoolean>* prop= new Tango::MultiAttrProp<Tango::DevBoolean>;
		status= ParseMultiAttrProp(prop,node);
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

		device_attr= new DeviceAttrImpl<Tango::DevBoolean>(attr,prop);
		device_attr->SetPolled(isPolled);
		if(hasMemorized) {
			device_attr->SetMemorized(true);
			device_attr->SetDefaultPropName(memorizedDefaultPropName);
		}

		if(ParseValueNode<Tango::DevBoolean>(device_attr,dataValueTag)<0){
			WARN_LOG("Failed to parse value node!");
			delete device_attr;
			device_attr= 0;
			return 0;
		}
	}
	#if TANGO_VERSION_MAJOR >= 9
	else if(data_type==Tango::DEV_ENUM){
		Tango::MultiAttrProp<Tango::DevEnum>* prop= new Tango::MultiAttrProp<Tango::DevEnum>;
		status= ParseMultiAttrProp(prop,node);
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
		
		device_attr= new DeviceAttrImpl<Tango::DevEnum>(attr,prop);	
		device_attr->SetPolled(isPolled);
		if(hasMemorized) {
			device_attr->SetMemorized(true);
			device_attr->SetDefaultPropName(memorizedDefaultPropName);
		}		

		if(ParseValueNode<Tango::DevEnum>(device_attr,dataValueTag)<0){
			WARN_LOG("Failed to parse value node!");
			delete device_attr;
			device_attr= 0;
			return 0;
		}
	}
	#endif
	else if(data_type==Tango::DEV_STRING){
		Tango::MultiAttrProp<Tango::DevString>* prop= new Tango::MultiAttrProp<Tango::DevString>;
		status= ParseMultiAttrProp(prop,node);
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
		
		device_attr= new DeviceAttrImpl<Tango::DevString>(attr,prop);
		device_attr->SetPolled(isPolled);
		if(hasMemorized) {
			device_attr->SetMemorized(true);
			device_attr->SetDefaultPropName(memorizedDefaultPropName);
		}
		if(ParseValueNode<Tango::DevString>(device_attr,dataValueTag)<0){
			WARN_LOG("Failed to parse value node!");
			delete device_attr;
			device_attr= 0;
			return 0;
		}
	}
	else if(data_type==Tango::DEV_STATE){
		Tango::MultiAttrProp<Tango::DevState>* prop= new Tango::MultiAttrProp<Tango::DevState>;
		status= ParseMultiAttrProp(prop,node);
			
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

		device_attr= new DeviceAttrImpl<Tango::DevState>(attr,prop);
		device_attr->SetPolled(isPolled);
		if(hasMemorized) {
			device_attr->SetMemorized(true);
			device_attr->SetDefaultPropName(memorizedDefaultPropName);
		}		
		if(ParseValueNode<Tango::DevState>(device_attr,dataValueTag)<0){
			WARN_LOG("Failed to parse value node!");
			delete device_attr;
			device_attr= 0;
			return 0;
		}
	}
	if(status<0) {
		WARN_LOG("Failed to parse attr property!");
		return 0;
	}

	
	//Parse formula node
	pugi::xml_node formulaTag_new= node.child("Formula");
	if(!formulaTag_new.empty()){
		DEBUG_LOG("Parsing attr formula tag...");
		FormulaData* formula_data= ParseFormulaNode(formulaTag_new);
		if(formula_data){
			//Set attr name in formula data
			formula_data->SetAttrName(name);
			DEBUG_LOG("Formula data for attr "<<name<<" (name="<<formula_data->GetAttrName()<<")");

			device_attr->SetFormulaData(formula_data);
			DEBUG_LOG("Formula data for attr "<<name<<" (name="<<formula_data->GetAttrName()<<")");

		}	
		else{
			WARN_LOG("Failed to parse attr formula data!");	
			return 0;
		}	
	}

	
	return device_attr;

}//close ParseAttrNode()



FormulaData* DevConfigParser::ParseFormulaNode(pugi::xml_node& node){
		
	//Get formula expression
	std::string formulaExprTagName= "Expression";
	std::string formula_expr= "";
	if(node.child(formulaExprTagName.c_str()).empty()) {
		WARN_LOG("Missing "<<formulaExprTagName<<" node!");
		return nullptr;
	}
	else{
		formula_expr= node.child(formulaExprTagName.c_str()).text().as_string();
		//Utils_ns::CodeUtils::StripBlankSpaces(formula_expr);
	}
	
	
	//Get formula evaluation mode
	std::string formulaEvalModeTagName= "EvaluationMode";
	pugi::xml_node formulaEvalModeTag= node.child(formulaEvalModeTagName.c_str());
	std::vector<Tango::EventType> evt_types;

	if(formulaEvalModeTag.empty()) {
		WARN_LOG("Missing "<<formulaEvalModeTagName<<" node!");
		return nullptr;
	}
	else{
		for (pugi::xml_node it = formulaEvalModeTag.child("Value"); it; it = it.next_sibling("Value")){
			std::string mode= it.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(mode);
			Tango::EventType evt_type;
			if(GetFormulaEvalMode(evt_type,mode)<0) return nullptr;
			evt_types.push_back(evt_type);
		}//end loop evaluation modes
	}//close else

	if(evt_types.empty()){
		WARN_LOG("Empty formula evalution mode!");
		return nullptr;
	}

	//Get formula post-actions
	std::string formulaPostActionsTagName= "PostActions";
	pugi::xml_node formulaPostActionsTag= node.child(formulaPostActionsTagName.c_str());
	std::vector<std::string> post_actions;
	if(!formulaPostActionsTag.empty()) {
		for (pugi::xml_node it = formulaPostActionsTag.child("Action"); it; it = it.next_sibling("Action")){
			std::string action= it.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(action);
			post_actions.push_back(action);
		}//end loop actions
	}//close if

	//Get set invalid quality if old flag
	pugi::xml_node setInvalidQualityIfOldTag= node.child("SetInvalidQualityIfOld");	
	bool setInvalidQualityIfOld= !setInvalidQualityIfOldTag.empty();

	//Get formula symbols
	std::string formulaSymbolsTagName= "FormulaVars";
	pugi::xml_node formulaSymbolsTag= node.child(formulaSymbolsTagName.c_str());
	std::map<std::string,FormulaVarPtr> formula_symbols;

	if(formulaEvalModeTag.empty()) {
		WARN_LOG("Missing "<<formulaSymbolsTagName<<" node!");
		return nullptr;
	}
	else{
		for (pugi::xml_node it = formulaSymbolsTag.child("FormulaVar"); it; it = it.next_sibling("FormulaVar")){
			if(it.child("Label").empty() || it.child("Value").empty() || it.child("InitDataValue").empty()) {
				WARN_LOG("Missing Label/Name and/or Value field in Var node!");
				return nullptr;
			}
			if(it.child("ExpDataType").empty() || it.child("ExpDataFormat").empty()) {
				WARN_LOG("Missing ExpDataType/ExpDataFormat fields in Var node!");
				return nullptr;
			}
			std::string var_label= it.child("Label").text().as_string();
			std::string var_value= it.child("Value").text().as_string();
			std::string var_init_value= it.child("InitDataValue").text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(var_label);
			Utils_ns::CodeUtils::StripBlankSpaces(var_value);
			Utils_ns::CodeUtils::StripBlankSpaces(var_init_value);

			std::string str_data_type= it.child("ExpDataType").text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(str_data_type);
			long int data_type= GetAttrDataType(str_data_type);
			if(data_type<0){
				WARN_LOG("Failed to determine data type (type="<<str_data_type<<")!");
				return nullptr;
			}
	
			std::string str_data_format= it.child("ExpDataFormat").text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(str_data_format);
			int data_format= GetAttrDataFormat(str_data_format);
			if(data_format<0){
				WARN_LOG("Failed to determine data format (type="<<str_data_format<<")!");
				return nullptr;
			}
	
			long int data_size= 1;
			if(it.child("ExpDataSize").empty()){
				if(data_format==Tango::SPECTRUM){
					WARN_LOG("Missing data size tag (required for spectrum proxy attr in formula)!");
					return nullptr;
				}
			}
			else{
				data_size= it.child("ExpDataSize").text().as_llong();
				if(data_size<=0){
					WARN_LOG("Failed to determine data size!");
					return nullptr;
				}
			}
			
			DEBUG_LOG("FormulaVar "<<var_label<<" info (initValue="<<var_init_value<<" expDataType="<<data_type<<", expDataTypeStr="<<str_data_type<<", expDataFormat="<<data_format<<", expDataFormatStr="<<str_data_format<<", expDataSize="<<data_size<<")");

			//Check if attr is a proxy remote attr (FQDN) or an internal attribute
			FormulaVarPtr formula_var= 0;
	
			if(Utils_ns::TangoUtils::IsFQDNProxyAttr(var_value)){//proxy attr
				//Parse var value to extract device & attr name (NB: it should be a FQD attr name)
				std::string device_name("");
				std::string attr_name("");
				std::string domain_name("");
				bool hasQualityTag= false;
				DEBUG_LOG("Parsing attr var value "<<var_value<<" to get domain/device/attr names...");
				if(Utils_ns::TangoUtils::ParseFullAttrName(domain_name,device_name,attr_name,hasQualityTag,var_value)<0){
					WARN_LOG("Failed to extract dev & attr name from full attr name!");
					return nullptr;
				}
				if(domain_name==""){
					WARN_LOG("Failed to parse domain name from formula attr (check if you specified attr name as FQDN!)!");
					return nullptr;
				}
				std::string device_name_full= domain_name + std::string("/") + device_name;
				int value_type= FormulaVar::eATTR_VALUE;
				if(hasQualityTag) value_type= FormulaVar::eATTR_QUALITY_FACTOR;
		
				//Create proxy formula var				
				formula_var= std::make_shared<ProxyAttrFormulaVar>(device_name_full,attr_name,value_type);
				std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->SetInitDataValue(var_init_value);
				std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->SetExpectedDataInfo(data_type,data_format,data_size);

			}//close if proxy attr
			else{//Internal attr
				//Check if quality is specified in the attr name
				std::string attr_name= var_value;
				int value_type= FormulaVar::eATTR_VALUE;
				std::size_t pos_quality = var_value.find(".quality");
				if(pos_quality!=std::string::npos){	
					value_type= FormulaVar::eATTR_QUALITY_FACTOR;
					std::string var_value_wquality= var_value.substr(0,attr_name.length()-8);
					attr_name= var_value_wquality;
				}
				
				//Create internal attr formula var				
				formula_var= std::make_shared<InternalAttrFormulaVar>(attr_name,value_type);
				
			}//close else internal attr

			
			//Add symbol in map 
			formula_symbols.insert( std::make_pair(var_label,formula_var) );

		}//end loop symbols
	}//close else
		
	if(formula_symbols.empty()){
		WARN_LOG("Empty formula symbols!");
		return nullptr;
	}
	
	//Fill and return formula data
	FormulaData* formula_data= new FormulaData(formula_expr,formula_symbols,evt_types);
	formula_data->SetPostActions(post_actions);
	formula_data->SetInvalidQualityIfOldFlag(setInvalidQualityIfOld);

	return formula_data;

}//close ParseFormulaNode()

int DevConfigParser::GetFormulaEvalMode(Tango::EventType& ev_type,std::string mode){
	
	if(mode=="CHANGE"){
		ev_type= Tango::CHANGE_EVENT;
	}	
	else if(mode=="PERIODIC"){
		ev_type= Tango::PERIODIC_EVENT;
	}
	else if(mode=="USER"){
		ev_type= Tango::USER_EVENT;
	}
	else{
		WARN_LOG("Invalid formula eval mode given (mode="<<mode<<")!");
		return -1;
	}
	return 0;

}//close GetFormulaEvalMode()


AlarmData* DevConfigParser::ParseAlarmDataNode(pugi::xml_node& node){

	//========================================================	
	//==   MANDATORY FIELD
	//========================================================
	//Get alarm name
	pugi::xml_node name_tag= node.child("Name");
	if(name_tag.empty()){
		WARN_LOG("Invalid alarm data detected (missing mandatory name field), skip it!");
		return nullptr;
	}	
	std::string alarm_name= name_tag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(alarm_name);

	//Get alarm formula
	pugi::xml_node formula_tag= node.child("Formula");
	if(formula_tag.empty()){
		WARN_LOG("Invalid alarm data detected (missing mandatory formula field), skip it!");
		return nullptr;
	}	
	std::string alarm_formula= formula_tag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(alarm_formula);

	//Get alarm symbols
	std::string formulaSymbolsTagName= "FormulaVars";
	pugi::xml_node formulaSymbolsTag= node.child(formulaSymbolsTagName.c_str());
	std::map<std::string,std::string> formula_symbols;

	if(formulaSymbolsTag.empty()) {
		WARN_LOG("Invalid alarm data detected (missing mandatory formula vars field), skip it!");
		return nullptr;
	}
	
	for (pugi::xml_node it = formulaSymbolsTag.child("FormulaVar"); it; it = it.next_sibling("FormulaVar")){
		if(it.child("Label").empty() || it.child("Value").empty()) {
			WARN_LOG("Missing Label/Name and/or Value field in Var node!");
			return nullptr;
		}
		std::string var_label= it.child("Label").text().as_string();
		std::string var_value= it.child("Value").text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(var_label);
		Utils_ns::CodeUtils::StripBlankSpaces(var_value);

		//Add symbol in map 
		formula_symbols.insert( std::make_pair(var_label,var_value) );

	}//end loop map entries
			
	//Get alarm level
	pugi::xml_node level_tag= node.child("Level");
	if(level_tag.empty()){
		WARN_LOG("Invalid alarm data detected (missing mandatory level field), skip it!");
		return nullptr;
	}	
	std::string alarm_level= level_tag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(alarm_level);

	//Get alarm groups
	pugi::xml_node group_tag= node.child("Groups");
	std::vector<std::string> alarm_groups;
	for (pugi::xml_node it = group_tag.child("Group"); it; it = it.next_sibling("Group")){
		std::string group= it.text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(group);
		alarm_groups.push_back(group);
	}//end loop group entries

	//Get alarm message
	pugi::xml_node message_tag= node.child("Message");
	if(message_tag.empty()){
		WARN_LOG("Invalid alarm data detected (missing mandatory message field), skip it!");
		return nullptr;
	}	
	std::string alarm_message= message_tag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(alarm_message);


	//========================================================	
	//==   CREATE ALARM DATA WITH MANDATORY FIELDS
	//========================================================
	//## Create alarm data
	AlarmData* alarm_data= new AlarmData (alarm_name,alarm_formula,formula_symbols,alarm_level,alarm_groups,alarm_message);

	//========================================================	
	//==   OPTIONAL FIELDS
	//========================================================
	pugi::xml_node onDelay_tag= node.child("OnDelay");
	if(!onDelay_tag.empty()){	
		int on_delay= onDelay_tag.text().as_int();
		alarm_data->SetOnDelay(on_delay);
	}
	pugi::xml_node offDelay_tag= node.child("OffDelay");
	if(!offDelay_tag.empty()){	
		int off_delay= offDelay_tag.text().as_int();
		alarm_data->SetOffDelay(off_delay);
	}
	pugi::xml_node silentTime_tag= node.child("SilentTime");
	if(!silentTime_tag.empty()){	
		int silent_time= silentTime_tag.text().as_int();
		alarm_data->SetSilentTime(silent_time);
	}
	pugi::xml_node onCommand_tag= node.child("OnCommand");
	if(!onCommand_tag.empty()){	
		std::string on_command= onCommand_tag.text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(on_command);
		alarm_data->SetOnCommand(on_command);
	}
	pugi::xml_node offCommand_tag= node.child("OffCommand");
	if(!offCommand_tag.empty()){	
		std::string off_command= offCommand_tag.text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(off_command);
		alarm_data->SetOffCommand(off_command);
	}
	pugi::xml_node enabled_tag= node.child("Enabled");
	if(!enabled_tag.empty()){	
		bool enabled= enabled_tag.text().as_bool(true);
		alarm_data->SetEnabled(enabled);
	}

	
	//========================================================	
	//==   SET POLLING INFO
	//========================================================
	//Set polling info
	pugi::xml_node pollingTag= node.child("Polling");
	bool isPolled= false;	
	long int poll_period= -1;
	if(!pollingTag.empty()){
		pugi::xml_node pollingPeriodTag= pollingTag.child("Period");
		if(!pollingPeriodTag.empty()){
			isPolled= true;
			poll_period= pollingPeriodTag.text().as_llong();
		}
		else {
			WARN_LOG("Missing polling period in parsed alarm data!");
		}
	}//close if

	if(isPolled){
		alarm_data->SetPolled(true);
		alarm_data->SetPollPeriod(poll_period);
	}

	//========================================================	
	//==   VALIDATE ALARM DATA
	//========================================================
	if(!alarm_data->IsValid()){
		WARN_LOG("Alarm data parsed is not valid (hint: missing or invalid fields detected)!");
		delete alarm_data;
		alarm_data= nullptr;		
	}


	return alarm_data;

}//close ParseAlarmNode()


ArchivedData* DevConfigParser::ParseArchivedDataNode(pugi::xml_node& node){

	//========================================================	
	//==   MANDATORY FIELD
	//========================================================
	//Get alarm name
	pugi::xml_node name_tag= node.child("Name");
	if(name_tag.empty()){
		WARN_LOG("Invalid archived data detected (missing mandatory name field), skip it!");
		return nullptr;
	}	
	std::string archived_attr_name= name_tag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(archived_attr_name);

	//Get archived strategy
	pugi::xml_node group_tag= node.child("Strategies");
	std::vector<std::string> strategies;
	for (pugi::xml_node it = group_tag.child("Strategy"); it; it = it.next_sibling("Strategy")){
		std::string strategy= it.text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(strategy);
		strategies.push_back(strategy);
	}//end loop group entries

	//========================================================	
	//==   CREATE ARCHIVED DATA WITH MANDATORY FIELDS
	//========================================================
	//## Create archived data
	ArchivedData* archived_data= new ArchivedData (archived_attr_name,strategies);

	//========================================================	
	//==   OPTIONAL FIELDS
	//========================================================
	pugi::xml_node ttl_tag= node.child("TimeToLive");
	int ttl= 0;
	if(!ttl_tag.empty()){	
		ttl= ttl_tag.text().as_int();
		archived_data->SetTTL(ttl);
	}
	pugi::xml_node enabled_tag= node.child("EnabledAtStartup");
	if(!enabled_tag.empty()){	
		bool enabled= enabled_tag.text().as_bool(true);
		archived_data->SetEnabled(enabled);
	}

	//========================================================	
	//==   VALIDATE ARCHIVED DATA
	//========================================================
	//INFO_LOG("archived_attr_name="<<archived_attr_name<<", ttl="<<ttl<<" strategies size="<<strategies.size());
	if(!archived_data->IsValid()){
		WARN_LOG("Archived data parsed is not valid (hint: missing or invalid fields detected)!");
		delete archived_data;
		archived_data= nullptr;		
	}

	return archived_data;

}//close ParseArchivedDataNode()

}//close namespace

