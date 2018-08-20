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
* @file AttrFormulaData.tpp
* @class AttrFormulaData
* @brief AttrFormulaData container 
*
* AttrFormulaData container 
* @author S. Riggi
* @date 15/01/2016
*/

// ============================================================================
// class: AttrDataConverter
// ============================================================================
/*
template<typename T,typename K>
struct AttrDataConverter {
  static int convert_scalar(K& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		if(!attr_data) return -1;
		Utils_ns::ScalarAttrData<T>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::ScalarAttrData<T>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		T attr_value= ad->get_data();
		casted_attr_value= static_cast<K>(attr_value);
	
		return 0;
	}//close convert_scalar()	

	static int convert_spectrum(std::vector<K>& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		if(!attr_data) return -1;
		Utils_ns::SpectrumAttrData<T>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		std::vector<T> attr_value= ad->get_data();
		casted_attr_value.clear();
		casted_attr_value= CodeUtils::container_cast<std::vector<K>>(attr_value);

		return 0;
	}//close convert_spectrum()

};//close struct

template<typename T>
struct AttrDataConverter<T,T> {
	static int convert_scalar(T& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		if(!attr_data) return -1;	
		Utils_ns::ScalarAttrData<T>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::ScalarAttrData<T>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		casted_attr_value= ad->get_data();
	
		return 0;
	}//close convert_scalar()	

	static int convert_spectrum(std::vector<T>& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		if(!attr_data) return -1;
		Utils_ns::SpectrumAttrData<T>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::SpectrumAttrData<T>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		casted_attr_value= ad->get_data();

		return 0;
	}//close convert_spectrum()
};//close struct specialization T-->T


template<>
struct AttrDataConverter<Tango::DevEnum,std::string> {
	static int convert_scalar(std::string& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		if(!attr_data) return -1;		
		Utils_ns::ScalarEnumAttrData* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::ScalarEnumAttrData*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		casted_attr_value= ad->get_data_str();
	
		return 0;

	}//close convert_scalar()

	static int convert_spectrum(std::vector<std::string>& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		WARN_LOG("Casting spectrum enum attr data to string not supported!");
		return -1;
	}//close convert_spectrum()

};//close struct specialization for enum-->string


template<typename T>
struct AttrDataConverter<std::string,T> {

	static int convert_scalar(T& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		if(!attr_data) return -1;	
		Utils_ns::ScalarAttrData<std::string>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::ScalarAttrData<std::string>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		std::istringstream istream( ad->get_data() );
  	if (!(istream >> casted_attr_value)){
			WARN_LOG("Failed to convert string data type to templated type!");
			return -1;
		}
	
		return 0;
	}//close convert_scalar()

	static int convert_spectrum(std::vector<T>& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		WARN_LOG("Casting spectrum typed attr data to string not supported!");
		return -1;
	}//close convert_spectrum()
};//close struct partial specialization for string-->numeric type


template<typename T>
struct AttrDataConverter<T,std::string> {

	static int convert_scalar(std::string& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		if(!attr_data) return -1;	
		Utils_ns::ScalarAttrData<T>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::ScalarAttrData<T>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		T attr_value= ad->get_data();	
		std::ostringstream ss;
		ss << attr_value;
		casted_attr_value= ss.str();
		return 0;
	}
		
	static int convert_spectrum(std::vector<std::string>& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		WARN_LOG("Casting spectrum typed attr data to string not supported!");
		return -1;
	}//close convert_spectrum()
};//close struct partial specialization for numeric type-->string


template<>
struct AttrDataConverter<std::string,std::string> {
	static int convert_scalar(std::string& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){
		if(!attr_data) return -1;	
		Utils_ns::ScalarAttrData<std::string>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::ScalarAttrData<std::string>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		casted_attr_value= ad->get_data();
		return 0;
	}

	static int convert_spectrum(std::vector<std::string>& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data){	
		WARN_LOG("Casting spectrum string attr data to string not supported!");
		return -1;
	}//close convert_spectrum()
};

template<typename T,typename K>
inline int GetScalarAttrDataValues(K& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data) { 
	return AttrDataConverter<T,K>::convert_scalar(casted_attr_value,attr_quality,attr_timestamp,attr_data); 
}

template<typename T,typename K>
inline int GetSpectrumAttrDataValues(std::vector<K>& casted_attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp,AttrData* attr_data) { 
	return AttrDataConverter<T,K>::convert_spectrum(casted_attr_value,attr_quality,attr_timestamp,attr_data); 
}
*/

