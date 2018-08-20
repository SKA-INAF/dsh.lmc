
template<typename T>
int DevConfigParser::ParseMultiAttrProp(Tango::MultiAttrProp<T>* prop,pugi::xml_node& node) {

	//Set description property
	std::string description= "";
	if(!node.child("Description").empty()) {
		description= node.child("Description").text().as_string();
	}
	prop->description= description;

	//Set label property
	std::string label= "";
	if(!node.child("Label").empty()) {
		label= node.child("Label").text().as_string();
	}
	prop->label= label;

	//Set unit property
	std::string unit= "";
	if(!node.child("Unit").empty()) {
		unit= node.child("Unit").text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(unit);
	}
	prop->unit= unit;		

	//Set value range property
	pugi::xml_node dataRangeTag= node.child("ValueRange");
	if(!dataRangeTag.empty()){
		pugi::xml_node minValueTag= dataRangeTag.child("MinValue");
		pugi::xml_node maxValueTag= dataRangeTag.child("MaxValue");
		pugi::xml_node minAlarmTag= dataRangeTag.child("MinAlarm");
		pugi::xml_node maxAlarmTag= dataRangeTag.child("MaxAlarm");
		pugi::xml_node minWarningTag= dataRangeTag.child("MinWarning");
		pugi::xml_node maxWarningTag= dataRangeTag.child("MaxWarning");

		//Min & max values
		if(!minValueTag.empty()){
			std::string smin= minValueTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smin);
			T xmin= Utils_ns::CodeUtils::getValueFromString<T>(smin);
			//prop->min_value= xmin;
			(prop->min_value).set_val(xmin);
		}
		else prop->min_value= Tango::AlrmValueNotSpec;
		if(!maxValueTag.empty()){
			std::string smax= maxValueTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smax);
			T xmax= Utils_ns::CodeUtils::getValueFromString<T>(smax);
			//prop->max_value= xmax;
			(prop->max_value).set_val(xmax);
		}
		else prop->max_value= Tango::AlrmValueNotSpec;

		//Min & max alarm values	
		if(!minAlarmTag.empty()){
			std::string smin= minAlarmTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smin);
			T xmin= Utils_ns::CodeUtils::getValueFromString<T>(smin);
			prop->min_alarm= xmin;
		}
		else prop->min_alarm= Tango::AlrmValueNotSpec;
		if(!maxAlarmTag.empty()){
			std::string smax= maxAlarmTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smax);
			T xmax= Utils_ns::CodeUtils::getValueFromString<T>(smax);
			prop->max_alarm= xmax;
		}
		else prop->max_alarm= Tango::AlrmValueNotSpec;

		//Min & max warning values
		if(!minWarningTag.empty()){
			std::string smin= minWarningTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smin);
			T xmin= Utils_ns::CodeUtils::getValueFromString<T>(smin);
			prop->min_warning= xmin;
		}
		else prop->min_warning= Tango::AlrmValueNotSpec;
		if(!maxWarningTag.empty()){
			std::string smax= maxWarningTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smax);
			T xmax= Utils_ns::CodeUtils::getValueFromString<T>(smax);
			prop->max_warning= xmax;
		}
		else prop->max_warning= Tango::AlrmValueNotSpec; 


		/*
		bool hasDataRange= ( !minValueTag.empty() && !maxValueTag.empty() );
		bool hasAlarmRange= ( !minAlarmTag.empty() && !maxAlarmTag.empty() );
		bool hasWarningRange= ( !minWarningTag.empty() && !maxWarningTag.empty() );
		if(hasDataRange){
			std::string smin= minValueTag.text().as_string();
			std::string smax= maxValueTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smin);
			Utils_ns::CodeUtils::StripBlankSpaces(smax);
			T xmin= Utils_ns::CodeUtils::getValueFromString<T>(smin);
			T xmax= Utils_ns::CodeUtils::getValueFromString<T>(smax);
			prop->min_value= xmin;
			prop->max_value= xmax;
		}
		else{
			prop->min_value= Tango::AlrmValueNotSpec;
			prop->max_value= Tango::AlrmValueNotSpec;
		}
		if(hasAlarmRange){
			std::string smin= minAlarmTag.text().as_string();
			std::string smax= maxAlarmTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smin);
			Utils_ns::CodeUtils::StripBlankSpaces(smax);
			T xmin= Utils_ns::CodeUtils::getValueFromString<T>(smin);
			T xmax= Utils_ns::CodeUtils::getValueFromString<T>(smax);
			prop->min_alarm= xmin;
			prop->max_alarm= xmax;
		}
		else{
			prop->min_alarm= Tango::AlrmValueNotSpec;
			prop->max_alarm= Tango::AlrmValueNotSpec;
		}
		if(hasWarningRange){
			std::string smin= minWarningTag.text().as_string();
			std::string smax= maxWarningTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smin);
			Utils_ns::CodeUtils::StripBlankSpaces(smax);
			T xmin= Utils_ns::CodeUtils::getValueFromString<T>(smin);
			T xmax= Utils_ns::CodeUtils::getValueFromString<T>(smax);
			prop->min_warning= xmin;
			prop->max_warning= xmax;
		}
		else{
			prop->min_warning= Tango::AlrmValueNotSpec;
			prop->max_warning= Tango::AlrmValueNotSpec;
		}
		*/

		//DEBUG_LOG("(prop->max_value).is_val()="<<(prop->max_value).is_val()<<", (prop->min_warning).is_val()="<<(prop->min_warning).is_val());
		

	}//close if value range

	//Set Enum values
	pugi::xml_node possibleValuesTag= node.child("PossibleValues");
	std::vector<std::string> enum_labels;
	if(!possibleValuesTag.empty()){
		for (pugi::xml_node it = possibleValuesTag.child("Value"); it; it = it.next_sibling("Value")){
			std::string enum_label= it.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(enum_label);
			enum_labels.push_back(enum_label);
		}//end loop enum labels
		if(enum_labels.size()>0) {
			prop->enum_labels= enum_labels;
		}
	}//close if

	//Get event info
	pugi::xml_node eventsTag= node.child("Events");
	bool hasEvents= !eventsTag.empty();	
	bool hasRelChangeEvent= false;
	bool hasRelChangeValue= false;
	
	double relChangeValue= -1;
	bool hasAbsChangeEvent= false;
	bool hasAbsChangeValue= false;
	
	double absChangeValue= -1;
	//bool hasPushChangeEvent= false;		
	bool hasPeriodicEvent= false;
	bool hasEventPeriod= false;
	
	long int periodValue= -1;

	bool hasArchiveEvent= false;
	bool hasArchiveRelChangeEvent= false;
	double archiveRelChangeValue= -1;
	bool hasArchiveAbsChangeEvent= false;
	double archiveAbsChangeValue= -1;
	bool hasArchiveEventPeriod= false;
	long int archivePeriodValue= -1;

	if(hasEvents){
		
		//Rel Change Event
		pugi::xml_node relChangeEventTag= eventsTag.child("RelChangeEvent");
		hasRelChangeEvent= !relChangeEventTag.empty();

		if(hasRelChangeEvent){
			pugi::xml_node relChangeValueTag= relChangeEventTag.child("ChangeValue");
			hasRelChangeValue= !relChangeValueTag.empty();
			if(hasRelChangeValue) {
				relChangeValue= relChangeValueTag.text().as_double();	
			}
			else{
				WARN_LOG("Missing rel_change_value!");
				hasRelChangeEvent= false;
			}
		}//close if rel change

		//Abs Change Event
		pugi::xml_node absChangeEventTag= eventsTag.child("AbsChangeEvent");
		hasAbsChangeEvent= !absChangeEventTag.empty();
		if(hasAbsChangeEvent){
			pugi::xml_node absChangeValueTag= absChangeEventTag.child("ChangeValue");
			hasAbsChangeValue= !absChangeValueTag.empty();
			if(hasAbsChangeValue) {
				absChangeValue= absChangeValueTag.text().as_double();
			}
			else{
				WARN_LOG("Missing abs_change_value!");
				hasAbsChangeValue= false;
			}
		}//close if abs change
				
		//Periodic event
		pugi::xml_node periodicEventTag= eventsTag.child("PeriodicEvent");
		hasPeriodicEvent= !periodicEventTag.empty();
		if(hasPeriodicEvent){
			pugi::xml_node periodValueTag= periodicEventTag.child("Period");
			hasEventPeriod= !periodValueTag.empty();
			if(hasEventPeriod) {
				periodValue= periodValueTag.text().as_llong();
			}
			else {
				WARN_LOG("Missing period value!");
				hasEventPeriod= false;
			}
		}	

		//Archive event
		pugi::xml_node archiveEventTag= eventsTag.child("ArchiveEvent");
		hasArchiveEvent= !archiveEventTag.empty();
		if(hasArchiveEvent){

			//Check periodic archive event
			pugi::xml_node archivePeriodValueTag= archiveEventTag.child("Period");
			hasArchiveEventPeriod= !archivePeriodValueTag.empty();
			if(hasArchiveEventPeriod) {
				archivePeriodValue= archivePeriodValueTag.text().as_llong();
			}
			else {
				DEBUG_LOG("No archive period value specified in config");
				//hasArchiveEventPeriod= false;
			}

			//Check abs change archive event
			pugi::xml_node archiveAbsChangeEventTag= archiveEventTag.child("AbsChange");
			hasArchiveAbsChangeEvent= !archiveAbsChangeEventTag.empty();
			if(hasArchiveAbsChangeEvent){
				archiveAbsChangeValue= archiveAbsChangeEventTag.text().as_double();
			}	
			else{
				DEBUG_LOG("No archive abs_change_value specified in config");
				//hasArchiveAbsChangeValue= false;
			}
			
			//Check rel change archive event
			pugi::xml_node archiveRelChangeEventTag= archiveEventTag.child("RelChange");
			hasArchiveRelChangeEvent= !archiveRelChangeEventTag.empty();
			if(hasArchiveRelChangeEvent){
				archiveRelChangeValue= archiveRelChangeEventTag.text().as_double();	
			}
			else{
				DEBUG_LOG("No archive rel_change_value specified in config");
				//hasArchiveRelChangeEvent= false;
			}
			
			//Check if at least one event mode is given
			if(!hasArchiveEventPeriod && !hasArchiveAbsChangeEvent && !hasArchiveRelChangeEvent){
				WARN_LOG("Missing data for at least one archive events mode (periodic/change/rel), no archive event will be set!");
				hasArchiveEvent= false;
			}
		}//close if archive events
		

		//Pushed event
		//pugi::xml_node pushChangeEventTag= eventsTag.child("PushEvent");
		//hasPushChangeEvent= !pushChangeEventTag.empty();

		//Set properties
		if(hasPeriodicEvent && hasEventPeriod){
			prop->event_period= periodValue;
		}
		if(hasRelChangeEvent && hasRelChangeValue){
			prop->rel_change= relChangeValue;
		}
		if(hasAbsChangeEvent && hasAbsChangeValue){
			prop->abs_change= absChangeValue;
		}
		if(hasArchiveEvent){
			if(hasArchiveEventPeriod) prop->archive_period= archivePeriodValue;
			if(hasArchiveAbsChangeEvent) prop->archive_abs_change= archiveAbsChangeValue;
			if(hasArchiveRelChangeEvent) prop->archive_rel_change= archiveRelChangeValue;	 		
		}
	}//close if has events


	return 0;

}//close ParseMultiAttrProp()


