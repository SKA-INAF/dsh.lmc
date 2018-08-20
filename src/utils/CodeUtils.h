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
* @file CodeUtils.h
* @class CodeUtils
* @brief Utility functions for programming shortcut tasks
*
* Utility functions for programming shortcut tasks
* @author S. Riggi
* @date 15/01/2016
*/


#ifndef _CODE_UTILS_h
#define _CODE_UTILS_h 1

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

//# JSON CPP
#ifndef JSON_JSON_H_INCLUDED
	#include <json/json.h>
#endif

//# BOOST
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

//# C+ headers
#include <vector>
#include <algorithm>
#include <map>


namespace Utils_ns {


// ============================================================================
// class: CodeUtils
// ============================================================================
class CodeUtils {

  public:
		
		/** 
		\brief Class constructor: initialize structures.
 		*/
    CodeUtils();
		/**
		* \brief Class destructor: free allocated memory
		*/
   	virtual ~CodeUtils();

		
	public:
		/**
		* \brief Generate uuid string
		*/
		static std::string GenerateUUID(){
			boost::uuids::uuid random_uuid = boost::uuids::random_generator()();
			return boost::lexical_cast<std::string>(random_uuid);
		}

		/**
		* \brief Convert json object to string
		*/
		static int JsonToString(std::string& jsonString,Json::Value& jsonObj,bool isMinified=true){
			//Encode to string
			try {
				if(isMinified){// write in a minified way
					Json::FastWriter fastWriter;
					jsonString= fastWriter.write(jsonObj);
				}
				else{	// write in a nice readible way
					Json::StyledWriter formattedWriter;
					jsonString= formattedWriter.write(jsonObj);
				}
			}//close try
			catch(...){
				ERROR_LOG("Failed to encode to json string!");
				return -1;
			}		
			return 0;
		}//close JsonToString()

		/** 
		\brief Parse json string and return Json object
 		*/
		static int StringToJson(Json::Value& root,std::string& jsonString){		
			Json::Reader reader;
			if(!reader.parse(jsonString, root)) {
				ERROR_LOG("Failed to encode string to json ("<<reader.getFormattedErrorMessages()<<")");
				return -1;
			}	
			if(root.isNull()) return -1;
			return 0;
		}//close StringToJson()

		
		/** 
		\brief Check if object is a pointer
 		*/
		template<typename T>
			struct IsPointer { static const bool value = false; };

		/** 
		\brief Check if object is a pointer
 		*/
		template<typename T>
			struct IsPointer<T*> { static const bool value = true; };

		/**
		* \brief Find item in a vector and returns item position
		*/
		template<class T>
			static bool FindItem(std::vector<T>& v, T itemValue,int& pos) {
				if(v.size()<=0){
					pos= -1;
					return false;
				}	
				typename std::vector<T>::iterator it= std::find(v.begin(),v.end(),itemValue);
				if(it==v.end()){
					pos= -1;
					return false;
				}	
				pos= (int)(it-v.begin());
				return true;
			}//close FindItem()


		/**
		* \brief Delete selected items from a vector
		*/
		template<class T>
			static void DeleteItems(std::vector<T>& data, const std::vector<int>& deleteIndices) {
    	std::vector<bool> markedElements(data.size(), false);
    	std::vector<T> tempBuffer;
    	tempBuffer.reserve(data.size()-deleteIndices.size());

   	 	for (std::vector<int>::const_iterator itDel = deleteIndices.begin(); itDel != deleteIndices.end(); itDel++)
      	markedElements[*itDel] = true;

    	for (size_t i=0; i<data.size(); i++) {
				if (!markedElements[i]) {
					tempBuffer.push_back(data[i]);
				}
    	}//end loop data
    	data = tempBuffer;
		}//close DeleteItems()


