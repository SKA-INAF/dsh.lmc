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
* @file AttrData.h
* @class AttrData
* @brief AttrData container
*
* AttrData container 
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _ATTR_DATA_h
#define _ATTR_DATA_h 1

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS_h
	#include <CodeUtils.h>
#endif

//Tango headers
#ifndef _TANGO_H
	#include <tango.h>
#endif

#ifndef _LOG4TANGO_H_
	#include <log4tango.h>
#endif

//Yat headers
#ifndef _YAT_DATA_BUFFER_H_
	#include <yat/memory/DataBuffer.h>
#endif

namespace Utils_ns {

//Forward declarations
//...

// ============================================================================
// class: AttrData
// ============================================================================
class AttrData {
	public:
  	//Pure virtual methods
		//virtual int set_data(void* buf_addr, size_t buf_xdim=1, Tango::AttrQuality quality=Tango::ATTR_VALID) = 0;
		//virtual int get_data(void*& buf_addr, size_t& buf_xdim) = 0;
		virtual int set_data(Tango::DeviceAttribute* da) = 0;
		
		/** 
		\brief Reset data
 		*/
		virtual void reset () = 0;

	public:
  	/** 
		\brief Standard constructor of abstract AttrData class
 		*/
  	AttrData ()
    	: m_attr_name(),
    	  m_data_type(Tango::DEV_VOID)
  	{
			//Invalid timestamp
			set_invalid_timestamp();

			//Valid attr quality factor
    	m_data_quality= Tango::ATTR_VALID;

			//Init var
			m_is_value_set= false;
			m_has_formula= false;
			m_has_attr_formula= false;
  	}
  
		/** 
		\brief Parametric constructor 
 		*/
  	AttrData (const std::string& attr_name, int data_type, Tango::AttrQuality quality=Tango::ATTR_VALID)
    	: m_attr_name(attr_name), m_data_type(data_type), m_data_quality(quality)
  	{
			//Invalid timestamp
			set_invalid_timestamp();

    	//Init val
			m_is_value_set= false;
			m_has_formula= false;
			m_has_attr_formula= false;
  	}
  
		/** 
		\brief Destructor 
 		*/
  	virtual ~AttrData() {
			
		}//close destructor

	public:
  	/** 
		\brief Returns attribute name
 		*/
		const std::string& get_attr_name () const {return m_attr_name;}
  
  	/** 
		\brief Returns attribute data type in Tango enum (e.g. Tango::DEV_STRING, Tango::DEV_FLOAT, ...)
 		*/
		int get_data_type () const {return m_data_type;}

  	/** 
		\brief Returns attribute data format in Tango enum (e.g. Tango::SCALAR, Tango::SPECTRUM, ...)
 		*/	
		int get_data_format () const {return m_data_format;}

		/** 
		\brief Returns attribute data size 
 		*/
		long get_data_size() const {return m_data_size;}

		/** 
		\brief Returns attribute timestamp struct
 		*/
  	const Tango::TimeVal& get_timestamp () const {return m_timestamp;}

		/** 
		\brief Returns attribute data quality factor in Tango format (e.g. Tango::ATTR_VALID, ...)
 		*/
  	const Tango::AttrQuality& get_data_quality () const {return m_data_quality;}

		/** 
		\brief Set timestamp
 		*/
		virtual void set_timestamp(Tango::TimeVal& value);

		/** 
		\brief Set timestamp from C timeval struct
 		*/
		virtual void set_timestamp(struct timeval &t);

		/** 
		\brief Set timestamp from time_t 
 		*/
		virtual void set_timestamp(time_t &t);

		/** 
		\brief Set timestamp from C++ chrono time
 		*/
		virtual void set_timestamp(std::chrono::system_clock::time_point& t);

		/** 
		\brief Set data quality factor
 		*/
		virtual void set_data_quality(Tango::AttrQuality& value);

		
		/** 
		\brief Return true if value was set (at least once), return false if value is only initialized
 		*/
		virtual bool is_value_set(){return m_is_value_set;}

		/** 
		\brief Return true if a formula is registered for this attr in device
 		*/
		virtual bool has_formula(){return m_has_formula;}
		virtual bool has_attr_formula(){return m_has_attr_formula;}

		/** 
		\brief Set has_formula flag
 		*/
		virtual void set_has_formula(bool value){m_has_formula= value;}
		virtual void set_has_attr_formula(bool value){m_has_attr_formula= value;}

