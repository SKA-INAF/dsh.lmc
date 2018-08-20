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
* @file TaskManager.h
* @class TaskManager
* @brief TaskManager class
*
* TaskManager class
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _TASK_MANAGER_h
#define _TASK_MANAGER_h 1

#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS
	#include <CodeUtils.h>
#endif

#ifndef _TASK_h
	#include <Task.h>
#endif

//Tango headers
#ifndef _TANGO_H
	#include <tango.h>
#endif

#ifndef _LOG4TANGO_H_
	#include <log4tango.h>
#endif

//Yat headers
#ifndef _YAT_GENERIC_CONTAINER_H_
	#include <yat/any/GenericContainer.h>
#endif

#ifndef _YAT_DATA_BUFFER_H_
	#include <yat/memory/DataBuffer.h>
#endif

#ifndef __YAT_TIME_H__
	#include <yat/time/Time.h>
#endif

#ifndef _YAT_TIMER_H_
	#include <yat/time/Timer.h>
#endif

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <stdexcept>

#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <time.h>
#include <ctime>
#include <cmath>

#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Utils_ns {

//Forward declarations
class Task;

// ============================================================================
// class: TaskManager
// ============================================================================
typedef std::chrono::system_clock::time_point Timestamp;
typedef std::shared_ptr<Task> TaskPtr;
typedef std::vector<TaskPtr> TaskCollection;
typedef std::set<TaskPtr,CompareTask> TaskQueue;
typedef std::set<TaskPtr,CompareTaskByExpirationTime> TaskMonitorQueue;
typedef std::vector<TaskSequencePtr> TaskSequenceCollection;

class TaskManager {

	public:
		TaskManager(Tango::DeviceImpl* dev);
		~TaskManager();

	public:

		/** 
		\brief Get queued tasks
 		*/
		TaskQueue const& GetTaskQueue() const {return m_taskQueue;}		

		/** 
		\brief Convert task status from string label
 		*/
		static int GetTaskStatusFromString(int& status,std::string status_str);

		/** 
		\brief Convert task status to string label
 		*/
		static int GetTaskStatusString(std::string& status_str,int status);

		/** 
		\brief Signal wake up of waiting threads
 		*/
		void WakeUp();
		/** 
		\brief Signal end 
 		*/		
		void End();
		/** 
		\brief Execute task
 		*/
		void ExecuteTask(Task& task);

		/** 
		\brief Find tasks in queue by name and id
 		*/
		int FindTasksInQueue(std::vector<TaskPtr>& matched_tasks,std::string id,std::string name);

		/** 
		\brief Add task sequence to queue & list
 		*/
		int AddTaskSequence(TaskSequencePtr taskSequence,bool addToQueue);
		/** 
		\brief Remove task sequence from list and queue 
 		*/
		int RemoveTaskSequence(std::string name);

		/** 
		\brief Revoke task sequence: remove from queue, set to CANCELED in list
 		*/
		int RevokeTaskSequence(std::string name);

		/** 
		\brief Revoke a task: remove from queue, set to CANCELED in list
 		*/
		int RevokeTask(std::string name);

		/** 
		\brief Pop task from queue
 		*/
		int PopTask(Task& task);
		
		/** 
		\brief Flush queue
 		*/
		int FlushQueue();

		/** 
		\brief Set sequence task status
 		*/
		int SetSequenceTaskStatus(std::string id,std::string name,int status,std::string status_info="");
		/** 
		\brief Update asynch task status
 		*/
		int SetAsynchTaskStatus(std::string attr_name,int status,std::string status_info="");

		/** 
		\brief Get number of tasks in queue
 		*/
		int GetNTasksInQueue(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return static_cast<int>(m_taskQueue.size());
		}
		
		/** 
		\brief Get number of sequence tasks in running status
 		*/
		int GetNRunningSequenceTasks(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return get_n_running_sequence_tasks();
		}
		
		/** 
		\brief Get number of sequence tasks in idle status
 		*/
		int GetNIdleSequenceTasks(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return get_n_idle_sequence_tasks();
		}
		