		template<class T>
			static void DeleteItems(std::vector<T*>& data, const std::vector<int>& deleteIndices) {
    	std::vector<bool> markedElements(data.size(), false);
    	std::vector<T> tempBuffer;
    	tempBuffer.reserve(data.size()-deleteIndices.size());

   	 	for (std::vector<int>::const_iterator itDel = deleteIndices.begin(); itDel != deleteIndices.end(); itDel++)
      	markedElements[*itDel] = true;

    	for (size_t i=0; i<data.size(); i++) {
				if (markedElements[i]) {//Free memory!
					if(data[i]){
						delete data[i];
						data[i]= 0;
					}			
				}
				else{
					tempBuffer.push_back(data[i]);
				}
    	}//end loop data
    	data = tempBuffer;
		}//close DeleteItems()

		
		template<class T>
		static void DeleteItem(std::vector<T*>& data, const int& deleteIndex) {
    	std::vector<int> deleteIndices;
			deleteIndices.push_back(deleteIndex);
			return DeleteItems<T>(data,deleteIndices);
		}//close DeleteItem()

		template<class T>
		static void DeleteItem(std::vector<T>& data, const int& deleteIndex) {
    	std::vector<int> deleteIndices;
			deleteIndices.push_back(deleteIndex);
			return DeleteItems<T>(data,deleteIndices);
		}//close DeleteItem()

		/**
		* \brief Cast from container type to another type
		*/
		template <typename To, typename From>
		static To container_cast(From && from) {
	    using std::begin; using std::end; // Koenig lookup enabled
    	return To(begin(from), end(from));
		}

		/**
		* \brief Order vectors and get ordering index
		*/
		template<class T> struct index_cmp{

  		index_cmp(const T arr) : arr(arr) {}
  		bool operator()(const size_t a, const size_t b) const
 			{
    		return arr[a] < arr[b];
  		}
  		const T arr;
		};
		template<class T> struct descending_index_cmp{

  		descending_index_cmp(const T arr) : arr(arr) {}
  		bool operator()(const size_t a, const size_t b) const
 			{
    		return arr[a] > arr[b];
  		}
  		const T arr;
		};

		template< class T >
			static void reorder(std::vector<T> & unordered,std::vector<size_t> const & index_map,std::vector<T> & ordered){
  			// copy for the reorder according to index_map, because unsorted may also be
  			// sorted
  			std::vector<T> copy = unordered;
  			ordered.resize(index_map.size());
  			for(int i = 0; i<index_map.size();i++)
					ordered[i] = copy[index_map[i]];
			}

		template <class T>
			static void sort(std::vector<T> & unsorted,std::vector<T> & sorted,std::vector<size_t> & index_map){
  			// Original unsorted index map
  			index_map.resize(unsorted.size());
 				for(size_t i=0;i<unsorted.size();i++)
					index_map[i] = i;
  
  			// Sort the index map, using unsorted for comparison
  			std::sort(index_map.begin(),index_map.end(),index_cmp<std::vector<T>& >(unsorted));
  			sorted.resize(unsorted.size());
  			reorder(unsorted,index_map,sorted);
			}
	
		template <class T>
			static void sort_descending(std::vector<T> & unsorted,std::vector<T> & sorted,std::vector<size_t> & index_map){
  			// Original unsorted index map
  			index_map.resize(unsorted.size());
 				for(size_t i=0;i<unsorted.size();i++)
					index_map[i] = i;
  
  			// Sort the index map, using unsorted for comparison
  			std::sort(index_map.begin(),index_map.end(),descending_index_cmp<std::vector<T>& >(unsorted));
  			sorted.resize(unsorted.size());
  			reorder(unsorted,index_map,sorted);
			}


		static int StringFindAndReplace(std::string& str, const std::string& oldstr, const std::string& newstr){
  		size_t pos = 0;
			int nfound= 0;
  		while((pos = str.find(oldstr, pos)) != std::string::npos){
    		str.replace(pos, oldstr.length(), newstr);
     		pos += newstr.length();
				nfound++;
  		}		
			if(nfound==0) return -1;
			return 0;
		}//close StringFindAndReplace()


		// return an evenly spaced 1-d grid of doubles.
		template <typename T>
			static std::vector<T> linspace(T first,T last,int len){
				std::vector<T> result(len);
  			double step = (last-first) / (len - 1);
  			for (int i=0; i<len; i++) { result[i] = first + i*step; }
  			return result;
			}

		/**
		* \brief Extract a scalar value from string
		*/
		template<typename T>
			static T getValueFromString(std::string& input){
				T value= T();
				std::stringstream sstream;
				sstream.str(input);
				sstream>>value;
				return value;
			}

