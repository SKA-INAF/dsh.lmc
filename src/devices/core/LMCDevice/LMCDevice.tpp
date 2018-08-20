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
* @file LMCDevice.tpp
* @class LMCDevice
* @brief LMCDevice 
*
* LMCDevice
* @author S. Riggi
* @date 15/01/2017
*/

template<typename T>
inline int LMCDevice::GetDynAttrValue(T& attr_value,std::string attr_name){

	try {
		//Retrieve attr
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);
		int data_format= sp.get_tango_data_format();
		
		//Check attr type & format
		if(data_format!=Tango::SCALAR){
			std::stringstream ss;
			ss<<"Invalid attr format and/or type for "<<attr_name<<" (expected scalar)!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("GET_DYN_ATTR_FAILED",ss.str());	
		}

		//Get attr data
		Utils_ns::ScalarAttrData<T>* attr_data = dynamic_cast<Utils_ns::ScalarAttrData<T>*>(sp.get_user_data<Utils_ns::AttrData>());
		attr_value= attr_data->get_data();

	}//close try block
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr not found?)!");
		return -1;
	}
	catch(...){
		_ERROR_LOG("Failed to get dyn attribute "<<attr_name<<" from manager (C++ exception)!");
		return -1;
	}
	
	return 0;

}//close GetDynAttrValue()

#if TANGO_VERSION_MAJOR >= 9
template<>
inline int LMCDevice::GetDynAttrValue<Tango::DevEnum>(Tango::DevShort& attr_value,std::string attr_name){

	try {
		//Retrieve attr from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);
		int data_format= sp.get_tango_data_format();
		int data_type= sp.get_tango_data_type();

		//Check attr type & format
		if(data_format!=Tango::SCALAR || data_type!=Tango::DEV_ENUM){
			std::stringstream ss;
			ss<<"Invalid attr format and/or type for "<<attr_name<<" (expected scalar+enum)!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("GET_DYN_ATTR_FAILED",ss.str());
		}

		//Get attr data
		Utils_ns::ScalarAttrData<Tango::DevShort>* attr_data = dynamic_cast<Utils_ns::ScalarAttrData<Tango::DevShort>*>(sp.get_user_data<Utils_ns::AttrData>());	
		if(!attr_data){
			std::stringstream ss;
			ss<<"Failed to retrieve attr data for attr "<<attr_name<<"!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("GET_DYN_ATTR_FAILED",ss.str());	
		}
		attr_value= attr_data->get_data();

	}//close try block
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr invalid or not found?)!");
		return -1;
	}
	catch(...){
		_ERROR_LOG("Failed to get dyn attribute "<<attr_name<<" from manager (C++ exception)!");
		return -1;
	}
	
	return 0;

}//close GetDynAttrValue specialization for enum attr
#endif

template<typename T>
inline int LMCDevice::GetDynAttrValue(std::vector<T>& attr_values,std::string attr_name){

	//Clear input vector
	attr_values.clear();

	try {
		//Retrieve attr from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);
		int data_format= sp.get_tango_data_format();
		//int data_type= sp.get_tango_data_type();
		
		//Check attr type & format
		if(data_format!=Tango::SPECTRUM){
			std::stringstream ss;
			ss<<"Invalid attr format and/or type for "<<attr_name<<" (expected spectrum)!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("GET_DYN_ATTR_FAILED",ss.str());
		}
	
		//Get attr value and copy to input vector
		//void* buf_addr = 0;
  	//size_t buf_xdim = 0;
  	//size_t buf_ydim = 0;
		Utils_ns::SpectrumAttrData<T>* attr_data = dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(sp.get_user_data<Utils_ns::AttrData>());
		//attr_data->get_value(buf_addr, buf_xdim, buf_ydim);
		//attr_values= std::vector<T>(reinterpret_cast<T*>(buf_addr),buf_xdim);
		attr_values= attr_data->get_data();
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr invalid or not found?)!");
		return -1;
	}
	catch(...){
		_ERROR_LOG("Failed to get dyn attribute "<<attr_name<<" from manager (C++ exception)!");
		return -1;
	}

	return 0;

}//close GetDynAttrValue() spectrum attrs

/*
template<>
inline int LMCDevice::GetDynAttrValue<Tango::DevBoolean>(std::vector<bool>& attr_values,std::string attr_name){

	//Clear input vector
	attr_values.clear();

	try {
		//Retrieve attribute from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);
		int data_format= sp.get_tango_data_format();
		int data_type= sp.get_tango_data_type();

		//Check attr type & format
		if(data_format!=Tango::SPECTRUM || data_type!=Tango::DEV_BOOLEAN){
			std::stringstream ss;
			ss<<"Invalid attr format and/or type for "<<attr_name<<" (expected spectrum+bool)!";
			_ERROR_LOG(ss.str());
			Tango::Except::throw_exception( 
	 			(const char *)"GET_DYN_ATTR_FAILED",
	    	ss.str().c_str(),
	   		(const char *)"LMCDevice::GetDynAttrValue"
	 		);	
		}

		//Get attr value and copy to input vector
		//void* buf_addr = 0;
  	//size_t buf_xdim = 0;
  	//size_t buf_ydim = 0;
		Utils_ns::SpectrumAttrData<bool>* attr_data = dynamic_cast<Utils_ns::SpectrumAttrData<bool>*>(sp.get_user_data<Utils_ns::AttrData>());
		//attr_data->get_value(buf_addr, buf_xdim, buf_ydim);
		//for(unsigned int i=0;i<buf_xdim;i++){
		//	bool attr_value= *(reinterpret_cast<bool*>(buf_addr)+i);
		//	attr_values.push_back(attr_value);	
		//}
		attr_values= attr_data->get_data();
	}//close try block
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr invalid or not found?)!");
		return -1;
	}
	catch(...){
		_ERROR_LOG("Failed to get dyn attribute "<<attr_name<<" from manager (C++ exception)!");
		return -1;
	}

	return 0;

}//close GetDynAttrValue() spectrum attrs specialization for bool
*/

//========================================================================
//===                SETTER
//========================================================================

template<typename T>
inline void LMCDevice::SetScalarDynAttr(yat4tango::DynamicAttribute* dya,Tango::Attribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_ATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::ScalarAttrData<T>* ad = dynamic_cast<Utils_ns::ScalarAttrData<T>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn attribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_ATTR_FAILED",ss.str());
		}

		//If attribute has formula force re-evaluation of formula before getting data values
		//NB: This is done to automatically (i.e. by the internal attr polling) switch attr quality factor if any of the attr var proxy goes offline for long times
		//if(ad->has_formula()) SetFormulaScalarDynAttrValue<T>(dya->get_name());

		//Retrieve data/quality/date
		T attr_data= ad->get_data();
		timeval t= Utils_ns::TangoUtils::GetTimeNow();//This is the read time (do not touch the internal attr data timestamp)
		Tango::AttrQuality attr_quality= ad->get_data_quality();

		//_INFO_LOG("Attr "<<dya->get_name()<<" READ: value="<<attr_data<<", quality="<<attr_quality);

		//Set value
		//NB: If quality stored in dyn attr is INVALID force date quality set, otherwise use set_value(...) method
		//    This is because if I use only the set_value_date_quality(...) method the quality is not switched back from ALARM/WARN to NORMAL (quality is 
		//    calculated in the read polling.  
		T* attr_data_copy= new T;
		*attr_data_copy= attr_data;
		
		if(attr_quality==Tango::ATTR_INVALID) {
			_DEBUG_LOG("Setting attr "<<dya->get_name()<<" quality factor to ATTR_INVALID!");
			tga->set_value_date_quality(attr_data_copy,t,attr_quality,1,0,true);
		}
		else {
			tga->set_value(attr_data_copy,1,0,true);
		}

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetScalarDynAttr()

template<>
inline void LMCDevice::SetScalarDynAttr<std::string>(yat4tango::DynamicAttribute* dya,Tango::Attribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_ATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::ScalarAttrData<std::string>* ad = dynamic_cast<Utils_ns::ScalarAttrData<std::string>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn attribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_ATTR_FAILED",ss.str());
		}

		//If attribute has formula force re-evaluation of formula before getting data values
		//NB: This is done to automatically (i.e. by the internal attr polling) switch attr quality factor if any of the attr var proxy goes offline for long times
		//if(ad->has_formula()) SetFormulaScalarDynAttrValue<std::string>(dya->get_name());

		//Get attr data and set 
		std::string attr_data= ad->get_data();
		timeval t= Utils_ns::TangoUtils::GetTimeNow();//This is the read time (do not touch the internal attr data timestamp)
		Tango::AttrQuality attr_quality= ad->get_data_quality();

		//Convert to Tango string
		Tango::DevString* attr_value= new Tango::DevString;
		*attr_value= Tango::string_dup( attr_data.c_str() );

		//Set attr value
		//NB: If quality stored in dyn attr is INVALID force date quality set, otherwise use set_value(...) method
		//    This is because if I use only the set_value_date_quality(...) method the quality is not switched back from ALARM/WARN to NORMAL (quality is 
		//    calculated in the read polling.  
		if(attr_quality==Tango::ATTR_INVALID) {
			_WARN_LOG("Setting attr "<<dya->get_name()<<" quality factor to ATTR_INVALID!");
			tga->set_value_date_quality(attr_value,t,attr_quality,1,0,true);
		}
		else {
			tga->set_value(attr_value,1,0,true);
		}

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetScalarDynAttr spec for string()


template<typename T>
inline void LMCDevice::SetSpectrumDynAttr(yat4tango::DynamicAttribute* dya,Tango::Attribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::SpectrumAttrData<T>* ad = dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn attribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_FAILED",ss.str());
		}

		//If attribute has formula force re-evaluation of formula before getting data values
		//NB: This is done to automatically (i.e. by the internal attr polling) switch attr quality factor if any of the attr var proxy goes offline for long times
		//if(ad->has_formula()) SetFormulaSpectrumDynAttrValue<T>(dya->get_name(),ad->get_data_size());

		//Get attr data
		std::vector<T> attr_data= ad->get_data();
		timeval t= Utils_ns::TangoUtils::GetTimeNow();//This is the read time (do not touch the internal attr data timestamp)
		Tango::AttrQuality attr_quality= ad->get_data_quality();
		size_t n= attr_data.size();  

		//Set attr value
		//NB: If quality stored in dyn attr is INVALID force date quality set, otherwise use set_value(...) method
		//    This is because if I use only the set_value_date_quality(...) method the quality is not switched back from ALARM/WARN to NORMAL (quality is 
		//    calculated in the read polling. 
		if(attr_quality==Tango::ATTR_INVALID) {
			_DEBUG_LOG("Setting attr "<<dya->get_name()<<" quality factor to ATTR_INVALID!");
			tga->set_value_date_quality(attr_data.data(),t,attr_quality,n,0,false);
		}
		else {
			tga->set_value(attr_data.data(),n,0,false);
		}

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetSpectrumDynAttr()


template<>
inline void LMCDevice::SetSpectrumDynAttr<Tango::DevBoolean>(yat4tango::DynamicAttribute* dya,Tango::Attribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::SpectrumAttrData<Tango::DevBoolean>* ad = dynamic_cast<Utils_ns::SpectrumAttrData<Tango::DevBoolean>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn attribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_FAILED",ss.str());
		}

		//If attribute has formula force re-evaluation of formula before getting data values
		//NB: This is done to automatically (i.e. by the internal attr polling) switch attr quality factor if any of the attr var proxy goes offline for long times
		//if(ad->has_formula()) SetFormulaSpectrumDynAttrValue<Tango::DevBoolean>(dya->get_name(),ad->get_data_size());

		//Get attr data
		std::vector<bool> attr_data= ad->get_data();
		timeval t= Utils_ns::TangoUtils::GetTimeNow();//This is the read time (do not touch the internal attr data timestamp)
		Tango::AttrQuality attr_quality= ad->get_data_quality();
		size_t n= attr_data.size();  
	
		//Fill buffer
		Tango::DevBoolean* attr_values= new Tango::DevBoolean[n];
		for(unsigned int i=0;i<n;i++) *(attr_values+i)= attr_data[i];

		//Set attr value
		//NB: If quality stored in dyn attr is INVALID force date quality set, otherwise use set_value(...) method
		//    This is because if I use only the set_value_date_quality(...) method the quality is not switched back from ALARM/WARN to NORMAL (quality is 
		//    calculated in the read polling
		if(attr_quality==Tango::ATTR_INVALID) {
			_DEBUG_LOG("Setting attr "<<dya->get_name()<<" quality factor to ATTR_INVALID!");
			tga->set_value_date_quality(attr_values,t,attr_quality,n,0,true);
		}
		else {
			tga->set_value(attr_values,n,0,true);
		}

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetSpectrumDynAttr()

template<>
inline void LMCDevice::SetSpectrumDynAttr<std::string>(yat4tango::DynamicAttribute* dya,Tango::Attribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::SpectrumAttrData<std::string>* ad = dynamic_cast<Utils_ns::SpectrumAttrData<std::string>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn attribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_FAILED",ss.str());
		}

		//If attribute has formula force re-evaluation of formula before getting data values
		//NB: This is done to automatically (i.e. by the internal attr polling) switch attr quality factor if any of the attr var proxy goes offline for long times
		//if(ad->has_formula()) SetFormulaSpectrumDynAttrValue<std::string>(dya->get_name(),ad->get_data_size());

		//Get attr data and set 
		std::vector<std::string> attr_data= ad->get_data();
		timeval t= Utils_ns::TangoUtils::GetTimeNow();//This is the read time (do not touch the internal attr data timestamp)
		Tango::AttrQuality attr_quality= ad->get_data_quality();
		size_t n= attr_data.size();  
	
		//Fill buffer
		Tango::DevString* attr_values= new Tango::DevString[n];
		for(unsigned int i=0;i<n;i++) attr_values[i]= Tango::string_dup(attr_data[i].c_str());

		//Set attr value
		//NB: If quality stored in dyn attr is INVALID force date quality set, otherwise use set_value(...) method
		//    This is because if I use only the set_value_date_quality(...) method the quality is not switched back from ALARM/WARN to NORMAL (quality is 
		//    calculated in the read polling
		if(attr_quality==Tango::ATTR_INVALID) {
			_DEBUG_LOG("Setting attr "<<dya->get_name()<<" quality factor to ATTR_INVALID!");
			tga->set_value_date_quality(attr_values,t,attr_quality,n,0,true);
		}
		else {
			tga->set_value(attr_values,n,0,true);
		}

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetSpectrumDynAttr()


template<typename T>
inline void LMCDevice::SetScalarDynWAttr(yat4tango::DynamicAttribute* dya,Tango::WAttribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_WATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::ScalarAttrData<T>* ad = dynamic_cast<Utils_ns::ScalarAttrData<T>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn wattribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_WATTR_FAILED",ss.str());
		}

		//Get write attr value and set to dyn attr 
		T w_val;
		tga->get_write_value(w_val);
		timeval w_t= tga->get_write_date();
		Tango::TimeVal w_timestamp;
		Utils_ns::TangoUtils::TimevalToTangoTimestamp(w_timestamp,w_t);
		Tango::AttrQuality w_quality= tga->get_quality();

		//_INFO_LOG("Attr "<<dya->get_name()<<" WRITE: value="<<w_val<<", quality="<<w_quality);

		if(ad->set_data(w_val,w_quality)<0){//data are copied internally
			std::stringstream ss;
			ss<<"Failed to set dyn wattribute "<<dya->get_name()<<" value from wval!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_WATTR_FAILED",ss.str());
		}
		ad->set_timestamp(w_timestamp);

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetScalarDynWAttr()

template<>
inline void LMCDevice::SetScalarDynWAttr<std::string>(yat4tango::DynamicAttribute* dya,Tango::WAttribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_WATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::ScalarAttrData<std::string>* ad = dynamic_cast<Utils_ns::ScalarAttrData<std::string>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn wattribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_WATTR_FAILED",ss.str());
		}

		//Get write attr value and set to dyn attr 
		Tango::DevString w_val;
		tga->get_write_value(w_val);
		std::string attr_data(w_val);

		timeval w_t= tga->get_write_date();
		Tango::TimeVal w_timestamp;
		Utils_ns::TangoUtils::TimevalToTangoTimestamp(w_timestamp,w_t);
		Tango::AttrQuality w_quality= tga->get_quality();

		if(ad->set_data(attr_data,w_quality)<0){//data are copied internally
			std::stringstream ss;
			ss<<"Failed to set dyn wattribute "<<dya->get_name()<<" value from wval!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_WATTR_FAILED",ss.str());
		}
		ad->set_timestamp(w_timestamp);

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetScalarDynWAttr() specialization for string