template<typename T>
inline int DevConfigParser::ParseValueNode(DeviceAttr* dev_attr, pugi::xml_node& node) {

	//Check input dev attr
	if(!dev_attr) {
		WARN_LOG("Null ptr to device attr given!");
		return -1;
	}
	
	//Check input node 
	if(node.empty()){
		WARN_LOG("Missing value field!");
		return -1;
	}

	//Get init value
	pugi::xml_node initValueTag= node.child("InitValue");
	if(initValueTag.empty()){
		WARN_LOG("Missing initialization value field!");
		return -1;
	}	
	std::string sInitValue= initValueTag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(sInitValue);
	T initValue= Utils_ns::CodeUtils::getValueFromString<T>(sInitValue);

	//Get safe value (if given)
	pugi::xml_node safeValueTag= node.child("SafeValue");
	if(!safeValueTag.empty()){
		std::string sSafeValue= safeValueTag.text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(sSafeValue);
		T safeValue= Utils_ns::CodeUtils::getValueFromString<T>(sSafeValue);
		//dev_attr->SetInitData(&initValue,&safeValue);
		dev_attr->SetInitData(initValue,safeValue);
	}	
	else{
		DEBUG_LOG("Missing safe value field (will use init value when needed)...");
		//dev_attr->SetInitData(&initValue);
		dev_attr->SetInitData(initValue);
	}
	
	//Get randomize field
	pugi::xml_node randomizeValueTag= node.child("Randomize");
	if(randomizeValueTag.empty()){
		dev_attr->SetRandomize(false);
		dev_attr->SetGeneratorModel( new AttrConstModel<T>(initValue) );
		return 0;
	}

	//Try gaus random model
	bool hasRandomModel= false;
	pugi::xml_node gausRandomModelTag= randomizeValueTag.child("GausRandomModel");

	if(!gausRandomModelTag.empty()){
		pugi::xml_node minValueTag= gausRandomModelTag.child("Min");
		pugi::xml_node maxValueTag= gausRandomModelTag.child("Max");
		pugi::xml_node meanValueTag= gausRandomModelTag.child("GausMean");
		pugi::xml_node sigmaValueTag= gausRandomModelTag.child("GausSigma");		
		bool hasModelPars= !minValueTag.empty() && !maxValueTag.empty() && !meanValueTag.empty() && !sigmaValueTag.empty();
		if(hasModelPars){
			std::string smin= minValueTag.text().as_string();
			std::string smax= maxValueTag.text().as_string();
			std::string smean= meanValueTag.text().as_string();
			std::string ssigma= sigmaValueTag.text().as_string();	
			Utils_ns::CodeUtils::StripBlankSpaces(smin);
			Utils_ns::CodeUtils::StripBlankSpaces(smax);
			Utils_ns::CodeUtils::StripBlankSpaces(smean);
			Utils_ns::CodeUtils::StripBlankSpaces(ssigma);
			T minValue= Utils_ns::CodeUtils::getValueFromString<T>(smin);
			T maxValue= Utils_ns::CodeUtils::getValueFromString<T>(smax);
			T meanValue= Utils_ns::CodeUtils::getValueFromString<T>(smean);
			T sigmaValue= Utils_ns::CodeUtils::getValueFromString<T>(ssigma);

			//DEBUG_LOG("gausRandomModelTag: smin/smax="<<smin<<"/"<<smax<<" min/max="<<minValue<<"/"<<maxValue);

			dev_attr->SetGeneratorModel( new AttrGaussRandomModel<T>(initValue,meanValue,sigmaValue,minValue,maxValue) );
			dev_attr->SetRandomize(true);
			hasRandomModel= true;
		}//close if hasModelPars
	}//close if gausModel

	pugi::xml_node uniformRandomModelTag= randomizeValueTag.child("UniformRandomModel");
	if(!uniformRandomModelTag.empty()){
		pugi::xml_node minValueTag= uniformRandomModelTag.child("Min");
		pugi::xml_node maxValueTag= uniformRandomModelTag.child("Max");
		bool hasModelPars= !minValueTag.empty() && !maxValueTag.empty();
		if(hasModelPars){
			std::string smin= minValueTag.text().as_string();
			std::string smax= maxValueTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(smin);
			Utils_ns::CodeUtils::StripBlankSpaces(smax);
			T minValue= Utils_ns::CodeUtils::getValueFromString<T>(smin);
			T maxValue= Utils_ns::CodeUtils::getValueFromString<T>(smax);
			dev_attr->SetGeneratorModel( new AttrUniformRandomModel<T>(initValue,minValue,maxValue) );
			dev_attr->SetRandomize(true);
			hasRandomModel= true;
		}
	}//close if uniformModel

	pugi::xml_node discreteRandomModelTag= randomizeValueTag.child("DiscreteRandomModel");
	if(!discreteRandomModelTag.empty()){
		pugi::xml_node probWeightsTag= discreteRandomModelTag.child("ProbWeights");
		bool hasModelPars= !probWeightsTag.empty();
		if(hasModelPars){
			std::vector<double> prob_weights;
			for (pugi::xml_node it = probWeightsTag.child("Weight"); it; it = it.next_sibling("Weight")){
				std::string sweight= it.text().as_string();
				Utils_ns::CodeUtils::StripBlankSpaces(sweight);
				double weight= Utils_ns::CodeUtils::getValueFromString<double>(sweight);
				prob_weights.push_back(weight);
			}//end loop enum labels

			if(prob_weights.size()>0) {
				dev_attr->SetGeneratorModel( new AttrDiscreteRandomModel<T>(initValue,prob_weights) );
				dev_attr->SetRandomize(true);
				hasRandomModel= true;
			}
		}//close if
	}//close if discreteModel

	if(!hasRandomModel){
		WARN_LOG("Randomize field detected but empty or invalid model field found!");
		return -1;
	}
	
	return 0;

}//close ParseValueNode()