		/**
		* \brief Strip blank spaces from string
		*/
		static int StripBlankSpaces(std::string& s){
			try {
				s.erase( 
					std::remove_if(
						s.begin(),
						s.end(),
						[](char c){ 
      				return std::isspace(static_cast<unsigned char>(c));
      			}
					),
					s.end()
				);
			}//close try
			catch(std::exception& e){
				ERROR_LOG("C++ exception ("<<e.what()<<") occurred while stripping blank spaces from string!");
				return -1;
			}
			return 0;
		}//close StripBlankSpaces()

		/**
		* \brief Strip all text matching pattern from a string
		*/
		static void StripPatternFromString(std::string& s, std::string p) { 
  		std::string::size_type n = p.length();
  		for (std::string::size_type i = s.find(p);i != string::npos; i = s.find(p)) s.erase(i, n);
		}

		/**
		* \brief Trasform string to camel case
		*/
		static void TransformToCamelCase (std::string &s){

			//Replace underscores/dashes with spaces and convert all to lower case  
			std::transform(s.begin(), s.end(), s.begin(), [](char ch) {
				bool isSeparator= (ch=='-' || ch=='_' || ch==',' || ch==';');
  			if(isSeparator) return ' ';
				ch= tolower(ch);
				return ch;
			});

			//Slip words and concatenate
			std::istringstream iss(s);
  		std::string word;
			int word_counter= 0;
			std::string transformed_string= "";
  		while(iss >> word) {
				//For words after the first change first char to upper case
				if(word_counter>0){
					std::string::iterator it = std::find_if( word.begin(), word.end(), [](char ch) {return isalpha(ch);} );
					if(it!=word.end()){
						*it= toupper(*it); 
					}
				}
				word_counter++;
				transformed_string+= word;
  		}
	
			s= transformed_string;

		}//close transformToCamelCase()
	
	
	private:

};//close CodeUtils


/**
* \brief Extract a bool scalar value from string 
*/
template<>
inline bool CodeUtils::getValueFromString<bool>(std::string& input){
	bool value= bool();
	std::stringstream sstream;
	sstream.str(input);
	sstream>> std::boolalpha >> value;
	return value;
}

/**
* \brief Extract a DevState scalar value from string 
*/
template<>
inline Tango::DevState CodeUtils::getValueFromString<Tango::DevState>(std::string& input){
	Tango::DevState value= Tango::UNKNOWN;
	if(input=="ON") return Tango::ON;
  else if(input=="OFF") return Tango::OFF;
  else if(input=="CLOSE") return Tango::CLOSE;
  else if(input=="OPEN") return Tango::OPEN;
  else if(input=="INSERT") return Tango::INSERT;
  else if(input=="EXTRACT") return Tango::EXTRACT;
  else if(input=="MOVING") return Tango::MOVING;
  else if(input=="STANDBY") return Tango::STANDBY;
  else if(input=="FAULT") return Tango::FAULT;
  else if(input=="INIT") return Tango::INIT;
  else if(input=="RUNNING") return Tango::RUNNING;
  else if(input=="ALARM") return Tango::ALARM;
  else if(input=="DISABLE") return Tango::DISABLE;
  else if(input=="UNKNOWN") return Tango::UNKNOWN;
	return value;
}


// ============================================================================
// class: AbstractSet
// ============================================================================
class AbstractSet {
	public:
		AbstractSet(){};
		virtual ~AbstractSet(){};
	public:
		virtual std::string GetPrintable() = 0;
};
typedef std::shared_ptr<AbstractSet> AbstractSetPtr;

// ============================================================================
// class: Set
// ============================================================================
template <class T>
class Set : public AbstractSet {
	public:
		Set(){};
		Set(std::vector<T> v) {
			m_vec.clear();
			m_vec= std::move(v);
		};
		Set(std::initializer_list<T> init_set) {
			m_vec.clear();
			m_vec= init_set;
		};
		~Set(){};
	public:
		virtual bool HasValueInSet(T value){
			if(m_vec.empty()) return false;
			typename std::vector<T>::iterator it= std::find(m_vec.begin(),m_vec.end(),value);
			if(it==m_vec.end()) return false;
			return true;
		}

