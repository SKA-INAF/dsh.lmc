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
* @file DeviceData.h
* @class DeviceData
* @brief DeviceData container
*
* DeviceData container for Tango
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _DEVICE_DATA_h
#define _DEVICE_DATA_h 1

#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS
	#include <CodeUtils.h>
#endif

//Tango headers
#ifndef _TANGO_H
	#include <tango.h>
#endif

#include <vector>


namespace Utils_ns {

//Forward declaration
class FormulaData;


// ============================================================================
// class: AttrGenerationModel
// ============================================================================
class AttrGenerationModel {
	public: 
		virtual ~AttrGenerationModel() {}
		
		enum GenModel {eCONST=0,eUNIFORM=1,eGAUSS=2,eDISCRETE=3};
	public:
		int GetModelType() {return model;}
	protected:
		int model;
};

// ============================================================================
// class: AttrConstModel
// ============================================================================
template <typename T>
class AttrConstModel : public AttrGenerationModel {
	public: 
		AttrConstModel(T init)
			: initValue(init)
		{
			model= eCONST;
		}
		virtual ~AttrConstModel() {}
	public:
		
	public:
		T initValue;
};


// ============================================================================
// class: AttrGaussRandomModel
// ============================================================================
template <typename T>
class AttrGaussRandomModel : public AttrGenerationModel {
	public: 
		AttrGaussRandomModel(T init,T mean,T sigma,T min,T max)
			: initValue(init),gaussMean(mean),gaussSigma(sigma),minValue(min),maxValue(max)
		{
			model= eGAUSS;
		}
		virtual ~AttrGaussRandomModel() {}
	public:
		
	public:
		T initValue;
		T gaussMean;
		T gaussSigma;
		T minValue;
		T maxValue;
};

// ============================================================================
// class: AttrUniformRandomModel
// ============================================================================
template <typename T>
class AttrUniformRandomModel : public AttrGenerationModel {
	public: 
		AttrUniformRandomModel(T init,T min,T max)
			: initValue(init),minValue(min),maxValue(max)
		{
			model= eUNIFORM;
		}
		virtual ~AttrUniformRandomModel() {}
	public:
		
	public:
		T initValue;
		T minValue;
		T maxValue;
};

// ============================================================================
// class: AttrDiscreteRandomModel
// ============================================================================
template <typename T>
class AttrDiscreteRandomModel : public AttrGenerationModel {
	public: 
		AttrDiscreteRandomModel(T init,std::vector<double>& w)
			: initValue(init),probWeights(w)
		{
			model= eDISCRETE;
		}
		virtual ~AttrDiscreteRandomModel() {}
	public:
		
	public:
		T initValue;
		std::vector<double> probWeights;
};


// ============================================================================
// class: DeviceAttrInitData
// ============================================================================
class DeviceAttrInitData {
	public:
		DeviceAttrInitData(){};
		virtual ~DeviceAttrInitData(){};
	public:
		virtual int SetInitValue(void* buf_addr) = 0;
		virtual int SetSafeValue(void* buf_addr) = 0;
		virtual int GetInitValue(void*& buf_addr) = 0;
		virtual int GetSafeValue(void*& buf_addr) = 0;
		virtual bool HasInitValue() const = 0;
		virtual bool HasSafeValue() const = 0;
};

// ============================================================================
// class: DeviceAttrInitDataImpl
// ============================================================================
template <typename T>
class DeviceAttrInitDataImpl : public DeviceAttrInitData {
	public: 

		/** 
		\brief Constructor
 		*/
		DeviceAttrInitDataImpl(T init=T())
			: DeviceAttrInitData(), init_value(init)
		{
			hasInitValue= true;
			hasSafeValue= false;
		}
	
		/** 
		\brief Constructor
 		*/
		DeviceAttrInitDataImpl(T init,T safe)
			: DeviceAttrInitData(), init_value(init), safe_value(safe)
		{
			hasInitValue= true;
			hasSafeValue= true;
		}

		/** 
		\brief Destructor
 		*/
		virtual ~DeviceAttrInitDataImpl() {}

	public:
		virtual int SetInitValue(void* buf_addr) {	
			if(!buf_addr) return -1;
			init_value= *(reinterpret_cast<T*>(buf_addr));
			hasInitValue= true;
			return 0;
		}

		virtual int SetSafeValue(void* buf_addr) {
			if(!buf_addr) return -1;
			safe_value= *(reinterpret_cast<T*>(buf_addr));
			hasSafeValue= true;
			return 0;
		}

		virtual int GetInitValue(void*& buf_addr) {
			buf_addr= &init_value;
			return 0;
		}
		virtual int GetSafeValue(void*& buf_addr) {
			buf_addr= &safe_value;
			return 0;
		}