template<typename T>
inline void LMCDevice::SetSpectrumDynWAttr(yat4tango::DynamicAttribute* dya,Tango::WAttribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_WATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::SpectrumAttrData<T>* ad = dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn wattribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_WATTR_FAILED",ss.str());
		}

		//Get write attr value and set to dyn attr 
		const T* w_val;
		tga->get_write_value(w_val);

		timeval w_t= tga->get_write_date();
		Tango::TimeVal w_timestamp;
		Utils_ns::TangoUtils::TimevalToTangoTimestamp(w_timestamp,w_t);
		Tango::AttrQuality w_quality= tga->get_quality();

		long n= tga->get_write_value_length();

		if(ad->set_data(w_val,n,w_quality)<0){//data are copied internally
			std::stringstream ss;
			ss<<"Failed to set dyn wattribute "<<dya->get_name()<<" value from buffer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_WATTR_FAILED",ss.str());
		}
		ad->set_timestamp(w_timestamp);

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetSpectrumDynWAttr()


template<>
inline void LMCDevice::SetSpectrumDynWAttr<std::string>(yat4tango::DynamicAttribute* dya,Tango::WAttribute* tga){

	//Check args
	if(!dya || !tga){
		std::stringstream ss;
		ss<<"Null ptr to input dyn attr, failed to set "<<dya->get_name()<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_WATTR_FAILED",ss.str());
	}

	//Set attr value
	try{
		//Get attribute from manager
		Utils_ns::SpectrumAttrData<std::string>* ad = dynamic_cast<Utils_ns::SpectrumAttrData<std::string>*>(dya->get_user_data<Utils_ns::AttrData>());	
		if(!ad) {
			std::stringstream ss;
			ss<<"Failed to get dyn wattribute "<<dya->get_name()<<" pointer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_WATTR_FAILED",ss.str());
		}

		//Get write attr value and set to dyn attr 
		const Tango::ConstDevString* w_val;
		tga->get_write_value(w_val);
	
		timeval w_t= tga->get_write_date();
		Tango::TimeVal w_timestamp;
		Utils_ns::TangoUtils::TimevalToTangoTimestamp(w_timestamp,w_t);
		Tango::AttrQuality w_quality= tga->get_quality();

		long n= tga->get_write_value_length();
		std::vector<std::string> attr_values;
		for(int i=0;i<n;i++) attr_values.push_back( std::string(w_val[i]) );
		
		if(ad->set_data(attr_values,w_quality)<0){//data are copied internally
			std::stringstream ss;
			ss<<"Failed to set dyn wattribute "<<dya->get_name()<<" value from buffer!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_WATTR_FAILED",ss.str());
		}
		ad->set_timestamp(w_timestamp);

	}//close try
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(std::exception& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetSpectrumDynWAttr() specialization for string


template<typename T>
inline void LMCDevice::SetDynAttrValueInManager(T& attr_value,std::string attr_name,Tango::AttrQuality attr_quality){

	//Set attribute value in attr manager
	try {
		//Get attribute from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);

		//Set data in manager
		Utils_ns::ScalarAttrData<T>* attr_data = dynamic_cast<Utils_ns::ScalarAttrData<T>*>(sp.get_user_data<Utils_ns::AttrData>());
		if(attr_data->set_data(attr_value,attr_quality)<0){
			std::stringstream ss;
			ss<<"Failed to set value of attr "<<attr_name<<" internally!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_VALUE_IN_MANAGER_FAILED",ss.str());
		}

	}//close try
	catch(...){
		throw;
	}

}//close SetDynAttrValueInManager() for scalar attrs


template<>
inline void LMCDevice::SetDynAttrValueInManager<Tango::DevEnum>(Tango::DevShort& attr_value,std::string attr_name,Tango::AttrQuality attr_quality){

	//Check if value is allowed according to enum labels
	try {
		Tango::MultiAttrProp<Tango::DevEnum> multi_attr_prop;
		get_device_attr()->get_attr_by_name(attr_name.c_str()).get_properties(multi_attr_prop);
		std::vector<std::string> enum_labels= multi_attr_prop.enum_labels;
		if(!enum_labels.empty() && (attr_value<0 || attr_value>=(int)enum_labels.size()) ){//in case of DevShort this is empty so no check is required (TBC)
			std::stringstream ss;
			ss<<"Invalid attr value ("<<attr_value<<") given, valid enum labels are in range [0,"<<enum_labels.size()-1<<")!";
			_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_VALUE_IN_MANAGER_FAILED",ss.str());
		}
	}//close try
	catch(...){
		throw;
	}

	//Set attribute value in attr manager
	try {
		//Get attribute from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);

		//Set data in manager
		Utils_ns::ScalarAttrData<Tango::DevShort>* attr_data = dynamic_cast<Utils_ns::ScalarAttrData<Tango::DevShort>*>(sp.get_user_data<Utils_ns::AttrData>());
		if(attr_data->set_data(attr_value,attr_quality)<0){
			std::stringstream ss;
			ss<<"Failed to set value of attr "<<attr_name<<" internally!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_VALUE_IN_MANAGER_FAILED",ss.str());
		}

	}//close try
	catch(...){
		throw;
	}
	
}//close SetDynAttrValueInManager() for scalar attrs (specialization for enum)



template<typename T>
inline void LMCDevice::SetDynAttrValueInManager(std::vector<T>& attr_value,std::string attr_name,Tango::AttrQuality attr_quality){

	//Set attribute value in attr manager
	try {
		//Get attribute from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);

		//Set data in manager
		Utils_ns::SpectrumAttrData<T>* attr_data = dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(sp.get_user_data<Utils_ns::AttrData>());
		if(attr_data->set_data(attr_value,attr_quality)<0){
			std::stringstream ss;
			ss<<"Failed to set value of attr "<<attr_name<<" internally!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_VALUE_IN_MANAGER_FAILED",ss.str());
		}
	}//close try block
	catch(...){
		throw;
	}

}//close SetDynAttrValueInManager() for spectrum attrs


template<>
inline void LMCDevice::SetDynAttrValueInManager<Tango::DevEnum>(std::vector<Tango::DevShort>& attr_value,std::string attr_name,Tango::AttrQuality attr_quality){

	//Check if valid enum are given
	try {
		Tango::MultiAttrProp<Tango::DevEnum> multi_attr_prop;
		get_device_attr()->get_attr_by_name(attr_name.c_str()).get_properties(multi_attr_prop);
		std::vector<std::string> enum_labels= multi_attr_prop.enum_labels;
	
		if(!enum_labels.empty()){
			for(unsigned int i=0;i<attr_value.size();i++){
				if(attr_value[i]<0 || attr_value[i]>=(int)enum_labels.size()){
					std::stringstream ss;
					ss<<"Invalid attr value ("<<attr_value[i]<<") given, valid enum labels are in range [0,"<<enum_labels.size()-1<<")!";
					_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_VALUE_IN_MANAGER_FAILED",ss.str());
				}
			}//end loop values
		}//close if
	}//close try
	catch(...){
		throw;
	}	

	//Set attribute value in attr manager
	try {
		//Get attribute from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);

		//Set data in manager
		Utils_ns::SpectrumAttrData<Tango::DevShort>* attr_data = dynamic_cast<Utils_ns::SpectrumAttrData<Tango::DevShort>*>(sp.get_user_data<Utils_ns::AttrData>());
		if(attr_data->set_data(attr_value,attr_quality)<0){
			std::stringstream ss;
			ss<<"Failed to set value of attr "<<attr_name<<" internally!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_VALUE_IN_MANAGER_FAILED",ss.str());
		}
	}//close try block
	catch(...){
		throw;
	}

}//close SetDynAttrValueInManager() for spectrum attrs (specialization for enum)

template<typename T>
inline void LMCDevice::SetDynAttrSafeValueInManager(T& attr_safe_value_set,std::string attr_name,bool changeAttrQuality,Tango::AttrQuality attr_quality){

	//Set attribute value in attr manager
	try {
		//Get attribute from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);

		//Set data in manager
		Utils_ns::ScalarAttrData<T>* attr_data = dynamic_cast<Utils_ns::ScalarAttrData<T>*>(sp.get_user_data<Utils_ns::AttrData>());
		if(changeAttrQuality){
			if(attr_data->set_data_to_safe(attr_safe_value_set,attr_quality)<0){
				std::stringstream ss;
				ss<<"Failed to set safe value of attr "<<attr_name<<" internally!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_SAFE_VALUE_IN_MANAGER_FAILED",ss.str());
			}		
		}
		else{
			if(attr_data->set_data_to_safe(attr_safe_value_set)<0){
				std::stringstream ss;
				ss<<"Failed to set safe value of attr "<<attr_name<<" internally!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_SAFE_VALUE_IN_MANAGER_FAILED",ss.str());
			}
		}
	}//close try
	catch(...){
		throw;
	}

}//close SetDynAttrSafeValueInManager() for scalar attrs


template<typename T>
inline void LMCDevice::SetDynAttrSafeValueInManager(std::vector<T>& attr_safe_value_set,std::string attr_name,bool changeAttrQuality,Tango::AttrQuality attr_quality){

	//Set attribute value in attr manager
	try {
		//Get attribute from manager
		yat4tango::DynamicAttribute & sp = m_dam->get_attribute(attr_name);

		//Set data in manager
		Utils_ns::SpectrumAttrData<T>* attr_data = dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(sp.get_user_data<Utils_ns::AttrData>());
		if(changeAttrQuality){
			if(attr_data->set_data_to_safe(attr_safe_value_set,attr_quality)<0){
				std::stringstream ss;
				ss<<"Failed to set safe value of attr "<<attr_name<<" internally!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_SAFE_VALUE_IN_MANAGER_FAILED",ss.str());
			}
		}
		else{
			if(attr_data->set_data_to_safe(attr_safe_value_set)<0){
				std::stringstream ss;
				ss<<"Failed to set safe value of attr "<<attr_name<<" internally!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("SET_DYN_ATTR_SAFE_VALUE_IN_MANAGER_FAILED",ss.str());
			}
		}
	}//close try block
	catch(...){
		throw;
	}

}//close SetDynAttrSafeValueInManager() for spectrum attrs


template<typename T>
inline void LMCDevice::SetWAttrValue(T& attr_value,std::string attr_name,Tango::AttrQuality attr_quality,bool isMemorized){

	//Write value in wattribute in device
	try{
		Tango::WAttribute& wattr= get_device_attr()->get_w_attr_by_name(attr_name.c_str());
		wattr.set_write_value(attr_value);
		wattr.set_quality(attr_quality);

		//Fill __value property in DB	
		if( isMemorized && Utils_ns::TangoUtils::PutAttrProp<T>(this,attr_name,"__value",attr_value)<0 ){
			std::stringstream ss;
			ss<<"Failed to put __value property for attr "<<attr_name<<" in DB!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_W_ATTR_VALUE_FAILED",ss.str());
		}	
	}//close try
	catch(...){
		throw;
	}

}//close SetWAttrValue()

template<typename T>
inline void LMCDevice::SetWAttrValue(std::vector<T>& attr_value,std::string attr_name,Tango::AttrQuality attr_quality,bool isMemorized){

	//Write value in wattribute in device
	long attr_value_size= static_cast<long>(attr_value.size());
	try{
		Tango::WAttribute& wattr= get_device_attr()->get_w_attr_by_name(attr_name.c_str());
		wattr.set_write_value(attr_value,attr_value_size);
		wattr.set_quality(attr_quality);
	}
	catch(...){
		throw;
	}

}//close SetWAttrValue()

template<typename T>
inline void LMCDevice::PushUserEvent(T& attr_value,std::string attr_name){

	std::vector<std::string> filt_names;
	std::vector<double> filt_vals;
	bool release = false;
	try{
		push_event(attr_name,filt_names,filt_vals,&attr_value,1,0,release);
	}
	catch(...){
		throw;
	}

}//close PushUserEvent() for scalar attrs

template<>
inline void LMCDevice::PushUserEvent<std::string>(std::string& attr_value,std::string attr_name){

	std::vector<std::string> filt_names;
	std::vector<double> filt_vals;
	bool release = true;//in this case ask to release memory since we are copying data here	
	try{
		Tango::DevString* attr_value_str= new Tango::DevString;
		*attr_value_str= CORBA::string_dup(attr_value.c_str());
		push_event(attr_name,filt_names,filt_vals,attr_value_str,1,0,release);
	}
	catch(...){
		throw;
	}

}//close PushUserEvent() for scalar attrs (specialization for string)


template<typename T>
inline void LMCDevice::PushUserEvent(std::vector<T>& attr_value,std::string attr_name){

	long attr_value_size= static_cast<long>(attr_value.size());
	std::vector<std::string> filt_names;
	std::vector<double> filt_vals;
	bool release = false;
	try{
		push_event(attr_name,filt_names,filt_vals,attr_value.data(),attr_value_size,0,release);
	}
	catch(...){
		throw;
	}

}//close PushUserEvent() for spectrum attrs

template<>
inline void LMCDevice::PushUserEvent<std::string>(std::vector<std::string>& attr_value,std::string attr_name){

	long attr_value_size= static_cast<long>(attr_value.size());
	std::vector<std::string> filt_names;
	std::vector<double> filt_vals;
	bool release = true;//in this case ask to release memory since we are copying data here	
	try{
		Tango::DevString* attr_value_str= new Tango::DevString[attr_value_size];
		for(unsigned int i=0;i<attr_value.size();i++){
			*(attr_value_str+i)= CORBA::string_dup(attr_value[i].c_str());
		}
		push_event(attr_name,filt_names,filt_vals,attr_value_str,attr_value_size,0,release);
	}
	catch(...){
		throw;
	}

}//close PushUserEvent() for spectrum attrs (specialization for string)

template<>
inline void LMCDevice::PushUserEvent<Tango::DevBoolean>(std::vector<Tango::DevBoolean>& attr_value,std::string attr_name){

	long attr_value_size= static_cast<long>(attr_value.size());
	std::vector<std::string> filt_names;
	std::vector<double> filt_vals;
	bool release = true;//in this case ask to release memory since we are copying data here	
	try{
		Tango::DevBoolean* attr_value_bool= new Tango::DevBoolean[attr_value_size];
		for(unsigned int i=0;i<attr_value.size();i++){
			*(attr_value_bool+i)= attr_value[i];
		}
		push_event(attr_name,filt_names,filt_vals,attr_value_bool,attr_value_size,0,release);
	}
	catch(...){
		throw;
	}

}//close PushUserEvent() for spectrum attrs (specialization for bool)

template<typename T>
inline int LMCDevice::SetDynAttrValue(T& attr_value,std::string attr_name,Tango::AttrQuality attr_quality,bool emit_event){

	//Check if attribute exists
	Tango::AttrDataFormat data_format;
	bool isWritable= false;
	bool isMemorized= false;
	try{
		Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_format= attr.get_data_format();
		isWritable= attr.is_writ_associated();
		if(isWritable){
			isMemorized= get_device_attr()->get_w_attr_by_name(attr_name.c_str()).is_memorized();
		}
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		_ERROR_LOG("Attribute "<<attr_name<<" does not exist in device!");
		return -1;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		_ERROR_LOG("Attribute "<<attr_name<<" does not exist in device!");
		return -1;
	}
	
	//Check if attribute is scalar
	if(data_format!=Tango::SCALAR){
		_ERROR_LOG("Invoked method is only for scalar attribute!");
		return -1;
	}

	//Set attribute value in attr manager
	try{
		SetDynAttrValueInManager<T>(attr_value,attr_name,attr_quality);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr not found?)!");
		return -1;
	}
	catch(...){
		_ERROR_LOG("Failed to get dyn attribute "<<attr_name<<" from manager (C++ exception)!");
		return -1;
	}

	//If writable attr set write value
	if(isWritable){
		try{
			SetWAttrValue<T>(attr_value,attr_name,attr_quality,isMemorized);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_ERROR_LOG("Failed to set writable dyn attribute "<<attr_name<<" value from manager (attr not found?)!");
			return -1;
		}
		catch(...){
			_ERROR_LOG("Failed to get writable dyn attribute "<<attr_name<<" from manager (C++ exception)!");
			return -1;
		}
	}//close if

	// Push event?
	if(emit_event){
		try{
			PushUserEvent<T>(attr_value,attr_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_ERROR_LOG("Failed to push event for dyn attribute "<<attr_name<<" value!");
			return -1;
		}
		catch(...){
			_ERROR_LOG("Failed to push event for dyn attribute "<<attr_name<<" value (C++ exception)!");
			return -1;
		}
	}

	return 0;

}//close SetDynAttrValue()