// ============================================================================
// class: AttrFormulaEvaluator
// ============================================================================
template<typename T,typename K>
struct AttrFormulaEvaluator {
	
	static int Evaluate(AttrFormulaData* formula_data,K& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		// Check formula data		
		if(!formula_data) return -1;
		
		// Define typedefs
		typedef exprtk::expression<T> expression_t;
  	typedef exprtk::parser<T> parser_t;
		typedef exprtk::symbol_table<T> symbol_table_t;
	
		// Create and populate symbol table
		symbol_table_t symbol_table;
		AttrFormulaDataValues<T> data_values;
		Tango::TimeVal timeref= TangoUtils::GetTimestampNow();
		if(formula_data->FillSymbolTable<T>(symbol_table,data_values,timeref)<0){
			WARN_LOG("Failed to populate symbol table (check if all vars have been set)!");
			return -1;
		}

		DEBUG_LOG("formula= "<<formula_data->GetFormula()<<" data_values="<<(data_values.scalar_values).size());
		typedef typename std::map<std::string,AttrFormulaVarData<T,T>>::iterator mapIter;
		for(mapIter it=(data_values.scalar_values).begin();it!=(data_values.scalar_values).end();it++){
			std::string var_name= it->first;
			DEBUG_LOG("var="<<var_name<<", attr_value="<<(it->second).attr_value<<", attr_value(in symbol_table)="<<symbol_table.get_variable(var_name.c_str())->value());
		}

		// Add result variable to symbol table
		T res;
		symbol_table.add_variable("res",res);

		// Register symbol tables 
		expression_t expression;
		expression.register_symbol_table(TangoUtils::tango_symbols);//Tango constants
		expression.register_symbol_table(symbol_table);

		// Parse expression	
		parser_t parser;
  	if (!parser.compile(formula_data->GetFormula(),expression)){
  		WARN_LOG("Expression parsing failed (err="<<parser.error()<<")!");
			return -1;
		}

		// Evaluate expression
  	//T formula_val= expression.value();
		expression.value();
		formula_result= static_cast<K>(symbol_table.get_variable("res")->value());
		//DEBUG_LOG("Formula results: formula_val="<<formula_val<<", res="<<res<<" formula_result="<<formula_result);
	
		//## Iterate over data to estimate formula quality factor 
		//## NB: Formula quality is set to INVALID if at least one of the attribute value has invalid quality 
		//## or if last attr value is older than a threshold (given in argument)
		if(formula_data->ComputeFormulaAttrQuality<T>(formula_attr_quality,data_values,attr_timestamp_threshold)<0){
			WARN_LOG("Failed to compute formula attr quality factor!");
			return -1;
		}

		return 0;
	}//close Evaluate()


	static int Evaluate(AttrFormulaData* formula_data,std::vector<K>& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){

		// Clear result
		formula_result.clear();	
	
		// Check formula data		
		if(!formula_data) return -1;
		
		// Define typedefs
		typedef exprtk::expression<T> expression_t;
  	typedef exprtk::parser<T> parser_t;
		typedef exprtk::symbol_table<T> symbol_table_t;
	
		// Create and populate symbol table
		symbol_table_t symbol_table;
		AttrFormulaDataValues<T> data_values;
		Tango::TimeVal timeref= TangoUtils::GetTimestampNow();
		if(formula_data->FillSymbolTable<T>(symbol_table,data_values,timeref)<0){
			WARN_LOG("Failed to populate symbol table (check if all vars have been set)!");
			return -1;
		}

		// Add result variable to symbol table
		std::vector<T> res;
		symbol_table.add_vector("res",res);

		// Register symbol tables 
		expression_t expression;
		expression.register_symbol_table(TangoUtils::tango_symbols);//Tango constants
		expression.register_symbol_table(symbol_table);

		// Parse expression	
		parser_t parser;
  	if (!parser.compile(formula_data->GetFormula(),expression)){
  		WARN_LOG("Expression parsing failed (err="<<parser.error()<<")!");
			return -1;
		}

		// Evaluate expression
  	//T formula_val= expression.value();
		expression.value();
		formula_result.reserve(res.size());
		for(unsigned int i=0;i<res.size();i++){
			formula_result[i]= static_cast<K>(res[i]);
		}
		
		//## Iterate over data to estimate formula quality factor 
		//## NB: Formula quality is set to INVALID if at least one of the attribute value has invalid quality 
		//## or if last attr value is older than a threshold (given in argument)
		if(formula_data->ComputeFormulaAttrQuality<T>(formula_attr_quality,data_values,attr_timestamp_threshold)<0){
			WARN_LOG("Failed to compute formula attr quality factor!");
			return -1;
		}

		return 0;
	}//close Evaluate()

};//close struct AttrFormulaEvaluator()