		void SetSafeValue(T& val){
			safe_value= val;
			hasSafeValue= true;
		}
		void SetInitValue(T& val){
			init_value= val;
			hasInitValue= true;
		}
		const T& GetSafeValue() const {return safe_value;}
		const T& GetInitValue() const {return init_value;}
		virtual bool HasSafeValue() const {return hasSafeValue;}
		virtual bool HasInitValue() const {return hasInitValue;}

	public:
		
		bool hasInitValue;
		T init_value;
		bool hasSafeValue;
		T safe_value;

};//close class DeviceAttrInitDataImpl

// ============================================================================
// class: DeviceAttr
// ============================================================================
class DeviceAttr {

	public:
	
		/** 
		\brief Constructor
 		*/
		DeviceAttr() {
			attrInitData= 0;
			data_size= 1;
			data_format= Tango::SCALAR;
		};

		/** 
		\brief Destructor
 		*/
		virtual ~DeviceAttr() 
		{
			if(attrInitData){
				delete attrInitData;
				attrInitData= 0;
			}
		}//close destructor

	public:

		/** 
		\brief Return attribute (pure virtual)
 		*/
		virtual Tango::Attr* GetAttr() const = 0;
		virtual void SetAttr(Tango::Attr* a) = 0;
		virtual void SetSubscriptionEndpoint(std::string endpoint) = 0;
		virtual void SetSubscriptionAttr(std::string subattrname) = 0;
		virtual bool IsPolled() const = 0;
		virtual void SetPolled(bool) = 0;
		virtual bool IsMemorized() const = 0;
		virtual void SetMemorized(bool) = 0;
		virtual bool IsForwarded() const = 0;
		virtual void SetForwarded(bool) = 0;
		virtual bool HasSubscriptionPoint() const = 0;
		virtual void SetHasSubscriptionPoint(bool) = 0;
		virtual std::string GetSubscriptionEndPoint() const = 0;
		virtual std::string GetSubscriptionAttrName() const = 0;
		virtual void SetRandomize(bool) = 0;
		virtual bool IsRandomize() const = 0;
		virtual void SetGeneratorModel(AttrGenerationModel*) = 0;	
		virtual AttrGenerationModel* GetGeneratorModel() const = 0;
		virtual bool HasGeneratorModel() const = 0;

		virtual bool HasFormulaData() const = 0;
		virtual FormulaData* GetFormulaData() const = 0;
		virtual void SetFormulaData(FormulaData* data) = 0;

		virtual std::string GetDefaultPropName() const = 0;	
		virtual void SetDefaultPropName(std::string value) = 0;
		
		/** 
		\brief Set init attr data (init & safe values)
 		*/
		template<typename K>
		int SetInitData(K& init_value,K& safe_value) {
			if(attrInitData){
				delete attrInitData;
				attrInitData= 0;
			}	
			attrInitData= new DeviceAttrInitDataImpl<K>(init_value,safe_value);
			return 0;
		}
		/** 
		\brief Set init attr data 
 		*/
		template<typename K>
		int SetInitData(K& init_value) {
			if(attrInitData){
				delete attrInitData;
				attrInitData= 0;
			}	
			attrInitData= new DeviceAttrInitDataImpl<K>(init_value);
			return 0;
		}

		/** 
		\brief Get init attr data 
 		*/
		template<typename K>
		int GetInitValue(K& init_value){
			if(!attrInitData) return -1;
			try{
				init_value= dynamic_cast<DeviceAttrInitDataImpl<K>*>(attrInitData)->GetInitValue();
			}
			catch(...){	
				return -1;
			}
			return 0;
		}

		/** 
		\brief Get safe attr data 
 		*/
		template<typename K>
		int GetSafeValue(K& safe_value){
			if(!attrInitData) return -1;
			try{
				safe_value= dynamic_cast<DeviceAttrInitDataImpl<K>*>(attrInitData)->GetSafeValue();
			}
			catch(...){	
				return -1;
			}
			return 0;
		}

		/** 
		\brief Check if has safe attr data stored
 		*/
		virtual bool HasSafeValue() const {
			if(!attrInitData) return false;
			return attrInitData->HasSafeValue();
		}
	
		/** 
		\brief Check if has init attr data stored
 		*/
		virtual bool HasInitValue() const {
			if(!attrInitData) return false;
			return attrInitData->HasInitValue();
		}


		/** 
		\brief Set init attr data (init & safe values)
 		*/
		template <typename K>
		int SetInitValue(K& init_value) {
			if(attrInitData){
				delete attrInitData;
				attrInitData= 0;
			}
			attrInitData= new DeviceAttrInitDataImpl<K>(init_value);	
			return 0;
		}	
		