template<typename T>
inline int LMCDevice::SetDynAttrValue(std::vector<T>& attr_value,std::string attr_name,Tango::AttrQuality attr_quality,bool emit_event){

	//Check if attribute exists
	Tango::AttrDataFormat data_format;	
	long max_data_size;
	bool isWritable= false;	
	try{
		Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_format= attr.get_data_format();
		max_data_size= attr.get_max_dim_x();
		isWritable= attr.is_writ_associated();
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		_ERROR_LOG("Attribute "<<attr_name<<" does not exist in device!");
		return -1;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		_ERROR_LOG("Attribute "<<attr_name<<" does not exist in device!");
		return -1;
	}

	//Check if attribute is spectrum
	if(data_format!=Tango::SPECTRUM){
		_ERROR_LOG("Invoked method is only for spectrum attribute!");
		return -1;
	}

	//Check data size given
	long attr_value_size= static_cast<long>(attr_value.size());
	if(attr_value_size>max_data_size){
		_ERROR_LOG("Given data size (size="<<attr_value_size<<") larger than maximum allocated attr dim (dim="<<max_data_size<<")");
		return -1;
	}

	//Set attribute value in attr manager
	try {
		SetDynAttrValueInManager(attr_value,attr_name,attr_quality);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr not found?)!");
		return -1;
	}
	catch(...){
		_ERROR_LOG("Failed to get dyn attribute "<<attr_name<<" from manager (C++ exception)!");
		return -1;
	}

	//If writable attr set write value (spectrum attr cannot be memorized in Tango)
	if(isWritable){
		try{
			SetWAttrValue(attr_value,attr_name,attr_quality,false);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_ERROR_LOG("Failed to set writable dyn attribute "<<attr_name<<" value from manager (attr not found?)!");
			return -1;
		}
		catch(...){
			_ERROR_LOG("Failed to get writable dyn attribute "<<attr_name<<" from manager (C++ exception)!");
			return -1;
		}
	}

	// Push event?
	if(emit_event){
		try{
			PushUserEvent(attr_value,attr_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_ERROR_LOG("Failed to push event for dyn attribute "<<attr_name<<" value!");
			return -1;
		}
		catch(...){
			_ERROR_LOG("Failed to push event for dyn attribute "<<attr_name<<" value (C++ exception)!");
			return -1;
		}
	}

	return 0;

}//close SetDynAttrValue() for spectrum attrs




template<typename T>
inline void LMCDevice::SetScalarDynAttrToSafeValue(std::string attr_name,bool changeAttrQuality,Tango::AttrQuality attr_quality,bool emit_event){

	//Check if attribute exists
	Tango::AttrDataFormat data_format;
	bool isWritable= false;
	bool isMemorized= false;
	try{
		Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_format= attr.get_data_format();
		isWritable= attr.is_writ_associated();
		if(isWritable){
			isMemorized= get_device_attr()->get_w_attr_by_name(attr_name.c_str()).is_memorized();
		}
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		_ERROR_LOG("Attribute "<<attr_name<<" does not exist in device!");
		throw;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		_ERROR_LOG("Attribute "<<attr_name<<" does not exist in device!");
		throw;
	}
	
	//Check if attribute is scalar
	if(data_format!=Tango::SCALAR){
		std::string errMsg("Invoked method is only for scalar attribute!");
		_ERROR_LOG(errMsg);
		_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_ATTR_TO_SAFE_VALUE_FAILED",errMsg);
	}

	//Set attribute value in attr manager
	T attr_safe_value_set;
	try{
		SetDynAttrSafeValueInManager<T>(attr_safe_value_set,attr_name,changeAttrQuality,attr_quality);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to set dyn attribute "<<attr_name<<" safe value from manager (attr not found?)!");
		throw;
	}
	catch(...){
		_ERROR_LOG("Failed to get dyn attribute "<<attr_name<<" safe from manager (C++ exception)!");
		throw;
	}

	//If writable attr set write value
	if(isWritable){
		try{
			SetWAttrValue<T>(attr_safe_value_set,attr_name,attr_quality,isMemorized);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_ERROR_LOG("Failed to set writable dyn attribute "<<attr_name<<" safe value from manager (attr not found?)!");
			throw;
		}
		catch(...){
			_ERROR_LOG("Failed to set writable dyn attribute "<<attr_name<<" safe value from manager (C++ exception)!");
			throw;
		}
	}//close if

	// Push event?
	if(emit_event){
		try{
			PushUserEvent<T>(attr_safe_value_set,attr_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_ERROR_LOG("Failed to push event for dyn attribute "<<attr_name<<" value!");
			throw;
		}
		catch(...){
			_ERROR_LOG("Failed to push event for dyn attribute "<<attr_name<<" value (C++ exception)!");
			throw;
		}
	}

}//close SetScalarDynAttrToSafeValue()


template<typename T>
inline void LMCDevice::SetSpectrumDynAttrToSafeValue(std::string attr_name,bool changeAttrQuality,Tango::AttrQuality attr_quality,bool emit_event){

	//Check if attribute exists
	Tango::AttrDataFormat data_format;	
	long max_data_size;
	bool isWritable= false;	
	try{
		Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_format= attr.get_data_format();
		max_data_size= attr.get_max_dim_x();
		isWritable= attr.is_writ_associated();
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		_ERROR_LOG("Attribute "<<attr_name<<" does not exist in device!");
		throw;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		_ERROR_LOG("Attribute "<<attr_name<<" does not exist in device!");
		throw;
	}

	//Check if attribute is spectrum
	if(data_format!=Tango::SPECTRUM){
		std::string errMsg("Invoked method is only for spectrum attribute!");
		_ERROR_LOG(errMsg);
		_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_TO_SAFE_VALUE_FAILED",errMsg);
	}

	
	//Set attribute value in attr manager
	std::vector<T> attr_safe_value_set;
	try {
		SetDynAttrSafeValueInManager(attr_safe_value_set,attr_name,changeAttrQuality,attr_quality);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr not found?)!");
		throw;
	}
	catch(...){
		_ERROR_LOG("Failed to get dyn attribute "<<attr_name<<" from manager (C++ exception)!");
		throw;
	}

	//Check data size given
	long attr_value_size= static_cast<long>(attr_safe_value_set.size());
	if(attr_value_size>max_data_size){
		std::stringstream ss;
		ss<<"Given data size (size="<<attr_value_size<<") larger than maximum allocated attr dim (dim="<<max_data_size<<")";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_TO_SAFE_VALUE_FAILED",ss.str());
	}

	//If writable attr set write value (spectrum attr cannot be memorized in Tango)
	if(isWritable){
		try{
			SetWAttrValue(attr_safe_value_set,attr_name,attr_quality,false);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_ERROR_LOG("Failed to set writable dyn attribute "<<attr_name<<" value from manager (attr not found?)!");
			throw;
		}
		catch(...){
			_ERROR_LOG("Failed to get writable dyn attribute "<<attr_name<<" from manager (C++ exception)!");
			throw;
		}
	}

	// Push event?
	if(emit_event){
		try{
			PushUserEvent(attr_safe_value_set,attr_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_ERROR_LOG("Failed to push event for dyn attribute "<<attr_name<<" value!");
			throw;
		}
		catch(...){
			_ERROR_LOG("Failed to push event for dyn attribute "<<attr_name<<" value (C++ exception)!");
			throw;
		}
	}


}//close SetSpectrumDynAttrToSafeValue() for spectrum attrs



template<typename T>
inline void LMCDevice::SetFormulaScalarDynAttrValue(std::string attr_name,bool emit_event){

	//Find formula data for given attribute
	if(!m_formulaManager){
		std::stringstream ss;
		ss<<"Formula attr manager is not initialized, cannot retrieve formula data for attr "<<attr_name<<"!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());	
	}

	Utils_ns::FormulaData* formula_data= m_formulaManager->FindFormula(attr_name);
	if(!formula_data){
		std::stringstream ss;
		ss<<"Cannot find formula data for attr "<<attr_name<<" (check attr syntax or if formula is registered)!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());
	}

	//Evaluate formula and set value
	try {
		SetFormulaScalarDynAttrValue<T>(attr_name,formula_data,emit_event);
	}
	catch(Tango::DevFailed& e){
		throw;
	}
	catch(...){
		throw;
	}

}//close SetFormulaScalarDynAttrValue()


template<typename T>
inline void LMCDevice::SetFormulaScalarDynAttrValue(std::string attr_name,Utils_ns::FormulaData* formula_data,bool emit_event){

	//Check if formula is initialized
	if(!formula_data->IsInitialized()){
		std::stringstream ss;
		ss<<"Attr formula for attr "<<attr_name<<" is not initialized yet (some proxy are offline?), will evaluate formula using init values...";
		_WARN_LOG(ss.str());
	}

	//Evaluate formula (use double as type for numeric symbols in formula)
	T formula_res;
	Tango::AttrQuality formula_attr_quality;
	long attr_timestamp_threshold= formulaVarMaxTimeInterval;//in seconds

	if(Utils_ns::EvaluateScalarFormula<double,T>(formula_data,formula_res,formula_attr_quality,attr_timestamp_threshold)<0){
		std::stringstream ss;
		ss<<"Failed to evaluate formula for attr "<<attr_name<<"!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());	
	}

	//Set dyn attr value & quality
	if(SetDynAttrValue<T>(formula_res,attr_name,formula_attr_quality,emit_event)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());	
	}

}//close SetFormulaScalarDynAttrValue()


#if TANGO_VERSION_MAJOR >= 9
template<>
inline void LMCDevice::SetFormulaScalarDynAttrValue<Tango::DevEnum>(std::string attr_name,Utils_ns::FormulaData* formula_data,bool emit_event){

	//Check if formula is initialized
	if(!formula_data->IsInitialized()){
		std::stringstream ss;
		ss<<"Attr formula for attr "<<attr_name<<" is not initialized yet (some proxy are offline?), will evaluate formula using init values...";
		_WARN_LOG(ss.str());
	}

	//Check data type
	long data_type;
	Tango::MultiAttrProp<Tango::DevEnum> multi_attr_prop;
	std::vector<std::string> enum_labels;
	try{
		Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_type= attr.get_data_type();
		if(data_type==Tango::DEV_ENUM) {
			attr.get_properties(multi_attr_prop);
			enum_labels= multi_attr_prop.enum_labels;
		}
	}
	catch(...){//an exception is thrown if the attribute is not existing
		throw;
	}

	//Check if attribute type is Tango::DevEnum or DevShort
	long attr_timestamp_threshold= formulaVarMaxTimeInterval;//in seconds
	Tango::AttrQuality formula_attr_quality;
	Tango::DevShort formula_attr_value;
	
	if(data_type==Tango::DEV_ENUM){

		//Evaluate formula (use double as type for numeric symbols in formula)
		//NB: Enum values in formula shall be specified as string labels
		std::string formula_res= "";
		if(Utils_ns::EvaluateScalarFormula<double,std::string>(formula_data,formula_res,formula_attr_quality,attr_timestamp_threshold)<0){
			std::stringstream ss;
			ss<<"Failed to evaluate formula for attr "<<attr_name<<"!";
			_WARN_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());	
		}		

		//Convert label in DevShort
		std::vector<std::string>::iterator it= std::find(enum_labels.begin(),enum_labels.end(),formula_res);
		if(enum_labels.empty() || it==enum_labels.end()){
			std::stringstream ss;
			ss<<"Cannot find enum label "<<formula_res<<" in enum labels for attr "<<attr_name<<"!";
			_WARN_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());
		}
	
		formula_attr_value= static_cast<Tango::DevShort>(it-enum_labels.begin());

	}//close DEV_ENUM

	else if(data_type==Tango::DEV_SHORT){

		//Evaluate formula (use double as type for numeric symbols in formula)
		if(Utils_ns::EvaluateScalarFormula<double,Tango::DevShort>(formula_data,formula_attr_value,formula_attr_quality,attr_timestamp_threshold)<0){
			std::stringstream ss;
			ss<<"Failed to evaluate formula for attr "<<attr_name<<"!";
			_WARN_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());	
		}

	}//close DEV_SHORT

	else{//should not get here
		std::stringstream ss;
		ss<<"Unexpected data type, cannot evaluate formula!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());				
	}
	
	//Set dyn attr value & quality
	if(SetDynAttrValue<Tango::DevShort>(formula_attr_value,attr_name,formula_attr_quality,emit_event)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SCALAR_DYN_ATTR_VALUE_FAILED",ss.str());	
	}

}//close SetFormulaScalarDynAttrValue() specialization for enum/short
#endif


template<typename T>
inline void LMCDevice::SetFormulaSpectrumDynAttrValue(std::string attr_name,long data_size,bool emit_event){

	//Find formula data for given attribute
	if(!m_formulaManager){
		std::stringstream ss;
		ss<<"Attr formula for attr "<<attr_name<<" is not initialized yet (some proxy are offline?), will evaluate formula using init values...";
		_WARN_LOG(ss.str());
	}

	Utils_ns::FormulaData* formula_data= m_formulaManager->FindFormula(attr_name);
	if(!formula_data){
		std::stringstream ss;
		ss<<"Cannot find formula data for attr "<<attr_name<<" (check attr syntax or if formula is registered)!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SPECTRUM_DYN_ATTR_VALUE_FAILED",ss.str());
	}

	//Evaluate formula and set value
	try {
		SetFormulaSpectrumDynAttrValue<T>(attr_name,formula_data,data_size,emit_event);
	}
	catch(Tango::DevFailed& e){
		throw;
	}

}//close SetFormulaSpectrumDynAttrValue()



template<typename T>
inline void LMCDevice::SetFormulaSpectrumDynAttrValue(std::string attr_name,Utils_ns::FormulaData* formula_data,long data_size,bool emit_event){

	//Check if formula is initialized
	if(!formula_data->IsInitialized()){
		std::stringstream ss;
		ss<<"Attr formula for attr "<<attr_name<<" is not initialized yet (some proxy are offline?), will evaluate formula using init values...";
		_WARN_LOG(ss.str());
	}

	//Evaluate formula (use double as type for numeric symbols in formula)
	std::vector<T> formula_res;
	Tango::AttrQuality formula_attr_quality;
	long attr_timestamp_threshold= formulaVarMaxTimeInterval;//in seconds

	if(Utils_ns::EvaluateSpectrumFormula<double,T>(formula_data,formula_res,formula_attr_quality,data_size,attr_timestamp_threshold)<0){
		std::stringstream ss;
		ss<<"Failed to evaluate formula for attr "<<attr_name<<"!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SPECTRUM_DYN_ATTR_VALUE_FAILED",ss.str());	
	}

	//Set dyn attr value & quality
	if(SetDynAttrValue<T>(formula_res,attr_name,formula_attr_quality,emit_event)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_WARN_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_FORMULA_SPECTRUM_DYN_ATTR_VALUE_FAILED",ss.str());	
	}

}//close SetFormulaSpectrumDynAttrValue()