template<>
inline int DevConfigParser::ParseValueNode<Tango::DevString>(DeviceAttr* dev_attr, pugi::xml_node& node) {

	//Check input dev attr
	if(!dev_attr) {
		WARN_LOG("Null ptr to device attr given!");
		return -1;
	}
	
	//Check input node 
	if(node.empty()){
		WARN_LOG("Missing value field!");
		return -1;
	}

	//Get init value
	pugi::xml_node initValueTag= node.child("InitValue");
	if(initValueTag.empty()){
		WARN_LOG("Missing initialization value field!");
		return -1;
	}	
	std::string initValue= initValueTag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(initValue);

	//Get safe value (if given)
	pugi::xml_node safeValueTag= node.child("SafeValue");
	if(!safeValueTag.empty()){
		std::string sSafeValue= safeValueTag.text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(sSafeValue);
		std::string safeValue= Utils_ns::CodeUtils::getValueFromString<std::string>(sSafeValue);
		//dev_attr->SetInitData(&initValue,&safeValue);
		dev_attr->SetInitData(initValue,safeValue);
	}	
	else{
		DEBUG_LOG("Missing safe value field (will use init value when needed)...");
		//dev_attr->SetInitData(&initValue);
		dev_attr->SetInitData(initValue);
	}
	
	//Get randomize field
	pugi::xml_node randomizeValueTag= node.child("Randomize");
	if(randomizeValueTag.empty()){
		dev_attr->SetRandomize(false);
		dev_attr->SetGeneratorModel( new AttrConstModel<std::string>(initValue) );
		return 0;
	}
	
	//Try gaus random model
	bool hasRandomModel= false;
	
	pugi::xml_node discreteRandomModelTag= randomizeValueTag.child("DiscreteRandomModel");
	if(!discreteRandomModelTag.empty()){
		pugi::xml_node probWeightsTag= discreteRandomModelTag.child("ProbWeights");
		bool hasModelPars= !probWeightsTag.empty();
		if(hasModelPars){
			std::vector<double> prob_weights;
			for (pugi::xml_node it = probWeightsTag.child("Weight"); it; it = it.next_sibling("Weight")){
				std::string sweight= it.text().as_string();
				Utils_ns::CodeUtils::StripBlankSpaces(sweight);
				double weight= Utils_ns::CodeUtils::getValueFromString<double>(sweight);
				prob_weights.push_back(weight);
			}//end loop enum labels

			if(prob_weights.size()>0) {
				dev_attr->SetGeneratorModel( new AttrDiscreteRandomModel<std::string>(initValue,prob_weights) );
				dev_attr->SetRandomize(true);
				hasRandomModel= true;
			}
		}//close if
	}//close if discreteModel

	if(!hasRandomModel){
		WARN_LOG("Randomize field detected but empty or invalid model field found!");
		return -1;
	}

	return 0;

}//close ParseValueNode()

