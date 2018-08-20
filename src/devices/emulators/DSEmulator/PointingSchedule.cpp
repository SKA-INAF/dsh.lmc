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
* @file PointingSchedule.cc
* @class PointingSchedule
* @brief PointingSchedule
*
* PointingSchedule
* @author S. Riggi
* @date 19/01/2018
*/


#include "PointingSchedule.h"

#include <tango.h>

#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

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

#include <map>


namespace DSEmulator_ns {

//bool PointingSchedule::m_wakeUpSignal;
//bool PointingSchedule::m_endSignal;	

PointingSchedule::PointingSchedule(Tango::DeviceImpl* dev)
	: m_dev(dev)
{

	//m_wakeUpSignal= false;
	//m_endSignal= false;
	m_maxQueueSize= 100;
	m_expiredTimeThr= 100;//ms
	m_useUTC= true;

}//close constructor

PointingSchedule::~PointingSchedule()
{

}//close destructor

int PointingSchedule::Pop(PointingData& popped_data,double& tdiff_ms)
{
	//## Check if queue is empty
	if(m_pointingQueue.empty()) return -1;

	//## Get first item in queue 
	auto it= m_pointingQueue.begin();
	double t_epoch= (*it)->t;
	double tdiff= -1;
	tdiff_ms= 0;
	int status= -1;
	if(t_epoch>=0){//for negative timestamp (e.g. STOW task) the activation is forced
		yat::Time t_now;
		t_now.set_current(m_useUTC);
		tdiff= Utils_ns::TangoUtils::GetYatTimeDiffUSec(t_now, (*it)->timestamp);	
		tdiff_ms= tdiff/1000.; 		
	}
	
	//## Check if activation is reached
	if(tdiff_ms<=0){//activation reached

		if(fabs(tdiff_ms)<m_expiredTimeThr){//data is not expired and can be popped
			popped_data= *(*it);
			popped_data.dt= -tdiff_ms;
			__INFO_LOG(m_dev,"Pointing data (az="<<popped_data.az<<", el="<<popped_data.el<<", t="<<popped_data.t<<", tdiff(ms)="<<tdiff_ms<<") popped from the queue...");
		
			m_pointingQueue.erase(it);
			status= 0;
		}
		else{//Data expired so remove them from queue and return invalid flag at the end
			__WARN_LOG(m_dev,"Pointing data at queue top expired (tdiff(ms)="<<tdiff_ms<<"), will not execute this pointing!");
			m_pointingQueue.erase(it);	
			status= -1;
		}
	}//close if
	else{
		__INFO_LOG(m_dev,"Pointing data activation not reached yet (dt(ms)="<<tdiff_ms<<")");
		status= 1;
	}

	return status;

}//close Pop()

int PointingSchedule::Push(PointingDataPtr data)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	//Check given data
	if(!data){
		__DEBUG_LOG(m_dev,"Null ptr to given task sequence, cannot add it!");
		return -1;
	}
		
	//Check if queue quota will not be exceeded
	bool isQuotaReached= is_queue_full();
	int pntType= data->pntType;
	if(isQuotaReached && pntType!=PointingData::eSTOW){
		__WARN_LOG(m_dev,"Pointing data queue is full, cannot add data!");
		return -1;
	}

	//Add in queue
	try {	
		m_pointingQueue.insert(data);
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while adding data in queue!");
		return -1;
	}	
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while adding data in queue!");
		return -1;
	}	

	__DEBUG_LOG(m_dev,"Queue size is "<<m_pointingQueue.size());

	return 0;

}//close Push()


int PointingSchedule::Push(std::vector<PointingDataPtr>& data)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	//Check given data
	if(data.empty()){
		__DEBUG_LOG(m_dev,"Empty data list given, nothing to be added!");
		return -1;
	}
		
	//Check if queue quota would be exceeded
	int N= static_cast<int>(data.size());
	int queueSize= static_cast<int>(m_pointingQueue.size());
	bool isQuotaReached= ((N+queueSize)>=m_maxQueueSize);
	if(isQuotaReached){
		__WARN_LOG(m_dev,"Pointing data queue is full, cannot add data!");
		return -1;
	}

	//Add in queue
	try {
		for(int i=0;i<N;i++) m_pointingQueue.insert(data[i]);
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while adding data in queue!");
		return -1;
	}	
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while adding data in queue!");
		return -1;
	}	

	__DEBUG_LOG(m_dev,"Queue size is "<<m_pointingQueue.size());

	return 0;

}//close Push()

