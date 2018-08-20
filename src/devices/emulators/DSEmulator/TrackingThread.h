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
* @file TrackingThread.h
* @class TrackingThread
* @brief TrackingThread class
*
* TrackingThread class
* @author S. Riggi
* @date 19/01/2018
*/

#ifndef _TRACKING_THREAD_H
#define _TRACKING_THREAD_H

#ifndef DSEmulator_H
	#include "DSEmulator.h"
#endif

#ifndef _POINTING_SCHEDULE_H_
	#include "PointingSchedule.h"
#endif

#ifndef _TANGO_H
	#include <tango.h>
#endif

//C++ headers
#include <random>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>



namespace DSEmulator_ns {
  
//Forward declarations
class DSEmulator;
class PointingSchedule;


// ============================================================================
// class: TrackingThread
// ============================================================================
class TrackingThread {

	public:

		/** 
		\brief Constructor
 		*/
    TrackingThread(DSEmulator* dev,PointingSchedule* pntSchedule);

		/** 
		\brief Destructor
 		*/
    ~TrackingThread();

	public:
		/** 
		\brief Start the thread
 		*/		
		void Start(){
    	m_thread = std::thread(&TrackingThread::Run,this);
    }
		
		/** 
		\brief Stop
 		*/
		void Stop(){
			__DEBUG_LOG(m_dev,"Called Stop in tracking thread...");
			std::lock_guard<std::mutex> lock( m_mutex );
			m_stopThread = true;
			m_cond.notify_all();
			__DEBUG_LOG(m_dev,"Stop() ended");
		}

		/** 
		\brief Add pointing coordinates to tracking queue
 		*/
		int PushTrackCoordsToQueue(PointingDataPtr pdata);

		/** 
		\brief Enable tracking
 		*/
		void EnableTracking(){
			__DEBUG_LOG(m_dev,"Called EnableTracking in tracking thread...");
			std::lock_guard<std::mutex> lock( m_mutex );
			m_trackingEnabled = true;
			m_cond.notify_all();
			__DEBUG_LOG(m_dev,"EnableTracking() ended");
		}

		/** 
		\brief Disable tracking
 		*/
		void DisableTracking(){
			__DEBUG_LOG(m_dev,"Called DisableTracking in tracking thread...");
			std::lock_guard<std::mutex> lock( m_mutex );
			m_trackingEnabled = false;
			m_cond.notify_all();
			__DEBUG_LOG(m_dev,"DisableTracking() ended");
		}

		/** 
		\brief Notify data available
 		*/
		void NotifyDataAvailable(){
			__DEBUG_LOG(m_dev,"Called NotifyDataAvailable in tracking thread...");
			std::lock_guard<std::mutex> lock( m_mutex );
			m_cond.notify_all();
			__DEBUG_LOG(m_dev,"NotifyDataAvailable() ended");
		}

	private:
		/** 
		\brief Main thread function 
 		*/
		void Run();

		/** 
		\brief Smear pointing data with given resolution (in arcsec)
 		*/	
		void SmearPointing(PointingData& pdata,double reso);

		/** 
		\brief Slew up to desired position
 		*/
		int Slew(PointingData& pdata,double slewRate,double slewTimeStep,double pointingReso);

		/** 
		\brief Set track coordinates
 		*/
		int SetTrackCoords(PointingData& pdata,bool smearCoords,double pointingReso=0);

		/** 
		\brief Pop task from queue
 		*/
		int PopTrackCoordsFromQueue(PointingData& data);

	private:
	
		//Member classes
		DSEmulator* m_dev;	
		PointingSchedule* m_pntSchedule;

		//Thread stuff (mutexes, cond vars, etc)
		mutable std::mutex m_mutex;	
		//std::atomic<bool> m_stopThread;
		bool m_stopThread;
		bool m_trackingEnabled;
		std::condition_variable m_cond;
		std::thread m_thread;

		//other variables
		std::mt19937 m_randgen;
		
};//close TrackingThread class

}//close namespace

#endif
