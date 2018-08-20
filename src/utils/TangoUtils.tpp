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


template <typename T>
int TangoUtils::TangoStateStrToValue(T& state_value,std::string state_str){
	
	//Check args
	if(state_str=="") return -1;
	
	//Check state str map
	if(m_tangoStateStrMap.empty()) return -1;//shoul not occur since map is initialized 
	std::map<std::string,Tango::DevState>::iterator it= m_tangoStateStrMap.find(state_str);
	if(it==m_tangoStateStrMap.end()) return -1;//Invalid State string given!
	
	//Get state
	Tango::DevState state= it->second;
	
	//Cast state to desired type
	state_value= static_cast<T>(state);

	return 0;

}//close TangoStateStrToValue()

template <typename T>
int TangoUtils::TangoStateStrToValue(std::vector<T>& state_values,std::vector<std::string>& state_str){
	
	//Init 
	state_values.clear();

	//Check args
	if(state_str.empty()) return -1;
	
	for(unsigned int i=0;i<state_str.size();i++){
		T state_value;
		if(TangoStateStrToValue(state_value,state_str[i])<0){
			state_values.clear();
			return -1;
		}
		state_values.push_back(state_value);
	}

	return 0;

}//close TangoStateStrToValue() for list


template <typename T> 
int TangoUtils::WriteAttrToGroup(Tango::DeviceImpl* dev,Tango::Group* grp,std::string attr_name,T& attr_value){

	//Check device 
	if(!dev){
		return -1;
	}

	//Check group
	if(!grp){
		__ERROR_LOG(dev,"Null ptr to input group given!");
		return -1;
	}

	//Write attr to group
	__DEBUG_LOG(dev,"Writing attr "<<attr_name<<" value "<<attr_value<<" to group...");
	try {
		Tango::DeviceAttribute da(attr_name, attr_value);
		Tango::GroupReplyList reply= grp->write_attribute(da);

		if (reply.has_failed()){
			std::stringstream ss;
			for (unsigned int i=0;i<reply.size();i++) {
				if (reply[i].has_failed()){
					ss<<"Device "<<reply[i].dev_name()<<" errors {"; 
					const Tango::DevErrorList& el = reply[i].get_err_stack();
					for (unsigned int err=0; err<el.length();err++) {
						ss<<el[err].desc.in()<<",";
					}
					ss<<"}, ";
				}//close if
			}//end loop replies
			
			__ERROR_LOG(dev,"At least one error occurred while writing attr to Group");
			Tango::Except::throw_exception( 
				(const char *)"WRITE_ATTR_TO_GROUP_FAILED",
    	  ss.str().c_str(),
    	  (const char *)"TangoUtils::WriteAttrToGroup"
			);

		}//close if has failed
		reply.reset();
	}//close try block
	catch(Tango::DevFailed& e){
		throw;
	}
	return 0;

}//close WriteAttrToGroup()


template <typename T> 
int TangoUtils::PutAttrProp(Tango::DeviceImpl* dev,std::string& attr_name,std::string prop_name,T& prop_value){

	//Check device 
	if(!dev){
		return -1;
	}

	//Put property to DB
	try {
		Tango::DbDatum mem_attr_data(attr_name);
		Tango::DbDatum mem_prop_data(prop_name);
		mem_attr_data << 1; 
		mem_prop_data << prop_value;

		Tango::DbData db_data;
		db_data.push_back(mem_attr_data); 
		db_data.push_back(mem_prop_data); 

		dev->get_db_device()->put_attribute_property(db_data);

	}//close try 
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		__ERROR_LOG(dev,"Failed to put property "<<prop_name<<" in attribute "<<attr_name<<"!");
		return -1;
	}

	return 0;

}//close PutAttrProp()


template <> 
inline int TangoUtils::PutAttrProp<std::vector<bool>>(Tango::DeviceImpl* dev,std::string& attr_name,std::string prop_name,std::vector<bool>& prop_value){

	//Check device 
	if(!dev){
		return -1;
	}

	//Create a short vector as no operators >> are available for bool vectors
	std::vector<short> _prop_value;
	for(unsigned int i=0;i<prop_value.size();i++){
		_prop_value.push_back(static_cast<short>(prop_value[i]));
	}
	
	//Put property to DB
	try {
		Tango::DbDatum mem_attr_data(attr_name);
		Tango::DbDatum mem_prop_data(prop_name);
		mem_attr_data << 1; 
		mem_prop_data << _prop_value;

		Tango::DbData db_data;
		db_data.push_back(mem_attr_data); 
		db_data.push_back(mem_prop_data); 

		dev->get_db_device()->put_attribute_property(db_data);

	}//close try 
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		__ERROR_LOG(dev,"Failed to put property "<<prop_name<<" in attribute "<<attr_name<<"!");
		return -1;
	}

	return 0;

}//close PutAttrProp()