		/** 
		\brief Update timestamp
 		*/
		virtual void update_timestamp(){
			set_timestamp_to_now();
		}

		/** 
		\brief Return stringified value
 		*/
		virtual int get_data_str(std::string& data_str){
			data_str= "";
			return 0;		
		}

		/** 
		\brief Set labels
 		*/
		virtual void set_labels(std::vector<std::string> labels){m_labels=labels;}

	protected:

		
		/** 
		\brief Update timestamp to current time
 		*/
		void set_timestamp_to_now();

		/** 
		\brief Set invalid timestamp
 		*/
		void set_invalid_timestamp();
	

	protected:
		yat::Mutex m_lock;
  	std::string m_attr_name;
  	int m_data_type;
		int m_data_format;
		long m_data_size;
		Tango::TimeVal m_timestamp;
		Tango::AttrQuality m_data_quality;
		bool m_is_value_set;
		bool m_has_formula;	
		bool m_has_attr_formula;
		std::vector<std::string> m_labels;

};//close AttrData class
typedef AttrData* AttrDataPtr;
typedef std::map<std::string, AttrDataPtr> AttrDataPtrMap;

// ============================================================================
// class: ScalarAttrData
// ============================================================================
template <typename T> 
class ScalarAttrData : public AttrData {
	protected:
  	T m_init_data;
		T m_data;
		T m_safe_data;

	public:
		/** 
		\brief Standard constructor
 		*/
  	ScalarAttrData ()
  	  : AttrData(), m_init_data(T()), m_data()
  	{
			m_data_format= Tango::SCALAR;
			m_safe_data= m_init_data;
			m_data_size= 1;
  	}
  
		/** 
		\brief Parametric constructor
 		*/
		ScalarAttrData (const std::string& attr_name, int data_type, T init_data = T(), Tango::AttrQuality quality=Tango::ATTR_VALID)
    	: AttrData(attr_name, data_type, quality), m_init_data(init_data), m_data(init_data)
  	{
			m_data_format= Tango::SCALAR;
			m_safe_data= m_init_data;
			m_data_size= 1;
    	this->reset();
  	}

	public:
		/** 
		\brief Return attr data (override pure virtual base method)
 		*/
		virtual const T& get_data() const {
			return m_data;
		}

		/** 
		\brief Set safe data value
 		*/
		virtual int set_safe_data_value(T& value) {
			yat::MutexLock guard(m_lock);
			m_safe_data= value;
			return 0;
		}

		/** 
		\brief Set attr data to safe value
 		*/
		virtual int set_data_to_safe(T& safe_data_set,Tango::AttrQuality quality){
			yat::MutexLock guard(m_lock);
			m_data= m_safe_data;
			m_data_quality= quality;
			set_timestamp_to_now();
			m_is_value_set= true;
			safe_data_set= m_safe_data;
			return 0;
		}

		/** 
		\brief Set attr data to safe value. Do not change quality factor.
 		*/
		virtual int set_data_to_safe(T& safe_data_set){
			yat::MutexLock guard(m_lock);
			m_data= m_safe_data;
			set_timestamp_to_now();
			m_is_value_set= true;
			safe_data_set= m_safe_data;
			return 0;
		}
	
		/** 
		\brief Set attr data from given value (override pure virtual base method)
 		*/
		virtual int set_data(T& value,Tango::AttrQuality quality=Tango::ATTR_VALID){
			yat::MutexLock guard(m_lock);
			m_data= value;
			m_data_quality= quality;
			set_timestamp_to_now();
			m_is_value_set= true;
			//DEBUG_LOG("m_data="<<m_data);
			return 0;
		}
	