		/** 
		\brief Set safe attr data (init & safe values)
 		*/
		template <typename K>
		int SetSafeValue(K& safe_value) {
			if(attrInitData){
				delete attrInitData;
				attrInitData= 0;
			}
			attrInitData= new DeviceAttrInitDataImpl<K>(K());
			dynamic_cast<DeviceAttrInitDataImpl<K>*>(attrInitData)->SetSafeValue(safe_value);
			return 0;
		}	
		
	protected:
		DeviceAttrInitData* attrInitData;
		int data_format;
		long int data_size;
		
};//close class DeviceAttr



// ============================================================================
// class: DeviceAttrImpl
// ============================================================================
template <typename T>
class DeviceAttrImpl : public DeviceAttr {
	
	typedef Tango::MultiAttrProp<T> MultiProp;
	
	public:
		/** 
		\brief Default Constructor
 		*/
		DeviceAttrImpl() 
			: DeviceAttr()
		{
			subscribe_endpoint= "";
			subscribe_attr= "";
			isPolled= false;
			isMemorized= false;		
			isForwarded= false;
			hasSubscriptionPoint= false;
			attrRandModel= 0;
			randomize= false;
			formulaData= 0;
			default_prop_name= "";
		};
		

		/** 
		\brief Parametric Constructor
 		*/
		DeviceAttrImpl(Tango::Attr* a,MultiProp* p) 
			: DeviceAttr(), attr(a), prop(p)
		{
			subscribe_endpoint= "";
			subscribe_attr= "";
			isPolled= false;
			isMemorized= false;
			isForwarded= false;	
			hasSubscriptionPoint= false;
			attrRandModel= 0;
			randomize= false;
			formulaData= 0;	
			default_prop_name= "";	
		}
		
		/** 
		\brief Destructor
 		*/
		virtual ~DeviceAttrImpl(){
			if(attr){
				delete attr;
				attr= 0;
			}
			if(prop){
				delete prop;
				prop= 0;
			}
			if(attrRandModel){
				delete attrRandModel;
				attrRandModel= 0;
			}
			if(formulaData){
				delete formulaData;	
				formulaData= 0;
			}
		};//close destructor()

		
	public:
		/** 
		\brief Set Tango attr
 		*/
		Tango::Attr* GetAttr() const {return attr;} 

		/** 
		\brief Get Tango attr properties
 		*/
		MultiProp* GetProp() const {return prop;}

		/** 
		\brief Set Tango attr
 		*/
		virtual void SetAttr(Tango::Attr* aAttr){attr=aAttr;}

		/** 
		\brief Set Tango attr properties
 		*/
		virtual void SetProp(MultiProp* aProp){prop=aProp;}

		/** 
		\brief Set subscription endpoint (DEPRECATED)
 		*/
		virtual void SetSubscriptionEndpoint(std::string endpoint){subscribe_endpoint=endpoint;}

		/** 
		\brief Set subscription attr (DEPRECATED)
 		*/
		virtual void SetSubscriptionAttr(std::string subattrname){subscribe_attr=subattrname;}

		/** 
		\brief Is polled?
 		*/
		bool IsPolled() const {return isPolled;}

		/** 
		\brief Set attribute as polled
 		*/
		void SetPolled(bool value){isPolled=value;}

		/** 
		\brief Is memorized attr?
 		*/
		bool IsMemorized() const {return isMemorized;}

		/** 
		\brief Set attribute as memorized
 		*/
		void SetMemorized(bool value){isMemorized=value;}

		/** 
		\brief Is forwarded attr?
 		*/
		bool IsForwarded() const {return isForwarded;}

		/** 
		\brief Set attribute as forwarded
 		*/
		void SetForwarded(bool value){isForwarded=value;}

		/** 
		\brief Has subscription endpoint (DEPRECATED)
 		*/
		bool HasSubscriptionPoint() const {return hasSubscriptionPoint;}

		/** 
		\brief Set har subscription endpoint (DEPRECATED)
 		*/
		void SetHasSubscriptionPoint(bool value){hasSubscriptionPoint=value;}

		/** 
		\brief Get subscription endpoint (DEPRECATED)
 		*/
		std::string GetSubscriptionEndPoint() const {return subscribe_endpoint;}

		/** 
		\brief Get subscription name (DEPRECATED)
 		*/
		std::string GetSubscriptionAttrName() const {return subscribe_attr;}

		/** 
		\brief Set randomized attr
 		*/
		void SetRandomize(bool flag) {randomize=flag;}

		/** 
		\brief Is attr randomized?
 		*/
		bool IsRandomize() const {return randomize;}

		/** 
		\brief Set attr generator model
 		*/
		void SetGeneratorModel(AttrGenerationModel* model) {attrRandModel=model;}

		/** 
		\brief Get attr generator model
 		*/
		AttrGenerationModel* GetGeneratorModel() const {return attrRandModel;}

		/** 
		\brief Has attr generator model
 		*/
		bool HasGeneratorModel() const {
			if(!attrRandModel) return false;
			return true;
		}