template <typename T> 
int TangoUtils::ExtractPropValue(Tango::DeviceImpl* dev,T& prop_value,std::string& prop_name){

	//Check device & prop name given
	if(!dev){
		return -1;
	}

	if(prop_name==""){
		__ERROR_LOG(dev,"Empty prop name given!");
		return -1;
	}

	//Extract property value
	Tango::DbData	dev_prop;
	dev_prop.push_back(Tango::DbDatum(prop_name.c_str()));
	bool hasPropertyValue= false;
	try {
		dev->get_db_device()->get_property(dev_prop);
		dev_prop[0] >> prop_value; 
		hasPropertyValue= true;
	}
	catch(Tango::DevFailed&){
		__ERROR_LOG(dev,"Cannot get property "<<prop_name);
		return -1;
	}

	//If no property value is present return error
	if (!hasPropertyValue){
		__ERROR_LOG(dev,"Property "<<prop_name<<" has no value stored!");
		return -1;
	}	
	
	return 0;

}//close ExtractPropValue()


template<typename T>
inline bool TangoUtils::IsCmdAllowed(Tango::DeviceImpl* dev,std::string cmd_name,std::string attr_name,T attr_value,bool check){

	//Check device 
	if(!dev){
		return false;
	}

	//Check for cmd & attr name given
	//If the command name is not valid (e.g. passed by user mistake) check fails and cmd is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Find if command exist in device
		try{
			dev->get_device_class()->get_cmd_by_name(cmd_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			__WARN_LOG(dev,"Command "<<cmd_name<<" not found in device, no rule will be added (check cmd name given)!");
			return false;
		}

		//Check if attribute exists in device
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
			data_format= attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}

		//Check if attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			__WARN_LOG(dev,"Invalid attr data format found (cmd state machine is supported for scalar attribute only), check failed!");
			return false;
		}
	}//close if check

	//Check if given command is allowed in rules
	__DEBUG_LOG(dev,"Search if command "<<cmd_name<<" is allowed when attr "<<attr_name<<" is equal to "<<attr_value<<"...");
	bool isAllowed= Utils_ns::RuleFactory::Instance().IsCmdAllowed<T>(cmd_name,attr_name,attr_value);
	__DEBUG_LOG(dev,"Command "<<cmd_name<<" is allowed? "<<isAllowed);
	
	return isAllowed;

}//close IsCmdAllowed()

#if TANGO_VERSION_MAJOR >= 9
template<>
inline bool TangoUtils::IsCmdAllowed<Tango::DevEnum>(Tango::DeviceImpl* dev,std::string cmd_name,std::string attr_name,Tango::DevShort attr_value,bool check){

	//Check device 
	if(!dev){
		return false;
	}

	//Check for cmd & attr name given
	//If the command name is not valid (e.g. passed by user mistake) check fails and cmd is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Find if command exist in device
		try{
			dev->get_device_class()->get_cmd_by_name(cmd_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			__WARN_LOG(dev,"Command "<<cmd_name<<" not found in device, no rule will be added (check cmd name given)!");
			return false;
		}
	}//close if check


	//This is mandatory because we need to discriminate short & enum attrs		
	//Check if attribute exists in device
	Tango::AttrDataFormat data_format;
	long data_type;
	try{
		Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_format= attr.get_data_format();
		data_type= attr.get_data_type();	
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}

	//Check if attribute is not a scalar
	if(data_format!=Tango::SCALAR){
		__WARN_LOG(dev,"Invalid attr data format found (cmd state machine is supported for scalar attribute only), check failed!");
		return false;
	}

	//If enum attr get enum label
	std::string attr_value_str= "";
	if(data_type==Tango::DEV_ENUM && GetEnumAttrLabelValue(attr_value_str,dev,attr_name,attr_value)<0){
		__WARN_LOG(dev,"Failed to retrieve enum label for attribute "<<attr_name<<"!");
		return false;
	}

	//Check if given command is allowed in rules	
	//If attr is a short same procedure as above	
	//If attr is an enum get string label and compare labels
	
	bool isAllowed= false;
	if(data_type==Tango::DEV_ENUM) {
		__DEBUG_LOG(dev,"Search if command "<<cmd_name<<" is allowed when attr "<<attr_name<<" (type="<<data_type<<") is equal to "<<attr_value_str<<"...");
		isAllowed= IsCmdAllowed<std::string>(dev,cmd_name,attr_name,attr_value_str,false);
	}
	else {	
		__DEBUG_LOG(dev,"Search if command "<<cmd_name<<" is allowed when attr "<<attr_name<<" (type="<<data_type<<") is equal to "<<attr_value<<"...");
		isAllowed= IsCmdAllowed<Tango::DevShort>(dev,cmd_name,attr_name,attr_value,false);
	}

	return isAllowed;

}//close IsCmdAllowed specialization for DevShort/Enum
#endif