		/** 
		\brief Set attr data from DeviceAttribute (override pure virtual base method)
 		*/
		virtual int set_data(Tango::DeviceAttribute* da){
			yat::MutexLock guard(m_lock);
			if(!da) return -1;
			T value;
			try {
				(*da)>>value;
			}
			catch (Tango::DevFailed& e) {	
				Tango::Except::print_exception(e);
				WARN_LOG("Tango exception while extracting DeviceAttribute to scalar value!");
				return -1;	
			}
			m_data= value;
			m_data_quality= da->get_quality();
			m_timestamp= da->get_date();
			m_is_value_set= true;
			DEBUG_LOG("Set data from DeviceAttribute: m_data="<<m_data<<", m_data_quality="<<m_data_quality<<" m_timestamp=("<<m_timestamp.tv_sec<<","<<m_timestamp.tv_usec<<")");
			return 0;
		}

		
		/** 
		\brief Set attr data from buffer (override pure virtual base method)
 		*/
		/*
		virtual int set_data(void* buf_addr,size_t buf_xdim=1,Tango::AttrQuality quality=Tango::ATTR_VALID){
			yat::MutexLock guard(m_lock);
			if(!buf_addr) return -1;
			m_data= *(reinterpret_cast<T*>(buf_addr));
			m_data_quality= quality;
			set_timestamp_to_now();
			return 0;
		}
		*/
		/** 
		\brief Set attr data from given value in buffer (override pure virtual base method)
 		*/
		/*
		virtual int get_data(void*& buf_addr, size_t& buf_xdim){
			yat::MutexLock guard(m_lock);
			buf_addr= &m_data;
			buf_xdim= 1;
			return 0;
		}
		*/

		/** 
		\brief Return stringified value
 		*/
		virtual int get_data_str(std::string& data_str){
			return get_data_str_i(data_str);
		}

	protected:

		/** 
		\brief Reset data
 		*/
		virtual int get_data_str_i(std::string& data_str){
			std::stringstream ss;
			ss<<m_data;
			data_str= ss.str();
			return 0;
		}

		/** 
		\brief Reset data
 		*/
  	virtual void reset () {m_data= m_init_data;}

};//close ScalarAttrData class

//Override get_data_str for enum attr
template<>
inline int ScalarAttrData<Tango::DevEnum>::get_data_str(std::string& data_str){

	//If DevShort return standard method
	get_data_str_i(data_str);
	if(m_data_type==Tango::DEV_SHORT) return 0;

	//Return labels for enum
	if(m_labels.empty()){
		WARN_LOG("Empty labels for attr "<<m_attr_name<<", returning stringified value (not enum label)!");
		return -1;
	}
	if(m_data<0 || m_data>=static_cast<Tango::DevShort>(m_labels.size()) ){
		WARN_LOG("Enum value ("<<m_data<<") exceeds enum label range, returning stringified value (not enum label)!");
		return -1;
	}
	data_str= m_labels[m_data];
	return 0;

}//close get_data_str()



// ============================================================================
// class: ScalarEnumAttrData
// ============================================================================
#if TANGO_VERSION_MAJOR >= 9
class ScalarEnumAttrData : public ScalarAttrData<Tango::DevShort>
{
	protected:
		std::vector<std::string> m_enum_labels;

	public:
		/** 
		\brief Constructor
 		*/
		ScalarEnumAttrData(const std::string& attr_name, int data_type, std::vector<std::string> labels, Tango::DevShort init_data = Tango::DevShort(), Tango::AttrQuality quality=Tango::ATTR_VALID)
			: ScalarAttrData<Tango::DevShort>(attr_name,data_type,init_data,quality), m_enum_labels(labels)
		{
			m_data_type= Tango::DEV_ENUM;		
			m_data_size= 1;
		}

	public:
		/** 
		\brief Set attr data from given value (override pure virtual base method)
 		*/
		virtual int set_data(Tango::DevShort& value,Tango::AttrQuality quality=Tango::ATTR_VALID){
			yat::MutexLock guard(m_lock);

			//Check if given enum code exceeds number of labels
			if(value<0 || value>(int)(m_enum_labels.size()-1)){
				std::stringstream ss;
				ss<<"Invalid enum value given ("<<value<<"), valid values are {0,"<<m_enum_labels.size()-1<<"}";
				ERROR_LOG(ss.str());
				return -1;
			}
			m_data= value;
			m_data_quality= quality;
			set_timestamp_to_now();
			m_is_value_set= true;
			return 0;
		}//close set_data()

		/** 
		\brief Get string label corresponding to data
 		*/
		virtual const std::string& get_data_str(){
			return m_enum_labels[m_data];		
		}
	
		/** 
		\brief Get string labels 
 		*/
		virtual const std::vector<std::string>& get_data_labels(){
			return m_enum_labels;		
		}

};//close ScalarEnumAttrData class
#endif

// ============================================================================
// class: SpectrumAttrData
// ============================================================================
template <typename T> 
class SpectrumAttrData : public AttrData
{
	private:
 		T m_init_data;
		//yat::Buffer<T> m_data;
		size_t m_xdim;
		std::vector<T> m_data;
  	T m_safe_data;