/*
int PointingSchedule::PopDataFromQueue(PointingData& popped_data) {
	
	std::unique_lock<std::mutex> mlock(m_mutex);

	//## If queue is empty wait until signalled
	__DEBUG_LOG(m_dev,"Waiting until signaled (queue size="<<m_pointingQueue.size()<<", m_wakeUpSignal? "<<m_wakeUpSignal<<", m_endSignal? "<<m_endSignal<<")...");
	m_cond.wait(mlock,[this]{return (!m_pointingQueue.empty() || m_wakeUpSignal || m_endSignal);});
	__DEBUG_LOG(m_dev,"Schedule is awake and will try to pop data from queue...");
  	
	m_wakeUpSignal = false;

	if(m_endSignal){
		__DEBUG_LOG(m_dev,"End signal catched, exiting...");
		return -1;
	}
	if(m_pointingQueue.empty()){
		__DEBUG_LOG(m_dev,"Empty pointing queue...");
		return -1;
	}
	
	//## Remove all expired data from the queue
	//if(remove_expired_data_in_queue()<0){
	//	__DEBUG_LOG(m_dev,"Failed to remove expired pointing data present in queue...");
	//	return -1;
	//}

	//## Get first pointing in queue and check if activation/expiration is reached
	auto it= m_pointingQueue.begin();
	
	yat::Time t_now;
	t_now.set_current(m_useUTC);
	double tdiff= Utils_ns::TangoUtils::GetYatTimeDiffUSec(t_now, (*it)->timestamp);	
	double tdiff_ms= tdiff/1000.; 
	int status= -1;
	__INFO_LOG(m_dev,"dt(ms)="<<tdiff_ms);
	
	if(tdiff_ms<=0){//activation reached

		if(fabs(tdiff_ms)<m_expiredTimeThr){//data is not expired and can be popped
			popped_data= *(*it);
			popped_data.dt= -tdiff_ms;
			__INFO_LOG(m_dev,"Pointing data (az="<<popped_data.az<<", el="<<popped_data.el<<", t="<<popped_data.t<<", tdiff(ms)="<<tdiff_ms<<") popped from the queue...");
		
			m_pointingQueue.erase(it);
			status= 0;
		}
		else{//Data expired so remove them from queue and return invalid flag at the end
			__WARN_LOG(m_dev,"Pointing data at queue top expired (tdiff(ms)="<<tdiff_ms<<"), will not execute this pointing!");
			m_pointingQueue.erase(it);
		}
	}//close if
	else{//sleep until activation
		auto wait_time= std::chrono::duration<double,std::milli>( fabs(tdiff_ms) );
		//auto wait_time= std::chrono::duration<double,std::micro>( fabs(tdiff) );
		__INFO_LOG(m_dev,"Waiting for "<<wait_time.count()<<" ms ...");

		if( m_cond.wait_for(mlock,wait_time, [](){return (m_wakeUpSignal || m_endSignal);}) ){
			__DEBUG_LOG(m_dev,"Finished waiting (signaled)...");
			if(m_wakeUpSignal) m_wakeUpSignal= false;
		}
		else {
			__DEBUG_LOG(m_dev,"Finished waiting (Timeout reached)...");
		}
	
		__DEBUG_LOG(m_dev,"Awake after timeout...");
	}

	return status;

}//close PopDataFromQueue()
*/


/*
int PointingSchedule::AddDataToQueue(PointingDataPtr data){

	std::lock_guard<std::mutex> lock(m_mutex);

	//Check given data
	if(!data){
		__DEBUG_LOG(m_dev,"Null ptr to given task sequence, cannot add it!");
		return -1;
	}
		
	//Check if queue quota will not be exceeded
	bool isQuotaReached= is_queue_full();
	int pntType= data->pntType;
	if(isQuotaReached && pntType!=PointingData::eSTOW){
		__WARN_LOG(m_dev,"Pointing data queue is full, cannot add data!");
		return -1;
	}

	//Add in queue
	try {	
		m_pointingQueue.insert(data);
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while adding data in queue!");
		return -1;
	}	
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while adding data in queue!");
		return -1;
	}	


	//Emit signal to wake up any the waiting thread
	__DEBUG_LOG(m_dev,"Added data to queue (emit wake up signal)...");
	m_wakeUpSignal = true;
  m_cond.notify_all();

	__DEBUG_LOG(m_dev,"Queue size is "<<m_pointingQueue.size());

	return 0;

}//close AddDataToQueue()
*/

/*
void PointingSchedule::WakeUp(){
	
	std::lock_guard<std::mutex> lock(m_mutex);
	__DEBUG_LOG(m_dev,"Wake up signal throw...");
	m_wakeUpSignal = true;
	m_cond.notify_all();
	__DEBUG_LOG(m_dev,"Wake up done!");
	
}//close WakeUp()
*/

/*
void PointingSchedule::End(){

	std::lock_guard<std::mutex> lock(m_mutex);
	__DEBUG_LOG(m_dev,"End signal throw...");
	m_endSignal = true;
  m_cond.notify_all();
	__DEBUG_LOG(m_dev,"End signal done!");
	
}//close End()
*/

// ============================================================================
//                    PRIVATE METHODS
// ============================================================================
bool PointingSchedule::is_queue_full(){

	int nQueuedData= static_cast<int>(m_pointingQueue.size());
	return (nQueuedData>=m_maxQueueSize );
	
}//close is_queue_full()


int PointingSchedule::flush_queue(){
	
	//Erase all data in queue
	try{
		m_pointingQueue.erase(m_pointingQueue.begin(),m_pointingQueue.end());
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while removing all items in pointing queue!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while removing all items in pointing queue!");
		return -1;
	}

	return 0;

}//close flush_queue()

int PointingSchedule::remove_expired_data_in_queue(){

	//Get current timestamp
	yat::Time t_now;
	t_now.set_current(m_useUTC);
	
	//Find and remove expired data
	__DEBUG_LOG(m_dev,"Removing expired pointing data in queue...");
	int nDataRemoved= 0;
	try {
		std::set<PointingDataPtr>::iterator it = m_pointingQueue.begin();
		while ((it = std::find_if(it, m_pointingQueue.end(), MatchExpiredPointingData(t_now,m_expiredTimeThr))) != m_pointingQueue.end()){
    	it = m_pointingQueue.erase(it);
			nDataRemoved++;
		}
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while removing all matching EXPIRED pointing data occurrences in queue!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while removing all matching EXPIRED pointing data occurrences in queue!");
		return -1;
	}

	__DEBUG_LOG(m_dev,"Removed "<<nDataRemoved<<" expired pointing data in queue...");

	return 0;

}//close remove_expired_data_in_queue()


}//close namespace