template<typename T>
inline bool TangoUtils::IsAttrReadAllowed(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,T sm_attr_value,bool check){

	//Check device 
	if(!dev){
		return false;
	}

	//Check for attr name given
	//If the attr name is not valid (e.g. passed by user mistake) check fails and attr is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Check if attribute exists in device
		try{
			dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}

		//Check if state machine attr exists in device
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& sm_attr= dev->get_device_attr()->get_attr_by_name(sm_attr_name.c_str());
			data_format= sm_attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"SM attribute "<<sm_attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] SM attribute "<<sm_attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}

		//Check if sm attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			__WARN_LOG(dev,"Invalid sm attr data format found (state machine attribute must be scalars), check failed!");
			return false;
		}
		
	}//close if check

	//Check if given command is allowed in rules
	__DEBUG_LOG(dev,"Search if attribute "<<attr_name<<" READ is allowed when state machine attr "<<sm_attr_name<<" is equal to "<<sm_attr_value<<"...");
	bool isAllowed= Utils_ns::RuleFactory::Instance().IsAttrReadAllowed<T>(attr_name,sm_attr_name,sm_attr_value);
	__DEBUG_LOG(dev,"Attr "<<attr_name<<" READ is allowed? "<<isAllowed);
	
	return isAllowed;

}//close IsAttrReadAllowed()

#if TANGO_VERSION_MAJOR >= 9
template<>
inline bool TangoUtils::IsAttrReadAllowed<Tango::DevEnum>(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,Tango::DevShort sm_attr_value,bool check){

	//Check device 
	if(!dev){
		return false;
	}

	//Check for attr name given
	//If the attr name is not valid (e.g. passed by user mistake) check fails and attr READ is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Check if attribute exists in device
		try{
			dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
	}//close if check


	//This is mandatory because we need to discriminate short & enum attrs		
	//Check if attribute exists in device
	Tango::AttrDataFormat data_format;
	long data_type;
	try{
		Tango::Attribute& sm_attr= dev->get_device_attr()->get_attr_by_name(sm_attr_name.c_str());
		data_format= sm_attr.get_data_format();
		data_type= sm_attr.get_data_type();	
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		__WARN_LOG(dev,"SM attribute "<<sm_attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		__WARN_LOG(dev,"[Unknown exception] SM attribute "<<sm_attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}

	//Check if attribute is not a scalar
	if(data_format!=Tango::SCALAR){
		__WARN_LOG(dev,"Invalid sm attr data format found (state machine attribute must be scalars), check failed!");
		return false;
	}
	

	//If enum attr get enum label
	std::string sm_attr_value_str= "";
	if(data_type==Tango::DEV_ENUM && GetEnumAttrLabelValue(sm_attr_value_str,dev,sm_attr_name,sm_attr_value)<0){
		__WARN_LOG(dev,"Failed to retrieve enum label for sm attribute "<<sm_attr_name<<"!");
		return false;
	}

	//Check if given command is allowed in rules	
	//If attr is a short same procedure as above	
	//If attr is an enum get string label and compare labels
	
	bool isAllowed= false;
	if(data_type==Tango::DEV_ENUM) {
		__DEBUG_LOG(dev,"Search if attr "<<attr_name<<" READ is allowed when state machine attr "<<sm_attr_name<<" (type="<<data_type<<") is equal to "<<sm_attr_value_str<<"...");
		isAllowed= IsAttrReadAllowed<std::string>(dev,attr_name,sm_attr_name,sm_attr_value_str,false);
	}
	else {	
		__DEBUG_LOG(dev,"Search if attr "<<attr_name<<" READ is allowed when attr "<<sm_attr_name<<" (type="<<data_type<<") is equal to "<<sm_attr_value<<"...");
		isAllowed= IsAttrReadAllowed<Tango::DevShort>(dev,attr_name,sm_attr_name,sm_attr_value,false);
	}

	return isAllowed;

}//close IsAttrReadAllowed specialization for DevShort/Enum
#endif

template<typename T>
inline bool TangoUtils::IsAttrWriteAllowed(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,T sm_attr_value,bool check){

	//Check device 
	if(!dev){
		return false;
	}

	//Check for attr name given
	//If the attr name is not valid (e.g. passed by user mistake) check fails and attr is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Check if attribute exists in device
		try{
			dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}

		//Check if state machine attr exists in device
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& sm_attr= dev->get_device_attr()->get_attr_by_name(sm_attr_name.c_str());
			data_format= sm_attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"SM attribute "<<sm_attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] SM attribute "<<sm_attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}

		//Check if sm attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			__WARN_LOG(dev,"Invalid sm attr data format found (state machine attribute must be scalars), check failed!");
			return false;
		}
		
	}//close if check

	//Check if given command is allowed in rules
	__DEBUG_LOG(dev,"Search if attribute "<<attr_name<<" WRITE is allowed when state machine attr "<<sm_attr_name<<" is equal to "<<sm_attr_value<<"...");
	bool isAllowed= Utils_ns::RuleFactory::Instance().IsAttrWriteAllowed<T>(attr_name,sm_attr_name,sm_attr_value);
	__DEBUG_LOG(dev,"Attr "<<attr_name<<" WRITE is allowed? "<<isAllowed);
	
	return isAllowed;

}//close IsAttrWriteAllowed()