		virtual int GetValueInSet(T& item,T searchedValue) const {
			if(m_vec.empty()) return -1;
			typename std::vector<T>::const_iterator it= std::find(m_vec.begin(),m_vec.end(),searchedValue);
			if(it==m_vec.end()) return -1;
			size_t pos= it-m_vec.begin();
    	item= m_vec.at(pos);
			return 0;
    }
	
		virtual const std::vector<T>& GetValues() const {return m_vec;}
		
		virtual std::string GetPrintable(){
			std::stringstream ss;
			ss<<"{";
			for(unsigned int i=0;i<m_vec.size();i++){
    		ss<< m_vec[i] <<",";
			}	
			ss<<"}";
			return ss.str();
		}
	protected:
		std::vector<T> m_vec;
};

// ============================================================================
// class: AbstractMapSet
// ============================================================================
class AbstractMapSet {
	public:
		AbstractMapSet(){};
		~AbstractMapSet(){};
		virtual std::string GetPrintable() = 0;
};
typedef std::shared_ptr<AbstractMapSet> AbstractMapSetPtr;

// ============================================================================
// class: MapSet
// ============================================================================
template <class T>
class MapSet : public AbstractMapSet {
	public:

		MapSet(){};
		~MapSet(){};

	public:
	
		template<class K>	
		void Add(T key,std::vector<K> vec){
			AbstractSetPtr values= std::make_shared<Set<K>>(vec);	
			m_map.insert( std::make_pair(key,values) );
		}
		void Add(T key,std::vector<T> vec){
			return Add<T>(key,vec);
		}

		template<class K>	
		void Add(T key,std::initializer_list<K> init_set){
			AbstractSetPtr values= std::make_shared<Set<K>>(init_set);	
			m_map.insert( std::make_pair(key,values) );
		}
		void Add(T key,std::initializer_list<T> init_set){
			return Add<T>(key,init_set);
		}

		template<class K>
		bool HasValueInSet(T key,K value){
			//Find if map key is present
			typename std::map<T,AbstractSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!= m_map.end());
			if(!hasKey) return false;
			
			//Search for value
			auto itemSet= std::static_pointer_cast<Set<K>>(it->second);
			return itemSet->HasValueInSet(value);			
		}//close HasValueInSet()

		template<class K>
		bool HasAllowedValueInSet(T key,K value){
			//Find if map key is present, if no rule is present return allowed
			typename std::map<T,AbstractSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!= m_map.end());
			if(!hasKey) return true;
			
			//Search for value
			auto itemSet= std::static_pointer_cast<Set<K>>(it->second);
			return itemSet->HasValueInSet(value);			
		}//close HasAllowedValueInSet()

		template<class K>
		int GetSetValues(std::vector<K>& items,T key) {
			typename std::map<T,AbstractSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!= m_map.end());
			if(!hasKey) return -1;

			//typedef typename std::shared_ptr<Set<K>> SetPtr;
			auto thisSet= std::static_pointer_cast<Set<K>>(it->second);
			if(!thisSet) return -1;
			items= thisSet->GetValues();
			return 0;
		}

		template<class K>
		int GetSetValue(const K& item,T key,K seachedValue){
			typename std::map<T,AbstractSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!= m_map.end());
			if(!hasKey) return -1;

			auto thisSet= std::static_pointer_cast<Set<K>>(it->second);
			if(!thisSet) return -1;
			return thisSet->GetValueInSet(item,seachedValue);
		}
	

		virtual void Dump(){
			typedef typename std::map<T,AbstractSetPtr>::iterator mapIter;
			for(mapIter it=m_map.begin();it!=m_map.end();it++){
				std::string s= (it->second)->GetPrintable();
				cout<<"key="<<it->first<<" "<<s<<endl;			
			}
		}//close Dump()

		virtual std::string GetPrintable(){
			typedef typename std::map<T,AbstractSetPtr>::iterator mapIter;
			std::stringstream ss;
			for(mapIter it=m_map.begin();it!=m_map.end();it++){
				std::string s= (it->second)->GetPrintable();
				ss<<"key="<<it->first<<" "<<s<<endl;
			}
			return ss.str();
		}//close GetPrintable()

		virtual void GetKeys(std::vector<T>& key_list) {
			key_list.clear();
			typedef typename std::map<T,AbstractSetPtr>::iterator mapIter;
			for(mapIter it=m_map.begin();it!=m_map.end();it++) key_list.push_back(it->first);
		}

		virtual const std::map<T,AbstractSetPtr>& GetMap() const {
			return m_map;			
		}

	protected:		
		std::map<T,AbstractSetPtr> m_map;

};//close MapSet class