template<typename T>
int LMCDevice::InitDynAttrFromGeneratorProps(Utils_ns::AttrDataPtr attr_data_ptr,Utils_ns::DeviceAttr* device_attr,std::string attr_name){
	
	//Check device attr
	if(!device_attr) {
		_ERROR_LOG("Null ptr to input attr given!");
		return -1;
	}
	if(!attr_data_ptr){
		_ERROR_LOG("Null ptr to input attr data given!");
		return -1;
	}
	bool isMemorized= device_attr->IsMemorized();	

	//Cast pointer
	Utils_ns::ScalarAttrData<T>* casted_attr_data_ptr= 0;
	try{
		casted_attr_data_ptr = dynamic_cast<Utils_ns::ScalarAttrData<T>*>(attr_data_ptr);
	}
	catch(std::exception& e){
		_ERROR_LOG("Failed to cast attr data ptr to scalar attr (err="<<e.what()<<")");
		return -1;	
	}
	if(!casted_attr_data_ptr){
		_ERROR_LOG("Null ptr to scalar attr data given!");
		return -1;
	}

	//Check if has generator
	bool hasGeneratorModel= device_attr->HasGeneratorModel();
	if(!hasGeneratorModel){
		_ERROR_LOG("No generator model present for this attribute!");
		return -1;
	}
	int generatorModelType= device_attr->GetGeneratorModel()->GetModelType();
	Tango::DbDatum genModelType_prop_data("genModel");
	genModelType_prop_data << generatorModelType;

	bool isRandomizedAttr= device_attr->IsRandomize();
	Tango::DbDatum isRandomize_prop_data("randomize");
	isRandomize_prop_data << isRandomizedAttr;

	//Put properties to DB
	try {
		Tango::DbDatum attr_data(attr_name);		
		Tango::DbData db_data;
		
		//Get generator model
		if(generatorModelType==Utils_ns::AttrGenerationModel::eCONST){
			_DEBUG_LOG("Get access to AttrConstModel...");
			Utils_ns::AttrConstModel<T>* genModel= dynamic_cast<Utils_ns::AttrConstModel<T>*>( device_attr->GetGeneratorModel() );

			//Get init value and set attr value
			_DEBUG_LOG("Get init value from AttrConstModel...");
			T initValue;
			if(isMemorized) {
				device_attr->GetInitValue<T>(initValue);
				_DEBUG_LOG("Get init value from AttrConstModel (initValue="<<initValue<<")");	
				casted_attr_data_ptr->set_data(initValue);
				_DEBUG_LOG("Set init value from AttrConstModel (initValue="<<initValue<<")");
			}
			else{
				initValue= genModel->initValue;
				_DEBUG_LOG("Get init value from AttrConstModel (initValue="<<initValue<<")");	
				casted_attr_data_ptr->set_data(initValue);
				_DEBUG_LOG("Set init value from AttrConstModel (initValue="<<initValue<<")");
			}

			Tango::DbDatum initValue_prop_data("genInitValue");
			initValue_prop_data << initValue;

			//Set db data
			_DEBUG_LOG("Setting model pars in DB...");
			attr_data << 3;
			db_data.push_back(attr_data); 
			db_data.push_back(isRandomize_prop_data);
			db_data.push_back(genModelType_prop_data);			
			db_data.push_back(initValue_prop_data); 
		}	
		else if(generatorModelType==Utils_ns::AttrGenerationModel::eUNIFORM){
			Utils_ns::AttrUniformRandomModel<T>* genModel= dynamic_cast<Utils_ns::AttrUniformRandomModel<T>*>( device_attr->GetGeneratorModel() );

			//Get init value and set attr value
			T initValue= genModel->initValue;
			casted_attr_data_ptr->set_data(initValue);

			Tango::DbDatum initValue_prop_data("genInitValue");
			initValue_prop_data << initValue;

			//Get min/max values
			T minValue= genModel->minValue;
			T maxValue= genModel->maxValue;
			Tango::DbDatum minValue_prop_data("genMinValue");
			minValue_prop_data << minValue;
			Tango::DbDatum maxValue_prop_data("genMaxValue");
			maxValue_prop_data << maxValue;

			//Set db data
			attr_data << 5; 
			db_data.push_back(attr_data); 
			db_data.push_back(isRandomize_prop_data);
			db_data.push_back(genModelType_prop_data);
			db_data.push_back(initValue_prop_data); 
			db_data.push_back(minValue_prop_data); 
			db_data.push_back(maxValue_prop_data); 
		}
		else if(generatorModelType==Utils_ns::AttrGenerationModel::eGAUSS){
			_DEBUG_LOG("Get access to AttrGaussRandomModel...");
			Utils_ns::AttrGaussRandomModel<T>* genModel= dynamic_cast<Utils_ns::AttrGaussRandomModel<T>*>( device_attr->GetGeneratorModel() );
			
			//Get init value and set attr value
			_DEBUG_LOG("Get init value from AttrGaussRandomModel...");
			T initValue= genModel->initValue;
			casted_attr_data_ptr->set_data(initValue);

			Tango::DbDatum initValue_prop_data("genInitValue");
			initValue_prop_data << initValue;

			//Get min/max values
			_DEBUG_LOG("Get min/max value from AttrGaussRandomModel...");
			T minValue= genModel->minValue;
			T maxValue= genModel->maxValue;
			Tango::DbDatum minValue_prop_data("genMinValue");
			minValue_prop_data << minValue;
			Tango::DbDatum maxValue_prop_data("genMaxValue");
			maxValue_prop_data << maxValue;
			_DEBUG_LOG("AttrGaussRandomModel min/max="<<minValue<<"/"<<maxValue<<"...");

			//Get mean/sigma values
			_DEBUG_LOG("Get mean/sigma value from AttrGaussRandomModel...");
			T meanValue= genModel->gaussMean;
			T sigmaValue= genModel->gaussSigma;
			Tango::DbDatum meanValue_prop_data("genMeanValue");
			meanValue_prop_data << meanValue;
			Tango::DbDatum sigmaValue_prop_data("genSigmaValue");
			sigmaValue_prop_data << sigmaValue;

			//Set db data
			_DEBUG_LOG("Setting model pars in db_data...");
			attr_data << 7;
			db_data.push_back(attr_data); 
			db_data.push_back(isRandomize_prop_data);
			db_data.push_back(genModelType_prop_data); 
			db_data.push_back(initValue_prop_data); 
			db_data.push_back(minValue_prop_data); 
			db_data.push_back(maxValue_prop_data);
			db_data.push_back(meanValue_prop_data); 
			db_data.push_back(sigmaValue_prop_data); 
		}
		else if(generatorModelType==Utils_ns::AttrGenerationModel::eDISCRETE){
			Utils_ns::AttrDiscreteRandomModel<T>* genModel= dynamic_cast<Utils_ns::AttrDiscreteRandomModel<T>*>( device_attr->GetGeneratorModel() );
			
			//Get init value and set attr value
			T initValue= genModel->initValue;
			casted_attr_data_ptr->set_data(initValue);

			Tango::DbDatum initValue_prop_data("genInitValue");
			initValue_prop_data << initValue;
	
			//Get weight prob values
			std::vector<double> weights= genModel->probWeights;
			Tango::DbDatum weightsValue_prop_data("genProbWeightValue");
			weightsValue_prop_data << weights;	

			//Set db data
			attr_data << 4;
			db_data.push_back(attr_data); 
			db_data.push_back(isRandomize_prop_data);
			db_data.push_back(genModelType_prop_data); 
			db_data.push_back(initValue_prop_data); 
			db_data.push_back(weightsValue_prop_data); 
		}
		else {
			std::string errMsg= "Invalid generator model found!";
			Tango::Except::throw_exception( 
 				(const char *)"INIT_DYN_ATTR_FROM_GEN_PROPERTIES_FAILED",
	       errMsg.c_str(),
	       (const char *)"LMCDevice::InitDynAttrFromGeneratorProps"
	 		);
		}

		_DEBUG_LOG("Setting generator properties in DB...");
		get_db_device()->put_attribute_property(db_data);

	}//close try 
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to put generator properties in attribute "<<attr_name<<"!");
		return -1;
	}
	catch(std::exception& e){
		_ERROR_LOG("C++ exception ("<<e.what()<<") occurred when adding generator properties in attribute "<<attr_name<<"!");
		return -1;
	}
	catch(...){
		_ERROR_LOG("Unknown exception occurred when adding generator properties in attribute "<<attr_name<<"!");
		return -1;
	}

	_DEBUG_LOG("End");

	return 0;

}//close InitDynAttrFromGeneratorProps()



template<typename T>
int LMCDevice::InitDynSpectrumAttrFromGeneratorProps(Utils_ns::AttrDataPtr attr_data_ptr,Utils_ns::DeviceAttr* device_attr,std::string attr_name){
	
	//Check device attr
	if(!device_attr) {
		_ERROR_LOG("Null ptr to input attr given!");
		return -1;
	}

	if(!attr_data_ptr){
		_ERROR_LOG("Null ptr to input attr data given!");
		return -1;
	}

	//Cast pointer
	Utils_ns::SpectrumAttrData<T>* spectrum_attr_data= 0;
	try{
		spectrum_attr_data = dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(attr_data_ptr);
	}
	catch(std::exception& e){
		_ERROR_LOG("Failed to cast attr data ptr to spectrum attr (err="<<e.what()<<")");
		return -1;	
	}
	if(!spectrum_attr_data){
		_ERROR_LOG("Null ptr to spectrum attr data given!");
		return -1;
	}

	//Check if has generator
	bool hasGeneratorModel= device_attr->HasGeneratorModel();
	if(!hasGeneratorModel){
		_ERROR_LOG("No generator model present for this attribute!");
		return -1;
	}
	Tango::SpectrumAttr* attr= dynamic_cast<Tango::SpectrumAttr*>(device_attr->GetAttr());
	long int data_size= attr->get_max_x();
	int generatorModelType= device_attr->GetGeneratorModel()->GetModelType();
	Tango::DbDatum genModelType_prop_data("genModel");
	genModelType_prop_data << generatorModelType;

	bool isRandomizedAttr= device_attr->IsRandomize();
	Tango::DbDatum isRandomize_prop_data("randomize");
	isRandomize_prop_data << isRandomizedAttr;

	//Put properties to DB
	try {
		Tango::DbDatum attr_data(attr_name);		
		Tango::DbData db_data;
		
		//Get generator model
		if(generatorModelType==Utils_ns::AttrGenerationModel::eCONST){
			_DEBUG_LOG("Get access to AttrConstModel...");
			Utils_ns::AttrConstModel<T>* genModel= dynamic_cast<Utils_ns::AttrConstModel<T>*>( device_attr->GetGeneratorModel() );

			//Get init value and set attr value
			_DEBUG_LOG("Get init value from AttrConstModel...");
			T initValue= genModel->initValue;
			std::vector<T> initValues(data_size,initValue);
			_DEBUG_LOG("Get init value from AttrConstModel (initValue="<<initValue<<")");	

			if(spectrum_attr_data->set_data(initValues)<0){
				std::stringstream ss;
				ss<<"Failed to set value of attr "<<attr_name<<" from buffer!";
				_ERROR_LOG(ss.str());
				Tango::Except::throw_exception( 
					(const char *)"SET_DYN_ATTR_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"LMCDevice::SetDynAttrValue"
				);
			}
	
			_DEBUG_LOG("Set init value from AttrConstModel (initValue="<<initValue<<")");
			
			Tango::DbDatum initValue_prop_data("genInitValue");
			initValue_prop_data << initValue;

			//Set db data
			_DEBUG_LOG("Setting model pars in DB...");
			attr_data << 3;
			db_data.push_back(attr_data); 
			db_data.push_back(isRandomize_prop_data);
			db_data.push_back(genModelType_prop_data);			
			db_data.push_back(initValue_prop_data); 
		}	
		else if(generatorModelType==Utils_ns::AttrGenerationModel::eUNIFORM){
			Utils_ns::AttrUniformRandomModel<T>* genModel= dynamic_cast<Utils_ns::AttrUniformRandomModel<T>*>( device_attr->GetGeneratorModel() );

			//Get init value and set attr value
			T initValue= genModel->initValue;
			std::vector<T> initValues(data_size,initValue);

			if(spectrum_attr_data->set_data(initValues)<0){
				std::stringstream ss;
				ss<<"Failed to set value of attr "<<attr_name<<" from buffer!";
				_ERROR_LOG(ss.str());
				Tango::Except::throw_exception( 
					(const char *)"SET_DYN_ATTR_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"LMCDevice::SetDynAttrValue"
				);
			}

			Tango::DbDatum initValue_prop_data("genInitValue");
			initValue_prop_data << initValue;

			//Get min/max values
			T minValue= genModel->minValue;
			T maxValue= genModel->maxValue;
			Tango::DbDatum minValue_prop_data("genMinValue");
			minValue_prop_data << minValue;
			Tango::DbDatum maxValue_prop_data("genMaxValue");
			maxValue_prop_data << maxValue;

			//Set db data
			attr_data << 5; 
			db_data.push_back(attr_data); 
			db_data.push_back(isRandomize_prop_data);
			db_data.push_back(genModelType_prop_data);
			db_data.push_back(initValue_prop_data); 
			db_data.push_back(minValue_prop_data); 
			db_data.push_back(maxValue_prop_data); 
		}
		else if(generatorModelType==Utils_ns::AttrGenerationModel::eGAUSS){
			_DEBUG_LOG("Get access to AttrGaussRandomModel...");
			Utils_ns::AttrGaussRandomModel<T>* genModel= dynamic_cast<Utils_ns::AttrGaussRandomModel<T>*>( device_attr->GetGeneratorModel() );
			
			//Get init value and set attr value
			_DEBUG_LOG("Get init value from AttrGaussRandomModel...");
			T initValue= genModel->initValue;
			std::vector<T> initValues(data_size,initValue);
			
			if(spectrum_attr_data->set_data(initValues)<0){
				std::stringstream ss;
				ss<<"Failed to set value of attr "<<attr_name<<" from buffer!";
				_ERROR_LOG(ss.str());
				Tango::Except::throw_exception( 
					(const char *)"SET_DYN_ATTR_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"LMCDevice::SetDynAttrValue"
				);
			}			

			Tango::DbDatum initValue_prop_data("genInitValue");
			initValue_prop_data << initValue;

			//Get min/max values
			_DEBUG_LOG("Get min/max value from AttrGaussRandomModel...");
			T minValue= genModel->minValue;
			T maxValue= genModel->maxValue;
			Tango::DbDatum minValue_prop_data("genMinValue");
			minValue_prop_data << minValue;
			Tango::DbDatum maxValue_prop_data("genMaxValue");
			maxValue_prop_data << maxValue;
			_DEBUG_LOG("AttrGaussRandomModel min/max="<<minValue<<"/"<<maxValue<<"...");

			//Get mean/sigma values
			_DEBUG_LOG("Get mean/sigma value from AttrGaussRandomModel...");
			T meanValue= genModel->gaussMean;
			T sigmaValue= genModel->gaussSigma;
			Tango::DbDatum meanValue_prop_data("genMeanValue");
			meanValue_prop_data << meanValue;
			Tango::DbDatum sigmaValue_prop_data("genSigmaValue");
			sigmaValue_prop_data << sigmaValue;

			//Set db data
			_DEBUG_LOG("Setting model pars in db_data...");
			attr_data << 7;
			db_data.push_back(attr_data); 
			db_data.push_back(isRandomize_prop_data);
			db_data.push_back(genModelType_prop_data); 
			db_data.push_back(initValue_prop_data); 
			db_data.push_back(minValue_prop_data); 
			db_data.push_back(maxValue_prop_data);
			db_data.push_back(meanValue_prop_data); 
			db_data.push_back(sigmaValue_prop_data); 
		}
		else if(generatorModelType==Utils_ns::AttrGenerationModel::eDISCRETE){
			Utils_ns::AttrDiscreteRandomModel<T>* genModel= dynamic_cast<Utils_ns::AttrDiscreteRandomModel<T>*>( device_attr->GetGeneratorModel() );
			
			//Get init value and set attr value
			T initValue= genModel->initValue;
			std::vector<T> initValues(data_size,initValue);
			
			if(spectrum_attr_data->set_data(initValues)<0){
				std::stringstream ss;
				ss<<"Failed to set value of attr "<<attr_name<<" from buffer!";
				_ERROR_LOG(ss.str());
				Tango::Except::throw_exception( 
					(const char *)"SET_DYN_ATTR_FAILED",
    	  	ss.str().c_str(),
    	  	(const char *)"LMCDevice::SetDynAttrValue"
				);
			}			

			Tango::DbDatum initValue_prop_data("genInitValue");
			initValue_prop_data << initValue;
	
			//Get weight prob values
			std::vector<double> weights= genModel->probWeights;
			Tango::DbDatum weightsValue_prop_data("genProbWeightValue");
			weightsValue_prop_data << weights;	

			//Set db data
			attr_data << 4;
			db_data.push_back(attr_data); 
			db_data.push_back(isRandomize_prop_data);
			db_data.push_back(genModelType_prop_data); 
			db_data.push_back(initValue_prop_data); 
			db_data.push_back(weightsValue_prop_data); 
		}
		else {
			std::string errMsg= "Invalid generator model found!";
			Tango::Except::throw_exception( 
 				(const char *)"INIT_DYN_ATTR_FROM_GEN_PROPERTIES_FAILED",
	       errMsg.c_str(),
	       (const char *)"LMCDevice::InitDynAttrFromGeneratorProps"
	 		);
		}

		_DEBUG_LOG("Setting generator properties in DB...");
		get_db_device()->put_attribute_property(db_data);

	}//close try 
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		_ERROR_LOG("Failed to put generator properties in attribute "<<attr_name<<"!");
		return -1;
	}
	catch(std::exception& e){
		_ERROR_LOG("C++ exception ("<<e.what()<<") occurred when adding generator properties in attribute "<<attr_name<<"!");
		return -1;
	}
	catch(...){
		_ERROR_LOG("Unknown exception occurred when adding generator properties in attribute "<<attr_name<<"!");
		return -1;
	}
	
	_DEBUG_LOG("End");

	return 0;

}//close InitDynSpectrumAttrFromGeneratorProps 