#if TANGO_VERSION_MAJOR >= 9
template<>
inline bool TangoUtils::IsAttrWriteAllowed<Tango::DevEnum>(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,Tango::DevShort sm_attr_value,bool check){

	//Check device 
	if(!dev){
		return false;
	}

	//Check for attr name given
	//If the attr name is not valid (e.g. passed by user mistake) check fails and attr READ is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Check if attribute exists in device
		try{
			dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
	}//close if check


	//This is mandatory because we need to discriminate short & enum attrs		
	//Check if attribute exists in device
	Tango::AttrDataFormat data_format;
	long data_type;
	try{
		Tango::Attribute& sm_attr= dev->get_device_attr()->get_attr_by_name(sm_attr_name.c_str());
		data_format= sm_attr.get_data_format();
		data_type= sm_attr.get_data_type();	
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		__WARN_LOG(dev,"SM attribute "<<sm_attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		__WARN_LOG(dev,"[Unknown exception] SM attribute "<<sm_attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}

	//Check if attribute is not a scalar
	if(data_format!=Tango::SCALAR){
		__WARN_LOG(dev,"Invalid sm attr data format found (state machine attribute must be scalars), check failed!");
		return false;
	}
	

	//If enum attr get enum label
	std::string sm_attr_value_str= "";
	if(data_type==Tango::DEV_ENUM && GetEnumAttrLabelValue(sm_attr_value_str,dev,sm_attr_name,sm_attr_value)<0){
		__WARN_LOG(dev,"Failed to retrieve enum label for sm attribute "<<sm_attr_name<<"!");
		return false;
	}

	//Check if given command is allowed in rules	
	//If attr is a short same procedure as above	
	//If attr is an enum get string label and compare labels
	
	bool isAllowed= false;
	if(data_type==Tango::DEV_ENUM) {
		__DEBUG_LOG(dev,"Search if attr "<<attr_name<<" WRITE is allowed when state machine attr "<<sm_attr_name<<" (type="<<data_type<<") is equal to "<<sm_attr_value_str<<"...");
		isAllowed= IsAttrWriteAllowed<std::string>(dev,attr_name,sm_attr_name,sm_attr_value_str,false);
	}
	else {	
		__DEBUG_LOG(dev,"Search if attr "<<attr_name<<" WRITE is allowed when attr "<<sm_attr_name<<" (type="<<data_type<<") is equal to "<<sm_attr_value<<"...");
		isAllowed= IsAttrWriteAllowed<Tango::DevShort>(dev,attr_name,sm_attr_name,sm_attr_value,false);
	}

	return isAllowed;

}//close IsAttrWriteAllowed specialization for DevShort/Enum
#endif


template<typename T> 
inline bool TangoUtils::IsAttrAllowed(Tango::DeviceImpl* dev,std::string attr_name,T attr_value_current,T attr_value_new,bool check){

	//Check device 
	if(!dev){
		return false;
	}

	//Check for attr name given
	//If the attr name is not valid (e.g. passed by user mistake) check fails and attr is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Check if attribute exists in device
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
			data_format= attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}

		//Check if attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			__WARN_LOG(dev,"Invalid attr data format found (cmd state machine is supported for scalar attribute only), check failed!");
			return false;
		}
	}//close if check

	//Check if given command is allowed in rules
	return Utils_ns::RuleFactory::Instance().IsAttrAllowed<T>(attr_name,attr_value_current,attr_value_new);

}//close IsAttrAllowed()