// ============================================================================
// class: StringMapSet
// ============================================================================
class StringMapSet : public MapSet<std::string> {
	public:
		StringMapSet() : MapSet<std::string>(){};
		~StringMapSet(){};
	protected:
				
};
typedef std::shared_ptr<StringMapSet> StringMapSetPtr;


// ============================================================================
// class: BiMapSet
// ============================================================================
template <class T>
class BiMapSet {
	public:
		
		BiMapSet(){};
		~BiMapSet(){};

	public:


		template<class K>
		int Get2ndKeyList(std::vector<K>& items,T key){
			//Find if map key is present
			typename std::map<T,AbstractMapSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!= m_map.end());
			if(!hasKey) return -1;

			//Return list of all second keys in vector
			auto mapItem= std::static_pointer_cast<MapSet<K>>(it->second);
			mapItem->GetKeys(items);
			return 0;
		}

		template<class K,class S>
		int GetSetValues(const std::vector<S>& items,T key,K key2){
			//Find if map key is present
			typename std::map<T,AbstractMapSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!= m_map.end());
			if(!hasKey) return -1;
			
			//Find if second key is present
			auto mapItem= std::static_pointer_cast<MapSet<K>>(it->second);
			return mapItem->template GetSetValues<S>(items,key2);
		}

		template<class K,class S>	
		void Add(T key,K key2,std::vector<S> vec){
			//Find if key is present
			typename std::map<T,AbstractMapSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!=m_map.end());
			if(hasKey){
				auto item= std::static_pointer_cast<MapSet<K>>(it->second);
				//ListMapPtr item= it->second;
				item->template Add<S>(key2,vec);
			}
			else{
				auto item= std::make_shared<MapSet<K>>();
				item->template Add<S>(key2,vec);
				m_map.insert( std::make_pair(key,item) );
			}
		}
		template<class K>
		void Add(T key,K key2,std::vector<K> vec){
			return Add<K,K>(key,key2,vec);
		}

		template<class K,class S>	
		void Add(T key,K key2,std::initializer_list<S> init_set){
			std::vector<S> vec= init_set;
			return Add<K,S>(key,key2,vec);	
		}
		template<class K>
		void Add(T key,K key2,std::initializer_list<K> init_set){
			return Add<K,K>(key,key2,init_set);
		}
	
		template<class K,class S>
		bool HasValueInSet(T key,K key2,S value){
			//Find if map key is present
			typename std::map<T,AbstractMapSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!= m_map.end());
			if(!hasKey) return false;
			
			//Find if second key is present
			auto mapItem= std::static_pointer_cast<MapSet<K>>(it->second);
			return mapItem->template HasValueInSet<S>(key2,value);
		}//close HasValueInSet()

		template<class K,class S>
		bool HasAllowedValueInSet(T key,K key2,S value){
			//Find if map key is present. If not present return allowed value
			typename std::map<T,AbstractMapSetPtr>::iterator it= m_map.find(key);
			bool hasKey= (!m_map.empty() && it!= m_map.end());
			if(!hasKey) return true;
			
			//Find if second key is present
			auto mapItem= std::static_pointer_cast<MapSet<K>>(it->second);
			return mapItem->template HasAllowedValueInSet<S>(key2,value);
		}//close HasAllowedValueInSet()

		void Dump(){
			typedef typename std::map<T,AbstractMapSetPtr>::iterator mapIter;
			for(mapIter it=m_map.begin();it!=m_map.end();it++){
				std::string s= (it->second)->GetPrintable();
				cout<<"key1="<<it->first<<" ["<<s<<"]"<<endl;	
			}
		}

	protected:		
		std::map<T,AbstractMapSetPtr> m_map;
};

// ============================================================================
// class: StringBiMapSet
// ============================================================================
class StringBiMapSet : public BiMapSet<std::string> {
	public:
		StringBiMapSet() : BiMapSet<std::string>(){};
		~StringBiMapSet(){};

	protected:				
};
typedef std::shared_ptr<StringBiMapSet> StringBiMapSetPtr;






}//close namespace


#endif 