		/** 
		\brief Has formula data
 		*/
		bool HasFormulaData() const {
			if(formulaData) return true;
			return false;
		}
		/** 
		\brief Get formula data
 		*/
		FormulaData* GetFormulaData() const {return formulaData;}
		/** 
		\brief Set formula data
 		*/
		void SetFormulaData(FormulaData* data){formulaData= data;}
		/** 
		\brief Get default property name (for memorized attr)
 		*/
		std::string GetDefaultPropName() const {return default_prop_name;}
		/** 
		\brief Set default property name (for memorized attr)
 		*/	
		void SetDefaultPropName(std::string value){default_prop_name=value;}

	protected:
		
	public:
		Tango::Attr* attr;
		MultiProp* prop;
		bool hasSubscriptionPoint;
		std::string subscribe_endpoint;
		std::string subscribe_attr;
		bool isPolled;
		bool isMemorized;
		bool isForwarded;
		bool randomize;
		AttrGenerationModel* attrRandModel;
		FormulaData* formulaData;
		std::string default_prop_name;
		
};//close DeviceAttrImpl


// ============================================================================
// class: DeviceFwdAttrImpl
// ============================================================================
class DeviceFwdAttrImpl : public DeviceAttr {
	
	public:
		DeviceFwdAttrImpl(){
			subscribe_endpoint= "";
			subscribe_attr= "";
			isPolled= false;
			isMemorized= false;		
			isForwarded= true;
			hasSubscriptionPoint= false;
			attrRandModel= 0;
			randomize= false;
		};
		DeviceFwdAttrImpl(Tango::Attr* a) 
			: attr(a)
		{
			subscribe_endpoint= "";
			subscribe_attr= "";
			isPolled= false;
			isMemorized= false;
			isForwarded= true;	
			hasSubscriptionPoint= false;
			attrRandModel= 0;
			randomize= false;
		}
		
		virtual ~DeviceFwdAttrImpl(){
			if(attr){
				delete attr;
				attr= 0;
			}
			if(attrRandModel){
				delete attrRandModel;
				attrRandModel= 0;
			}
		};

		
	public:
		Tango::Attr* GetAttr() const {return attr;} 
		virtual void SetAttr(Tango::Attr* aAttr){attr=aAttr;}
		virtual void SetSubscriptionEndpoint(std::string endpoint){subscribe_endpoint=endpoint;}
		virtual void SetSubscriptionAttr(std::string subattrname){subscribe_attr=subattrname;}
		bool IsPolled() const {return isPolled;}
		void SetPolled(bool value){isPolled=value;}
		bool IsMemorized() const {return isMemorized;}
		void SetMemorized(bool value){isMemorized=value;}
		bool IsForwarded() const {return isForwarded;}
		void SetForwarded(bool value){isForwarded=value;}
		bool HasSubscriptionPoint() const {return hasSubscriptionPoint;}
		void SetHasSubscriptionPoint(bool value){hasSubscriptionPoint=value;}
		std::string GetSubscriptionEndPoint() const {return subscribe_endpoint;}
		std::string GetSubscriptionAttrName() const {return subscribe_attr;}
		void SetRandomize(bool flag) {randomize=flag;}
		bool IsRandomize() const {return randomize;}
		void SetGeneratorModel(AttrGenerationModel* model) {attrRandModel=model;}
		AttrGenerationModel* GetGeneratorModel() const {return attrRandModel;}
		bool HasGeneratorModel() const {
			if(!attrRandModel) return false;
			return true;
		}
		
		bool HasFormulaData() const {return false;}
		FormulaData* GetFormulaData() const {return nullptr;}
		void SetFormulaData(FormulaData* data){};

		/** 
		\brief Get default property name (for memorized attr)
 		*/
		std::string GetDefaultPropName() const {return std::string("");}
		/** 
		\brief Set default property name (for memorized attr)
 		*/	
		void SetDefaultPropName(std::string value){}
	

		virtual int SetInitData(void* init_data_buffer,void* safe_data_buffer=0) {
			if(!init_data_buffer) return -1;
			//does nothing...
			return 0;
		}

		virtual int GetInitValueBuf(void*& buf_addr) {
			return -1;
		}
		virtual int GetSafeValueBuf(void*& buf_addr) {
			return -1;
		}
		virtual bool HasSafeValue() const {return false;}
		virtual bool HasInitValue() const {return false;}

	public:
		Tango::Attr* attr;
		bool hasSubscriptionPoint;
		std::string subscribe_endpoint;
		std::string subscribe_attr;
		bool isPolled;
		bool isMemorized;
		bool isForwarded;
		bool randomize;
		AttrGenerationModel* attrRandModel;
};//close DeviceFwdAttrImpl



}//close namespace Utils_ns


#endif