		/** 
		\brief Get number of sequence tasks in failed status
 		*/
		int GetNFailedSequenceTasks(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return get_n_failed_sequence_tasks();
		}
		
		/** 
		\brief Get total number of sequence tasks in failed status (including also EXPIRED and ABORTED)
 		*/
		int GetNTotFailedSequenceTasks(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return get_n_tot_failed_sequence_tasks();
		}


		/** 
		\brief Get number of sequence tasks in collection
 		*/
		int GetNSequenceTasks(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return get_n_sequence_tasks();
		}

		/** 
		\brief Check if task quota was reached
 		*/
		bool IsTaskQuotaReached(){
			std::unique_lock<std::mutex> mlock(m_mutex);
			return is_task_quota_reached();
		}
		

		/** 
		\brief Set maximum number of tasks in the queue
 		*/
		void SetMaxNTasksInQueue(int val){
			std::unique_lock<std::mutex> mlock(m_mutex);
			m_maxTasksInQueue= val;
		}
		/** 
		\brief Set maximum number of tasks in the collection
 		*/
		void SetMaxNTasksInCollection(int val){
			std::unique_lock<std::mutex> mlock(m_mutex);
			m_maxTasksInCollection= val;
		}

		/** 
		\brief Get task info for queued task
 		*/
		int GetQueuedTaskInfo(std::vector<std::string>& info);

		
		/** 
		\brief Get sequence task info
 		*/
		int GetSequenceTaskInfo(std::vector<std::string>& info);

		
		
		/** 
		\brief Clear completed sequence tasks older than given time threshold in seconds
 		*/
		int ClearSequenceTasks(long int historyTimeThr,bool keepRunningTasks, bool useUTC);

		/** 
		\brief Clear completed sequence tasks older than given time threshold in seconds
 		*/
		int ClearCompletedSequenceTasks(long int historyTimeThr=0,bool useUTC=false);
		
		/** 
		\brief Clear all sequence tasks
 		*/
		int ClearAllSequenceTasks();

		/** 
		\brief Retrieve task sequence status info
 		*/
		int GetTaskSequenceStatusInfo(std::vector<TaskInfo>& status_info, bool useDevAttrFilter, std::string dev_attr_name);

	private:

		/** 
		\brief Find task in sequence (internal method)
 		*/
		TaskPtr find_task_in_sequence(std::string id,std::string name,std::string seq_id,std::string seq_name);

		/** 
		\brief Find task sequence (internal method)
 		*/
		TaskSequencePtr find_task_sequence(int& index,std::string id,std::string name);

		
		/** 
		\brief Find task in queue (internal method)
 		*/
		TaskPtr find_task_in_queue(std::string id,std::string name);

		/** 
		\brief Find tasks in queue (internal method)
 		*/
		int find_tasks_in_queue(std::vector<TaskPtr>& matched_tasks,std::string id,std::string name);

		/** 
		\brief Find task in sequence (internal method)
 		*/
		int find_sequence_tasks_in_queue(std::vector<TaskPtr>& matched_tasks,std::string seq_id);

		/** 
		\brief Find task in sequence with output device attr (internal method)
 		*/
		int find_task_sequences_by_dev_attr(std::vector<TaskSequencePtr>& matched_task_sequences,std::string dev_attr_name);

		

		/** 
		\brief Remove all sequence tasks with same name (internal method)
 		*/
		int remove_sequence_tasks(std::string name);

		/** 
		\brief Remove all tasks from queue by matching id and name (internal method)
 		*/
		int remove_tasks_in_queue(std::string id,std::string name);
		/** 
		\brief Remove all tasks from queue by matching iname (internal method)
 		*/	
		int remove_tasks_in_queue(std::string name);

		/** 
		\brief Remove all expired tasks in queue (internal method)
 		*/
		int remove_expired_tasks_in_queue();