template<typename T>
inline bool LMCDevice::IsCmdAllowed(std::string cmd_name,std::string attr_name,T attr_value,bool check){

	//Check for cmd & attr name given
	//If the command name is not valid (e.g. passed by user mistake) check fails and cmd is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Find if command exist in device
		try{
			get_device_class()->get_cmd_by_name(cmd_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_WARN_LOG("Command "<<cmd_name<<" not found in device, no rule will be added (check cmd name given)!");
			return false;
		}

		//Check if attribute exists in device
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
			data_format= attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			_WARN_LOG("Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			_WARN_LOG("[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}

		//Check if attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			_WARN_LOG("Invalid attr data format found (cmd state machine is supported for scalar attribute only), check failed!");
			return false;
		}
	}//close if check

	//Check if given command is allowed in rules
	_DEBUG_LOG("Search if command "<<cmd_name<<" is allowed when attr "<<attr_name<<" is equal to "<<attr_value<<"...");
	bool isAllowed= Utils_ns::RuleFactory::Instance().IsCmdAllowed<T>(cmd_name,attr_name,attr_value);
	_DEBUG_LOG("Command "<<cmd_name<<" is allowed? "<<isAllowed);
	
	return isAllowed;

}//close IsCmdAllowed()

#if TANGO_VERSION_MAJOR >= 9
template<>
inline bool LMCDevice::IsCmdAllowed<Tango::DevEnum>(std::string cmd_name,std::string attr_name,Tango::DevShort attr_value,bool check){

	//Check for cmd & attr name given
	//If the command name is not valid (e.g. passed by user mistake) check fails and cmd is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Find if command exist in device
		try{
			get_device_class()->get_cmd_by_name(cmd_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_WARN_LOG("Command "<<cmd_name<<" not found in device, no rule will be added (check cmd name given)!");
			return false;
		}
	}//close if check

	//This is mandatory because we need to discriminate short & enum attrs		
	//Check if attribute exists in device
	Tango::AttrDataFormat data_format;
	long data_type;
	try{
		Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_format= attr.get_data_format();
		data_type= attr.get_data_type();	
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		_WARN_LOG("Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		_WARN_LOG("[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}

	//Check if attribute is not a scalar
	if(data_format!=Tango::SCALAR){
		_WARN_LOG("Invalid attr data format found (cmd state machine is supported for scalar attribute only), check failed!");
		return false;
	}

	//If enum attr get enum label
	std::string attr_value_str= "";
	if(data_type==Tango::DEV_ENUM && Utils_ns::TangoUtils::GetEnumAttrLabelValue(attr_value_str,this,attr_name,attr_value)<0){
		_WARN_LOG("Failed to retrieve enum label for attribute "<<attr_name<<"!");
		return false;
	}

	//Check if given command is allowed in rules	
	//If attr is a short same procedure as above	
	//If attr is an enum get string label and compare labels
	
	bool isAllowed= false;
	if(data_type==Tango::DEV_ENUM) {
		_DEBUG_LOG("Search if command "<<cmd_name<<" is allowed when attr "<<attr_name<<" (type="<<data_type<<") is equal to "<<attr_value_str<<"...");
		isAllowed= IsCmdAllowed<std::string>(cmd_name,attr_name,attr_value_str,false);
	}
	else {	
		_DEBUG_LOG("Search if command "<<cmd_name<<" is allowed when attr "<<attr_name<<" (type="<<data_type<<") is equal to "<<attr_value<<"...");
		isAllowed= IsCmdAllowed<Tango::DevShort>(cmd_name,attr_name,attr_value,false);
	}

	return isAllowed;

}//close IsCmdAllowed specialization for DevShort/Enum
#endif



template<typename T> 
inline bool LMCDevice::IsAttrAllowed(std::string attr_name,T attr_value_current,T attr_value_new,bool check){

	//Check for attr name given
	//If the attr name is not valid (e.g. passed by user mistake) check fails and attr is by default not allowed (TO FORCE USER TO CHECK GIVEN ARGUMENTS!)
	if(check){
		//Check if attribute exists in device
		Tango::AttrDataFormat data_format;	
		try{
			Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
			data_format= attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			_WARN_LOG("Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			_WARN_LOG("[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
			return false;
		}

		//Check if attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			_WARN_LOG("Invalid attr data format found (cmd state machine is supported for scalar attribute only), check failed!");
			return false;
		}
	}//close if check

	//Check if given command is allowed in rules
	return Utils_ns::RuleFactory::Instance().IsAttrAllowed<T>(attr_name,attr_value_current,attr_value_new);

}//close IsAttrAllowed()

#if TANGO_VERSION_MAJOR >= 9
template<> 
inline bool LMCDevice::IsAttrAllowed<Tango::DevEnum>(std::string attr_name,Tango::DevShort attr_value_current,Tango::DevShort attr_value_new,bool check){

	//Check if attribute exists in device
	Tango::AttrDataFormat data_format;
	long data_type;
	try{
		Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
		data_format= attr.get_data_format();	
		data_type= attr.get_data_type();	
	}
	catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
		_WARN_LOG("Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}
	catch(...){//an exception is thrown if the attribute is not existing
		_WARN_LOG("[Unknown exception] Attribute "<<attr_name<<" not found in device, check failed (check attr name given)!");
		return false;
	}

	//Check if attribute is not a scalar
	if(data_format!=Tango::SCALAR){
		_WARN_LOG("Invalid attr data format found (attr state machine is supported for scalar attribute only), check failed!");
		return false;
	}

	//Check if given attr is allowed in rules
	bool isAllowed= false;
	if(data_type==Tango::DEV_ENUM) {
		//Get enum labels
		std::string attr_value_current_str= "";
		if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(attr_value_current_str,this,attr_name,attr_value_current)<0){
			_WARN_LOG("Failed to retrieve enum label for attribute "<<attr_name<<"!");
			return false;
		}
		std::string attr_value_new_str= "";
		if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(attr_value_new_str,this,attr_name,attr_value_new)<0){
			_WARN_LOG("Failed to retrieve enum label for attribute "<<attr_name<<"!");
			return false;
		}

		//Check rule
		_DEBUG_LOG("Search if attr "<<attr_name<<" transition ("<<attr_value_current_str<<" --> "<<attr_value_new_str<<") is allowed");
		isAllowed= IsAttrAllowed<std::string>(attr_name,attr_value_current_str,attr_value_new_str,false);
	}//close if
	else {	
		isAllowed= IsAttrAllowed<Tango::DevShort>(attr_name,attr_value_current,attr_value_new,false);
	}

	return isAllowed;

}//close IsAttrAllowed specialization for enum/short attr
#endif

template<typename T> 
inline bool LMCDevice::CheckIfCmdIsAllowed(std::string cmd_name,std::string attr_name,long data_type){

	//Retrieve current attr value
	T attr_value;	
	if(GetDynAttrValue<T>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<attr_name<<" value!";
		_THROW_TANGO_EXCEPTION("CHECK_IF_CMD_IS_ALLOWED_FAILED",ss.str());
	}

	//Check if given command is allowed in rules
	_DEBUG_LOG("Search if command "<<cmd_name<<" is allowed when attr "<<attr_name<<" is equal to "<<attr_value<<"...");
	bool isAllowed= Utils_ns::RuleFactory::Instance().IsCmdAllowed<T>(cmd_name,attr_name,attr_value);
	_DEBUG_LOG("Command "<<cmd_name<<" is allowed? "<<isAllowed);
	
	return isAllowed;

}//close CheckIfCmdIsAllowed()

#if TANGO_VERSION_MAJOR >= 9
template<> 
inline bool LMCDevice::CheckIfCmdIsAllowed<Tango::DevEnum>(std::string cmd_name,std::string attr_name,long data_type){

	//Retrieve current attr value
	Tango::DevShort attr_value;	
	if(GetDynAttrValue<Tango::DevShort>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<attr_name<<" value!";
		_THROW_TANGO_EXCEPTION("CHECK_IF_CMD_IS_ALLOWED_FAILED",ss.str());
	}

	//Check if given command is allowed in rules	
	//If attr is a short same procedure as above	
	//If attr is an enum get string label and compare labels
	bool isAllowed= false;
	if(data_type==Tango::DEV_ENUM) {
		std::string attr_value_str= "";
		if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(attr_value_str,this,attr_name,attr_value)<0){
			std::stringstream ss;
			ss<<"Failed to retrieve labels for enum attr "<<attr_name<<" (attr_value="<<attr_value<<")!";
			_THROW_TANGO_EXCEPTION("CHECK_IF_CMD_IS_ALLOWED_FAILED",ss.str());
		}
		isAllowed= Utils_ns::RuleFactory::Instance().IsCmdAllowed<std::string>(cmd_name,attr_name,attr_value_str);
	}
	else {	
		isAllowed= Utils_ns::RuleFactory::Instance().IsCmdAllowed<Tango::DevShort>(cmd_name,attr_name,attr_value);
	}

	return isAllowed;

}//close CheckIfCmdIsAllowed()
#endif

template<typename T> 
inline bool LMCDevice::CheckIfAttrReadIsAllowed(std::string attr_name,std::string sm_attr_name,long data_type){

	//Retrieve current attr value
	T sm_attr_value;	
	if(GetDynAttrValue<T>(sm_attr_value,sm_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get sm attr "<<sm_attr_name<<" value!";
		_THROW_TANGO_EXCEPTION("CHECK_IF_ATTR_READ_IS_ALLOWED_FAILED",ss.str());
	}

	//Check if given command is allowed in rules
	_DEBUG_LOG("Search if attr "<<attr_name<<" read is allowed when sm attr "<<sm_attr_name<<" is equal to "<<sm_attr_value<<"...");
	bool isAllowed= Utils_ns::RuleFactory::Instance().IsAttrReadAllowed<T>(attr_name,sm_attr_name,sm_attr_value);
	_DEBUG_LOG("Attr "<<attr_name<<" read is allowed? "<<isAllowed);
	
	return isAllowed;

}//close CheckIfAttrReadIsAllowed()


#if TANGO_VERSION_MAJOR >= 9
template<> 
inline bool LMCDevice::CheckIfAttrReadIsAllowed<Tango::DevEnum>(std::string attr_name,std::string sm_attr_name,long data_type){

	//Retrieve current attr value
	Tango::DevShort sm_attr_value;	
	if(GetDynAttrValue<Tango::DevShort>(sm_attr_value,sm_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get sm attr "<<sm_attr_name<<" value!";
		_THROW_TANGO_EXCEPTION("CHECK_IF_ATTR_READ_IS_ALLOWED_FAILED",ss.str());
	}

	//Check if given command is allowed in rules	
	//If attr is a short same procedure as above	
	//If attr is an enum get string label and compare labels
	bool isAllowed= false;
	if(data_type==Tango::DEV_ENUM) {
		std::string sm_attr_value_str= "";
		if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(sm_attr_value_str,this,sm_attr_name,sm_attr_value)<0){
			std::stringstream ss;
			ss<<"Failed to retrieve labels for enum sm attr "<<sm_attr_name<<" (attr_value="<<sm_attr_value<<")!";
			_THROW_TANGO_EXCEPTION("CHECK_IF_ATTR_READ_IS_ALLOWED_FAILED",ss.str());
		}
		isAllowed= Utils_ns::RuleFactory::Instance().IsAttrReadAllowed<std::string>(attr_name,sm_attr_name,sm_attr_value_str);
	}
	else {	
		isAllowed= Utils_ns::RuleFactory::Instance().IsAttrReadAllowed<Tango::DevShort>(attr_name,sm_attr_name,sm_attr_value);
	}

	return isAllowed;

}//close CheckIfAttrReadIsAllowed()
#endif

template<typename T> 
inline bool LMCDevice::CheckIfAttrWriteIsAllowed(std::string attr_name,std::string sm_attr_name,long data_type){

	//Retrieve current attr value
	T sm_attr_value;	
	if(GetDynAttrValue<T>(sm_attr_value,sm_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get sm attr "<<sm_attr_name<<" value!";
		_THROW_TANGO_EXCEPTION("CHECK_IF_ATTR_WRITE_IS_ALLOWED_FAILED",ss.str());
	}

	//Check if given command is allowed in rules
	_DEBUG_LOG("Search if attr "<<attr_name<<" write is allowed when sm attr "<<sm_attr_name<<" is equal to "<<sm_attr_value<<"...");
	bool isAllowed= Utils_ns::RuleFactory::Instance().IsAttrWriteAllowed<T>(attr_name,sm_attr_name,sm_attr_value);
	_DEBUG_LOG("Attr "<<attr_name<<" write is allowed? "<<isAllowed);
	
	return isAllowed;

}//close CheckIfAttrWriteIsAllowed()

#if TANGO_VERSION_MAJOR >= 9
template<> 
inline bool LMCDevice::CheckIfAttrWriteIsAllowed<Tango::DevEnum>(std::string attr_name,std::string sm_attr_name,long data_type){

	//Retrieve current attr value
	Tango::DevShort sm_attr_value;	
	if(GetDynAttrValue<Tango::DevShort>(sm_attr_value,sm_attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get sm attr "<<sm_attr_name<<" value!";
		_THROW_TANGO_EXCEPTION("CHECK_IF_ATTR_WRITE_IS_ALLOWED_FAILED",ss.str());
	}

	//Check if given command is allowed in rules	
	//If attr is a short same procedure as above	
	//If attr is an enum get string label and compare labels
	bool isAllowed= false;
	if(data_type==Tango::DEV_ENUM) {
		std::string sm_attr_value_str= "";
		if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(sm_attr_value_str,this,sm_attr_name,sm_attr_value)<0){
			std::stringstream ss;
			ss<<"Failed to retrieve labels for enum sm attr "<<sm_attr_name<<" (attr_value="<<sm_attr_value<<")!";
			_THROW_TANGO_EXCEPTION("CHECK_IF_ATTR_WRITE_IS_ALLOWED_FAILED",ss.str());
		}
		isAllowed= Utils_ns::RuleFactory::Instance().IsAttrWriteAllowed<std::string>(attr_name,sm_attr_name,sm_attr_value_str);
	}
	else {	
		isAllowed= Utils_ns::RuleFactory::Instance().IsAttrWriteAllowed<Tango::DevShort>(attr_name,sm_attr_name,sm_attr_value);
	}

	return isAllowed;

}//close CheckIfAttrWriteIsAllowed()
#endif

template<typename T> 
inline int LMCDevice::IsCmdAttrAllowed(std::string cmd_name,std::string attr_name,T* attr_value_new,bool check){

	//Retrieve current attr value
	T attr_value;	
	if(GetDynAttrValue<T>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		Tango::Except::throw_exception( 
	 		(const char *)"IS_CMD_ATTR_ALLOWED_FAILED",
	    ss.str().c_str(),
	    (const char *)"LMCDevice::IsCmdAttrAllowed"
	 	);
		return -1;
	}	

	//Check if command is allowed in this attr value
	if( !IsCmdAllowed<T>(cmd_name,attr_name,attr_value,check) ){
		std::stringstream ss;
		ss<<"Command cannot be inkoved with current attr value ("<<attr_value<<")!";
		_WARN_LOG(ss.str());
		Tango::Except::throw_exception( 
	 		(const char *)"IS_CMD_ATTR_ALLOWED_FAILED",
	    ss.str().c_str(),
	    (const char *)"LMCDevice::IsCmdAttrAllowed"
	 	);
		return -1;
	}	

	//Check if allowed mode transition
	if( attr_value_new && !IsAttrAllowed<T>(attr_name,attr_value,*attr_value_new,check) ){
		std::stringstream ss;
		ss<<"Requested attr transition ("<<attr_value<<"-->"<<*attr_value_new<<") is forbidden!";
		_WARN_LOG(ss.str());
		Tango::Except::throw_exception( 
	 		(const char *)"IS_CMD_ATTR_ALLOWED_FAILED",
	    ss.str().c_str(),
	    (const char *)"LMCDevice::IsCmdAttrAllowed"
		);
	}//close if

	return 0;

}//close IsCmdAttrAllowed()

/*
template<typename T>
inline int LMCDevice::AddCmdRule(std::string cmd_name,std::string attr_name,std::vector<T> attr_values,bool check){
		
	//Check cmd & attr for given device
	if(check){
		//Find if command exist in device
		try{
			get_device_class()->get_cmd_by_name(cmd_name);
		}
		catch(Tango::DevFailed& e){
			Tango::Except::print_exception(e);
			_WARN_LOG("Command "<<cmd_name<<" not found in device, no rule will be added (check cmd name given)!");
			return -1;
		}

		//Check if attribute exists in device
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
			data_format= attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			_WARN_LOG("Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			_WARN_LOG("[Unknown exception] Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			_WARN_LOG("Invalid attr data format found (cmd state machine is supported for scalar attribute only), no rule will be added!");
			return -1;
		}

	}//close if check

	//Register rule in factory
	if(Utils_ns::RuleFactory::Instance().AddCmdRule<T>(cmd_name,attr_name,attr_values)<0){
		_WARN_LOG("Failed to register rule for cmd "<<cmd_name<<" in device, no rule will be added!");
		return -1;
	}

	return 0;

}//close AddCmdRule()
*/

/*
template<typename T>
inline int LMCDevice::AddAttrRule(std::string attr_name,T attr_value,std::vector<T> attr_values,bool check){
		
	//Find if command exist in device
	if(check){
		//long data_type;
		Tango::AttrDataFormat data_format;
		try{
			Tango::Attribute& attr= get_device_attr()->get_attr_by_name(attr_name.c_str());
			//data_type= attr.get_data_type();
			data_format= attr.get_data_format();	
		}
		catch(Tango::DevFailed &e){//an exception is thrown if the attribute is not existing
			_WARN_LOG("Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}
		catch(...){//an exception is thrown if the attribute is not existing
			_WARN_LOG("[Unknown exception] Attribute "<<attr_name<<" not found in device, no rule will be added (check attr name given)!");
			return -1;
		}

		//Check if attribute is not a scalar
		if(data_format!=Tango::SCALAR){
			_WARN_LOG("Invalid attr data format found (attr state machine is supported for scalar attribute only), no rule will be added!");
			return -1;
		}
	}//close if check

	//Register rule in factory
	if(Utils_ns::RuleFactory::Instance().AddAttrRule<T>(attr_name,attr_value,attr_values)<0){
		_WARN_LOG("Failed to register rule for attr "<<attr_name<<" in device, no rule will be added!");
		return -1;
	}

	return 0;

}//close AddAttrRule()
*/

template<typename T>
inline int LMCDevice::SetSMDynAttrValue(T& attr_value,std::string attr_name,T* attr_value_fallback,Tango::AttrQuality attr_quality,bool emit_event){

	//Get current attribute value
	T attr_value_current;	
	if(GetDynAttrValue<T>(attr_value_current,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to get attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		return -1;
	}

	//Check if attr value transition is allowed in state machine
	if(!IsAttrAllowed(attr_name,attr_value_current,attr_value,false)){
		std::stringstream ss;
		ss<<"Transition for attr "<<attr_name<<" from value "<<attr_value_current<<" to "<<attr_value<<" is forbidden!";
		_ERROR_LOG(ss.str());
		return -1;
	}

	//Do the transition (set to fallback value in case of failure)
	T attr_value_safe= attr_value_current; 
	if(attr_value_fallback) attr_value_safe= *attr_value_fallback;
	if(SetDynAttrValue<T>(attr_value,attr_name,attr_quality,emit_event)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" to "<<attr_value<<" (setting attr to fallback value)!";
		_ERROR_LOG(ss.str());
				
		//Set to fallback value
		if(SetDynAttrValue<T>(attr_value_safe,attr_name,attr_quality,emit_event)<0){
			std::stringstream ss;
			ss<<"Failed to set attr "<<attr_name<<" to fallback value "<<attr_value_safe<<"!";
			_ERROR_LOG(ss.str());							
		}//close if
	
		return -1;
	}//close if

	return 0;

}//close SetDynAttrValueSM()


//===============================================================
//===               CREATE & INIT DYN ATTR
//===============================================================
template<typename T>
inline int LMCDevice::CreateDynAttrInfo(yat4tango::DynamicAttributeInfo& dai,Utils_ns::DeviceAttr* device_attr,Tango::MultiAttrProp<T>* multi_attr_prop){

	//Check input attr parsed from config
	if(!device_attr) {
		_ERROR_LOG("Null ptr to input attr given!");
		return -1;
	}
	if(!multi_attr_prop){
		_ERROR_LOG("Null ptr to input multi attr prop given!");
		return -1;
	}

	//## Get basic attr info
	Tango::Attr* attr= device_attr->GetAttr();
	bool isMemorized= device_attr->IsMemorized();
	std::string attr_name= attr->get_name();	
	long int data_type= attr->get_type();
	Tango::AttrDataFormat data_format= attr->get_format();
	Tango::AttrWriteType attr_write_type= attr->get_writable();
	Tango::DispLevel dispLevel= attr->get_disp_level();
	long int data_size= 1;
	if(data_format==Tango::SCALAR){
		data_size= 1;
	}
	else if(data_format==Tango::SPECTRUM){
		data_size= dynamic_cast<Tango::SpectrumAttr*>(attr)->get_max_x();
	}
	else{
		std::stringstream ss;	
		ss<<"Invalid/unsupported data format (format="<<data_format<<"), cannot create dyn attr!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("CREATE_DYN_ATTR_INFO_FAILED",ss.str());	
	}
		
	//##################################
	//## Fill dyn attribute info
	//##################################
	dai.dev = this;

	//--> TangoAttributeInfoEx
	/*
	string 			name;               ///< Name
	AttrWriteType 	writable;           ///< Writable type (Read, Write,...)
	AttrDataFormat 	data_format;        ///< Data format (Scalar, Spectrum,...)
	int 			data_type;          ///< Data type
	int 			max_dim_x;          ///< Max dim X
	int 			max_dim_y;          ///< Max dim Y
	string 			description;        ///< Description
	string 			label;              ///< Label
	string 			unit;               ///< Unit
	string 			standard_unit;      ///< Standard unit
	string 			display_unit;       ///< Display unit
	string 			format;             ///< Format
	string 			min_value;          ///< Min value
	string 			max_value;          ///< Max value
	string 			min_alarm;          ///< Min alarm
	string 			max_alarm;          ///< Max alarm
	string 			writable_attr_name; ///< Writable att. name
	*/
  dai.tai.name = attr_name;
	dai.tai.writable = attr_write_type;
  dai.tai.data_format = data_format;
	dai.tai.data_type= data_type;
	dai.tai.max_dim_x= data_size;//This is not settable (hard-coded in Tango!), set to LONG_MAX in yat4tango
	dai.tai.description= multi_attr_prop->description;
	dai.tai.label= multi_attr_prop->label;
	dai.tai.unit= multi_attr_prop->unit;
	dai.tai.format= multi_attr_prop->format;
	dai.tai.min_value= (multi_attr_prop->min_value).get_str();
	dai.tai.max_value= (multi_attr_prop->max_value).get_str();
	dai.tai.min_alarm= (multi_attr_prop->min_alarm).get_str();
	dai.tai.max_alarm= (multi_attr_prop->max_alarm).get_str();
	dai.tai.disp_level = dispLevel;//Tango::OPERATOR;

	//--> Read & Write callbacks
  if(attr_write_type==Tango::READ || attr_write_type==Tango::READ_WRITE || attr_write_type==Tango::READ_WITH_WRITE) 
		dai.rcb = yat4tango::DynamicAttributeReadCallback::instanciate(*this, &LMCDevice::dyn_attr_read_cb);
  if(attr_write_type==Tango::WRITE || attr_write_type==Tango::READ_WRITE || attr_write_type==Tango::READ_WITH_WRITE) 
		dai.wcb = yat4tango::DynamicAttributeWriteCallback::instanciate(*this,&LMCDevice::dyn_attr_write_cb);
	
	//if(isMemorized) dai.tai.memorized= Tango::MEMORIZED;//Tango::MEMORIZED_WRITE_INIT
	if(isMemorized) {
		dai.memorized= true;
		dai.write_memorized_value_at_init= false;
	}
	else{
		dai.memorized= false;
	}
	if(enableDynAttrCleanup) dai.cdb = true;
	else dai.cdb = false;
	if(device_attr->IsPolled()) dai.polling_period_in_ms= attr->get_polling_period();

	return 0;

}//close CreateDynAttrInfo()

template<typename T,typename K>
inline void LMCDevice::CreateAndInitDynAttr(Utils_ns::DeviceAttr* device_attr){

	//Check input attr parsed from config
	if(!device_attr) {
		std::string errMsg("Null ptr to input device given!");
		_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",errMsg);
	}

	//## Get basic attr info
	Tango::Attr* attr= device_attr->GetAttr();
	std::string attr_name= attr->get_name();	
	long int data_type= attr->get_type();
	Tango::AttrDataFormat data_format= attr->get_format();
	bool isMemorized= device_attr->IsMemorized();	
	
	long int data_size= 1;
	if(data_format==Tango::SCALAR){
		data_size= 1;
	}
	else if(data_format==Tango::SPECTRUM){
		data_size= dynamic_cast<Tango::SpectrumAttr*>(attr)->get_max_x();
	}
	else{
		std::stringstream ss;	
		ss<<"Invalid/unsupported data format (format="<<data_format<<"), cannot create dyn attr!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("CREATE_DYN_ATTR_INFO_FAILED",ss.str());	
	}
		
	//## Get attr prop
	Tango::MultiAttrProp<K>* multi_attr_prop= 0;
	try{
		multi_attr_prop= (dynamic_cast<Utils_ns::DeviceAttrImpl<K>*>(device_attr))->prop;
	}
	catch(std::exception& e){
		_ERROR_LOG("C++ exception while casting multi_attr_prop of dyn attr (err="<<e.what()<<")!");
		throw;
	}

	//## Set init value for memorized attrs from default value property (if given)
	bool hasMemorizedDefaultPropName= false;
	if(isMemorized){
		std::string memorized_default_prop_name= device_attr->GetDefaultPropName();	
		_DEBUG_LOG("Retrieving attr "<<attr_name<<" init value from property "<<memorized_default_prop_name<<" ...");
		if(memorized_default_prop_name!=""){
			T prop_value;
			if(Utils_ns::TangoUtils::ExtractPropValue<T>(this,prop_value,memorized_default_prop_name)==0){
				_DEBUG_LOG("Setting attr "<<attr_name<<" init value to "<<prop_value<<" ...");
				device_attr->SetInitData(prop_value);
				hasMemorizedDefaultPropName= true;
			}
			else{
				_ERROR_LOG("Failed to extract value from property "<<memorized_default_prop_name<<", memorized attr will be initialized to const value in config!");
			}
		}//close if property not empty
	}//close if is memorized
	
	//###########################################################
	//##           CHECK ATTRIBUTE IN DEVICE
	//###########################################################
	//## Check if attribute is already present in the attribute list
	bool hasAttr= Utils_ns::TangoUtils::HasAttribute(this,attr_name);
	if(hasAttr) _DEBUG_LOG("Attribute "<<attr_name<<" (type="<<data_type<<", data_format="<<data_format<<") already exist in the list, update it!");
	else _DEBUG_LOG("Attribute "<<attr_name<<" (type="<<data_type<<", data_format="<<data_format<<") is not present is the list, adding it!");	
	
	//##################################
	//## Get dyn attribute info
	//##################################
	yat4tango::DynamicAttributeInfo dai;
	if(CreateDynAttrInfo<K>(dai,device_attr,multi_attr_prop)<0){
		std::string errMsg("Failed to create a dyn attr info!");
		_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",errMsg);
	}
  
	//##################################
	//## Create a new attr		
	//##################################
	Utils_ns::AttrDataPtr ad= 0;

	try{
		//Add attribute if not present
		if(!hasAttr) {				
			//Create and init scalar attr
			T initValue;
			if(device_attr->GetInitValue<T>(initValue)<0){
				std::stringstream ss;	
				ss<<"Failed to get init value stored in device attr "<<attr_name<<", cannot create dyn attr!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
			}
			_DEBUG_LOG("Retrieved init value "<<initValue<<" for attr "<<attr_name<<" ...");

			if(data_format==Tango::SCALAR) {		
				_DEBUG_LOG("Create and init scalar attr "<<attr_name<<" ...");
				ad = new Utils_ns::ScalarAttrData<T>(attr_name,data_type,initValue);
			}
			else if(data_format==Tango::SPECTRUM) {
				_DEBUG_LOG("Create and init spectrum attr "<<attr_name<<" ...");
				ad = new Utils_ns::SpectrumAttrData<T>(attr_name,data_type,data_size,initValue); 	
			}
			else{
				std::stringstream ss;	
				ss<<"Invalid/unsupported data format (format="<<data_format<<"), cannot create dyn attr!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
			} 

			//Set labels (for enum)
			ad->set_labels(multi_attr_prop->enum_labels);

			//Attach user data to dyn attr
			dai.set_user_data(ad);

			//Add attr to device
			_DEBUG_LOG("Adding attr "<<attr_name<<" to dyn manager...");
			m_dam->add_attribute(dai);
		}
			
		//Retrieve attr list
		_DEBUG_LOG("Retrieve attr list...");
		long attr_index= get_device_attr()->get_attr_ind_by_name(attr_name.c_str());		
		std::vector<Tango::Attribute*> attribute_list= get_device_attr()->get_attribute_list();
		
		//Set properties	
		_DEBUG_LOG("Set attr properties...");
		attribute_list[attr_index]->set_properties(*multi_attr_prop);

		//If memorized add default property name
		if(isMemorized && hasMemorizedDefaultPropName){
			std::string memorized_default_prop_name= device_attr->GetDefaultPropName();
			if(Utils_ns::TangoUtils::PutAttrProp(this,attr_name,"default_value_prop_name",memorized_default_prop_name)<0){
				_ERROR_LOG("Failed to set attr property default_value_prop_name (value="<<memorized_default_prop_name<<") for attr "<<attr_name<<"!");
			}
		}

		//Set generator attr properties
		_DEBUG_LOG("Set generator attr properties...");	
		yat4tango::DynamicAttribute & dyn_attr = m_dam->get_attribute(attr_name);
		Utils_ns::AttrDataPtr attr_data= 0;
		if(data_format==Tango::SCALAR) {
			attr_data = dynamic_cast<Utils_ns::ScalarAttrData<T>*>(dyn_attr.get_user_data<Utils_ns::AttrData>()); 

			//Set safe value (if given)
			if(device_attr->HasSafeValue()){
				T safeValue;
				if(device_attr->GetSafeValue<T>(safeValue)<0){
					std::stringstream ss;	
					ss<<"Failed to get safe value stored in device attr "<<attr_name<<", cannot create dyn attr!";
					_ERROR_LOG(ss.str());
					_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
				}

				dynamic_cast<Utils_ns::ScalarAttrData<T>*>(attr_data)->set_safe_data_value(safeValue);
				
			}//close if has safe value

			if(InitDynAttrFromGeneratorProps<T>(attr_data,device_attr,attr_name)<0){
				std::stringstream ss;	
				ss<<"Failed to add generator properties to attribute "<<attr_name;
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());
			}
		}
		else if(data_format==Tango::SPECTRUM) {
			attr_data = dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(dyn_attr.get_user_data<Utils_ns::AttrData>());

			//Set safe value (if given)
			if(device_attr->HasSafeValue()){
				T safeValue;
				if(device_attr->GetSafeValue<T>(safeValue)<0){
					std::stringstream ss;	
					ss<<"Failed to get safe value stored in device attr "<<attr_name<<", cannot create dyn attr!";
					_ERROR_LOG(ss.str());
					_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
				}

				dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(attr_data)->set_safe_data_value(safeValue);
			}//close if has safe value

			if(InitDynSpectrumAttrFromGeneratorProps<T>(attr_data,device_attr,attr_name)<0){
				std::stringstream ss;	
				ss<<"Failed to add generator properties to attribute "<<attr_name;
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());
			}
		}		
		else{
			std::stringstream ss;	
			ss<<"Invalid/unsupported data format (format="<<data_format<<"), cannot retrieve created dyn attr!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
		} 

		//Poll attribute?
		if(device_attr->IsPolled()){
			_DEBUG_LOG("Start polling on attribute "<<attr_name<<" [poll period="<<attr->get_polling_period()<<"]...");
			poll_attribute(attr_name,attr->get_polling_period());
		}

	}//close try block
	catch(Tango::DevFailed &e){
		_ERROR_LOG("Adding attr "<<attr_name<<" failed!");
		throw;
	}
	catch(std::exception &e){
		_ERROR_LOG("C++ exception occurred while adding attr "<<attr_name<<" (err="<<e.what()<<")!");
		throw;
	}
	catch(...){
		_ERROR_LOG("Unknown exception occurred while adding attr "<<attr_name<<"!");
		throw;
	}


	//##################################
	//## Register attr formula	
	//##################################
	if(device_attr->HasFormulaData() && RegisterFormula(device_attr,ad)<0){
		std::stringstream ss;	
		ss<<"Failed to register formula for attribute "<<attr_name;
		_WARN_LOG(ss.str());
	}

}//close CreateAndInitDynAttr()



template<>
inline void LMCDevice::CreateAndInitDynAttr<Tango::DevState,Tango::DevState>(Utils_ns::DeviceAttr* device_attr){

	//Check input attr parsed from config
	if(!device_attr) {
		std::string errMsg("Null ptr to input device given!");
		_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",errMsg);
	}

	//## Get basic attr info
	Tango::Attr* attr= device_attr->GetAttr();
	std::string attr_name= attr->get_name();	
	long int data_type= attr->get_type();
	Tango::AttrDataFormat data_format= attr->get_format();
	bool isMemorized= device_attr->IsMemorized();	
	
	long int data_size= 1;
	if(data_format==Tango::SCALAR){
		data_size= 1;
	}
	else if(data_format==Tango::SPECTRUM){
		data_size= dynamic_cast<Tango::SpectrumAttr*>(attr)->get_max_x();
	}
	else{
		std::stringstream ss;	
		ss<<"Invalid/unsupported data format (format="<<data_format<<"), cannot create dyn attr!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("CREATE_DYN_ATTR_INFO_FAILED",ss.str());	
	}
		
	//## Get attr prop
	Tango::MultiAttrProp<Tango::DevState>* multi_attr_prop= 0;
	try{
		multi_attr_prop= (dynamic_cast<Utils_ns::DeviceAttrImpl<Tango::DevState>*>(device_attr))->prop;
	}
	catch(std::exception& e){
		_ERROR_LOG("C++ exception while casting multi_attr_prop of dyn attr (err="<<e.what()<<")!");
		throw;
	}

	//## Set init value for memorized attrs from default value property (if given)
	bool hasMemorizedDefaultPropName= false;
	if(isMemorized){
		std::string memorized_default_prop_name= device_attr->GetDefaultPropName();	
		if(memorized_default_prop_name!=""){
			std::string prop_value;
			if(Utils_ns::TangoUtils::ExtractPropValue<std::string>(this,prop_value,memorized_default_prop_name)==0){
				Tango::DevState initValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevState>(prop_value);
				device_attr->SetInitData(initValue);
				hasMemorizedDefaultPropName= true;
			}
			else{
				_ERROR_LOG("Failed to extract value from property "<<memorized_default_prop_name<<", memorized attr will be initialized to const value in config!");
			}
		}//close if property not empty
	}//close if is memorized
	
	//###########################################################
	//##           CHECK ATTRIBUTE IN DEVICE
	//###########################################################
	//## Check if attribute is already present in the attribute list
	bool hasAttr= Utils_ns::TangoUtils::HasAttribute(this,attr_name);
	if(hasAttr) _DEBUG_LOG("Attribute "<<attr_name<<" (type="<<data_type<<", data_format="<<data_format<<") already exist in the list, update it!");
	else _DEBUG_LOG("Attribute "<<attr_name<<" (type="<<data_type<<", data_format="<<data_format<<") is not present is the list, adding it!");	
	
	//##################################
	//## Get dyn attribute info
	//##################################
	yat4tango::DynamicAttributeInfo dai;
	if(CreateDynAttrInfo<Tango::DevState>(dai,device_attr,multi_attr_prop)<0){
		std::string errMsg("Failed to create a dyn attr info!");
		_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",errMsg);
	}
  
	//##################################
	//## Create a new attr		
	//##################################
	Utils_ns::AttrDataPtr ad= 0;

	try{
		//Add attribute if not present
		if(!hasAttr) {				
			//Create and init scalar attr
			_DEBUG_LOG("Get init value set in DeviceAttr...");

			Tango::DevState initValue;
			if(device_attr->GetInitValue<Tango::DevState>(initValue)<0){
				std::stringstream ss;	
				ss<<"Failed to get init value stored in device attr "<<attr_name<<", cannot create dyn attr!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
			}

			if(data_format==Tango::SCALAR) {		
				_DEBUG_LOG("Create and init scalar attr "<<attr_name<<" ...");
				ad = new Utils_ns::ScalarAttrData<Tango::DevState>(attr_name,data_type,initValue);
			}
			else if(data_format==Tango::SPECTRUM) {
				_DEBUG_LOG("Create and init spectrum attr "<<attr_name<<" ...");
				ad = new Utils_ns::SpectrumAttrData<Tango::DevState>(attr_name,data_type,data_size,initValue); 	
			}
			else{
				std::stringstream ss;	
				ss<<"Invalid/unsupported data format (format="<<data_format<<"), cannot create dyn attr!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
			} 

			//Set labels (for enum)
			ad->set_labels(multi_attr_prop->enum_labels);

			//Attach user data to dyn attr
			dai.set_user_data(ad);

			//Add attr to device
			_DEBUG_LOG("Adding attr "<<attr_name<<" to dyn manager...");
			m_dam->add_attribute(dai);
		}
			
		//Retrieve attr list
		_DEBUG_LOG("Retrieve attr list...");
		long attr_index= get_device_attr()->get_attr_ind_by_name(attr_name.c_str());		
		std::vector<Tango::Attribute*> attribute_list= get_device_attr()->get_attribute_list();
		
		//Set properties	
		_DEBUG_LOG("Set attr properties...");
		attribute_list[attr_index]->set_properties(*multi_attr_prop);

		//If memorized add default property name
		if(isMemorized && hasMemorizedDefaultPropName){
			std::string memorized_default_prop_name= device_attr->GetDefaultPropName();
			if(Utils_ns::TangoUtils::PutAttrProp(this,attr_name,"default_value_prop_name",memorized_default_prop_name)<0){
				_ERROR_LOG("Failed to set attr property default_value_prop_name (value="<<memorized_default_prop_name<<") for attr "<<attr_name<<"!");
			}
		}

		//Set generator attr properties
		_DEBUG_LOG("Set generator attr properties...");	
		yat4tango::DynamicAttribute & dyn_attr = m_dam->get_attribute(attr_name);
		Utils_ns::AttrDataPtr attr_data= 0;
		if(data_format==Tango::SCALAR) {
			attr_data = dynamic_cast<Utils_ns::ScalarAttrData<Tango::DevState>*>(dyn_attr.get_user_data<Utils_ns::AttrData>()); 

			//Set safe value (if given)
			if(device_attr->HasSafeValue()){
				Tango::DevState safeValue;
				if(device_attr->GetSafeValue<Tango::DevState>(safeValue)<0){
					std::stringstream ss;	
					ss<<"Failed to get safe value stored in device attr "<<attr_name<<", cannot create dyn attr!";
					_ERROR_LOG(ss.str());
					_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
				}

				dynamic_cast<Utils_ns::ScalarAttrData<Tango::DevState>*>(attr_data)->set_safe_data_value(safeValue);
				
			}//close if has safe value

			if(InitDynAttrFromGeneratorProps<Tango::DevState>(attr_data,device_attr,attr_name)<0){
				std::stringstream ss;	
				ss<<"Failed to add generator properties to attribute "<<attr_name;
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());
			}
		}
		else if(data_format==Tango::SPECTRUM) {
			attr_data = dynamic_cast<Utils_ns::SpectrumAttrData<Tango::DevState>*>(dyn_attr.get_user_data<Utils_ns::AttrData>());

			//Set safe value (if given)
			if(device_attr->HasSafeValue()){
				Tango::DevState safeValue;
				if(device_attr->GetSafeValue<Tango::DevState>(safeValue)<0){
					std::stringstream ss;	
					ss<<"Failed to get safe value stored in device attr "<<attr_name<<", cannot create dyn attr!";
					_ERROR_LOG(ss.str());
					_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
				}

				dynamic_cast<Utils_ns::SpectrumAttrData<Tango::DevState>*>(attr_data)->set_safe_data_value(safeValue);
			}//close if has safe value

			if(InitDynSpectrumAttrFromGeneratorProps<Tango::DevState>(attr_data,device_attr,attr_name)<0){
				std::stringstream ss;	
				ss<<"Failed to add generator properties to attribute "<<attr_name;
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());
			}
		}		
		else{
			std::stringstream ss;	
			ss<<"Invalid/unsupported data format (format="<<data_format<<"), cannot retrieve created dyn attr!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("CREATE_INIT_DYN_ATTR_FAILED",ss.str());	
		} 

		//Poll attribute?
		if(device_attr->IsPolled()){
			_DEBUG_LOG("Start polling on attribute "<<attr_name<<" [poll period="<<attr->get_polling_period()<<"]...");
			poll_attribute(attr_name,attr->get_polling_period());
		}

	}//close try block
	catch(Tango::DevFailed &e){
		_ERROR_LOG("Adding attr "<<attr_name<<" failed!");
		throw;
	}
	catch(std::exception &e){
		_ERROR_LOG("C++ exception occurred while adding attr "<<attr_name<<" (err="<<e.what()<<")!");
		throw;
	}
	catch(...){
		_ERROR_LOG("Unknown exception occurred while adding attr "<<attr_name<<"!");
		throw;
	}


	//##################################
	//## Register attr formula	
	//##################################
	if(device_attr->HasFormulaData() && RegisterFormula(device_attr,ad)<0){
		std::stringstream ss;	
		ss<<"Failed to register formula for attribute "<<attr_name;
		_WARN_LOG(ss.str());
	}

}//close CreateAndInitDynAttr() specialization for DevState

