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
* @file DishManager.tpp
* @class DishManager
* @brief DishManager 
*
* DishManager
* @author S. Riggi
* @date 15/01/2017
*/

template<typename T>
inline void DishManager::WriteSEAttrs(std::string se_proxy_name,std::vector<std::string> attr_names,T& attr_value){

	//=====================================================
	//==      EXECUTE COMMAND
	//=====================================================
	//Check if proxy is registered in list
	std::string full_proxy_name= se_proxy_name;
	if( !m_proxyManager->FindProxy(full_proxy_name) ){
		_WARN_LOG("Proxy "<<full_proxy_name<<" not found in registered proxy list, trying with alias name...");
		
		//Get full proxy name corresponding to alias
		if(Utils_ns::TangoUtils::GetFQDNDeviceNameFromAlias(full_proxy_name,se_proxy_name)<0){
			std::stringstream ss;
			ss<<"Failed to retrieve full proxy name from alias "<<se_proxy_name<<"!";
			_WARN_LOG(ss.str());
			_THROW_TANGO_EXCEPTION("WRITE_SE_ATTRS_FAILED",ss.str());	
		}
	}

	//Prepare argin	
	std::vector<Tango::DeviceAttribute> da_list;
	for(unsigned int i=0;i<attr_names.size();i++){
		da_list.push_back(Tango::DeviceAttribute(attr_names[i], attr_value));
	}
	
	//Execute write command
	try { 
		m_proxyManager->WriteAttributes(full_proxy_name,da_list);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		std::stringstream ss;
		ss<<"Tango exception occurred while writing given attributes on proxy "<<full_proxy_name<<" (check proxy registered/syntax/proxy online)!";
		_WARN_LOG(ss.str());
		throw;
	}
	catch(std::exception& e){
		std::stringstream ss;
		ss<<"C++ exception occurred while writing given attributes on proxy "<<full_proxy_name<<" (err="<<e.what()<<")!";
		_WARN_LOG(ss.str());
		throw;
	}
	catch(...){
		std::stringstream ss;
		ss<<"Unknown exception occurred while writing given attributes on proxy "<<full_proxy_name<<"!";
		_WARN_LOG(ss.str());
		throw;
	}

}//close WriteSEAttrs()