	public:
  	/** 
		\brief Standard constructor
 		*/
		SpectrumAttrData ()
    	: AttrData(),
      	m_init_data(T()), m_xdim(0)
  	{
			m_data_format= Tango::SPECTRUM;
			m_data.clear();
			m_safe_data= m_init_data;
			m_data_size= 0;
  	}

		/** 
		\brief Parametric constructor
 		*/
 	 	SpectrumAttrData (const std::string& attr_name, int data_type, size_t data_size, T init_data = T(),Tango::AttrQuality quality=Tango::ATTR_VALID)
    	: AttrData(attr_name, data_type, quality), m_init_data(init_data), m_xdim(data_size)
  	{
			m_data_format= Tango::SPECTRUM;
			m_safe_data= m_init_data;
			m_data_size= data_size;
    	this->reset();
  	}

	public:
		/** 
		\brief Return data 
 		*/
		virtual const std::vector<T>& get_data() const {
			return m_data;
		}

		/** 
		\brief Set safe data value
 		*/
		virtual int set_safe_data_value(T& value) {
			yat::MutexLock guard(m_lock);
			m_safe_data= value;
			return 0;
		}

		/** 
		\brief Set attr data to safe value
 		*/
		virtual int set_data_to_safe(std::vector<T>& safe_data_set,Tango::AttrQuality quality){
			yat::MutexLock guard(m_lock);
			for(unsigned int i=0;i<m_data.size();i++){
				m_data[i]= m_safe_data;
			}
			m_data_quality= quality;
			set_timestamp_to_now();
			m_is_value_set= true;
			safe_data_set= m_data;
			return 0;
		}

		/** 
		\brief Set attr data to safe value. Do not change quality factor.
 		*/
		virtual int set_data_to_safe(std::vector<T>& safe_data_set){
			yat::MutexLock guard(m_lock);
			for(unsigned int i=0;i<m_data.size();i++){
				m_data[i]= m_safe_data;
			}
			set_timestamp_to_now();
			m_is_value_set= true;
			safe_data_set= m_data;
			return 0;
		}


		/** 
		\brief Set data from vector 
 		*/
		virtual int set_data(std::vector<T>& v,Tango::AttrQuality quality=Tango::ATTR_VALID) {
			yat::MutexLock guard(m_lock);
			try{
				m_data= v;
				m_xdim= static_cast<int>(v.size());
			}
			catch(std::exception& e){
				ERROR_LOG("C+ excetion occurred while setting data (err="<<e.what()<<")");
				return -1;
			}
			m_data_quality= quality;
			set_timestamp_to_now();
			m_is_value_set= true;
			return 0;
		}//close set_data()

		/** 
		\brief Set data from typed buffer 
 		*/
		virtual int set_data(const T* buffer,size_t bufsize,Tango::AttrQuality quality=Tango::ATTR_VALID) {
			yat::MutexLock guard(m_lock);
			if(!buffer || bufsize<=0) {
				ERROR_LOG("Null ptr to buf addr given or invalid buf size!");
				return -1;
			}
			try{
				m_data.assign(buffer,buffer+bufsize);//copy data
				m_xdim= static_cast<int>(bufsize);
			}
			catch(std::exception& e){
			  ERROR_LOG("C+ excetion occurred while setting data (err="<<e.what()<<")");
				return -1;
			}
			m_data_quality= quality;
			set_timestamp_to_now();	
			m_is_value_set= true;
			return 0;
		}//close set_data()


		/** 
		\brief Set attr data from DeviceAttribute (override pure virtual base method)
 		*/
		virtual int set_data(Tango::DeviceAttribute* da){
			yat::MutexLock guard(m_lock);
			if(!da) {
				WARN_LOG("Null ptr to given DeviceAttribute!");
				return -1;
			}
			std::vector<T> value;
			try {
				(*da)>>value;
			}
			catch (Tango::DevFailed& e) {	
				Tango::Except::print_exception(e);
				WARN_LOG("Tango exception while extracting DeviceAttribute to spectrum value!");
				return -1;	
			}
			m_data= value;
			m_data_quality= da->get_quality();
			m_timestamp= da->get_date();
			m_is_value_set= true;
			return 0;
		}//close set_data()

