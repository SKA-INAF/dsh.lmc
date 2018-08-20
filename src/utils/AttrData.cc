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
* @file AttrData.cc
* @class AttrData
* @brief AttrData container
*
* AttrData container 
* @author S. Riggi
* @date 15/01/2016
*/

#include <AttrData.h>


namespace Utils_ns {

// ============================================================================
// class: AttrData
// ============================================================================

void AttrData::set_timestamp(Tango::TimeVal& value){
	yat::MutexLock guard(m_lock);
	m_timestamp= value;
}

void AttrData::set_timestamp(struct timeval &t) {
	yat::MutexLock guard(m_lock);
	m_timestamp.tv_sec= t.tv_sec;
	m_timestamp.tv_usec= t.tv_usec;
	m_timestamp.tv_nsec= 0;
}

void AttrData::set_invalid_timestamp() {
	m_timestamp.tv_sec= -1;
	m_timestamp.tv_usec= -1;
	m_timestamp.tv_nsec= -1;
}

void AttrData::set_timestamp(time_t &t) {
	yat::MutexLock guard(m_lock);
	m_timestamp.tv_sec= (long)t;
	m_timestamp.tv_usec= 0;
	m_timestamp.tv_nsec= 0;
}

void AttrData::set_timestamp(std::chrono::system_clock::time_point& t) {
	yat::MutexLock guard(m_lock);
	auto millisecs= std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch());
	m_timestamp.tv_sec= millisecs.count()/1000.;
	m_timestamp.tv_usec= (millisecs.count()%1000)*1000.;
	m_timestamp.tv_nsec= 0;
}	

void AttrData::set_data_quality(Tango::AttrQuality& value){
	yat::MutexLock guard(m_lock);
	m_data_quality= value;
}

void AttrData::set_timestamp_to_now(){		
	struct timezone tz;
	struct timeval tv;
	gettimeofday(&tv,&tz);

	m_timestamp.tv_sec = (CORBA::Long)tv.tv_sec;
	m_timestamp.tv_usec = (CORBA::Long)tv.tv_usec;
	m_timestamp.tv_nsec = 0;
}//close set_timestamp_to_now()
	

// ============================================================================
// class: ScalarAttrData
// ============================================================================


}//close namespace Utils_ns