template<typename T>
struct AttrFormulaEvaluator<std::string,T>{

	static int Evaluate(AttrFormulaData* formula_data,T& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string symbols is not supported!");
		return -1;
	}//close Evaluate()

	static int Evaluate(AttrFormulaData* formula_data,std::vector<T>& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string symbols is not supported!");
		return -1;
	}//close Evaluate()

};//close struct specialization for string type


template<>
struct AttrFormulaEvaluator<std::string,std::string>{

	static int Evaluate(AttrFormulaData* formula_data,std::string& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string symbols is not supported!");
		return -1;
	}//close Evaluate()

	static int Evaluate(AttrFormulaData* formula_data,std::vector<string>& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string symbols is not supported!");
		return -1;
	}//close Evaluate()

};//close struct specialization for string type

template<typename T>
struct AttrFormulaEvaluator<T,std::string>{
	
	static int Evaluate(AttrFormulaData* formula_data,std::string& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		// Check formula data		
		if(!formula_data) return -1;
		
		// Define typedefs
		typedef exprtk::expression<T> expression_t;
  	typedef exprtk::parser<T> parser_t;
		typedef exprtk::symbol_table<T> symbol_table_t;
	
		// Create and populate symbol table
		symbol_table_t symbol_table;
		AttrFormulaDataValues<T> data_values;
		Tango::TimeVal timeref= TangoUtils::GetTimestampNow();
		if(formula_data->FillSymbolTable<T>(symbol_table,data_values,timeref)<0){
			WARN_LOG("Failed to populate symbol table (check if all vars have been set)!");
			return -1;
		}

		// Add result variable to symbol table
		std::string res;
		symbol_table.add_stringvar("res",res);

		// Register symbol tables 
		expression_t expression;
		expression.register_symbol_table(TangoUtils::tango_symbols);//Tango constants
		expression.register_symbol_table(symbol_table);

		// Parse expression	
		parser_t parser;
  	if (!parser.compile(formula_data->GetFormula(),expression)){
  		WARN_LOG("Expression parsing failed (err="<<parser.error()<<")!");
			return -1;
		}

		// Evaluate expression
  	//T formula_val= expression.value();
		expression.value();
		formula_result= symbol_table.stringvar_ref("res");
		//DEBUG_LOG("Formula results: formula_val="<<formula_val<<", res="<<res<<" formula_result="<<formula_result);
	
		//## Iterate over data to estimate formula quality factor 
		//## NB: Formula quality is set to INVALID if at least one of the attribute value has invalid quality 
		//## or if last attr value is older than a threshold (given in argument)
		if(formula_data->ComputeFormulaAttrQuality<T>(formula_attr_quality,data_values,attr_timestamp_threshold)<0){
			WARN_LOG("Failed to compute formula attr quality factor!");
			return -1;
		}

		return 0;
	}//close Evaluate()

	static int Evaluate(AttrFormulaData* formula_data,std::vector<std::string>& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string spectrum attr is not supported!");
		return -1;
	}//close Evaluate()

};//close partial struct specialization for string


template<typename T,typename K>
static int EvaluateScalarAttrFormula(AttrFormulaData* formula_data,K& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold) { 
	return AttrFormulaEvaluator<T,K>::Evaluate(formula_data,formula_result,formula_attr_quality,attr_timestamp_threshold);  
}

template<typename T,typename K>
static int EvaluateSpectrumAttrFormula(AttrFormulaData* formula_data,std::vector<K>& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold) { 
	return AttrFormulaEvaluator<T,K>::Evaluate(formula_data,formula_result,formula_attr_quality,attr_timestamp_threshold); 
}

// ============================================================================
// class: AttrFormulaVar
// ============================================================================
template<typename T>
inline int AttrFormulaVar::CreateScalarAttrData(T initValue){
	//Get attr info
	if(!attr_info) return -1;
	int data_type= attr_info->data_type;

	//Destroy existing attr data
	ClearAttrData();

	//Create a new attr data
	attr_data= new Utils_ns::ScalarAttrData<T>(attr_name,data_type,initValue);
	
	return 0;
}//close CreateScalarAttr()