#if TANGO_VERSION_MAJOR >= 9
template<> 
inline int DevConfigParser::ParseValueNode<Tango::DevEnum>(DeviceAttr* dev_attr, pugi::xml_node& node){
	
	//Check input dev attr
	if(!dev_attr) {
		WARN_LOG("Null ptr to device attr given!");
		return -1;
	}
	
	//Check input node 
	if(node.empty()){
		WARN_LOG("Missing value field!");
		return -1;
	}

	//Get init value
	pugi::xml_node initValueTag= node.child("InitValue");
	if(initValueTag.empty()){
		WARN_LOG("Missing initialization value field!");
		return -1;
	}


	//Get attr format
	long data_type= dev_attr->GetAttr()->get_type();
	std::string sInitValue= initValueTag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(sInitValue);

	if(data_type==Tango::DEV_ENUM){
		
		//Retrieve attr labels	
		Tango::MultiAttrProp<Tango::DevEnum>* multi_attr_prop= (dynamic_cast<Utils_ns::DeviceAttrImpl<Tango::DevEnum>*>(dev_attr))->prop;
		std::vector<std::string> enum_labels= multi_attr_prop->enum_labels;
		bool foundInitValue= false;
		Tango::DevShort initValue= -1;

		for(unsigned int i=0;i<enum_labels.size();i++){
			if(sInitValue==enum_labels[i]){
				initValue= i;
				foundInitValue= true;
			}
		}//end loop labels

		if(!foundInitValue){
			WARN_LOG("Cannot find init enum value ("<<sInitValue<<") among enum labels (invalid init or no labels specified)!");
			return -1;
		}

		//Get safe value (if given)
		pugi::xml_node safeValueTag= node.child("SafeValue");
		if(!safeValueTag.empty()){
			std::string safeValueStr= safeValueTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(safeValueStr);
			bool foundSafeValue= false;
			Tango::DevShort safeValue= -1;

			for(unsigned int i=0;i<enum_labels.size();i++){
				if(safeValueStr==enum_labels[i]){
					safeValue= i;
					foundSafeValue= true;
				}
			}//end loop labels

			if(!foundSafeValue){
				WARN_LOG("Cannot find safe enum value ("<<safeValueStr<<") among enum labels (invalid init or no labels specified)!");
				return -1;
			}
			//dev_attr->SetInitData(&initValue,&safeValue);
			dev_attr->SetInitData(initValue,safeValue);

		}//close if
		else{
			DEBUG_LOG("Missing safe value field (will use init value when needed)...");
			//dev_attr->SetInitData(&initValue);
			dev_attr->SetInitData(initValue);
		}

		//Get randomize field
		pugi::xml_node randomizeValueTag= node.child("Randomize");
		if(randomizeValueTag.empty()){
			dev_attr->SetRandomize(false);
			dev_attr->SetGeneratorModel( new AttrConstModel<Tango::DevShort>(initValue) );
			return 0;
		}

		//Try gaus random model
		bool hasRandomModel= false;

		pugi::xml_node discreteRandomModelTag= randomizeValueTag.child("DiscreteRandomModel");
		if(!discreteRandomModelTag.empty()){
			pugi::xml_node probWeightsTag= discreteRandomModelTag.child("ProbWeights");
			bool hasModelPars= !probWeightsTag.empty();
			if(hasModelPars){
				std::vector<double> prob_weights;
				for (pugi::xml_node it = probWeightsTag.child("Weight"); it; it = it.next_sibling("Weight")){
					std::string sweight= it.text().as_string();
					Utils_ns::CodeUtils::StripBlankSpaces(sweight);
					double weight= Utils_ns::CodeUtils::getValueFromString<double>(sweight);
					prob_weights.push_back(weight);
				}//end loop enum labels

				if(prob_weights.size()>0) {
					dev_attr->SetGeneratorModel( new AttrDiscreteRandomModel<Tango::DevShort>(initValue,prob_weights) );
					dev_attr->SetRandomize(true);
					hasRandomModel= true;
				}
			}//close if
		}//close if discreteModel

		if(!hasRandomModel){
			WARN_LOG("Randomize field detected but empty or invalid model field found!");
			return -1;
		}

	}//close if DEV_ENUM type

	else if(data_type==Tango::DEV_SHORT){
		//Get init attr value
		Tango::DevShort initValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevShort>(sInitValue);
	
		//Get safe value (if given)
		pugi::xml_node safeValueTag= node.child("SafeValue");
		if(!safeValueTag.empty()){
			std::string safeValueStr= safeValueTag.text().as_string();
			Utils_ns::CodeUtils::StripBlankSpaces(safeValueStr);			
			Tango::DevShort safeValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevShort>(safeValueStr);
			//dev_attr->SetInitData(&initValue,&safeValue);
			dev_attr->SetInitData(initValue,safeValue);
		}
		else{
			DEBUG_LOG("Missing safe value field (will use init value when needed)...");
			//dev_attr->SetInitData(&initValue);
			dev_attr->SetInitData(initValue);
		}

		//Get randomize field
		pugi::xml_node randomizeValueTag= node.child("Randomize");
		if(randomizeValueTag.empty()){
			dev_attr->SetRandomize(false);
			dev_attr->SetGeneratorModel( new AttrConstModel<Tango::DevShort>(initValue) );
			return 0;
		}

		//Try gaus random model
		bool hasRandomModel= false;
		pugi::xml_node gausRandomModelTag= randomizeValueTag.child("GausRandomModel");

		if(!gausRandomModelTag.empty()){
			pugi::xml_node minValueTag= gausRandomModelTag.child("Min");
			pugi::xml_node maxValueTag= gausRandomModelTag.child("Max");
			pugi::xml_node meanValueTag= gausRandomModelTag.child("GausMean");
			pugi::xml_node sigmaValueTag= gausRandomModelTag.child("GausSigma");		
			bool hasModelPars= !minValueTag.empty() && !maxValueTag.empty() && !meanValueTag.empty() && !sigmaValueTag.empty();
			if(hasModelPars){
				std::string smin= minValueTag.text().as_string();
				std::string smax= maxValueTag.text().as_string();
				std::string smean= meanValueTag.text().as_string();
				std::string ssigma= sigmaValueTag.text().as_string();	
				Utils_ns::CodeUtils::StripBlankSpaces(smin);
				Utils_ns::CodeUtils::StripBlankSpaces(smax);
				Utils_ns::CodeUtils::StripBlankSpaces(smean);
				Utils_ns::CodeUtils::StripBlankSpaces(ssigma);
				Tango::DevShort minValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevShort>(smin);
				Tango::DevShort maxValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevShort>(smax);
				Tango::DevShort meanValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevShort>(smean);
				Tango::DevShort sigmaValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevShort>(ssigma);

				dev_attr->SetGeneratorModel( new AttrGaussRandomModel<Tango::DevShort>(initValue,meanValue,sigmaValue,minValue,maxValue) );
				dev_attr->SetRandomize(true);
				hasRandomModel= true;
			}//close if hasModelPars
		}//close if gausModel

		//Try uniform random model
		pugi::xml_node uniformRandomModelTag= randomizeValueTag.child("UniformRandomModel");
		if(!uniformRandomModelTag.empty()){
			pugi::xml_node minValueTag= uniformRandomModelTag.child("Min");
			pugi::xml_node maxValueTag= uniformRandomModelTag.child("Max");
			bool hasModelPars= !minValueTag.empty() && !maxValueTag.empty();
			if(hasModelPars){
				std::string smin= minValueTag.text().as_string();
				std::string smax= maxValueTag.text().as_string();
				Utils_ns::CodeUtils::StripBlankSpaces(smin);
				Utils_ns::CodeUtils::StripBlankSpaces(smax);
				Tango::DevShort minValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevShort>(smin);
				Tango::DevShort maxValue= Utils_ns::CodeUtils::getValueFromString<Tango::DevShort>(smax);
				dev_attr->SetGeneratorModel( new AttrUniformRandomModel<Tango::DevShort>(initValue,minValue,maxValue) );
				dev_attr->SetRandomize(true);
				hasRandomModel= true;
			}
		}//close if uniformModel

		//Try discrete random model
		pugi::xml_node discreteRandomModelTag= randomizeValueTag.child("DiscreteRandomModel");
		if(!discreteRandomModelTag.empty()){
			pugi::xml_node probWeightsTag= discreteRandomModelTag.child("ProbWeights");
			bool hasModelPars= !probWeightsTag.empty();
			if(hasModelPars){
				std::vector<double> prob_weights;
				for (pugi::xml_node it = probWeightsTag.child("Weight"); it; it = it.next_sibling("Weight")){
					std::string sweight= it.text().as_string();
					Utils_ns::CodeUtils::StripBlankSpaces(sweight);
					double weight= Utils_ns::CodeUtils::getValueFromString<double>(sweight);
					prob_weights.push_back(weight);
				}//end loop enum labels

				if(prob_weights.size()>0) {
					dev_attr->SetGeneratorModel( new AttrDiscreteRandomModel<Tango::DevShort>(initValue,prob_weights) );
					dev_attr->SetRandomize(true);
					hasRandomModel= true;
				}
			}//close if
		}//close if discreteModel

		if(!hasRandomModel){
			WARN_LOG("Randomize field detected but empty or invalid model field found!");
			return -1;
		}
	}//close else if SHORT data type

	return 0;

}//close ParseValueNode() specialization for ENUM/SHORT
#endif