template<typename T>
void LMCDevice::EraseScalarDynAttrUserData(yat4tango::DynamicAttribute & dyn_attr){

	try {
		Utils_ns::ScalarAttrData<T>* attr_data= dynamic_cast<Utils_ns::ScalarAttrData<T>*>(dyn_attr.get_user_data<Utils_ns::AttrData>());
		if(attr_data){
			delete attr_data;
			attr_data= 0;
		} 
	}
	catch(...){
		throw;
	}

}//close EraseScalarDynAttrUserData()

template<typename T>
void LMCDevice::EraseSpectrumDynAttrUserData(yat4tango::DynamicAttribute & dyn_attr){

	try {
		Utils_ns::SpectrumAttrData<T>* attr_data= dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(dyn_attr.get_user_data<Utils_ns::AttrData>());
		if(attr_data){
			delete attr_data;
			attr_data= 0;
		} 
	}
	catch(...){
		throw;
	}

}//close EraseScalarDynAttrUserData()


//===============================================================
//===               RANDOMIZE DYN ATTR UNIFORM 
//===============================================================
template<typename T>
inline void LMCDevice::RandomizeScalarDynAttrUniform(Tango::DbData& genModelParData,std::string& attr_name,long data_type){

	//Get model par data
	T minValue= 0;
	T maxValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;

	//Generate random number
	std::uniform_real_distribution<T> distribution (minValue,std::nextafter(maxValue, DBL_MAX));
	T attr_value= distribution(m_randGenerator);
			
	//Set attribute	value
	if(SetDynAttrValue<T>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
	}

}//close RandomizeScalarDynAttrUniform()