inline int AttrFormulaVar::CreateScalarEnumAttrData(Tango::DevShort initValue){
	//Get attr info
	if(!attr_info) return -1;
	int data_type= attr_info->data_type;
	std::vector<std::string> enum_labels= attr_info->enum_labels;
	if(enum_labels.empty()) return -1;

	//Destroy existing attr data
	ClearAttrData();

	//Create a new attr data
	//attr_data= new Utils_ns::ScalarEnumAttrData(attr_name,data_type,enum_labels,initValue);
	attr_data= new Utils_ns::ScalarAttrData<Tango::DevEnum>(attr_name,data_type,initValue);
	attr_data->set_labels(enum_labels);

	return 0;
}//close CreateScalarEnumFormulaAttr()


template<typename T>
inline int AttrFormulaVar::CreateSpectrumAttrData(T initValue){	

	//Get attr info
	if(!attr_info) return -1;
	int data_type= attr_info->data_type;
	int data_size= attr_info->max_dim_x;

	//Destroy existing attr data
	ClearAttrData();

	//Create a new attr data
	attr_data = new Utils_ns::SpectrumAttrData<T>(attr_name,data_type,data_size,initValue);
	
	return 0;
}//close CreateSpectrumFormulaAttr()

template<typename T>
inline int AttrFormulaVar::GetScalarAttrDataFromInitValue(T& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){

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
inline int AttrFormulaVar::GetScalarAttrDataFromInitValue<Tango::DevBoolean>(Tango::DevBoolean& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){

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
inline int AttrFormulaVar::GetSpectrumAttrDataFromInitValue(std::vector<T>& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){

	//Do not set timestamp 

	//Set attr quality to INVALID since we are using an init value not a real value read from proxy device
	attr_quality= Tango::ATTR_INVALID;

	//Set attr_value to init value (given in string format)
	std::istringstream istream(attr_init_value_str);
	do {
  	for (T thisVal; istream >> thisVal;) {
    	attr_value.push_back(thisVal);
    }
    if (istream.fail()) {
    	istream.clear();
      std::string token;
      istream >> token;
    }
	}
  while (!istream.eof());

	return 0;

}//close GetSpectrumAttrDataFromInitValue()


template<>
inline int AttrFormulaVar::GetSpectrumAttrDataFromInitValue<Tango::DevBoolean>(std::vector<Tango::DevBoolean>& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){

	//Do not set timestamp 

	//Set attr quality to INVALID since we are using an init value not a real value read from proxy device
	attr_quality= Tango::ATTR_INVALID;

	//Set attr_value to init value (given in string format)
	std::istringstream istream(attr_init_value_str);
	do {
  	for (Tango::DevBoolean thisVal; istream >> std::boolalpha >> thisVal;) {
    	attr_value.push_back(thisVal);
    }
    if (istream.fail()) {
    	istream.clear();
      std::string token;
      istream >> token;
    }
	}
  while (!istream.eof());

	return 0;

}//close GetSpectrumAttrDataFromInitValue() specialization for bool

template<typename T>
inline int AttrFormulaVar::GetScalarAttrData(T& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){
			
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
			GetScalarAttrDataValues<Tango::DevFloat,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_DOUBLE: 
			GetScalarAttrDataValues<Tango::DevDouble,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_LONG: 
			GetScalarAttrDataValues<Tango::DevLong,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_SHORT: 
			GetScalarAttrDataValues<Tango::DevShort,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_ENUM: 
			GetScalarAttrDataValues<Tango::DevEnum,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_STRING: 
			GetScalarAttrDataValues<std::string,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_BOOLEAN: 
			GetScalarAttrDataValues<Tango::DevBoolean,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_STATE: 
			GetScalarAttrDataValues<Tango::DevState,T>(attr_value,attr_quality,attr_timestamp,attr_data);
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
inline int AttrFormulaVar::GetSpectrumAttrData(std::vector<T>& attr_value,Tango::AttrQuality& attr_quality,Tango::TimeVal& attr_timestamp){
			
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
			GetSpectrumAttrDataValues<Tango::DevFloat,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_DOUBLE: 
			GetSpectrumAttrDataValues<Tango::DevDouble,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_LONG: 
			GetSpectrumAttrDataValues<Tango::DevLong,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_SHORT: 
			GetSpectrumAttrDataValues<Tango::DevShort,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_ENUM: 
			GetSpectrumAttrDataValues<Tango::DevEnum,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_STRING: 
			GetSpectrumAttrDataValues<std::string,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_BOOLEAN: 
			GetSpectrumAttrDataValues<Tango::DevBoolean,T>(attr_value,attr_quality,attr_timestamp,attr_data);
			break;
		case Tango::DEV_STATE: 
			GetSpectrumAttrDataValues<Tango::DevState,T>(attr_value,attr_quality,attr_timestamp,attr_data);
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
// class: AttrFormulaData
// ============================================================================
template<typename T,typename K>
inline int AttrFormulaData::EvaluateFormula(K& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){

	return AttrFormulaEvaluator<T,K>::Evaluate(this,formula_result,formula_attr_quality,attr_timestamp_threshold);
	
}//close EvaluateFormula()

template<typename T,typename K>
inline int AttrFormulaData::EvaluateFormula(std::vector<K>& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){

	return AttrFormulaEvaluator<T,K>::Evaluate(this,formula_result,formula_attr_quality,attr_timestamp_threshold);

}//close EvaluateFormula()

template<typename T>
inline int AttrFormulaData::ComputeFormulaAttrQuality(Tango::AttrQuality& formula_attr_quality,AttrFormulaDataValues<T>& data_values,long attr_timestamp_threshold){

	//Init formula quality
	formula_attr_quality= Tango::ATTR_VALID;

	//Iterate over numeric attrs
	typedef typename std::map<std::string,AttrFormulaVarData<T,T>>::iterator mapIter;
	for(mapIter it=(data_values.scalar_values).begin();it!=(data_values.scalar_values).end();it++){
		std::string var_name= it->first;
		AttrFormulaVarPtr formula_var= m_symbols[var_name];
		T attr_quality= (it->second).attr_quality;
		long attr_timediff= (it->second).attr_timediff;
		int attr_value_type= formula_var->attr_value_type;
	
		//Check timestamp	
		float dt= fabs(attr_timediff);
		if(dt>attr_timestamp_threshold){
			DEBUG_LOG("Attr var "<<var_name<<" in formula was evaluated long time ago wrt to threshold (dt="<<dt<<">"<<attr_timestamp_threshold<<"), set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}

		//Check if quality factor is INVALID (only if value is used in formula and NOT the quality itself)
		if(attr_value_type==AttrFormulaVar::eATTR_VALUE && attr_quality==Tango::ATTR_INVALID){
			DEBUG_LOG("Attr var "<<var_name<<" has INVALID quality factor, set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}//close if

	}//end loop numeric attrs

	//Iterate over numeric spectrum attrs
	typedef std::vector<T> VecT;
	typedef typename std::map<std::string,AttrFormulaVarData<T,VecT>>::iterator mapSpectrumIter;
	for(mapSpectrumIter it=(data_values.spectrum_values).begin();it!=(data_values.spectrum_values).end();it++){
		std::string var_name= it->first;
		AttrFormulaVarPtr formula_var= m_symbols[var_name];
		T attr_quality= (it->second).attr_quality;
		long attr_timediff= (it->second).attr_timediff;
		int attr_value_type= formula_var->attr_value_type;
	
		//Check timestamp
		float dt= fabs(attr_timediff);
		if(dt>attr_timestamp_threshold){
			DEBUG_LOG("Attr var "<<var_name<<" in formula was evaluated long time ago wrt to threshold (dt="<<dt<<">"<<attr_timestamp_threshold<<"), set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}

		//Check if quality factor is INVALID (only if value is used in formula and NOT the quality itself)
		if(attr_value_type==AttrFormulaVar::eATTR_VALUE && attr_quality==Tango::ATTR_INVALID){
			DEBUG_LOG("Attr var "<<var_name<<" has INVALID quality factor, set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}//close if

	}//end loop spectrum attrs

	//Iterate over string attrs
	typedef typename std::map<std::string,AttrFormulaVarData<T,std::string>>::iterator mapStringIter;
	for(mapStringIter it=(data_values.scalar_string_values).begin();it!=(data_values.scalar_string_values).end();it++){
		std::string var_name= it->first;
		AttrFormulaVarPtr formula_var= m_symbols[var_name];
		T attr_quality= (it->second).attr_quality;
		long attr_timediff= (it->second).attr_timediff;
		int attr_value_type= formula_var->attr_value_type;

		//Check timestamp
		float dt= fabs(attr_timediff);
		if(dt>attr_timestamp_threshold){
			DEBUG_LOG("Attr var "<<var_name<<" in formula was evaluated long time ago wrt to threshold (dt="<<dt<<">"<<attr_timestamp_threshold<<"), set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}

		//Check if quality factor is INVALID (only if value is used in formula and NOT the quality itself)
		if(attr_value_type==AttrFormulaVar::eATTR_VALUE && attr_quality==Tango::ATTR_INVALID){
			DEBUG_LOG("Attr var "<<var_name<<" has INVALID quality factor, set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}//close if

	}//end loop string attrs

	return 0;

}//close ComputeFormulaAttrQuality()


template<typename T>
inline int AttrFormulaData::FillSymbolTable(exprtk::symbol_table<T>& symbol_table_t,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref){

	//## Iterate over formula vars
	typedef std::map<std::string,AttrFormulaVarPtr>::iterator mapIter;

	DEBUG_LOG("#"<<m_symbols.size()<<" vars present in formula...");

	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		//Get var info
		std::string var_name= it->first;	
		AttrFormulaVarPtr formula_var= it->second;
		std::string proxy_name= formula_var->device_name;
		std::string proxy_attr_name= formula_var->attr_name;
		Tango::AttributeInfoEx* attr_info= formula_var->attr_info;
		AttrData* attr_data= formula_var->attr_data;
	
		//Check if attr value was set (at least once), otherwise will use init value
		bool hasAttrData= (attr_info && attr_data && attr_data->is_value_set());
		int data_format= -1;
		int data_type= -1;
		if(hasAttrData){
			data_format= attr_data->get_data_format();
			data_type= attr_data->get_data_type();
		}
		else{
			data_format= formula_var->exp_data_format;
			data_type= formula_var->exp_data_type;
			WARN_LOG("Attr "<<proxy_attr_name<<" has no info/data or value never set (proxy offline?), will evaluate formula with init data (type="<<data_type<<", format="<<data_format<<")");
			//return -1;
		}
		
		//Get formula attr values
		if(data_format==Tango::SCALAR){
			if(data_type==Tango::DEV_STRING || data_type==Tango::DEV_ENUM) {//string attr
				DEBUG_LOG("Adding scalar string var "<<var_name<<" (attr_name="<<proxy_attr_name<<", hasAttrData?"<<hasAttrData<<", data_type="<<data_type<<") to symbol table...");			
				if(AddStringVarToSymbolTable<T>(symbol_table_t,var_name,formula_var,data_values,timeref,!hasAttrData)<0){
					WARN_LOG("Failed to register numeric symbol for attr "<<proxy_attr_name<<" in table, cannot evaluate formula!");
					return -1;
				}
			}
			else if(data_type==Tango::DEV_STATE) {//State
				DEBUG_LOG("Adding scalar State var "<<var_name<<" (attr_name="<<proxy_attr_name<<", hasAttrData?"<<hasAttrData<<", data_type="<<data_type<<") to symbol table...");			
				if(AddStateVarToSymbolTable<T>(symbol_table_t,var_name,formula_var,data_values,timeref,!hasAttrData)<0){
					WARN_LOG("Failed to register numeric symbol for attr "<<proxy_attr_name<<" in table, cannot evaluate formula!");
					return -1;
				}
			}
			else {//numeric attr
				DEBUG_LOG("Adding numeric scalar var "<<var_name<<" (attr_name="<<proxy_attr_name<<", hasAttrData?"<<hasAttrData<<", data_type="<<data_type<<") to symbol table...");		
				if(AddNumericVarToSymbolTable<T>(symbol_table_t,var_name,formula_var,data_values,timeref,!hasAttrData)<0){
					WARN_LOG("Failed to register numeric symbol for attr "<<proxy_attr_name<<" in table, cannot evaluate formula!");
					return -1;
				}
			}//close if numeric attr
		}//close if SCALAR

		else if(data_format==Tango::SPECTRUM){
			DEBUG_LOG("Adding spectrum var "<<var_name<<" (attr_name="<<proxy_attr_name<<", hasAttrData?"<<hasAttrData<<") to symbol table...");
		
			if(data_type==Tango::DEV_STRING || data_type==Tango::DEV_ENUM) {//string attr
				WARN_LOG("Spectrum string and enum attr requested for "<<proxy_attr_name<<"are not supported in formula, cannot evaluate formula!");
				return -1;
			}
			else if(data_type==Tango::DEV_STATE) {//State
				if(AddStateSpectrumVarToSymbolTable<T>(symbol_table_t,var_name,formula_var,data_values,timeref,!hasAttrData)<0){
					WARN_LOG("Failed to register numeric symbol for attr "<<proxy_attr_name<<" in table, cannot evaluate formula!");
					return -1;
				}
			}
			else {//numeric attr
				if(AddNumericSpectrumVarToSymbolTable<T>(symbol_table_t,var_name,formula_var,data_values,timeref,!hasAttrData)<0){
					WARN_LOG("Failed to register numeric symbol for attr "<<proxy_attr_name<<" in table, cannot evaluate formula!");
					return -1;
				}
			}
		}//close SPECTRUM

	}//end loop vars

	return 0;

}//close FillSymbolTable()

template<typename T>
inline int AttrFormulaData::AddNumericVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;

	//Get current data value, quality & timestamp
	T attr_value;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(useInitValue){
		if(formula_var->GetScalarAttrDataFromInitValue<T>(attr_value,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast init value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}
	else {
		if(formula_var->GetScalarAttrData<T>(attr_value,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}

	//Check if attr timestamp is too old wrt to now
	//NB: In this case formula is allowed to be evaluated but formula attr quality factor should be switched to INVALID
	long dt= TangoUtils::GetTimeDiffSec(attr_timestamp,timeref);
	
	//Make data to be inserted in symbol data
	AttrFormulaVarData<T,T> attr_var_data;
	attr_var_data.attr_value= attr_value;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	
	DEBUG_LOG("Adding numeric var "<<var_name<<" value (attr_value="<<attr_value<<", attr_quality="<<attr_quality<<" attr_timestamp("<<attr_timestamp.tv_sec<<","<<attr_timestamp.tv_usec<<")"<<", dt="<<dt<<") to symbol table...");

	(data_values.scalar_values).insert( std::make_pair(var_name,attr_var_data) );

	//Check what value has to be considered in formula: attr value vs quality factor
	if(formula_var->attr_value_type==AttrFormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.scalar_values)[var_name].attr_quality);
	}
	else{
		DEBUG_LOG("Adding numeric var "<<var_name<<" value (attr_value="<<attr_value<<") to symbol table...");
		symbol_table.add_variable(var_name.c_str(),(data_values.scalar_values)[var_name].attr_value);
	}
	
	DEBUG_LOG("data_values.scalar_values.size()="<<data_values.scalar_values.size());


	return 0;

}//close AddNumericVarToSymbolTable()


template<typename T>
inline int AttrFormulaData::AddStateVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;

	//Get current data value, quality & timestamp
	T attr_value;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(useInitValue){//NB: init value is given as string, so need conversion wrt scalar attr case
		std::string init_value_str= "";
		if(formula_var->GetScalarAttrDataFromInitValue<std::string>(init_value_str,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast init value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
		
		//Convert string to templated type
		if(Utils_ns::TangoUtils::TangoStateStrToValue(attr_value,init_value_str)<0){
			WARN_LOG("Failed to cast stringified DevState ("<<init_value_str<<" to desired type for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}		
	}
	else {//NB: Here the default conversion should be fine because AttrData are created as DevState
		if(formula_var->GetScalarAttrData<T>(attr_value,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}

	//Check if attr timestamp is too old wrt to now
	//NB: In this case formula is allowed to be evaluated but formula attr quality factor should be switched to INVALID
	long dt= TangoUtils::GetTimeDiffSec(attr_timestamp,timeref);
	
	//Make data to be inserted in symbol data
	AttrFormulaVarData<T,T> attr_var_data;
	attr_var_data.attr_value= attr_value;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	
	DEBUG_LOG("Adding State var "<<var_name<<" value (attr_value="<<attr_value<<", attr_quality="<<attr_quality<<" attr_timestamp("<<attr_timestamp.tv_sec<<","<<attr_timestamp.tv_usec<<")"<<", dt="<<dt<<") to symbol table...");

	(data_values.scalar_values).insert( std::make_pair(var_name,attr_var_data) );

	//Check what value has to be considered in formula: attr value vs quality factor
	if(formula_var->attr_value_type==AttrFormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.scalar_values)[var_name].attr_quality);
	}
	else{
		DEBUG_LOG("Adding State var "<<var_name<<" value (attr_value="<<attr_value<<") to symbol table...");
		symbol_table.add_variable(var_name.c_str(),(data_values.scalar_values)[var_name].attr_value);
	}
	
	return 0;

}//close AddStateVarToSymbolTable()


template<typename T>
inline int AttrFormulaData::AddNumericSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;

	//Get current data value, quality & timestamp
	std::vector<T> attr_values;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(useInitValue){
		if(formula_var->GetSpectrumAttrDataFromInitValue<T>(attr_values,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast init value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}
	else{
		if(formula_var->GetSpectrumAttrData<T>(attr_values,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}

	//Check if attr timestamp is too old wrt to now
	//NB: In this case formula is allowed to be evaluated but formula attr quality factor should be switched to INVALID
	long dt= TangoUtils::GetTimeDiffSec(timeref,attr_timestamp);
	
	//Check what value has to be considered in formula: attr value vs quality factor
	AttrFormulaVarData<T,std::vector<T>> attr_var_data;
	attr_var_data.attr_value= attr_values;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	(data_values.spectrum_values)[var_name]= attr_var_data;

	if(formula_var->attr_value_type==AttrFormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.spectrum_values)[var_name].attr_quality);
	}
	else{
		symbol_table.add_vector(var_name.c_str(),(data_values.spectrum_values)[var_name].attr_value);
	}
	
	return 0;

}//close AddNumericSpectrumVarToSymbolTable()


template<typename T>
inline int AttrFormulaData::AddStateSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;

	//Get current data value, quality & timestamp
	std::vector<T> attr_values;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(useInitValue){//NB: init value is given as string, so need conversion wrt scalar attr case
		std::vector<std::string> init_values_str;
		if(formula_var->GetSpectrumAttrDataFromInitValue<std::string>(init_values_str,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast init value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}

		//Convert string to templated type
		if(Utils_ns::TangoUtils::TangoStateStrToValue(attr_values,init_values_str)<0){
			WARN_LOG("Failed to cast stringified DevState list to desired type for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}	
	}
	else{
		if(formula_var->GetSpectrumAttrData<T>(attr_values,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}

	//Check if attr timestamp is too old wrt to now
	//NB: In this case formula is allowed to be evaluated but formula attr quality factor should be switched to INVALID
	long dt= TangoUtils::GetTimeDiffSec(timeref,attr_timestamp);
	
	//Check what value has to be considered in formula: attr value vs quality factor
	AttrFormulaVarData<T,std::vector<T>> attr_var_data;
	attr_var_data.attr_value= attr_values;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	(data_values.spectrum_values)[var_name]= attr_var_data;

	if(formula_var->attr_value_type==AttrFormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.spectrum_values)[var_name].attr_quality);
	}
	else{
		symbol_table.add_vector(var_name.c_str(),(data_values.spectrum_values)[var_name].attr_value);
	}
	
	return 0;

}//close AddStateSpectrumVarToSymbolTable()


template<typename T>
inline int AttrFormulaData::AddStringVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,AttrFormulaVarPtr formula_var,AttrFormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;

	//Get current data value, quality & timestamp
	std::string attr_value;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(useInitValue){
		if(formula_var->GetScalarAttrDataFromInitValue<std::string>(attr_value,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast init value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}
	else{
		if(formula_var->GetScalarAttrData<std::string>(attr_value,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}

	//Check if attr timestamp is too old wrt to now
	//NB: In this case formula is allowed to be evaluated but formula attr quality factor should be switched to INVALID
	long dt= TangoUtils::GetTimeDiffSec(timeref,attr_timestamp);
	
	//Check what value has to be considered in formula: attr value vs quality factor
	AttrFormulaVarData<T,std::string> attr_var_data;
	attr_var_data.attr_value= attr_value;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	(data_values.scalar_string_values)[var_name]= attr_var_data;

	if(formula_var->attr_value_type==AttrFormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.scalar_string_values)[var_name].attr_quality);
	}
	else{
		symbol_table.add_stringvar(var_name.c_str(),(data_values.scalar_string_values)[var_name].attr_value);
	}
	
	return 0;

}//close AddStringVarToSymbolTable()