template<> 
inline int DevConfigParser::ParseValueNode<Tango::DevState>(DeviceAttr* dev_attr, pugi::xml_node& node){
	
	//Check input dev attr
	if(!dev_attr) {
		WARN_LOG("Null ptr to device attr given!");
		return -1;
	}
	
	//Check input node 
	if(node.empty()){
		WARN_LOG("Missing value field!");
		return -1;
	}

	//Get init value
	pugi::xml_node initValueTag= node.child("InitValue");
	if(initValueTag.empty()){
		WARN_LOG("Missing initialization value field!");
		return -1;
	}


	//Get attr init value in string form
	std::string sInitValue= initValueTag.text().as_string();
	Utils_ns::CodeUtils::StripBlankSpaces(sInitValue);

	//Convert string to Tango::DevState
	Tango::DevState initValue;
	if(Utils_ns::TangoUtils::TangoStateStrToDevState(initValue,sInitValue)<0){
		WARN_LOG("Cannot get init DevState corresponding to string code ("<<sInitValue<<") (check if invalid init value was specified)!");
		return -1;
	}

	//Get safe value (if given)
	pugi::xml_node safeValueTag= node.child("SafeValue");
	if(!safeValueTag.empty()){
		std::string safeValueStr= safeValueTag.text().as_string();
		Utils_ns::CodeUtils::StripBlankSpaces(safeValueStr);
		
		Tango::DevState safeValue;
		if(Utils_ns::TangoUtils::TangoStateStrToDevState(safeValue,safeValueStr)<0){
			WARN_LOG("Cannot get safe DevState corresponding to string code ("<<safeValueStr<<") (check if invalid safe value was specified)!");
			return -1;
		}

		//dev_attr->SetInitData(&initValue,&safeValue);
		dev_attr->SetInitData(initValue,safeValue);

	}//close if
	else{
		DEBUG_LOG("Missing safe value field (will use init value when needed)...");
		//dev_attr->SetInitData(&initValue);
		dev_attr->SetInitData(initValue);
	}


	//Get randomize field
	pugi::xml_node randomizeValueTag= node.child("Randomize");
	if(randomizeValueTag.empty()){
		dev_attr->SetRandomize(false);
		dev_attr->SetGeneratorModel( new AttrConstModel<Tango::DevState>(initValue) );
		return 0;
	}

	//Try gaus random model
	bool hasRandomModel= false;

	pugi::xml_node discreteRandomModelTag= randomizeValueTag.child("DiscreteRandomModel");
	if(!discreteRandomModelTag.empty()){
		pugi::xml_node probWeightsTag= discreteRandomModelTag.child("ProbWeights");
		bool hasModelPars= !probWeightsTag.empty();
		if(hasModelPars){
			std::vector<double> prob_weights;
			for (pugi::xml_node it = probWeightsTag.child("Weight"); it; it = it.next_sibling("Weight")){
				std::string sweight= it.text().as_string();
				Utils_ns::CodeUtils::StripBlankSpaces(sweight);
				double weight= Utils_ns::CodeUtils::getValueFromString<double>(sweight);
				prob_weights.push_back(weight);
			}//end loop enum labels

			if(prob_weights.size()>0) {
				dev_attr->SetGeneratorModel( new AttrDiscreteRandomModel<Tango::DevState>(initValue,prob_weights) );
				dev_attr->SetRandomize(true);
				hasRandomModel= true;
			}
		}//close if
	}//close if discreteModel

	if(!hasRandomModel){
		WARN_LOG("Randomize field detected but empty or invalid model field found!");
		return -1;
	}

	return 0;

}//close ParseValueNode() specialization for DevState
