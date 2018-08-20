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
* @file PointingSchedule.h
* @class PointingSchedule
* @brief PointingSchedule
*
* PointingSchedule
* @author S. Riggi
* @date 19/01/2018
*/

#ifndef _POINTING_SCHEDULE_H_
#define _POINTING_SCHEDULE_H_

//Tango headers
#ifndef _TANGO_H
	#include <tango.h>
#endif


#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
#endif

#ifndef __YAT_TIME_H__
	#include <yat/time/Time.h>
#endif

#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace DSEmulator_ns {

// ============================================================================
// class: PointingData
// ============================================================================
class PointingData {

	public:
		/** 
		\brief Standard constructor
 		*/
		PointingData()
		{
			//Initialize yat time to empty
			timestamp.set_empty_unix();
			az= 0;
			el= 0;
			t= 0;
			dt= 0;
			pntType= eTRACK_PROGRAM;
		}

		/** 
		\brief Param constructor
 		*/
		PointingData(double _az,double _el,double _t,int type=eTRACK_PROGRAM)
			: az(_az), el(_el), t(_t), pntType(type)
		{
			//Initialize yat time to given utc epoch time (converted in seconds)
			if(_t>0) timestamp.set_double_unix(_t/1000.);//should be given in seconds
			else timestamp.set_empty_unix();
			dt= 0;
		}
		
		/** 
		\brief Destructor
 		*/
		~PointingData() {}

		/** 
		\brief Pointing type
 		*/
		enum PointingType {eSTOW=0,eTRACK_PROGRAM=1,eSCAN=2};

	public:
		
		std::string GetString(){
			std::stringstream ss;
			ss<<"(Az,El,t,type)=("<<az<<","<<el<<","<<t<<","<<pntType<<")";
			return ss.str();
		}

	public:
		double az;//azimuth
		double el;//elevation
		double t;//timestamp in ms from unix epoch utc
		yat::Time timestamp;//timestamp struct
		double dt;//difference between desired time and applied time in ms
		int pntType;

};//close class PointingData

typedef std::shared_ptr<PointingData> PointingDataPtr;

/** 
\brief Compare pointing data by timestamp
*/
struct PointingDataSorter {
	bool operator()( const PointingDataPtr d1, const PointingDataPtr d2 ) const {
  	//return d1->timestamp < d2->timestamp;	
		return d1->t < d2->t;
  }
};

/** 
\brief Match pointing data expired (beyond time interval from now)
*/
struct MatchExpiredPointingData {
	MatchExpiredPointingData(const yat::Time& t0,const long int& dt) 
		: m_t0(t0), m_dt(dt) 
	{}
 	bool operator()(const PointingDataPtr obj) const {
		double tdiff= Utils_ns::TangoUtils::GetYatTimeDiffUSec(m_t0, obj->timestamp);
		bool isOld= (tdiff>=m_dt);
  	return isOld;
 	}
 	private:
  	const yat::Time& m_t0;
		const long int& m_dt;
};

// ============================================================================
// class: PointingSchedule
// ============================================================================
typedef std::set<PointingDataPtr,PointingDataSorter> PointingQueue;

class PointingSchedule {

	public:
		/** 
		\brief Pointing schedule constructor
 		*/
		PointingSchedule(Tango::DeviceImpl* dev);

		/** 
		\brief Pointing schedule destructor
 		*/
		~PointingSchedule();

	public:

		/** 
		\brief Pop item from queue
 		*/
		int Pop(PointingData& data,double& tdiff_ms);

		/** 
		\brief Push item to queue
 		*/
		int Push(PointingDataPtr data);

		/** 
		\brief Push items to queue
 		*/
		int Push(std::vector<PointingDataPtr>& data);

		/** 
		\brief Get queued tasks
 		*/
		PointingQueue const& GetQueue() const {return m_pointingQueue;}		

		/** 
		\brief Signal wake up of waiting threads
 		*/
		//void WakeUp();
		/** 
		\brief Signal end 
 		*/		
		//void End();

		/** 
		\brief Add pointing data to queue
 		*/
		//int AddDataToQueue(PointingDataPtr data);
		
		/** 
		\brief Pop task from queue
 		*/
		//int PopDataFromQueue(PointingData& data);
		
		/** 
		\brief Get queue info
 		*/
		void GetQueueInfo(std::vector<std::string>& info){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return get_queue_info(info);
		}

		/** 
		\brief Flush queue
 		*/
		int FlushQueue(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return flush_queue();
		}

		/** 
		\brief Get number of pointing data in queue
 		*/
		int GetQueueSize(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return static_cast<int>(m_pointingQueue.size());
		}

		/** 
		\brief Check if quota is reached
 		*/
		bool IsQueueFull(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return is_queue_full();
		}
		
		/** 
		\brief Check if queue is empty
 		*/
		bool IsQueueEmpty(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return m_pointingQueue.empty();
		}

		/** 
		\brief Check if queue has items
 		*/
		bool HasEntriesInQueue(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return !m_pointingQueue.empty();
		}

		/** 
		\brief Set maximum number of tasks in the queue
 		*/
		void SetMaxQueueSize(int val){
			std::unique_lock<std::mutex> mlock(m_mutex);
			m_maxQueueSize= val;
		}
		
		/** 
		\brief Set maximum number of tasks in the queue
 		*/
		void UseUTC(bool choice){
			std::unique_lock<std::mutex> mlock(m_mutex);
			m_useUTC= choice;
		}

	private:
		/** 
		\brief Check if data queue quote is reached (internal method)
 		*/
		bool is_queue_full();
		/** 
		\brief Remove all pointing data from queue (internal method)
 		*/
		int flush_queue();
		/** 
		\brief Remove all expired pointing data in queue (internal method)
 		*/
		int remove_expired_data_in_queue();

		/** 
		\brief Get queue info (internal method)
 		*/
		void get_queue_info(std::vector<std::string>& info){
			info.clear();
			for(auto item : m_pointingQueue) {
  			std::string pntDataInfo= item->GetString();
				info.push_back(pntDataInfo);
			}
		}		

	private:
		//Host device
		Tango::DeviceImpl* m_dev;

		//Thread mutex variables
		//static bool m_wakeUpSignal;
		//static bool m_endSignal;	
		mutable std::mutex m_mutex;
		//std::condition_variable m_cond;

		//Task variables
		PointingQueue m_pointingQueue;
		
		//Options
		int m_maxQueueSize;
		int m_expiredTimeThr;//ms wrt to now beyond which pointing data is considered expired and removed from the queue
		bool m_useUTC;
		

};//close PointingSchedule class


}//close namespace 

#endif