		/** 
		\brief Set data from void buffer
 		*/
		/*
		virtual int set_data(void* buf_addr, size_t buf_xdim,Tango::AttrQuality quality=Tango::ATTR_VALID){	
			yat::MutexLock guard(m_lock);
			if(!buf_addr) return -1;
			T* buffer= reinterpret_cast<T*>(buf_addr);
			m_xdim= buf_xdim;
			m_data.assign(buffer,buffer+buf_xdim*sizeof(T));
			m_data_quality= quality;
			set_timestamp_to_now();
			return 0;
		}
		*/
		/** 
		\brief Return data to a void buffer 
 		*/
		/*
		virtual int get_data(void*& buf_addr, size_t& buf_xdim){
			yat::MutexLock guard(m_lock);
			//buf_addr = m_data.data();//this would not be valid for bool
			buf_xdim = m_data.size();
			buf_addr= new char[buf_xdim*sizeof(T)];
			std::copy(m_data.begin(), m_data.end(), buf_addr);
			return 0;
		}
		*/
	
		/** 
		\brief Return stringified value
 		*/
		virtual int get_data_str(std::string& data_str){
			return get_data_str_i(data_str);
		}

	protected:
		/** 
		\brief Reset data
 		*/
  	virtual void reset() {m_data.assign(m_xdim,m_init_data);}

		/** 
		\brief Get data string list
 		*/
		virtual int get_data_str_i(std::string& data_str){
			std::stringstream ss;
			for(unsigned int i=0;i<m_data.size()-1;i++){
				ss<<m_data[i]<<", ";
			}
			ss<<m_data[m_data.size()-1];
			data_str= ss.str();
			return 0;
		}

};//close SpectrumAttrData class


//Override get_data_str for enum attr
template<>
inline int SpectrumAttrData<Tango::DevEnum>::get_data_str(std::string& data_str){

	//If DevShort return standard method
	data_str= get_data_str_i(data_str);
	if(m_data_type==Tango::DEV_SHORT) return 0;

	//For enum return labels
	if(m_labels.empty()){
		std::stringstream errStream;
		errStream<<"Empty labels for attr "<<m_attr_name<<", returning stringified value (not enum label)!";
		WARN_LOG(errStream.str());
		return -1;
	}
	
	std::stringstream ss;
	bool isFailed= false;
	for(unsigned int i=0;i<m_data.size()-1;i++){
		if(m_data[i]<0 || m_data[i]>=static_cast<Tango::DevShort>(m_labels.size()) ){
			std::stringstream errStream;
			errStream<<"Enum value ("<<m_data[i]<<") exceeds enum label range, returning stringified value (not enum label)!";
			WARN_LOG(errStream.str());
			isFailed= true;
			break;
		}
		std::string s= m_labels[m_data[i]];
		ss<<s<<", ";
	}

	if(isFailed) return -1;

	if(m_data[m_data.size()-1]<0 || m_data[m_data.size()-1]>=static_cast<Tango::DevShort>(m_labels.size()) ){
		std::stringstream errStream;
		errStream<<"Enum value ("<<m_data[m_data.size()-1]<<") exceeds enum label range, returning stringified value (not enum label)!";
		WARN_LOG(errStream.str());
		return -1;
	}
	ss<<m_labels[m_data[m_data.size()-1]];
	data_str= ss.str();
	return 0;

}//close get_data_str()



// ============================================================================
// class: AttrDataConverter
// ============================================================================
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
		/*	
		Utils_ns::ScalarEnumAttrData* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::ScalarEnumAttrData*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		*/
		Utils_ns::ScalarAttrData<Tango::DevEnum>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::ScalarAttrData<Tango::DevEnum>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to scalar attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		//casted_attr_value= ad->get_data_str();
		if(ad->get_data_str(casted_attr_value)<0){
			WARN_LOG("Failed to cast enum attr data value to string!");
			return -1;
		}
	
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
		if(!attr_data) return -1;
		Utils_ns::SpectrumAttrData<std::string>* ad= 0;
		try{
			ad= dynamic_cast<Utils_ns::SpectrumAttrData<std::string>*>(attr_data);
		}
		catch(...){
			WARN_LOG("Failed to cast attr data to spectrum attr!");
			return -1;
		}
		attr_timestamp= ad->get_timestamp();
		attr_quality= ad->get_data_quality();
		casted_attr_value= ad->get_data();

		return 0;
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

}//close namespace

#endif