		/** 
		\brief Remove all sequence tasks from queue by matching name (internal method)
 		*/
		int remove_sequence_tasks_in_queue(std::string name);

		
		/** 
		\brief Revoke tasks in sequence by name (set all IDLE tasks to CANCELED for matching sequences) (internal method)
 		*/
		int revoke_sequence_tasks(std::string name);

		/** 
		\brief Revoke tasks by name (set all IDLE tasks to CANCELED for matching sequences) (internal method)
 		*/
		int revoke_tasks(std::string name);

		/** 
		\brief Flush queue and remove IDLE tasks (internal method)
 		*/
		int flush_queue();
		

		/** 
		\brief Get number of running sequence tasks (internal method)
 		*/
		int get_n_running_sequence_tasks(){
			return std::accumulate( m_taskSequences.begin(), m_taskSequences.end(), 0.0, [](double sum, TaskSequencePtr p){return (sum + p->GetNRunningTasks());} );
		}

		
		/** 
		\brief Get number of idle sequence tasks (internal method)
 		*/
		int get_n_idle_sequence_tasks(){
			return std::accumulate( m_taskSequences.begin(), m_taskSequences.end(), 0.0, [](double sum, TaskSequencePtr p){return (sum + p->GetNIdleTasks());} );
		}

		

		/** 
		\brief Get number of fault sequence tasks (internal method)
 		*/
		int get_n_failed_sequence_tasks(){
			return std::accumulate( m_taskSequences.begin(), m_taskSequences.end(), 0.0, [](double sum, TaskSequencePtr p){return (sum + p->GetNFailedTasks());} );
		}

		

		/** 
		\brief Get total number of fault sequence tasks (include also ABORTED & EXPIRED) (internal method)
 		*/
		int get_n_tot_failed_sequence_tasks(){
			int n_failed= std::accumulate( m_taskSequences.begin(), m_taskSequences.end(), 0.0, [](double sum, TaskSequencePtr p){return (sum + p->GetNFailedTasks());} );
			int n_aborted= std::accumulate( m_taskSequences.begin(), m_taskSequences.end(), 0.0, [](double sum, TaskSequencePtr p){return (sum + p->GetNAbortedTasks());} );
			int n_expired= std::accumulate( m_taskSequences.begin(), m_taskSequences.end(), 0.0, [](double sum, TaskSequencePtr p){return (sum + p->GetNExpiredTasks());} );;
			return n_failed + n_aborted + n_expired;
		}

		/** 
		\brief Get total number of sequence tasks (include also ABORTED & EXPIRED) (internal method)
 		*/
		int get_n_sequence_tasks(){
			double n= std::accumulate( m_taskSequences.begin(), m_taskSequences.end(), 0.0, [](double sum, TaskSequencePtr p){return (sum + p->GetNTasks());} );
			return static_cast<int>(n);
		}

		/** 
		\brief Check if task quota in queue and list is reached (internal method)
 		*/
		bool is_task_quota_reached();

		/** 
		\brief Execute task (internal method)
 		*/
		void execute_task(Task& task);

		
		/** 
		\brief Set task status (internal method)
 		*/
		int set_sequence_task_status(std::string id,std::string name,int status,std::string status_info="");

		/** 
		\brief Check if task is waiting for previous task in sequence to complete (internal method)
 		*/
		bool is_sequence_task_runnable(TaskPtr aTask);

		/** 
		\brief Find tasks in sequences matching watched attr name (internal method)
 		*/
		int find_tasks_by_watched_attr_name(std::vector<TaskPtr>& matching_tasks,std::string attr_name);

	private:
		//Host device
		Tango::DeviceImpl* m_dev;

		//Thread mutex variables
		static bool m_wakeUpSignal;
		static bool m_endSignal;	
		mutable std::mutex m_mutex;
		std::condition_variable m_cond;

		//Task variables
		TaskQueue m_taskQueue;
		TaskSequenceCollection m_taskSequences;
		
		//Options
		int m_defaultQueueWaitTime;//Default time to wait (in seconds) in queue when no activation times are available 
		int m_maxTasksInQueue;
		int m_maxTasksInCollection;

};//close TaskManager class

}//close namespace

#endif


