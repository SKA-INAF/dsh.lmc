/**
* @file SysUtils.cc
* @class SysUtils
* @brief Utility functions for system tasks
*
* Utility functions for system tasks
* @author S. Riggi
* @date 23/08/2010
*/


#include <SysUtils.h>
#include <Logger.h>

#include <boost/filesystem.hpp>


#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <ctime>

using namespace std;

namespace Utils_ns {

SysUtils::SysUtils(){

}

SysUtils::~SysUtils(){

}

int SysUtils::CheckFile(std::string path,Utils_ns::FileInfo& info,bool match_extension,std::string extension){

	//Check input file path
	if(path==""){	
		WARN_LOG("Empty filename given!");
		return -1;
	}
	if(!(&info)){
		ERROR_LOG("Null ptr to file info struct given!");
		return false;
	}

	//Check if file actually exists on filesystem
  try {
		//Check if file exists on filesystem
		boost::filesystem::path file_path(path.c_str());
		if (!boost::filesystem::exists(file_path)){
			WARN_LOG("File "<<path<<" not found in local filesystem!");
			return -1;
		}
		if (!boost::filesystem::is_regular_file(file_path)){
			WARN_LOG("File "<<path<<" is not a regular file!");
			return -1;
		}
		if (boost::filesystem::is_directory(file_path)){
    	WARN_LOG("File ("<<file_path<<") is a directory!");
			return -1;
    }
	
		//Get filename and extension
		if(!file_path.has_filename()){
			WARN_LOG("File ("<<file_path<<") does not have a filename!");
			return -1;
		}

		//Set file info
		info.filename= file_path.filename().string();
		info.filename_wext= file_path.stem().string();
		info.size= boost::filesystem::file_size(file_path);
        	
		//Check extension
		if(!file_path.has_extension()){
			WARN_LOG("Given file without extension!");
			return -1;
		}

		std::string file_extension= file_path.extension().string();	
		info.extension= file_extension;
						
		if(match_extension && file_extension!=extension){
			ERROR_LOG("Invalid file extension detected ("<<file_extension<<"!="<<extension<<")...");
			return false;
		}
	
		//Dump file info
		//std::string info_printable= info.GetPrintable();
		//INFO_LOG(info_printable);

  }//close try block

  catch (const boost::filesystem::filesystem_error& ex) {
    ERROR_LOG("Exception detected while checking file (err: "<<ex.what()<<")!");
		return -1;
  }

	return 0;

}//close CheckFile()


}//close namespace