template<typename T>
inline int TangoUtils::AddCmdRule(Tango::DeviceImpl* dev,std::string cmd_name,std::string attr_name,std::vector<T> attr_values,bool check){
		
	//Check device 
	if(!dev){
		return -1;
	}

	//Check cmd & attr for given device
	if(check){
		//Find if command exist in device
		try{
			dev->get_device_class()->get_cmd_by_name(cmd_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			__WARN_LOG(dev,"Command "<<cmd_name<<" not found in device, no rule will be added (check cmd name given)!");
			return -1;
		}

		//Check if attribute exists in device
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
			data_format= attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			__WARN_LOG(dev,"Invalid attr data format found (cmd state machine is supported for scalar attribute only), no rule will be added!");
			return -1;
		}

	}//close if check

	//Register rule in factory
	if(Utils_ns::RuleFactory::Instance().AddCmdRule<T>(cmd_name,attr_name,attr_values)<0){
		__WARN_LOG(dev,"Failed to register rule for cmd "<<cmd_name<<" in device, no rule will be added!");
		return -1;
	}

	return 0;

}//close AddCmdRule()


template<typename T>
inline int TangoUtils::AddAttrReadRule(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,std::vector<T> sm_attr_values,bool check){
		
	//Check device 
	if(!dev){
		return -1;
	}

	//Find if attribute exists in device
	if(check){

		//Check if attribute exists in device
		try{
			Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if state machine attribute exists in device
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& sm_attr= dev->get_device_attr()->get_attr_by_name(sm_attr_name.c_str());
			data_format= sm_attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<sm_attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<sm_attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if sm attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			__WARN_LOG(dev,"Invalid sm attr data format found (must be a scalar attr), no rule will be added!");
			return -1;
		}
		
	}//close if check

	//Register rule in factory
	if(Utils_ns::RuleFactory::Instance().AddAttrReadRule<T>(attr_name,sm_attr_name,sm_attr_values)<0){
		__WARN_LOG(dev,"Failed to register READ rule for attr "<<attr_name<<" in device, no rule will be added!");
		return -1;
	}

	return 0;

}//close AddAttrReadRule()


template<typename T>
inline int TangoUtils::AddAttrWriteRule(Tango::DeviceImpl* dev,std::string attr_name,std::string sm_attr_name,std::vector<T> sm_attr_values,bool check){
		
	//Check device 
	if(!dev){
		return -1;
	}

	//Find if attribute exists in device
	if(check){

		//Check if attribute exists in device and if it is a writable attr
		bool hasWriteAssociation= false;
		try{
			Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
			hasWriteAssociation= attr.is_writ_associated();
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if attribute is writable
		if(!hasWriteAssociation){
			__WARN_LOG(dev,"Attr "<<attr_name<<" is not a writable attribute, no WRITE rule will be added!");
			return -1;
		}

		//Check if state machine attr exists in device and if it is a scalar
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& sm_attr= dev->get_device_attr()->get_attr_by_name(sm_attr_name.c_str());
			data_format= sm_attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<sm_attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<sm_attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if sm attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			__WARN_LOG(dev,"Invalid sm attr data format found (must be a scalar), no rule will be added!");
			return -1;
		}
		
	}//close if check

	//Register rule in factory
	if(Utils_ns::RuleFactory::Instance().AddAttrWriteRule<T>(attr_name,sm_attr_name,sm_attr_values)<0){
		__WARN_LOG(dev,"Failed to register WRITE rule for attr "<<attr_name<<" in device, no rule will be added!");
		return -1;
	}

	return 0;

}//close AddAttrWriteRule()


template<typename T>
inline int TangoUtils::AddAttrRule(Tango::DeviceImpl* dev,std::string attr_name,T attr_value,std::vector<T> attr_values,bool check){
		
	//Check device 
	if(!dev){
		return -1;
	}

	//Find if attribute exists in device
	if(check){
		//long data_type;
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& attr= dev->get_device_attr()->get_attr_by_name(attr_name.c_str());
			//data_type= attr.get_data_type();
			data_format= attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			__WARN_LOG(dev,"[Unknown exception] Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			__WARN_LOG(dev,"Invalid attr data format found (attr state machine is supported for scalar attribute only), no rule will be added!");
			return -1;
		}
	}//close if check

	//Register rule in factory
	if(Utils_ns::RuleFactory::Instance().AddAttrRule<T>(attr_name,attr_value,attr_values)<0){
		__WARN_LOG(dev,"Failed to register rule for attr "<<attr_name<<" in device, no rule will be added!");
		return -1;
	}

	return 0;

}//close AddAttrRule()


