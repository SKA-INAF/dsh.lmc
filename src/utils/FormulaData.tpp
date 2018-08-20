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
* @file FormulaData.tpp
* @class FormulaData
* @brief FormulaData container 
*
* FormulaData container 
* @author S. Riggi
* @date 15/01/2016
*/


// ============================================================================
// class: FormulaEvaluator
// ============================================================================
template<typename T,typename K>
struct FormulaEvaluator {
	
	static int Evaluate(FormulaData* formula_data,K& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		// Check formula data		
		if(!formula_data) return -1;
		
		// Define typedefs
		typedef exprtk::expression<T> expression_t;
  	typedef exprtk::parser<T> parser_t;
		typedef exprtk::symbol_table<T> symbol_table_t;
	
		// Create and populate symbol table
		symbol_table_t symbol_table;
		FormulaDataValues<T> data_values;
		Tango::TimeVal timeref= TangoUtils::GetTimestampNow();
		if(formula_data->FillSymbolTable<T>(symbol_table,data_values,timeref)<0){
			WARN_LOG("Failed to populate symbol table (check if all vars have been set)!");
			return -1;
		}

		//DEBUG_LOG("formula= "<<formula_data->GetFormula()<<" data_values="<<(data_values.scalar_values).size());
		typedef typename std::map<std::string,FormulaVarData<T,T>>::iterator mapIter;
		for(mapIter it=(data_values.scalar_values).begin();it!=(data_values.scalar_values).end();it++){
			std::string var_name= it->first;
			//DEBUG_LOG("var="<<var_name<<", attr_value="<<(it->second).attr_value<<", attr_value(in symbol_table)="<<symbol_table.get_variable(var_name.c_str())->value());
		}

		// Add result variable to symbol table
		T res;
		symbol_table.add_variable("res",res);

		//Add Tango States to symbol table
		symbol_table.add_constant("ON", (unsigned int)Tango::ON);
		symbol_table.add_constant("OFF", (unsigned int)Tango::OFF);
		symbol_table.add_constant("CLOSE", (unsigned int)Tango::CLOSE);
		symbol_table.add_constant("OPEN", (unsigned int)Tango::OPEN);
		symbol_table.add_constant("INSERT", (unsigned int)Tango::INSERT);
		symbol_table.add_constant("EXTRACT", (unsigned int)Tango::EXTRACT);
		symbol_table.add_constant("MOVING", (unsigned int)Tango::MOVING);
		symbol_table.add_constant("STANDBY", (unsigned int)Tango::STANDBY);
		symbol_table.add_constant("FAULT", (unsigned int)Tango::FAULT);
		symbol_table.add_constant("INIT", (unsigned int)Tango::INIT);
		symbol_table.add_constant("RUNNING", (unsigned int)Tango::RUNNING);
		symbol_table.add_constant("ALARM", (unsigned int)Tango::ALARM);
		symbol_table.add_constant("DISABLE", (unsigned int)Tango::DISABLE);
		symbol_table.add_constant("UNKNOWN", (unsigned int)Tango::UNKNOWN);

		//Add Quality Factors to symbol table
		symbol_table.add_constant("ATTR_VALID", (unsigned int)Tango::ATTR_VALID);
		symbol_table.add_constant("ATTR_INVALID", (unsigned int)Tango::ATTR_INVALID);
		symbol_table.add_constant("ATTR_ALARM", (unsigned int)Tango::ATTR_ALARM);
		symbol_table.add_constant("ATTR_CHANGING", (unsigned int)Tango::ATTR_CHANGING);
		symbol_table.add_constant("ATTR_WARNING", (unsigned int)Tango::ATTR_WARNING);

		// Register symbol tables 
		expression_t expression;
		//expression.register_symbol_table(TangoUtils::tango_symbols);//Tango constants (not working, is it overridden by the next statement?)
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


	static int Evaluate(FormulaData* formula_data,std::vector<K>& formula_result,Tango::AttrQuality& formula_attr_quality,long data_size,long attr_timestamp_threshold){

		// Clear result
		formula_result.clear();	
	
		// Check formula data		
		if(!formula_data) return -1;

		//Check data size
		if(data_size<=0) {
			WARN_LOG("Invalid data size (size="<<data_size<<") specified (it should be >=1)!");
			return -1;
		}
		
		// Define typedefs
		typedef exprtk::expression<T> expression_t;
  	typedef exprtk::parser<T> parser_t;
		typedef exprtk::symbol_table<T> symbol_table_t;
	
		// Create and populate symbol table
		symbol_table_t symbol_table;
		FormulaDataValues<T> data_values;
		Tango::TimeVal timeref= TangoUtils::GetTimestampNow();
		if(formula_data->FillSymbolTable<T>(symbol_table,data_values,timeref)<0){
			WARN_LOG("Failed to populate symbol table (check if all vars have been set)!");
			return -1;
		}

		// Add result variable to symbol table
		std::vector<T> res;
		symbol_table.add_vector("res",res);

		//Add Tango States to symbol table
		symbol_table.add_constant("ON", (unsigned int)Tango::ON);
		symbol_table.add_constant("OFF", (unsigned int)Tango::OFF);
		symbol_table.add_constant("CLOSE", (unsigned int)Tango::CLOSE);
		symbol_table.add_constant("OPEN", (unsigned int)Tango::OPEN);
		symbol_table.add_constant("INSERT", (unsigned int)Tango::INSERT);
		symbol_table.add_constant("EXTRACT", (unsigned int)Tango::EXTRACT);
		symbol_table.add_constant("MOVING", (unsigned int)Tango::MOVING);
		symbol_table.add_constant("STANDBY", (unsigned int)Tango::STANDBY);
		symbol_table.add_constant("FAULT", (unsigned int)Tango::FAULT);
		symbol_table.add_constant("INIT", (unsigned int)Tango::INIT);
		symbol_table.add_constant("RUNNING", (unsigned int)Tango::RUNNING);
		symbol_table.add_constant("ALARM", (unsigned int)Tango::ALARM);
		symbol_table.add_constant("DISABLE", (unsigned int)Tango::DISABLE);
		symbol_table.add_constant("UNKNOWN", (unsigned int)Tango::UNKNOWN);

		//Add Quality Factors to symbol table
		symbol_table.add_constant("ATTR_VALID", (unsigned int)Tango::ATTR_VALID);
		symbol_table.add_constant("ATTR_INVALID", (unsigned int)Tango::ATTR_INVALID);
		symbol_table.add_constant("ATTR_ALARM", (unsigned int)Tango::ATTR_ALARM);
		symbol_table.add_constant("ATTR_CHANGING", (unsigned int)Tango::ATTR_CHANGING);
		symbol_table.add_constant("ATTR_WARNING", (unsigned int)Tango::ATTR_WARNING);

		// Register symbol tables 
		expression_t expression;
		//expression.register_symbol_table(TangoUtils::tango_symbols);//Tango constants
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

};//close struct FormulaEvaluator()

template<typename T>
struct FormulaEvaluator<std::string,T>{

	static int Evaluate(FormulaData* formula_data,T& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string symbols is not supported!");
		return -1;
	}//close Evaluate()

	static int Evaluate(FormulaData* formula_data,std::vector<T>& formula_result,Tango::AttrQuality& formula_attr_quality,long data_size,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string symbols is not supported!");
		return -1;
	}//close Evaluate()

};//close struct specialization for string type


template<>
struct FormulaEvaluator<std::string,std::string>{

	static int Evaluate(FormulaData* formula_data,std::string& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string symbols is not supported!");
		return -1;
	}//close Evaluate()

	static int Evaluate(FormulaData* formula_data,std::vector<string>& formula_result,Tango::AttrQuality& formula_attr_quality,long data_size,long attr_timestamp_threshold){
		WARN_LOG("Formula evaluation for string symbols is not supported!");
		return -1;
	}//close Evaluate()

};//close struct specialization for string type

template<typename T>
struct FormulaEvaluator<T,std::string>{
	
	static int Evaluate(FormulaData* formula_data,std::string& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){
		// Check formula data		
		if(!formula_data) return -1;
		
		// Define typedefs
		typedef exprtk::expression<T> expression_t;
  	typedef exprtk::parser<T> parser_t;
		typedef exprtk::symbol_table<T> symbol_table_t;
	
		// Create and populate symbol table
		symbol_table_t symbol_table;
		FormulaDataValues<T> data_values;
		Tango::TimeVal timeref= TangoUtils::GetTimestampNow();
		if(formula_data->FillSymbolTable<T>(symbol_table,data_values,timeref)<0){
			WARN_LOG("Failed to populate symbol table (check if all vars have been set)!");
			return -1;
		}

		// Add result variable to symbol table
		std::string res;
		symbol_table.add_stringvar("res",res);

		//Add Tango States to symbol table
		symbol_table.add_constant("ON", (unsigned int)Tango::ON);
		symbol_table.add_constant("OFF", (unsigned int)Tango::OFF);
		symbol_table.add_constant("CLOSE", (unsigned int)Tango::CLOSE);
		symbol_table.add_constant("OPEN", (unsigned int)Tango::OPEN);
		symbol_table.add_constant("INSERT", (unsigned int)Tango::INSERT);
		symbol_table.add_constant("EXTRACT", (unsigned int)Tango::EXTRACT);
		symbol_table.add_constant("MOVING", (unsigned int)Tango::MOVING);
		symbol_table.add_constant("STANDBY", (unsigned int)Tango::STANDBY);
		symbol_table.add_constant("FAULT", (unsigned int)Tango::FAULT);
		symbol_table.add_constant("INIT", (unsigned int)Tango::INIT);
		symbol_table.add_constant("RUNNING", (unsigned int)Tango::RUNNING);
		symbol_table.add_constant("ALARM", (unsigned int)Tango::ALARM);
		symbol_table.add_constant("DISABLE", (unsigned int)Tango::DISABLE);
		symbol_table.add_constant("UNKNOWN", (unsigned int)Tango::UNKNOWN);

		//Add Quality Factors to symbol table
		symbol_table.add_constant("ATTR_VALID", (unsigned int)Tango::ATTR_VALID);
		symbol_table.add_constant("ATTR_INVALID", (unsigned int)Tango::ATTR_INVALID);
		symbol_table.add_constant("ATTR_ALARM", (unsigned int)Tango::ATTR_ALARM);
		symbol_table.add_constant("ATTR_CHANGING", (unsigned int)Tango::ATTR_CHANGING);
		symbol_table.add_constant("ATTR_WARNING", (unsigned int)Tango::ATTR_WARNING);

		// Register symbol tables 
		expression_t expression;
		//expression.register_symbol_table(TangoUtils::tango_symbols);//Tango constants
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

	static int Evaluate(FormulaData* formula_data,std::vector<std::string>& formula_result,Tango::AttrQuality& formula_attr_quality,long data_size,long attr_timestamp_threshold){
		//WARN_LOG("Formula evaluation for string spectrum attr is not supported!");
		//return -1;

		// Clear result
		formula_result.clear();	
	
		// Check formula data		
		if(!formula_data) return -1;

		//Check data size
		if(data_size<=0) {
			WARN_LOG("Invalid data size (size="<<data_size<<") specified (it should be >=1)!");
			return -1;
		}
		
		// Define typedefs
		typedef exprtk::expression<T> expression_t;
  	typedef exprtk::parser<T> parser_t;
		typedef exprtk::symbol_table<T> symbol_table_t;
	
		// Create and populate symbol table
		symbol_table_t symbol_table;
		FormulaDataValues<T> data_values;
		Tango::TimeVal timeref= TangoUtils::GetTimestampNow();
		if(formula_data->FillSymbolTable<T>(symbol_table,data_values,timeref)<0){
			WARN_LOG("Failed to populate symbol table (check if all vars have been set)!");
			return -1;
		}

		// Add result variable to symbol table
		std::vector<std::string> res;
		res.assign(data_size,"");
		std::vector<std::string> res_var_names;
		res_var_names.assign(data_size,"");

		for(long i=0;i<data_size;i++){
			std::stringstream ss;
			ss<<"res_"<<i;
			std::string var_name= ss.str();
			res_var_names[i]= var_name;
			//DEBUG_LOG("Adding formula res "<<var_name<<"...");
			symbol_table.add_stringvar(var_name.c_str(),res[i]);
		}

		//Add Tango States to symbol table
		symbol_table.add_constant("ON", (unsigned int)Tango::ON);
		symbol_table.add_constant("OFF", (unsigned int)Tango::OFF);
		symbol_table.add_constant("CLOSE", (unsigned int)Tango::CLOSE);
		symbol_table.add_constant("OPEN", (unsigned int)Tango::OPEN);
		symbol_table.add_constant("INSERT", (unsigned int)Tango::INSERT);
		symbol_table.add_constant("EXTRACT", (unsigned int)Tango::EXTRACT);
		symbol_table.add_constant("MOVING", (unsigned int)Tango::MOVING);
		symbol_table.add_constant("STANDBY", (unsigned int)Tango::STANDBY);
		symbol_table.add_constant("FAULT", (unsigned int)Tango::FAULT);
		symbol_table.add_constant("INIT", (unsigned int)Tango::INIT);
		symbol_table.add_constant("RUNNING", (unsigned int)Tango::RUNNING);
		symbol_table.add_constant("ALARM", (unsigned int)Tango::ALARM);
		symbol_table.add_constant("DISABLE", (unsigned int)Tango::DISABLE);
		symbol_table.add_constant("UNKNOWN", (unsigned int)Tango::UNKNOWN);

		//Add Quality Factors to symbol table
		symbol_table.add_constant("ATTR_VALID", (unsigned int)Tango::ATTR_VALID);
		symbol_table.add_constant("ATTR_INVALID", (unsigned int)Tango::ATTR_INVALID);
		symbol_table.add_constant("ATTR_ALARM", (unsigned int)Tango::ATTR_ALARM);
		symbol_table.add_constant("ATTR_CHANGING", (unsigned int)Tango::ATTR_CHANGING);
		symbol_table.add_constant("ATTR_WARNING", (unsigned int)Tango::ATTR_WARNING);

		// Register symbol tables 
		expression_t expression;
		//expression.register_symbol_table(TangoUtils::tango_symbols);//Tango constants
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
		formula_result.assign(res.size(),"");
		DEBUG_LOG("Retrieving formula result values (data_size="<<data_size<<", size="<<res.size()<<")...");
		for(unsigned int i=0;i<res.size();i++){
			std::string var_value= symbol_table.stringvar_ref(res_var_names[i].c_str());
			DEBUG_LOG("Formula var "<<res_var_names[i]<<" value "<<var_value);
			formula_result[i]= var_value;
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

};//close partial struct specialization for string


template<typename T,typename K>
static int EvaluateScalarFormula(FormulaData* formula_data,K& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold) { 
	return FormulaEvaluator<T,K>::Evaluate(formula_data,formula_result,formula_attr_quality,attr_timestamp_threshold);  
}

template<typename T,typename K>
static int EvaluateSpectrumFormula(FormulaData* formula_data,std::vector<K>& formula_result,Tango::AttrQuality& formula_attr_quality,long data_size,long attr_timestamp_threshold) { 
	return FormulaEvaluator<T,K>::Evaluate(formula_data,formula_result,formula_attr_quality,data_size,attr_timestamp_threshold); 
}

// ============================================================================
// class: FormulaData
// ============================================================================
template<typename T,typename K>
inline int FormulaData::EvaluateFormula(K& formula_result,Tango::AttrQuality& formula_attr_quality,long attr_timestamp_threshold){

	return FormulaEvaluator<T,K>::Evaluate(this,formula_result,formula_attr_quality,attr_timestamp_threshold);
	
}//close EvaluateFormula()

template<typename T,typename K>
inline int FormulaData::EvaluateFormula(std::vector<K>& formula_result,Tango::AttrQuality& formula_attr_quality,long data_size,long attr_timestamp_threshold){

	return FormulaEvaluator<T,K>::Evaluate(this,formula_result,formula_attr_quality,data_size,attr_timestamp_threshold);

}//close EvaluateFormula()

template<typename T>
inline int FormulaData::ComputeFormulaAttrQuality(Tango::AttrQuality& formula_attr_quality,FormulaDataValues<T>& data_values,long attr_timestamp_threshold){

	//Init formula quality
	formula_attr_quality= Tango::ATTR_VALID;

	//Iterate over numeric attrs
	typedef typename std::map<std::string,FormulaVarData<T,T>>::iterator mapIter;
	for(mapIter it=(data_values.scalar_values).begin();it!=(data_values.scalar_values).end();it++){
		std::string var_name= it->first;
		std::string formula_var_name= (it->second).formula_var_name;
		std::string formula_attr_name= (it->second).formula_attr_name;

		T attr_quality= (it->second).attr_quality;
		float attr_timediff= static_cast<float>( (it->second).attr_timediff );

		DEBUG_LOG("Checking quality factor of var "<<var_name<<" (attr_name="<<m_attrName<<", formula_var_name= "<<formula_var_name<<", var_attr_name="<<formula_attr_name<<"), attr_timediff="<<attr_timediff<<"...");

		//FormulaVarPtr formula_var= m_symbols[var_name];
		FormulaVarPtr formula_var= m_symbols[formula_var_name];
		if(!formula_var){
			WARN_LOG("Failed to get access to formula var "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<"), this should not occur!!!");
			return -1;
		}
		int attr_value_type= formula_var->attr_value_type;

		//If formula attribute is the device attribute skip the check
		if(formula_attr_name==m_attrName){
			DEBUG_LOG("Skip quality check for var "<<var_name<<" (formula_var_name= "<<formula_var_name<<", var_attr_name="<<formula_attr_name<<") as equal to the formula device attribute  ("<<m_attrName<<")");
			continue; 
		}
	
		//Check timestamp
		float dt= std::fabs(attr_timediff);
		if(m_setInvalidQualityIfOld && dt>static_cast<float>(attr_timestamp_threshold)){
			DEBUG_LOG("Attr var "<<var_name<<" in formula "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<") was evaluated long time ago wrt to threshold (dt="<<dt<<">"<<attr_timestamp_threshold<<"), set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}

		//Check if quality factor is INVALID (only if value is used in formula and NOT the quality itself)
		if(attr_value_type==FormulaVar::eATTR_VALUE && attr_quality==Tango::ATTR_INVALID){
			DEBUG_LOG("Attr var "<<var_name<<" in formula "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<") has INVALID quality factor, set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}//close if

	}//end loop numeric attrs

	//Iterate over numeric spectrum attrs
	typedef std::vector<T> VecT;
	typedef typename std::map<std::string,FormulaVarData<T,VecT>>::iterator mapSpectrumIter;
	for(mapSpectrumIter it=(data_values.spectrum_values).begin();it!=(data_values.spectrum_values).end();it++){
		std::string var_name= it->first;
		std::string formula_var_name= (it->second).formula_var_name;
		std::string formula_attr_name= (it->second).formula_attr_name;
		T attr_quality= (it->second).attr_quality;
		float attr_timediff= static_cast<float>( (it->second).attr_timediff );

		DEBUG_LOG("Checking quality factor of var "<<var_name<<" (attr_name="<<m_attrName<<", formula_var_name= "<<formula_var_name<<", var_attr_name="<<formula_attr_name<<"), attr_timediff="<<attr_timediff<<"...");

		//FormulaVarPtr formula_var= m_symbols[var_name];
		FormulaVarPtr formula_var= m_symbols[formula_var_name];
		if(!formula_var){
			WARN_LOG("Failed to get access to formula var "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<"), this should not occur!!!");
			return -1;
		}
		int attr_value_type= formula_var->attr_value_type;

		//If formula attribute is the device attribute skip the check
		if(formula_attr_name==m_attrName){
			DEBUG_LOG("Skip quality check for var "<<var_name<<" (formula_var_name= "<<formula_var_name<<", var_attr_name="<<formula_attr_name<<") as equal to the formula device attribute  ("<<m_attrName<<")");
			continue; 
		}
	
		//Check timestamp
		float dt= std::fabs(attr_timediff);
		if(m_setInvalidQualityIfOld && dt>static_cast<float>(attr_timestamp_threshold)){
			DEBUG_LOG("Attr var "<<var_name<<" in formula "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<") was evaluated long time ago wrt to threshold (dt="<<dt<<">"<<attr_timestamp_threshold<<"), set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}

		//Check if quality factor is INVALID (only if value is used in formula and NOT the quality itself)
		if(attr_value_type==FormulaVar::eATTR_VALUE && attr_quality==Tango::ATTR_INVALID){
			DEBUG_LOG("Attr var "<<var_name<<" in formula "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<") has INVALID quality factor, set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}//close if

	}//end loop spectrum attrs

	//Iterate over string attrs
	typedef typename std::map<std::string,FormulaVarData<T,std::string>>::iterator mapStringIter;
	for(mapStringIter it=(data_values.scalar_string_values).begin();it!=(data_values.scalar_string_values).end();it++){
		std::string var_name= "";
		std::string formula_var_name= "";	
		std::string formula_attr_name= "";
		T attr_quality= T();
		float attr_timediff= 0;

		var_name= it->first;
		formula_var_name= (it->second).formula_var_name;	
		formula_attr_name= (it->second).formula_attr_name;
		attr_quality= (it->second).attr_quality;
		attr_timediff= static_cast<float>( (it->second).attr_timediff );

		DEBUG_LOG("Checking quality factor of var "<<var_name<<" (attr_name="<<m_attrName<<", formula_var_name= "<<formula_var_name<<", var_attr_name="<<formula_attr_name<<"), attr_timediff="<<attr_timediff<<"...");

		//FormulaVarPtr formula_var= m_symbols[var_name];
		FormulaVarPtr formula_var= m_symbols[formula_var_name];
		if(!formula_var){
			WARN_LOG("Failed to get access to formula var "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<"), this should not occur!!!");
			return -1;
		}
		int attr_value_type= formula_var->attr_value_type;

		//If formula attribute is the device attribute skip the check
		if(formula_attr_name==m_attrName){
			DEBUG_LOG("Skip quality check for var "<<var_name<<" (formula_var_name= "<<formula_var_name<<", var_attr_name="<<formula_attr_name<<") as equal to the formula device attribute  ("<<m_attrName<<")");
			continue; 
		}

		//Check timestamp
		float dt= std::fabs(attr_timediff);
		if(m_setInvalidQualityIfOld && dt>static_cast<float>(attr_timestamp_threshold)){
			DEBUG_LOG("Attr var "<<var_name<<" in formula "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<") was evaluated long time ago wrt to threshold (dt="<<dt<<">"<<attr_timestamp_threshold<<"), set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}

		//Check if quality factor is INVALID (only if value is used in formula and NOT the quality itself)
		if(attr_value_type==FormulaVar::eATTR_VALUE && attr_quality==Tango::ATTR_INVALID){
			DEBUG_LOG("Attr var "<<var_name<<" in formula "<<formula_var_name<<" (var_attr_name="<<formula_attr_name<<") has INVALID quality factor, set formula quality to INVALID!");
			formula_attr_quality= Tango::ATTR_INVALID;	
			return 0;
		}//close if

	}//end loop string attrs

	return 0;

}//close ComputeFormulaAttrQuality()


template<typename T>
inline int FormulaData::FillSymbolTable(exprtk::symbol_table<T>& symbol_table_t,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref){

	//## Iterate over formula vars
	typedef std::map<std::string,FormulaVarPtr>::iterator mapIter;

	DEBUG_LOG("#"<<m_symbols.size()<<" vars present in formula...");

	for(mapIter it=m_symbols.begin();it!=m_symbols.end();it++){
		//Get var info
		std::string var_name= it->first;	
		FormulaVarPtr formula_var= it->second;	
		bool is_proxy_attr= formula_var->is_proxy_attr;
		//std::string proxy_name= formula_var->attr_name;
		//if(is_proxy_attr) proxy_name= std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->device_name;
		std::string proxy_attr_name= formula_var->attr_name;
		//Tango::AttributeInfoEx* attr_info= formula_var->attr_info;
		AttrData* attr_data= formula_var->attr_data;
	
		//Check if attr value was set (at least once), otherwise will use init value
		//bool hasAttrData= (attr_info && attr_data && attr_data->is_value_set());
		bool hasAttrData= (attr_data && attr_data->is_value_set());
		int data_format= -1;
		int data_type= -1;
		long int data_size= -1;

		if(is_proxy_attr){
			if(hasAttrData){
				data_format= attr_data->get_data_format();
				data_type= attr_data->get_data_type();
				data_size= attr_data->get_data_size();
			}
			else{
				ProxyAttrFormulaVarPtr casted_formula_var= std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var);
				if(!casted_formula_var){
					WARN_LOG("Failed to cast proxy attr formula var for attr "<<proxy_attr_name<<"!");
					return -1;
				}
				data_format= casted_formula_var->exp_data_format;
				data_type= casted_formula_var->exp_data_type;
				data_size= casted_formula_var->exp_data_size;
				DEBUG_LOG("Attr "<<proxy_attr_name<<" has no info/data or value never set (proxy offline?), will evaluate formula with init data (type="<<data_type<<", format="<<data_format<<", size="<<data_size<<")");
				
			}
		}//close is proxy attr
		else {//internal value
			if(hasAttrData){
				data_format= attr_data->get_data_format();
				data_type= attr_data->get_data_type();
				data_size= attr_data->get_data_size();
			}
			else{//This should not occur!
				WARN_LOG("Internal attr "<<proxy_attr_name<<" has no info/data!");
				return -1;
			}
		}

		//Get formula attr values
		if(data_format==Tango::SCALAR){
			if(data_type==Tango::DEV_STRING) {//string attr
				DEBUG_LOG("Adding scalar string var "<<var_name<<" (attr_name="<<proxy_attr_name<<", hasAttrData?"<<hasAttrData<<", data_type="<<data_type<<") to symbol table...");			
				if(AddStringVarToSymbolTable<T>(symbol_table_t,var_name,formula_var,data_values,timeref,!hasAttrData)<0){
					WARN_LOG("Failed to register numeric symbol for attr "<<proxy_attr_name<<" in table, cannot evaluate formula!");
					return -1;
				}
			}
			#if TANGO_VERSION_MAJOR >= 9
			else if(data_type==Tango::DEV_ENUM) {//enum attr
				DEBUG_LOG("Adding scalar string var "<<var_name<<" (attr_name="<<proxy_attr_name<<", hasAttrData?"<<hasAttrData<<", data_type="<<data_type<<") to symbol table...");			
				if(AddStringVarToSymbolTable<T>(symbol_table_t,var_name,formula_var,data_values,timeref,!hasAttrData)<0){
					WARN_LOG("Failed to register numeric symbol for attr "<<proxy_attr_name<<" in table, cannot evaluate formula!");
					return -1;
				}
			}
			#endif
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
		
			if(data_type==Tango::DEV_STRING){
				if(AddStringSpectrumVarToSymbolTable<T>(symbol_table_t,var_name,formula_var,data_values,timeref,!hasAttrData)<0){
					WARN_LOG("Failed to register spectrum string symbol for attr "<<proxy_attr_name<<" in table, cannot evaluate formula!");
					return -1;
				}
			}
			#if TANGO_VERSION_MAJOR >= 9
			else if(data_type==Tango::DEV_ENUM) {//string attr
				WARN_LOG("Spectrum string and enum attr requested for "<<proxy_attr_name<<"are not supported in formula, cannot evaluate formula!");
				return -1;
			}
			#endif			
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
inline int FormulaData::AddNumericVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;
	bool is_proxy_attr= formula_var->is_proxy_attr;

	//Get current data value, quality & timestamp
	T attr_value;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(is_proxy_attr && useInitValue){
		if(std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->GetScalarAttrDataFromInitValue<T>(attr_value,attr_quality,attr_timestamp)<0){
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
	FormulaVarData<T,T> attr_var_data;
	attr_var_data.attr_value= attr_value;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	attr_var_data.formula_var_name= var_name;
	attr_var_data.formula_attr_name= formula_var->attr_name;

	DEBUG_LOG("Adding numeric var "<<var_name<<" value (attr_value="<<attr_value<<", attr_quality="<<attr_quality<<" attr_timestamp("<<attr_timestamp.tv_sec<<","<<attr_timestamp.tv_usec<<")"<<", dt="<<dt<<") to symbol table...");

	(data_values.scalar_values).insert( std::make_pair(var_name,attr_var_data) );

	//Check what value has to be considered in formula: attr value vs quality factor
	if(formula_var->attr_value_type==FormulaVar::eATTR_QUALITY_FACTOR){
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
inline int FormulaData::AddStateVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;
	bool is_proxy_attr= formula_var->is_proxy_attr;

	//Get current data value, quality & timestamp
	T attr_value;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(is_proxy_attr && useInitValue){//NB: init value is given as string, so need conversion wrt scalar attr case
		std::string init_value_str= "";
		if(std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->GetScalarAttrDataFromInitValue<std::string>(init_value_str,attr_quality,attr_timestamp)<0){
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
	FormulaVarData<T,T> attr_var_data;
	attr_var_data.attr_value= attr_value;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	attr_var_data.formula_var_name= var_name;
	attr_var_data.formula_attr_name= formula_var->attr_name;

	DEBUG_LOG("Adding State var "<<var_name<<" value (attr_value="<<attr_value<<", attr_quality="<<attr_quality<<" attr_timestamp("<<attr_timestamp.tv_sec<<","<<attr_timestamp.tv_usec<<")"<<", dt="<<dt<<") to symbol table...");

	(data_values.scalar_values).insert( std::make_pair(var_name,attr_var_data) );

	//Check what value has to be considered in formula: attr value vs quality factor
	if(formula_var->attr_value_type==FormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.scalar_values)[var_name].attr_quality);
	}
	else{
		DEBUG_LOG("Adding State var "<<var_name<<" value (attr_value="<<attr_value<<") to symbol table...");
		symbol_table.add_variable(var_name.c_str(),(data_values.scalar_values)[var_name].attr_value);
	}
	
	return 0;

}//close AddStateVarToSymbolTable()


template<typename T>
inline int FormulaData::AddNumericSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;
	bool is_proxy_attr= formula_var->is_proxy_attr;

	//Get current data value, quality & timestamp
	std::vector<T> attr_values;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(is_proxy_attr && useInitValue){
		if(std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->GetSpectrumAttrDataFromInitValue<T>(attr_values,attr_quality,attr_timestamp)<0){
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
	FormulaVarData<T,std::vector<T>> attr_var_data;
	attr_var_data.attr_value= attr_values;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	attr_var_data.formula_var_name= var_name;
	attr_var_data.formula_attr_name= formula_var->attr_name;
	(data_values.spectrum_values)[var_name]= attr_var_data;

	if(formula_var->attr_value_type==FormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.spectrum_values)[var_name].attr_quality);
	}
	else{
		symbol_table.add_vector(var_name.c_str(),(data_values.spectrum_values)[var_name].attr_value);
	}
	
	return 0;

}//close AddNumericSpectrumVarToSymbolTable()


template<typename T>
inline int FormulaData::AddStateSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;
	bool is_proxy_attr= formula_var->is_proxy_attr;

	//Get current data value, quality & timestamp
	std::vector<T> attr_values;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(is_proxy_attr && useInitValue){//NB: init value is given as string, so need conversion wrt scalar attr case
		std::vector<std::string> init_values_str;
		if(std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->GetSpectrumAttrDataFromInitValue<std::string>(init_values_str,attr_quality,attr_timestamp)<0){
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
	FormulaVarData<T,std::vector<T>> attr_var_data;
	attr_var_data.attr_value= attr_values;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	attr_var_data.formula_var_name= var_name;
	attr_var_data.formula_attr_name= formula_var->attr_name;
	(data_values.spectrum_values)[var_name]= attr_var_data;

	if(formula_var->attr_value_type==FormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.spectrum_values)[var_name].attr_quality);
	}
	else{
		symbol_table.add_vector(var_name.c_str(),(data_values.spectrum_values)[var_name].attr_value);
	}
	
	return 0;

}//close AddStateSpectrumVarToSymbolTable()


template<typename T>
inline int FormulaData::AddStringVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;
	bool is_proxy_attr= formula_var->is_proxy_attr;

	//Get current data value, quality & timestamp
	std::string attr_value;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(is_proxy_attr && useInitValue){
		if(std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->GetScalarAttrDataFromInitValue<std::string>(attr_value,attr_quality,attr_timestamp)<0){
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
	FormulaVarData<T,std::string> attr_var_data;
	attr_var_data.attr_value= attr_value;
	attr_var_data.attr_quality= static_cast<T>(attr_quality);
	attr_var_data.attr_timediff= dt;
	attr_var_data.formula_var_name= var_name;
	attr_var_data.formula_attr_name= formula_var->attr_name;

	DEBUG_LOG("Adding String var "<<var_name<<" value (attr_value="<<attr_value<<", attr_quality="<<attr_quality<<" attr_timestamp("<<attr_timestamp.tv_sec<<","<<attr_timestamp.tv_usec<<")"<<", dt="<<dt<<") to symbol table...");

	//(data_values.scalar_string_values)[var_name]= attr_var_data;
	(data_values.scalar_string_values).insert( std::make_pair(var_name,attr_var_data) );


	if(formula_var->attr_value_type==FormulaVar::eATTR_QUALITY_FACTOR){
		symbol_table.add_variable(var_name.c_str(),(data_values.scalar_string_values)[var_name].attr_quality);
	}
	else{
		symbol_table.add_stringvar(var_name.c_str(),(data_values.scalar_string_values)[var_name].attr_value);
	}
	
	return 0;

}//close AddStringVarToSymbolTable()


template<typename T>
inline int FormulaData::AddStringSpectrumVarToSymbolTable(exprtk::symbol_table<T>& symbol_table,std::string& var_name,FormulaVarPtr formula_var,FormulaDataValues<T>& data_values,Tango::TimeVal& timeref,bool useInitValue){

	//## Since Exprtk library does not support vector<string> as variables
	//## we add n (n size of vector) variables labeled as <name>_<index> in the formula
		
	//Get formula info
	std::string proxy_attr_name= formula_var->attr_name;
	bool is_proxy_attr= formula_var->is_proxy_attr;

	//Get current data value, quality & timestamp
	std::vector<std::string> attr_values;
	Tango::AttrQuality attr_quality; 
	Tango::TimeVal attr_timestamp;

	if(is_proxy_attr && useInitValue){
		if(std::dynamic_pointer_cast<ProxyAttrFormulaVar>(formula_var)->GetSpectrumAttrDataFromInitValue<std::string>(attr_values,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast init value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}
	else{
		if(formula_var->GetSpectrumAttrData<std::string>(attr_values,attr_quality,attr_timestamp)<0){
			WARN_LOG("Failed to get and cast value for attr "<<proxy_attr_name<<", cannot evaluate formula!");
			return -1;
		}
	}

	//Check if attr timestamp is too old wrt to now
	//NB: In this case formula is allowed to be evaluated but formula attr quality factor should be switched to INVALID
	long dt= TangoUtils::GetTimeDiffSec(timeref,attr_timestamp);
	
	for(unsigned int i=0;i<attr_values.size();i++){	
		std::stringstream ss;
		ss<<var_name<<"_"<<i;
		std::string this_var_name= ss.str();
		DEBUG_LOG("Adding formula var "<<this_var_name<<" to symbol table...");

		//Check what value has to be considered in formula: attr value vs quality factor
		FormulaVarData<T,std::string> attr_var_data;
		attr_var_data.attr_value= attr_values[i];
		attr_var_data.attr_quality= static_cast<T>(attr_quality);
		attr_var_data.attr_timediff= dt;
		attr_var_data.formula_var_name= var_name;
		attr_var_data.formula_attr_name= formula_var->attr_name;
		(data_values.scalar_string_values)[this_var_name]= attr_var_data;

		if(formula_var->attr_value_type==FormulaVar::eATTR_QUALITY_FACTOR){
			symbol_table.add_variable(this_var_name.c_str(),(data_values.scalar_string_values)[this_var_name].attr_quality);
		}
		else{
			symbol_table.add_stringvar(this_var_name.c_str(),(data_values.scalar_string_values)[this_var_name].attr_value);
		}
	}//end loop vector entries

	return 0;
	
}//close AddStringSpectrumVarToSymbolTable()


