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
* @file TrackingThread.cc
* @class TrackingThread
* @brief TrackingThread class
*
* TrackingThread class
* @author S. Riggi
* @date 19/01/2018
*/


#include <TrackingThread.h>

#ifndef _POINTING_SCHEDULE_H_
	#include "PointingSchedule.h"
#endif

#ifndef _LOGGER_h
	#include <Logger.h>
#endif

#ifndef _TANGO_H
	#include <tango.h>
#endif

//## Standard headers
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctime>
#include <stdexcept>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

#include <map>
#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <chrono>

using namespace std;

namespace DSEmulator_ns {

// ============================================================================
// class: TrackingThread
// ============================================================================

TrackingThread::TrackingThread(DSEmulator* dev,PointingSchedule* pntSchedule) 
	: m_dev(dev), m_pntSchedule(pntSchedule), m_thread() 
{		
	m_stopThread= false;
	m_trackingEnabled= false;
	std::random_device rd;
	m_randgen= std::mt19937(rd());

}//close constructor
    
TrackingThread::~TrackingThread()
{	
	__DEBUG_LOG(m_dev,"Deleting tracking thread...");

	//{
  //	std::lock_guard<std::mutex> lock( mutex );
    	m_stopThread = true;
  //}
  if(m_thread.joinable()) m_thread.join();
	__DEBUG_LOG(m_dev,"Tracking thread destroyed!");
	
}//close destructor


void TrackingThread::Run()
{
	__INFO_LOG(m_dev,"Starting tracking thread...");
	 
	//Starting infinite loop to process pointing data queue
	while(!m_stopThread){
		
		//## Wait on tracking enabled status + tracking queue not empty + stopThread flag
		__INFO_LOG(m_dev,"Waiting until tracking enabled and pnt coords available or if signalled...");
		{
			std::unique_lock<std::mutex> mlock(m_mutex);
			m_cond.wait(mlock,[this]{return (m_stopThread || (m_pntSchedule->HasEntriesInQueue() && m_trackingEnabled));});
		}

		//Check exit condition
    __INFO_LOG(m_dev,"Tracking thread exiting from wait state (stop?"<<m_stopThread<<", trackingEnabled? "<<m_trackingEnabled<<", hasItemsInQueue?"<<m_pntSchedule->HasEntriesInQueue()<<") ...");
		if(m_stopThread){
			__INFO_LOG(m_dev,"Tracking thread signalled to exit from infinite loop...");
			break;
		}
		if(!m_trackingEnabled){
			__INFO_LOG(m_dev,"Tracking is disabled...");
			continue;
		}

		//## Pop top item from queue.
		//## NB: Returns 0 if activation time reached, 1=if not reached, -1=if queue empty or data expired	
		PointingData pdata;
		if(PopTrackCoordsFromQueue(pdata)<0){
			continue;
		}
		__INFO_LOG(m_dev,"Pointing data (Az,El,t)=("<<pdata.az<<","<<pdata.el<<","<<pdata.t<<", type="<<pdata.pntType<<") popped from queue...");

		//## Slew if needed (if current pos is far from requested poiting, e.g. outside pointing reso)
		if(Slew(pdata,m_dev->slewRate,m_dev->slewTimeStep,m_dev->pointingResolution)<0){
			__WARN_LOG(m_dev,"Failed to perform slewing (or signalled to exit)...");
			continue;
		}

		//## Set tracking coordinates 
		bool smearCoords= m_dev->smearPointingCoords;
		if(pdata.pntType==PointingData::eSTOW) smearCoords= false; 
		if(SetTrackCoords(pdata,smearCoords,m_dev->pointingResolution)<0){
			__WARN_LOG(m_dev,"Failed to set track coordinates (or signalled to exit)...");
			continue;
		}

	}//end infinite loop

	  
	__INFO_LOG(m_dev,"End tracking thread run");

}//close Run()


int TrackingThread::PushTrackCoordsToQueue(PointingDataPtr data)
{
	//Check data
	if(!data){
		__ERROR_LOG(m_dev,"Null ptr to given pointing data!");
		return -1;
	}

	//Add data to queue	
	if(m_pntSchedule->Push(data)<0){
		__ERROR_LOG(m_dev,"Failed to add pointing data to queue!");
		return -1;
	}	

	//Notify waiting thread
	m_cond.notify_all();

	return 0;

}//close PushTrackCoordsToQueue()

int TrackingThread::PopTrackCoordsFromQueue(PointingData& data)
{
	//## Check if tracking enabled
	if(!m_trackingEnabled){
		__INFO_LOG(m_dev,"Tracking is not enabled in antenna, will not pop coords from queue!");
		return -1;
	}

	//## Pop top item from queue.
	//## NB: Returns 0 if activation time reached, 1=if not reached, -1=if queue empty or data expired	
	double tdiff_ms= 0;
	int status= m_pntSchedule->Pop(data,tdiff_ms);
	if(status==-1) {
		__INFO_LOG(m_dev,"Failed to pop data from pointing queue (empty queue or expired data");
		return -1;
	}
	else if(status==1){
		__INFO_LOG(m_dev,"Pointing activation time not reached yet, will wait for t(ms)="<<fabs(tdiff_ms)<<" ...");
		auto wait_time= std::chrono::duration<double,std::milli>( fabs(tdiff_ms) );
		{
			std::unique_lock<std::mutex> mlock(m_mutex);
			if(m_cond.wait_for(mlock,wait_time,[this]{return m_stopThread || !m_trackingEnabled;}) ){
				if(m_stopThread) __INFO_LOG(m_dev,"Tracking thread signalled to exit ...");
				else if(m_trackingEnabled) __INFO_LOG(m_dev,"Tracking thread was disabled ...");
			}
			else{
				__INFO_LOG(m_dev,"Pointing data wait timed out...");
			}
		}

		return -1;
		
	}//close if status==1
	
	return 0;

}//close PopTrackCoordsFromQueue()


int TrackingThread::SetTrackCoords(PointingData& pdata,bool smearCoords,double pointingReso)
{
	//###########################################################################
	//##    HARD-CODED PARAMETERS 
	//##    (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string operatingMode_attr_name= "operatingMode";
	std::string operatingModeStr_stow= "STOW";
	std::string powerState_attr_name= "powerState";
	std::string powerStateStr_full= "FULL-POWER";
	std::string powerStateStr_low= "LOW-POWER";
	std::string pointState_attr_name= "pointingState";
	std::string pointStateStr_track= "TRACK";
	std::string pointStateStr_ready= "READY";
	std::string pointStateStr_none= "NONE";
	std::string azPos_attr_name= "antennaAzPosition";
	std::string elPos_attr_name= "antennaElPosition";
	std::string targetLock_attr_name= "targetLock";
	std::vector<std::string> brakeState_attr_names {
		"indexerBrakeState",
		"azimuthBrakeState",
		"elevationBrakeState"
	};
	std::vector<std::string> servoState_attr_names {
		"indexerServoState",
		"azimuthServoState",
		"elevationServoState"
	};
	//============================================================================

	//Get pointing type    
	int pntType= pdata.pntType;

	//Smear pointing data with pointing resolution?
	if(smearCoords)	{
		SmearPointing(pdata,pointingReso);
	}

	//Set pointing state to TRACK (if this is not a stowing pnt) and targetLock attribute to true
	if(pntType!=PointingData::eSTOW){
		__INFO_LOG(m_dev,"Setting pointing state to TRACK ...");
		Tango::DevShort pointState_track;
		Utils_ns::TangoUtils::GetEnumAttrValue(pointState_track,m_dev,pointState_attr_name,pointStateStr_track);
		if( m_dev->SetDynAttrValue<Tango::DevEnum>(pointState_track,pointState_attr_name)<0 ){
			__ERROR_LOG(m_dev,"Failed to set attr "<<pointState_attr_name<<" to "<<pointState_track<<"!");
			return -1;
		}

		__INFO_LOG(m_dev,"Setting targetLock to true ...");
		Tango::DevBoolean targetLock= true;
		if( m_dev->SetDynAttrValue<Tango::DevBoolean>(targetLock,targetLock_attr_name)<0 ){
			__ERROR_LOG(m_dev,"Failed to set attr "<<targetLock_attr_name<<" to "<<targetLock<<"!");
			return -1;
		}
	}

	//Set pointing data to achieved coordinated attrs
	yat::Time now;
	now.set_current(m_dev->useUTCTime);
	double t= now.double_unix()*1000.;//convert in ms

	if( m_dev->SetDynAttrValue<Tango::DevDouble>(pdata.az,azPos_attr_name)<0 ){
		__ERROR_LOG(m_dev,"Failed to set attr "<<azPos_attr_name<<" to "<<pdata.az<<"!");
		return -1;
	}	
	if( m_dev->SetDynAttrValue<Tango::DevDouble>(pdata.el,elPos_attr_name)<0 ){
		__ERROR_LOG(m_dev,"Failed to set attr "<<elPos_attr_name<<" to "<<pdata.el<<"!");
		return -1;
	}	

	(m_dev->attr_achievedPointing_read)[0]= t;
	(m_dev->attr_achievedPointing_read)[1]= pdata.az;
	(m_dev->attr_achievedPointing_read)[2]= pdata.el;

	//If pointing queue is empty set point state to READY
	if(m_pntSchedule->IsQueueEmpty()){
		__INFO_LOG(m_dev,"Setting pointing state to READY ...");
		Tango::DevShort pointState_ready;
		Utils_ns::TangoUtils::GetEnumAttrValue(pointState_ready,m_dev,pointState_attr_name,pointStateStr_ready);
		if( m_dev->SetDynAttrValue<Tango::DevEnum>(pointState_ready,pointState_attr_name)<0 ){
			__ERROR_LOG(m_dev,"Failed to set attr "<<pointState_attr_name<<" to "<<pointState_ready<<"!");
			return -1;
		}
	}

	//## If stowing coordinates set:
	//1) Disable tracking
	//2) Apply brakes
	//3) Disable servo axis 
	//4) Set powerState to LOW-POWER
	//5) Set pointingState to NONE
	//6) Set mode to STOW
	//7) Set task status to COMPLETED
	//8) Set device Tango state to DISABLE
	if(pntType==PointingData::eSTOW){

		//## 1) Disable tracking & set target lock to false
		__INFO_LOG(m_dev,"Disabling tracking");
		DisableTracking();

		__INFO_LOG(m_dev,"Setting targetLock to false ...");
		Tango::DevBoolean targetLock= false;
		if( m_dev->SetDynAttrValue<Tango::DevBoolean>(targetLock,targetLock_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<targetLock_attr_name<<" to "<<targetLock<<"!";
			__ERROR_LOG(m_dev,ss.str());
			
			{//- enter critical section 	
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("FAILED",ss.str(),true);
			}//- leave critical section

			return -1;
		}

		//## 2) Apply brakes
		Tango::DevBoolean brakeState= true;
		for(size_t i=0;i<brakeState_attr_names.size();i++){
		__DEBUG_LOG(m_dev,"Setting attr ("<<brakeState_attr_names[i]<<") to "<< brakeState << "...");
			if( m_dev->SetDynAttrValue<Tango::DevBoolean>(brakeState,brakeState_attr_names[i])<0 ){
				std::stringstream ss;
				ss<<"Failed to set attr "<<brakeState_attr_names[i]<<" value!";
				__ERROR_LOG(m_dev,ss.str());

				{//- enter critical section 	
					omni_mutex_lock guard(m_dev->m_mutex);
					m_dev->SetStowStatus("FAILED",ss.str(),true);
				}//- leave critical section

				return -1;
			}//close if
		}//end loop attrs

		//## 3) Disable axis
		Tango::DevBoolean servoState= false;
 		for(size_t i=0;i<servoState_attr_names.size();i++){
			__DEBUG_LOG(m_dev,"Setting attr ("<<servoState_attr_names[i]<<") to "<< servoState << "...");
			if( m_dev->SetDynAttrValue<Tango::DevBoolean>(servoState,servoState_attr_names[i])<0 ){
				std::stringstream ss;
				ss<<"Failed to set attr "<<servoState_attr_names[i]<<" value!";
				__ERROR_LOG(m_dev,ss.str());

				{//- enter critical section 	
					omni_mutex_lock guard(m_dev->m_mutex);
					m_dev->SetStowStatus("FAILED",ss.str(),true);
				}//- leave critical section

				return -1;
			}
		}//end loop attrs

		
		//## 4) Set power state to LOW-POWER (only if previously set to FULL-POWER, do not override UPS!)

		//Retrieve current powerState attr value
		Tango::DevShort powerState_current;
		if( m_dev->GetDynAttrValue(powerState_current,powerState_attr_name)<0){
			std::stringstream ss;
			ss<<"Failed to get attr "<<powerState_attr_name<<" value!";
			__ERROR_LOG(m_dev,ss.str());

			{//- enter critical section 	
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("FAILED",ss.str(),true);
			}//- leave critical section
 
			return -1;
		}//close if

		std::string powerStateStr_current= "";
		Utils_ns::TangoUtils::GetEnumAttrLabelValue(powerStateStr_current,m_dev,powerState_attr_name,powerState_current);

		//If powerState=FULL-POWER set DS power state to LOW-POWER	
		if(powerStateStr_current==powerStateStr_full){
			__INFO_LOG(m_dev,"Setting power mode to LOW-POWER ..."); 
			
			Tango::DevShort powerState_low;
			Utils_ns::TangoUtils::GetEnumAttrValue(powerState_low,m_dev,powerState_attr_name,powerStateStr_low);			
			if( m_dev->SetSMDynAttrValue<Tango::DevEnum>(powerState_low,powerState_attr_name)<0 ){
				std::stringstream ss;
				ss<<"Failed to set attr "<<powerState_attr_name<<" value!";
				__ERROR_LOG(m_dev,ss.str());

				{//- enter critical section 
					omni_mutex_lock guard(m_dev->m_mutex);
					m_dev->SetStowStatus("FAILED",ss.str(),true);
				}//- leave critical section

				return -1;
			}//close if
		}//close if


		//## 5) Set pointingState to NONE	
		__INFO_LOG(m_dev,"Setting pointingState to NONE ...");
		Tango::DevShort pointState_stow;
		Utils_ns::TangoUtils::GetEnumAttrValue(pointState_stow,m_dev,pointState_attr_name,pointStateStr_none);
		__DEBUG_LOG(m_dev,"Setting pointing attr ("<<pointState_attr_name<<") to "<< pointStateStr_none << "...");
		if( m_dev->SetSMDynAttrValue<Tango::DevEnum>(pointState_stow,pointState_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<pointState_attr_name<<" to "<<pointStateStr_none<<"!";
			__ERROR_LOG(m_dev,ss.str());

			{//- enter critical section
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("FAILED","Stow command failed in execution (cannot set pointingState to NONE)",true);
			}//- leave critical section
 
			return -1;
		}//close set mode

		//## 6) Set mode to STOW	
		__INFO_LOG(m_dev,"Setting mode to STOW ...");
		Tango::DevShort operatingMode_stow;
		Utils_ns::TangoUtils::GetEnumAttrValue(operatingMode_stow,m_dev,operatingMode_attr_name,operatingModeStr_stow);
		__DEBUG_LOG(m_dev,"Setting mode attr ("<<operatingMode_attr_name<<") to "<< operatingModeStr_stow << "...");
		if( m_dev->SetSMDynAttrValue<Tango::DevEnum>(operatingMode_stow,operatingMode_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<operatingMode_attr_name<<" to "<<operatingModeStr_stow<<"!";
			__ERROR_LOG(m_dev,ss.str());
			{//- enter critical section
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("FAILED","Stow command failed in execution (cannot set mode to STOW)",true);
			}//- leave critical section
 
			return -1;
		}//close set mode

		//## 7) - Set device state to DISABLE and set status to COMPLETED
		{ //- enter critical section
			omni_mutex_lock guard(m_dev->m_mutex);
			std::string msg("Stowing completed with success");
			__INFO_LOG(m_dev,msg);
			m_dev->set_state(Tango::DISABLE);
  		m_dev->set_status(msg.c_str());	
			m_dev->SetStowStatus("COMPLETED",msg,true);
		} //- leave critical section 
	
	}//close if eSTOW
	
	return 0;

}//close SetTrackCoords()


int TrackingThread::Slew(PointingData& pdata,double slewRate,double slewTimeStep,double pointingReso)
{
	//###########################################################################
	//##    HARD-CODED PARAMETERS 
	//##    (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string pointState_attr_name= "pointingState";
	std::string pointStateStr= "SLEW"; 
	std::string azPos_attr_name= "antennaAzPosition";
	std::string elPos_attr_name= "antennaElPosition";
	std::string targetLock_attr_name= "targetLock";
	//===============================================

	//## Check if tracking enabled
	if(!m_trackingEnabled){
		__INFO_LOG(m_dev,"Tracking is not enabled in antenna, will not perform slewing!");
		return -1;
	}

	//Get stop azimuth & elevation
	double stopAz= pdata.az;
	double stopEl= pdata.el;
	int pntType= pdata.pntType;

	//If this is a slewing to stow position set stow task status to RUNNING
	if(pntType==PointingData::eSTOW){
		__INFO_LOG(m_dev,"Stowing started");
		{ 	
			omni_mutex_lock guard(m_dev->m_mutex);
			m_dev->SetStowStatus("RUNNING","Stowing started",true);
		}
	}

	//Get current azimuth & elevation
	double startAz= 0;
	double startEl= 0;
	if( m_dev->GetDynAttrValue(startAz,azPos_attr_name)<0){
		std::string errMsg("Failed to retrieve current Azimuth attr!");
		__ERROR_LOG(m_dev,errMsg);
		if(pntType==PointingData::eSTOW){
			{ 	
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("FAILED",errMsg,true);
			}
		}
		return -1;
	}
	if( m_dev->GetDynAttrValue(startEl,elPos_attr_name)<0){	
		std::string errMsg("Failed to retrieve current Elevation attr");
		__ERROR_LOG(m_dev,errMsg);
		if(pntType==PointingData::eSTOW){
			{ 	
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("FAILED",errMsg,true);
			}
		}
		return -1;
	}

	//Check if distance between desired and current pos is within pointing accuracy
	double dAz= stopAz-startAz;
	double dEl= stopEl-startEl;
	double reso= pointingReso/3600.;//convert in degrees
	bool azReached= (fabs(dAz)<=reso);
	bool elReached= (fabs(dEl)<=reso);
	if(azReached && elReached){//desired position already reached (slewing not needed)
		__INFO_LOG(m_dev,"Desired position ("<<stopAz<<","<<stopEl<<") already reached, no need to slew...");
		return 0;
	}
	
	//Init slewing pars
	double slewingTime= std::max(fabs(dAz),fabs(dEl))/slewRate;//in seconds
	auto slewWaitTime= std::chrono::duration<double,std::milli>(slewTimeStep);
	double slewingTimeStep= slewTimeStep/1000.;//convert in seconds
	double azPosStep= slewRate*slewingTimeStep;//in degrees
	double elPosStep= slewRate*slewingTimeStep;//in degrees
	if(dAz<0) azPosStep*= -1;	 
	if(dEl<0) elPosStep*= -1;
	double currentAz= startAz;
	double currentEl= startEl;
	bool stoppedByUser= false;
	__INFO_LOG(m_dev,"Slewing for ~"<<slewingTime<<" seconds (slewTimeStep(ms)="<<slewTimeStep<<") before reaching desired track coordinates...");
	
	
	//Set pointState to SLEW
	__INFO_LOG(m_dev,"Setting pointState to SLEW ...");
	Tango::DevShort pointState;
	Utils_ns::TangoUtils::GetEnumAttrValue(pointState,m_dev,pointState_attr_name,pointStateStr);
	if( m_dev->SetDynAttrValue<Tango::DevEnum>(pointState,pointState_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<pointState_attr_name<<" to "<<pointState<<"!";
		__ERROR_LOG(m_dev,ss.str());
		if(pntType==PointingData::eSTOW){
			{ 	
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("FAILED",ss.str(),true);
			}
		}
		return -1;
	}//close if set point state

	//Set target lock to false
	__INFO_LOG(m_dev,"Setting targetLock to false ...");
	Tango::DevBoolean targetLock= false;
	if( m_dev->SetDynAttrValue<Tango::DevBoolean>(targetLock,targetLock_attr_name)<0 ){
		std::stringstream ss;
		ss<<"Failed to set attr "<<targetLock_attr_name<<" to "<<targetLock<<"!";
		__ERROR_LOG(m_dev,ss.str());
		if(pntType==PointingData::eSTOW){
			{ 	
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("FAILED",ss.str(),true);
			}
		}
		return -1;
	}

	//## Start slewing	
	while ( !azReached || !elReached ){
		
		//If Az not reached do the step
		if(!azReached){
			currentAz+= azPosStep;
			if( (azPosStep>0 && (currentAz-stopAz)>=0) || (azPosStep<0 && (currentAz-stopAz)<=0) ) {
				currentAz= stopAz;
				azReached= true;
			}
		}		

		//If El not reached do the step
		if(!elReached){
			currentEl+= elPosStep;
			if( (elPosStep>0 && (currentEl-stopEl)>=0) || (elPosStep<0 && (currentEl-stopEl)<=0) ) {
				currentEl= stopEl;
				elReached= true;
			}
		}	

		//Set attributes
		yat::Time now;
		now.set_current(m_dev->useUTCTime);
		double t= now.double_unix()*1000.;//convert in ms
		__INFO_LOG(m_dev,"Slew step (t,Az,El)=("<<static_cast<long long int>(t)<<","<<currentAz<<","<<currentEl<<")");
		
		if( m_dev->SetDynAttrValue<Tango::DevDouble>(currentAz,azPos_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<azPos_attr_name<<" to "<<currentAz<<"!";
			__ERROR_LOG(m_dev,ss.str());
			if(pntType==PointingData::eSTOW){
				{ 	
					omni_mutex_lock guard(m_dev->m_mutex);
					m_dev->SetStowStatus("FAILED",ss.str(),true);
				}
			}
			return -1;
		}	
		if( m_dev->SetDynAttrValue<Tango::DevDouble>(currentEl,elPos_attr_name)<0 ){
			std::stringstream ss;
			ss<<"Failed to set attr "<<elPos_attr_name<<" to "<<currentEl<<"!";
			__ERROR_LOG(m_dev,ss.str());
			if(pntType==PointingData::eSTOW){
				{ 	
					omni_mutex_lock guard(m_dev->m_mutex);
					m_dev->SetStowStatus("FAILED",ss.str(),true);
				}
			}
			return -1;
		}	
		(m_dev->attr_achievedPointing_read)[0]= t;
		(m_dev->attr_achievedPointing_read)[1]= currentAz;
		(m_dev->attr_achievedPointing_read)[2]= currentEl;	

		//Sleep a time step (if not signalled to exit)
		{//- critical region start
			std::unique_lock<std::mutex> mlock(m_mutex);
			m_cond.wait_for(mlock,std::chrono::milliseconds(static_cast<long>(slewTimeStep)),[this]{return (m_stopThread || !m_trackingEnabled);});
		}//- critical region end

		__DEBUG_LOG(m_dev,"Slew wait ended with m_stopThread="<<m_stopThread);
		if(m_stopThread || !m_trackingEnabled){
			__INFO_LOG(m_dev,"Slewing signalled to stop ...");
			stoppedByUser= true;
			break;
		}

		__DEBUG_LOG(m_dev,"Go to next slew step...");

	}//end slew thread

	//Check if stopped by user
	if(stoppedByUser) {
		__INFO_LOG(m_dev,"Slewing stopped by user...");
		if(pntType==PointingData::eSTOW){
			{ 	
				omni_mutex_lock guard(m_dev->m_mutex);
				m_dev->SetStowStatus("CANCELED","Stow failed as slewing was canceled by user",true);
			}
		}
		return -1;
	}

	return 0;

}//close Slew()

void TrackingThread::SmearPointing(PointingData& pdata,double reso)
{
	// Generate 2 gaussian numbers
  std::normal_distribution<double> gen(0,reso/3600.);//convert in deg 
	double rand_az = gen(m_randgen); 
	double rand_el = gen(m_randgen); 

	//Smear pointing
	pdata.az+= rand_az;
	pdata.el+= rand_el;
	
}//close SmearPointing()

}//close namespace 