template<>
inline void LMCDevice::RandomizeScalarDynAttrUniform<Tango::DevLong>(Tango::DbData& genModelParData,std::string& attr_name,long data_type){

	//Get model par data
	Tango::DevLong minValue= 0;
	Tango::DevLong maxValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;

	//Generate random number
	std::uniform_int_distribution<Tango::DevLong> distribution (minValue,std::nextafter(maxValue, DBL_MAX));
	Tango::DevLong attr_value= distribution(m_randGenerator);
			
	//Set attribute	value
	if(SetDynAttrValue<Tango::DevLong>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
	}

}//close RandomizeScalarDynAttrUniform() specialization for long

template<>
inline void LMCDevice::RandomizeScalarDynAttrUniform<Tango::DevShort>(Tango::DbData& genModelParData,std::string& attr_name,long data_type){

	//Get model par data
	Tango::DevShort minValue= 0;
	Tango::DevShort maxValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;

	//Generate random number
	std::uniform_int_distribution<Tango::DevShort> distribution (minValue,std::nextafter(maxValue, DBL_MAX));
	Tango::DevShort attr_value= distribution(m_randGenerator);
			
	//Set attribute	value
	if(data_type==Tango::DEV_SHORT){
		if(SetDynAttrValue<Tango::DevShort>(attr_value,attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to set dyn attr "<<attr_name<<" value!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
		}
	}
	#if TANGO_VERSION_MAJOR >= 9
	else if(data_type==Tango::DEV_ENUM){
		if(SetDynAttrValue<Tango::DevEnum>(attr_value,attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to set dyn attr "<<attr_name<<" value!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
		}
	}
	#endif
	else{
		std::stringstream ss;
		ss<<"Invalid/unsupported data type!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
	}

}//close RandomizeScalarDynAttrUniform() specialization for short



template<>
inline void LMCDevice::RandomizeScalarDynAttrUniform<Tango::DevBoolean>(Tango::DbData& genModelParData,std::string& attr_name,long data_type){

	//Generate random number
	std::uniform_int_distribution<int> distribution (0,1);
	bool attr_value= static_cast<bool>(distribution(m_randGenerator));
			
	//Set attribute	value
	if(SetDynAttrValue<Tango::DevBoolean>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
	}

}//close RandomizeScalarDynAttrUniform() specialization for bool

template<>
inline void LMCDevice::RandomizeScalarDynAttrUniform<Tango::DevString>(Tango::DbData& genModelParData,std::string& attr_name,long data_type){

	std::stringstream ss;
	ss<<"Uniform randomization for string attr is invalid/unsupported!";
	_ERROR_LOG(ss.str());
	_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	

}//close RandomizeScalarDynAttrUniform() specialization for string

template<typename T>
inline void LMCDevice::RandomizeSpectrumDynAttrUniform(Tango::DbData& genModelParData,std::string& attr_name,long data_type,long data_size){

	//Get model par data
	T minValue= 0;
	T maxValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;

	//Generate random number
	std::uniform_real_distribution<T> distribution (minValue,std::nextafter(maxValue, DBL_MAX));
	std::vector<T> attr_values;
	for(int i=0;i<data_size;i++){
		T attr_value= distribution(m_randGenerator);
		attr_values.push_back(attr_value);
	}
			
	//Set attribute	value
	if(SetDynAttrValue<T>(attr_values,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_UNIFORM_FAILED",ss.str());	
	}

}//close RandomizeSpectrumDynAttrUniform()


template<>
inline void LMCDevice::RandomizeSpectrumDynAttrUniform<Tango::DevLong>(Tango::DbData& genModelParData,std::string& attr_name,long data_type,long data_size){

	//Get model par data
	Tango::DevLong minValue= 0;
	Tango::DevLong maxValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;

	//Generate random number
	std::uniform_int_distribution<Tango::DevLong> distribution (minValue,maxValue);
	std::vector<Tango::DevLong> attr_values;
	for(int i=0;i<data_size;i++){
		Tango::DevLong attr_value= distribution(m_randGenerator);
		attr_values.push_back(attr_value);
	}
			
	//Set attribute	value
	if(SetDynAttrValue<Tango::DevLong>(attr_values,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_UNIFORM_FAILED",ss.str());	
	}

}//close RandomizeSpectrumDynAttrUniform() specialization for long


template<>
inline void LMCDevice::RandomizeSpectrumDynAttrUniform<Tango::DevShort>(Tango::DbData& genModelParData,std::string& attr_name,long data_type,long data_size){

	//Get model par data
	Tango::DevShort minValue= 0;
	Tango::DevShort maxValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;

	//Generate random number
	std::uniform_int_distribution<Tango::DevShort> distribution (minValue,maxValue);
	std::vector<Tango::DevShort> attr_values;
	for(int i=0;i<data_size;i++){
		Tango::DevLong attr_value= distribution(m_randGenerator);
		attr_values.push_back(attr_value);
	}
			
	//Set attribute	value
	if(data_type==Tango::DEV_SHORT){
		if(SetDynAttrValue<Tango::DevShort>(attr_values,attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to set dyn attr "<<attr_name<<" value!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
		}
	}
	#if TANGO_VERSION_MAJOR >= 9
	else if(data_type==Tango::DEV_ENUM){
		if(SetDynAttrValue<Tango::DevEnum>(attr_values,attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to set dyn attr "<<attr_name<<" value!";
			_ERROR_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
		}
	}
	#endif	
	else{
		std::stringstream ss;
		ss<<"Invalid/unsupported data type!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_DYN_ATTR_UNIFORM_FAILED",ss.str());	
	}


}//close RandomizeSpectrumDynAttrUniform() specialization for short



template<>
inline void LMCDevice::RandomizeSpectrumDynAttrUniform<Tango::DevBoolean>(Tango::DbData& genModelParData,std::string& attr_name,long data_type,long data_size){

	//Generate random number
	std::uniform_int_distribution<int> distribution (0,1);
	std::vector<Tango::DevBoolean> attr_values;
	for(int i=0;i<data_size;i++){
		bool attr_value= static_cast<bool>(distribution(m_randGenerator));
		attr_values.push_back(attr_value);
	}
		
	//Set attribute	value
	if(SetDynAttrValue<Tango::DevBoolean>(attr_values,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_UNIFORM_FAILED",ss.str());	
	}

}//close RandomizeSpectrumDynAttrUniform() specialization for bool


template<>
inline void LMCDevice::RandomizeSpectrumDynAttrUniform<Tango::DevString>(Tango::DbData& genModelParData,std::string& attr_name,long data_type,long data_size){

	std::stringstream ss;
	ss<<"Uniform randomization for string attr is invalid/unsupported!";
	_ERROR_LOG(ss.str());
	_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_UNIFORM_FAILED",ss.str());	

}//close RandomizeSpectrumDynAttrUniform() specialization for string


//===============================================================
//===               RANDOMIZE DYN ATTR GAUSSIAN 
//===============================================================

template<typename T>
inline void LMCDevice::RandomizeScalarDynAttrGaussian(Tango::DbData& genModelParData,std::string& attr_name,long data_type){

	//Get model par data
	T minValue= 0;
	T maxValue= 0;
	T meanValue= 0;
	T sigmaValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;
	genModelParData[2]>>meanValue;
	genModelParData[3]>>sigmaValue;

	//Generate random number
	std::normal_distribution<T> distribution (meanValue,sigmaValue);
	T attr_value= 0;
	int nRandGen= 0;
	bool isFailed= false;
	int maxRandomGenerations= 1000;

	while (true) {
  	attr_value= distribution(m_randGenerator);
		if (attr_value>=minValue && attr_value<=maxValue) break;
		nRandGen++;
		if(nRandGen>=maxRandomGenerations){
			isFailed= true;
			break;
		}
  }
				
	if(isFailed){
		std::string errMsg= "Gaussian randomization failed (too many random generation)!";
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SCALAR_DYN_ATTR_GAUSSIAN_FAILED",errMsg);	
	}
		
	//Set attribute	value
	if(SetDynAttrValue<T>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_UNIFORM_FAILED",ss.str());		
	}

}//close RandomizeScalarDynAttrGaussian()


template<>
inline void LMCDevice::RandomizeScalarDynAttrGaussian<Tango::DevLong>(Tango::DbData& genModelParData,std::string& attr_name,long data_type){

	//Get model par data
	Tango::DevLong minValue= 0;
	Tango::DevLong maxValue= 0;
	Tango::DevLong meanValue= 0;
	Tango::DevLong sigmaValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;
	genModelParData[2]>>meanValue;
	genModelParData[3]>>sigmaValue;

	//Generate random number
	std::normal_distribution<double> distribution (meanValue,sigmaValue);
	double random_value= 0;
	int nRandGen= 0;
	bool isFailed= false;
	int maxRandomGenerations= 1000;

	while (true) {
  	random_value= distribution(m_randGenerator);
    if (random_value>=minValue && random_value<=maxValue) break;	
		nRandGen++;
		if(nRandGen>=maxRandomGenerations){
			isFailed= true;
			break;
		}
	}

	if(isFailed){
		std::string errMsg= "Gaussian randomization failed (too many random generation)!";
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SCALAR_DYN_ATTR_GAUSSIAN_FAILED",errMsg);	
	}

	//Casting number
	Tango::DevLong random_value_casted= static_cast<Tango::DevLong>( std::round(random_value) );
	Tango::DevLong attr_value= std::min(random_value_casted,maxValue);
	attr_value= std::max(random_value_casted,minValue);

	//Set attribute	value
	if(SetDynAttrValue<Tango::DevLong>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_GAUSSIAN_FAILED",ss.str());		
	}
}//close RandomizeScalarDynAttrGaussian specialization for long


template<>
inline void LMCDevice::RandomizeScalarDynAttrGaussian<Tango::DevShort>(Tango::DbData& genModelParData,std::string& attr_name,long data_type){

	//Get model par data
	Tango::DevShort minValue= 0;
	Tango::DevShort maxValue= 0;
	Tango::DevShort meanValue= 0;
	Tango::DevShort sigmaValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;
	genModelParData[2]>>meanValue;
	genModelParData[3]>>sigmaValue;

	//Generate random number
	std::normal_distribution<double> distribution (meanValue,sigmaValue);
	double random_value= 0;
	int nRandGen= 0;
	bool isFailed= false;
	int maxRandomGenerations= 1000;

	while (true) {
  	random_value= distribution(m_randGenerator);
    if (random_value>=minValue && random_value<=maxValue) break;	
		nRandGen++;
		if(nRandGen>=maxRandomGenerations){
			isFailed= true;
			break;
		}
	}

	if(isFailed){
		std::string errMsg= "Gaussian randomization failed (too many random generation)!";
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SCALAR_DYN_ATTR_GAUSSIAN_FAILED",errMsg);	
	}

	//Casting number
	Tango::DevShort random_value_casted= static_cast<Tango::DevShort>( std::round(random_value) );
	Tango::DevShort attr_value= std::min(random_value_casted,maxValue);
	attr_value= std::max(random_value_casted,minValue);

	//Set attribute	value
	if(SetDynAttrValue<Tango::DevShort>(attr_value,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_GAUSSIAN_FAILED",ss.str());		
	}
}//close RandomizeScalarDynAttrGaussian specialization for short

template<typename T>
inline void LMCDevice::RandomizeSpectrumDynAttrGaussian(Tango::DbData& genModelParData,std::string& attr_name,long data_type,long data_size){

	//Get model par data
	T minValue= 0;
	T maxValue= 0;
	T meanValue= 0;
	T sigmaValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;
	genModelParData[2]>>meanValue;
	genModelParData[3]>>sigmaValue;

	//Generate random number
	std::normal_distribution<T> distribution (meanValue,sigmaValue);
	std::vector<T> attr_values;
	int maxRandomGenerations= 1000;

	for(int i=0;i<data_size;i++){
		T attr_value= 0;
		int nRandGen= 0;
		bool isFailed= false;
		while (true) {
      attr_value= distribution(m_randGenerator);
			if (attr_value>=minValue && attr_value<=maxValue) break;
			nRandGen++;
			if(nRandGen>=maxRandomGenerations){
				isFailed= true;
				break;
			}
    }//end while
					
		if(isFailed){
			std::string errMsg= "Gaussian randomization failed (too many random generation)!";
			_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_GAUSSIAN_FAILED",errMsg);	
		}
		attr_values.push_back(attr_value);
	}//end loop data size
		
	//Set attribute	value
	if(SetDynAttrValue<T>(attr_values,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_GAUSSIAN_FAILED",ss.str());	
	}

}//close RandomizeSpectrumDynAttrGaussian()


template<>
inline void LMCDevice::RandomizeSpectrumDynAttrGaussian<Tango::DevLong>(Tango::DbData& genModelParData,std::string& attr_name,long data_type,long data_size){

	//Get model par data
	Tango::DevLong minValue= 0;
	Tango::DevLong maxValue= 0;
	Tango::DevLong meanValue= 0;
	Tango::DevLong sigmaValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;
	genModelParData[2]>>meanValue;
	genModelParData[3]>>sigmaValue;

	//Generate random number
	std::normal_distribution<double> distribution (meanValue,sigmaValue);
	std::vector<Tango::DevLong> attr_values;
	int maxRandomGenerations= 1000;

	for(int i=0;i<data_size;i++){
		double attr_value= 0;
		int nRandGen= 0;
		bool isFailed= false;
		while (true) {
    	attr_value= distribution(m_randGenerator);
			if (attr_value>=minValue && attr_value<=maxValue) break;
			nRandGen++;
			if(nRandGen>=maxRandomGenerations){
				isFailed= true;
				break;
			}
		}//end while
					
		if(isFailed){
			std::string errMsg= "Gaussian randomization failed (too many random generation)!";
			_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_GAUSSIAN_FAILED",errMsg);	
		}
				
		//Casting number
		Tango::DevLong attr_value_casted= static_cast<Tango::DevLong>( std::round(attr_value) );
		Tango::DevLong final_attr_value= std::min(attr_value_casted,maxValue);
		final_attr_value= std::max(attr_value_casted,minValue);
		attr_values.push_back(final_attr_value);
	}//end loop data size
		
	//Set attribute	value
	if(SetDynAttrValue<Tango::DevLong>(attr_values,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_GAUSSIAN_FAILED",ss.str());	
	}

}//close RandomizeSpectrumDynAttrGaussian() specialization for long


template<>
inline void LMCDevice::RandomizeSpectrumDynAttrGaussian<Tango::DevShort>(Tango::DbData& genModelParData,std::string& attr_name,long data_type,long data_size){

	//Get model par data
	Tango::DevShort minValue= 0;
	Tango::DevShort maxValue= 0;
	Tango::DevShort meanValue= 0;
	Tango::DevShort sigmaValue= 0;
	genModelParData[0]>>minValue;
	genModelParData[1]>>maxValue;
	genModelParData[2]>>meanValue;
	genModelParData[3]>>sigmaValue;

	//Generate random number
	std::normal_distribution<double> distribution (meanValue,sigmaValue);
	std::vector<Tango::DevShort> attr_values;
	int maxRandomGenerations= 1000;

	for(int i=0;i<data_size;i++){
		double attr_value= 0;
		int nRandGen= 0;
		bool isFailed= false;
		while (true) {
    	attr_value= distribution(m_randGenerator);
			if (attr_value>=minValue && attr_value<=maxValue) break;
			nRandGen++;
			if(nRandGen>=maxRandomGenerations){
				isFailed= true;
				break;
			}
		}//end while
					
		if(isFailed){
			std::string errMsg= "Gaussian randomization failed (too many random generation)!";
			_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_GAUSSIAN_FAILED",errMsg);	
		}
				
		//Casting number
		Tango::DevShort attr_value_casted= static_cast<Tango::DevShort>( std::round(attr_value) );
		Tango::DevShort final_attr_value= std::min(attr_value_casted,maxValue);
		final_attr_value= std::max(attr_value_casted,minValue);
		attr_values.push_back(final_attr_value);
	}//end loop data size
		
	//Set attribute	value
	if(SetDynAttrValue<Tango::DevShort>(attr_values,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set attr "<<attr_name<<" value!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("RANDOMIZE_SPECTRUM_DYN_ATTR_GAUSSIAN_FAILED",ss.str());	
	}

}//close RandomizeSpectrumDynAttrGaussian() specialization for short



//===============================================================
//===       SET DYN ATTR FROM DB DATUM 
//===============================================================
template<typename T,typename K>
inline int LMCDevice::SetScalarDynAttrValueFromDbDatum(Tango::DbDatum& db_datum,std::string& attr_name){

	T attr_value= T();
	db_datum>>attr_value;

	K attr_value_casted= static_cast<K>(attr_value);

	if(SetDynAttrValue<K>(attr_value_casted,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name;
		_THROW_TANGO_EXCEPTION("SET_SCALAR_DYN_ATTR_FROM_DBDATUM_FAILED",ss.str());					
	}

	return 0;

}//close SetScalarDynAttrValueFromDbDatum()

template<typename T,typename K>
inline int LMCDevice::SetSpectrumDynAttrValueFromDbDatum(Tango::DbDatum& db_datum,std::string& attr_name,long data_size){

	T attr_value;
	db_datum>>attr_value;
	//std::vector<K> attr_values(data_size,attr_value);
	std::vector<K> attr_values;
	for(long i=0;i<data_size;i++) attr_values.push_back( static_cast<K>(attr_value) );		
		
	if(SetDynAttrValue<K>(attr_values,attr_name)<0){
		std::stringstream ss;
		ss<<"Failed to set dyn attr "<<attr_name;
		_THROW_TANGO_EXCEPTION("SET_SPECTRUM_DYN_ATTR_FROM_DBDATUM_FAILED",ss.str());	
	}

	return 0;

}//close SetSpectrumDynAttrValueFromDbDatum()


//===============================================================
//===       INIT MEMORIZED DYN ATTR FROM DB DATUM 
//===============================================================
template<typename T>
inline void LMCDevice::InitMemorizedAttrFromDbDatum(Tango::DbDatum& db_datum,Tango::WAttribute& wattr,std::string attr_name,bool hasMemorizedValue,std::string attr_prop_name){

	//Extract property
	T prop_value;
	try {
		if(hasMemorizedValue){
			db_datum>>prop_value;
		}
		else {
			if( Utils_ns::TangoUtils::ExtractPropValue<T>(this,prop_value,attr_prop_name)<0 ){
				std::stringstream ss;
				ss<<"Failed to extract value for property "<<attr_prop_name;
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("INIT_MEMORIZED_DYN_ATTR_FROM_DBDATUM_FAILED",ss.str());	
			}

			if( Utils_ns::TangoUtils::PutAttrProp<T>(this,attr_name,"__value",prop_value)<0 ){
				std::stringstream ss;
				ss<<"Failed to put __value property for attr "<<attr_name<<" in DB!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("INIT_MEMORIZED_DYN_ATTR_FROM_DBDATUM_FAILED",ss.str());	
			}
		}

		
		//Set attribute value in attr manager
		try {
			SetDynAttrValueInManager(prop_value,attr_name);
		}
		catch(Tango::DevFailed& e){
			//Tango::Except::print_exception(e);
			_DEBUG_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr not found, e.g. not created dynamically?)!");
			//throw;
		}
		catch(...){
			_WARN_LOG("Failed to set dyn attribute "<<attr_name<<" from manager (C++ exception)!");
			//throw;
		}	
		

		//Set attr value to property value
		_DEBUG_LOG("Setting attr "<<attr_name<<" to property "<<attr_prop_name<<" value "<<prop_value);
		wattr.set_write_value (prop_value);

	}//close try
	catch(Tango::DevFailed& e){
		throw;		
	}
	catch(...){
		throw;		
	}

}//close InitMemorizedAttrFromDbDatum()


template<>
inline void LMCDevice::InitMemorizedAttrFromDbDatum<Tango::DevState>(Tango::DbDatum& db_datum,Tango::WAttribute& wattr,std::string attr_name,bool hasMemorizedValue,std::string attr_prop_name){

	//Extract property
	Tango::DevShort prop_value;
	try {
		if(hasMemorizedValue){
			db_datum>>prop_value;
		}
		else {
			if( Utils_ns::TangoUtils::ExtractPropValue<Tango::DevShort>(this,prop_value,attr_prop_name)<0 ){
				std::stringstream ss;
				ss<<"Failed to extract value for property "<<attr_prop_name;
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("INIT_MEMORIZED_DYN_ATTR_FROM_DBDATUM_FAILED",ss.str());	
			}

			if( Utils_ns::TangoUtils::PutAttrProp<Tango::DevShort>(this,attr_name,"__value",prop_value)<0 ){
				std::stringstream ss;
				ss<<"Failed to put __value property for attr "<<attr_name<<" in DB!";
				_ERROR_LOG(ss.str());
				_THROW_TANGO_EXCEPTION("INIT_MEMORIZED_DYN_ATTR_FROM_DBDATUM_FAILED",ss.str());	
			}
		}

		Tango::DevState prop_value_casted= static_cast<Tango::DevState>(prop_value); 
		

		//Set attribute value in attr manager
		try{
			SetDynAttrValueInManager(prop_value_casted,attr_name);
		}
		catch(Tango::DevFailed& e){
			//Tango::Except::print_exception(e);
			_DEBUG_LOG("Failed to set dyn attribute "<<attr_name<<" value from manager (attr not found, e.g. not created dynamically?)!");
			//throw;
		}
		catch(...){
			_WARN_LOG("Failed to set dyn attribute "<<attr_name<<" from manager (C++ exception)!");
			//throw;
		}	

		//Set attr value to property value
		wattr.set_write_value (prop_value_casted);

	}//close try
	catch(Tango::DevFailed& e){
		throw;		
	}
	catch(...){
		throw;		
	}

}//close InitMemorizedAttrFromDbDatum() specialization for DevState


