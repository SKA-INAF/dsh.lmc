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
// * DSH.LMC. If not, see http://www.gnu.org/licenses/.                      *
// **************************************************************************

/**
* @file FormulaVar.tpp
* @class FormulaVar
* @brief FormulaVar container 
*
* FormulaVar container 
* @author S. Riggi
* @date 15/01/2016
*/


// ============================================================================
// class: FormulaVar
// ============================================================================
template<typename T>
inline int FormulaVar::GetScalarAttrData(T& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){
			
	//Check attr type
	if(!attr_data) return -1;
	int data_format= attr_data->get_data_format();
	int data_type= attr_data->get_data_type();
	if(data_format!=Tango::SCALAR){
		WARN_LOG("Error while trying to get data for attr format "<<data_format<<" but expected format is "<<Tango::SCALAR<<"!");
		return -1;
	}
		
	//Extract value
	int status= 0;
	switch (data_type) {
		case Tango::DEV_FLOAT: 
			status= GetScalarAttrDataValues<Tango::DevFloat,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_DOUBLE: 
			status= GetScalarAttrDataValues<Tango::DevDouble,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_LONG: 
			status= GetScalarAttrDataValues<Tango::DevLong,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_SHORT: 
			status= GetScalarAttrDataValues<Tango::DevShort,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		#if TANGO_VERSION_MAJOR >= 9
		case Tango::DEV_ENUM: 
			status= GetScalarAttrDataValues<Tango::DevEnum,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		#endif
		case Tango::DEV_STRING: 
			status= GetScalarAttrDataValues<std::string,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_BOOLEAN: 
			status= GetScalarAttrDataValues<Tango::DevBoolean,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_STATE: 
			status= GetScalarAttrDataValues<Tango::DevState,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		default:
		{
			std::stringstream ss;
			ss<<"Unsupported data type ("<<data_type<<") for attribute "<<attr_name<<"!";
			ERROR_LOG(ss.str());
	    status= -1;
      break;
		}	
	}//close switch		
			
	return status;
		
}//close GetScalarAttrData()


template<typename T>
inline int FormulaVar::GetSpectrumAttrData(std::vector<T>& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){
			
	//Check attr type
	if(!attr_data) return -1;
	int data_format= attr_data->get_data_format();
	int data_type= attr_data->get_data_type();
	if(data_format!=Tango::SPECTRUM){
		return -1;
	}
	
	//Extract value
	int status= 0;	
	switch (data_type) {
		case Tango::DEV_FLOAT: 
			status= GetSpectrumAttrDataValues<Tango::DevFloat,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_DOUBLE: 
			status= GetSpectrumAttrDataValues<Tango::DevDouble,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_LONG: 
			status= GetSpectrumAttrDataValues<Tango::DevLong,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_SHORT: 
			status= GetSpectrumAttrDataValues<Tango::DevShort,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		#if TANGO_VERSION_MAJOR >= 9
		case Tango::DEV_ENUM: 
			status= GetSpectrumAttrDataValues<Tango::DevEnum,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		#endif
		case Tango::DEV_STRING: 
			status= GetSpectrumAttrDataValues<std::string,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_BOOLEAN: 
			status= GetSpectrumAttrDataValues<Tango::DevBoolean,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_STATE: 
			status= GetSpectrumAttrDataValues<Tango::DevState,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		default:
		{
			std::stringstream ss;
			ss<<"Unsupported data type ("<<data_type<<") for attribute "<<attr_name<<"!";
			ERROR_LOG(ss.str());
	    status= -1;
      break;
		}	
	}//close switch
	
	return status;

}//close GetSpectrumAttrData()


// ============================================================================
// class: ProxyAttrFormulaVar
// ============================================================================


template<typename T>
inline int ProxyAttrFormulaVar::GetScalarAttrDataFromInitValue(T& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){

	//Do not set timestamp 

	//Set attr quality to INVALID since we are using an init value not a real value read from proxy device
	attr_quality= Tango::ATTR_INVALID;

	//Set attr_value to init value (given in string format)
	std::istringstream istream(attr_init_value_str);
  if (!(istream >> attr_value)){
		WARN_LOG("Failed to convert string data type to templated type!");
		return -1;
	}

	return 0;

}//close GetScalarAttrDataFromInitValue()


template<>
inline int ProxyAttrFormulaVar::GetScalarAttrDataFromInitValue<Tango::DevBoolean>(Tango::DevBoolean& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){

	//Do not set timestamp 

	//Set attr quality to INVALID since we are using an init value not a real value read from proxy device
	attr_quality= Tango::ATTR_INVALID;

	//Set attr_value to init value (given in string format)
	std::istringstream istream(attr_init_value_str);
  if (!(istream >> std::boolalpha >> attr_value)){
		WARN_LOG("Failed to convert string data type to templated type!");
		return -1;
	}

	return 0;

}//close GetScalarAttrDataFromInitValue() specialization for bool



template<typename T>
inline int ProxyAttrFormulaVar::GetSpectrumAttrDataFromInitValue(std::vector<T>& attr_values,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){

	if(exp_data_size<=0) {
		WARN_LOG("Invalid data size (size="<<exp_data_size<<")");
		return -1;
	}

	//Do not set timestamp 

	//Set attr quality to INVALID since we are using an init value not a real value read from proxy device
	attr_quality= Tango::ATTR_INVALID;

	//Set attr_value to init value (given in string format)
	
	for(long int i=0;i<exp_data_size;i++){
		T attr_value;
		std::istringstream istream(attr_init_value_str);
		if (!(istream >> attr_value)){
			WARN_LOG("Failed to convert string data type to templated type!");
			return -1;
		}
		attr_values.push_back(attr_value);
	}//end loop data entries

	/*
	do {
  	for (T thisVal; istream >> thisVal;) {
    	attr_values.push_back(thisVal);
    }
    if (istream.fail()) {
    	istream.clear();
      std::string token;
      istream >> token;
    }
	}
  while (!istream.eof());
	*/

	return 0;

}//close GetSpectrumAttrDataFromInitValue()


template<>
inline int ProxyAttrFormulaVar::GetSpectrumAttrDataFromInitValue<Tango::DevBoolean>(std::vector<Tango::DevBoolean>& attr_values,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){

	if(exp_data_size<=0) {
		WARN_LOG("Invalid data size (size="<<exp_data_size<<")");
		return -1;
	}

	//Do not set timestamp 

	//Set attr quality to INVALID since we are using an init value not a real value read from proxy device
	attr_quality= Tango::ATTR_INVALID;

	//Set attr_value to init value (given in string format)
	
	for(long int i=0;i<exp_data_size;i++){
		Tango::DevBoolean attr_value;
		std::istringstream istream(attr_init_value_str);
		if (!(istream >> std::boolalpha >> attr_value)){
			WARN_LOG("Failed to convert string data type to templated type!");
			return -1;
		}
		attr_values.push_back(attr_value);
	}//end loop data entries

	/*
	do {
  	for (Tango::DevBoolean thisVal; istream >> std::boolalpha >> thisVal;) {
    	attr_values.push_back(thisVal);
    }
    if (istream.fail()) {
    	istream.clear();
      std::string token;
      istream >> token;
    }
	}
  while (!istream.eof());
	*/

	return 0;

}//close GetSpectrumAttrDataFromInitValue() specialization for bool

