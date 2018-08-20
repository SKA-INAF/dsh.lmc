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
* @file Task.h
* @class Task
* @brief Task class
*
* Task class
* @author S. Riggi
* @date 15/01/2016
*/

#ifndef _TASK_h
#define _TASK_h 1


#ifndef _LOGGER
	#include <Logger.h>
#endif

#ifndef _CODE_UTILS
	#include <CodeUtils.h>
#endif

#ifndef _TANGO_UTILS_h
	#include <TangoUtils.h>
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

// JSON CPP
#ifndef JSON_JSON_H_INCLUDED
	#include <json/json.h>
#endif

#ifndef JSON_READER_H_INCLUDED
	#include <json/reader.h>
#endif

#ifndef JSON_WRITER_H_INCLUDED
	#include <json/writer.h>
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
#include <chrono>

namespace Utils_ns {

// ============================================================================
// struct: TaskInfo
// ============================================================================
struct TaskInfo {
	public:
		std::string task_name;
		int status;
		std::string status_info;
		double creation_timestamp;
	public:
		TaskInfo(std::string _name,int _status,std::string _info,double timestamp)
			: task_name(_name), status(_status), status_info(_info), creation_timestamp(timestamp)
		{}
		~TaskInfo(){}
};

// ============================================================================
// class: Task
// ============================================================================
typedef std::chrono::system_clock::time_point Timestamp;

class Task {

	public:
		
		/** 
		\brief Task execution status
 		*/
		enum TaskStatus {
			eUNKNOWN= 0,
			eINIT= 1,
			eIDLE= 2,
			eRUNNING= 3,
			eCOMPLETED= 4,
			eFAILED= 5,
			eABORTED= 6,
			eEXPIRED= 7,
			eCANCELED= 8
		};
		/** 
		\brief Task priority
 		*/
		enum TaskPriority {
			eLOW_PRIORITY= 0,
			eMEDIUM_PRIORITY= 1,
			eHIGH_PRIORITY= 2
		};

		/** 
		\brief Task time ref
 		*/
		enum TimeRef {
			eTIME_NOW= 0,
			eCREATION_TIME= 1,
			eEXTERNAL_TIME= 2
		};

	public:
		/** 
		\brief Standard constructor
 		*/
		Task();
		/** 
		\brief Task constructor without argin & t_start/t_stop
 		*/
		Task(Tango::DeviceImpl* dev,std::string _cmd_name,int _priority=eMEDIUM_PRIORITY, bool _useUTC=false);

		/** 
		\brief Destructor
 		*/
		~Task();

		/** 
		\brief < operator (needed for set::insert) DEPRECATED!!!! use comparator CompareTask
 		*/
		bool operator<(const std::shared_ptr<Task> p) const {
			bool isLess= (std::tie(cmd_name,id) < std::tie(p->cmd_name,p->id) );
    	return isLess;
    }


	public:	
		/** 
		\brief Execute task and return output in CORBA::Any format
 		*/
		CORBA::Any* Run();
		/** 
		\brief Attach argument to task
 		*/
		void SetTaskArgin(CORBA::Any& argin,int data_type,int data_format);

		/** 
		\brief Set activation time from timestamp
 		*/
		int SetActivationTime(yat::Time& t_start);

		/** 
		\brief Set activation time from time delay
 		*/
		int SetActivationTimeDelay(long int delay_time);
		
		/** 
		\brief Set expiration time
 		*/
		int SetExpirationTime(yat::Time& t_stop);

		/** 
		\brief Set delayed activation time and max duration in ms wrt to now
 		*/
		int SetTimeDelayAndDuration(long int delay_time,long int max_duration, int time_ref_mode=eTIME_NOW, yat::Time time_ref=yat::Time());

		/** 
		\brief Set max duration in ms wrt to now
 		*/
		int SetMaxDuration(long int max_duration,int time_ref_mode=eTIME_NOW, yat::Time time_ref=yat::Time());

		/** 
		\brief Encode task data to json object
 		*/
		int EncodeToJson(Json::Value& root);

		/** 
		\brief Encode task data to string
 		*/
		int EncodeToJsonString(std::string& jsonString);

		/** 
		\brief Set async task type
 		*/
		void SetAsynchTask(std::string attr_name);

		/** 
		\brief Set status
 		*/
		void SetStatus(int _status,std::string _status_info="");

		/** 
		\brief Set sequence info
 		*/
		void SetSequenceInfo(std::string seq_id,int seq_index,std::vector<int> dep_task_indexes);

		/** 
		\brief Check if activation conditions are reached
 		*/
		bool IsActivationReached();

		/** 
		\brief Check if expiration conditions are reached
 		*/
		bool IsExpirationReached();

		/** 
		\brief Check if task is runnable
 		*/
		bool IsRunnable();
		/** 
		\brief Check if task is waiting for activation
 		*/
		bool IsWaitingForActivation();

		/** 
		\brief Check if task depends on a given task
 		*/
		bool DependOnTask(std::shared_ptr<Task> aTask);
		
	private:
		/** 
		\brief Generate a unique task id
 		*/
		void GenerateId();
		/** 
		\brief Set the activation timeout from a timestamp wrt now
 		*/
		int SetActivationTimeout(yat::Time& tstart);	
		/** 
		\brief Set the expiration timeout from a timestamp wrt now
 		*/
		int SetExpirationTimeout(yat::Time& tstop);
		/** 
		\brief Set the task creation time
 		*/
		void SetCreationTime(){creation_time.set_current(useUTC);}

		/** 
		\brief Init activation timeout
 		*/
		void InitActivationTimeout(long int delay_time);
		/** 
		\brief Init activation timeout
 		*/		
		void InitExpirationTimeout(long int delay_time);
		/** 
		\brief Init variables
 		*/
		void Init();

	public:
	
		//Cmd info		
		std::string id;
		std::string cmd_name;	
		int priority;
		CORBA::Any cmd_argin;
		bool hasArgin;
		int argin_type;
		int argin_format;

		//Cmd exec info
		int status;
		std::string status_info;
		bool is_asynch;
		std::string status_attr_name;//watched attribute name providing task status (for asynch tasks)
		
		//Sequence task info
		bool is_in_sequence;
		std::string sequence_id;
		std::string sequence_name;
		int sequence_index;
		bool depend_on_sequence_tasks;
		std::vector<int> dep_task_seq_indexes;//indexes of tasks in sequence depending to

		//Timestamp info
		bool useUTC;
		yat::Timeout* activation_timeout;
		yat::Timeout* expiration_timeout;
		bool hasActivationTime;
		yat::Time activation_time;
		bool hasExpirationTime;
		long int max_duration;
		yat::Time expiration_time;
		yat::Time creation_time;//timestamp when task is put in queue
		
		long int default_max_duration;//Default task maximum duration before timeout (in ms)
		yat::Time exp_max_activation_time;//Expected max activation time (meaningful only for dependent tasks, computed as the maximum expiration_time among the dependant tasks)
		yat::Time exp_max_expiration_time;//Expected max expiration time (meaningful only for dependent tasks, computed as the maximum expiration_time among the dependant tasks)

	private:
		//Device instance
		Tango::DeviceImpl* m_dev;


};//close class Task

typedef std::shared_ptr<Task> TaskPtr;

/** 
\brief Compare task by activation time & priority
*/
struct CompareTask {	
	bool operator()( const TaskPtr t1, const TaskPtr t2 ) const {

		return comparatorFcn(t1,t2);
	
		/*		
		//Check if tasks belong to the same sequence
		bool areInSameSequence= (
			(t1->is_in_sequence && t2->is_in_sequence) &&
			(t1->sequence_id == t2->sequence_id) &&
			(t1->sequence_name == t2->sequence_name)
		);

		if(areInSameSequence){
			bool lessComparison= (t1->sequence_index < t2->sequence_index);
			return lessComparison;
		}
		else{

			//CASE: Tasks have no activation times
			if(!t1->hasActivationTime && !t2->hasActivationTime){
				
				//NB: Ensure that !comp(a, b) && !comp(b, a) otherwise std::set will threat 
				//items as equal and will not insert nothing in the set

				//First: compare by priority
				bool compareByPriority= (t1->priority > t2->priority);
				if(t1->priority!=t2->priority) return compareByPriority;

				//Second: compare by expiration time
				bool compareByExpiration= (t1->expiration_time < t2->expiration_time);
				if(t1->expiration_time != t2->expiration_time) return compareByExpiration;
					
				//Last chance: compare by task id & name
				bool compareByNameAndId= (
					std::tie(t1->cmd_name,t1->id) < std::tie(t2->cmd_name,t2->id)
				);
				return compareByNameAndId;

			}//close if 
				
			//CASE: Task 1 has activation, Task 2 not
			//NB: If a task has activation time and the other not set the former as most important (regardless of priority value)
			else if(t1->hasActivationTime && !t2->hasActivationTime) {
				return true;
			}

			//CASE: Task 1 has activation, Task 2 not
			else if(!t1->hasActivationTime && t2->hasActivationTime) {
				return false;
			}
	
			//CASE: Both tasks have activation
			else {
				//If both have different activation times sort according to start time
				bool compareByActivation= (t1->activation_time < t2->activation_time);
				if(t1->activation_time != t2->activation_time) return compareByActivation;
				
				//First: compare by priorities
				bool compareByPriority= (t1->priority > t2->priority);
				if(t1->priority!=t2->priority) return compareByPriority;

				//Second: compare by expiration time
				bool compareByExpiration= (t1->expiration_time < t2->expiration_time);
				if(t1->expiration_time != t2->expiration_time) return compareByExpiration;
					
				//Last chance: compare by task id & name
				bool compareByNameAndId= (
					std::tie(t1->cmd_name,t1->id) < std::tie(t2->cmd_name,t2->id)
				);
				return compareByNameAndId;
			}
		}//close else
		*/
  }//close operator()

	/** 
	\brief Compare tasks 
	*/
	bool comparatorFcn(const TaskPtr t1, const TaskPtr t2) const {
		
		//## NB: Ensure that !comp(a, b) && !comp(b, a) otherwise std::set will threat 
		//## items as equal and will not insert nothing in the set

		//Check if tasks belong to the same sequence
		bool areInSameSequence= (
			(t1->is_in_sequence && t2->is_in_sequence) &&
			(t1->sequence_id == t2->sequence_id) &&
			(t1->sequence_name == t2->sequence_name)
		);

		if(areInSameSequence){
			bool t1_dependOn_t2= t1->DependOnTask(t2);
			bool t2_dependOn_t1= t2->DependOnTask(t1);

			//Case t1 depending on t2 for activation
			if(t1_dependOn_t2) return false;

			//Case t2 depending on t1 for activation
			if(t2_dependOn_t1) return true;

		}//close if in same sequence

		//Case t1 depend on other task, t2 not
		if(t1->depend_on_sequence_tasks && !t2->depend_on_sequence_tasks){
			//If t2 has no activation set as first
			if(!t2->hasActivationTime) return false;

			//First: compare t1_start^exp vs t2_start
			bool compareByTstart= (t1->exp_max_activation_time < t2->activation_time);
			if(t1->exp_max_activation_time != t2->activation_time) return compareByTstart;

		}//close if 
		
		//Case t2 depend on other task, t1 not
		if(t2->depend_on_sequence_tasks && !t1->depend_on_sequence_tasks){
			//If t1 has no activation set as first
			if(!t1->hasActivationTime) return true;

			//Compare t1_start vs t2_start^exp
			bool compareByTstart= (t1->activation_time < t2->exp_max_activation_time);
			if(t1->activation_time != t2->exp_max_activation_time) return compareByTstart;

		}//close if 

		//Case both tasks depending on other tasks
		if(t1->depend_on_sequence_tasks && t2->depend_on_sequence_tasks){
			//Compare t1_start^exp vs t2_start^exp
			bool compareByTstartExp= (t1->exp_max_activation_time < t2->exp_max_activation_time);
			if(t1->exp_max_activation_time != t2->exp_max_activation_time) return compareByTstartExp;
					
		}//close if

		//Case tasks not depending on other tasks
		if(!t1->depend_on_sequence_tasks && !t2->depend_on_sequence_tasks){
			//If t1 has no activation set as first
			if(!t1->hasActivationTime && t2->hasActivationTime) return true;

			//If t2 has no activation set as first
			if(!t2->hasActivationTime && t1->hasActivationTime) return false;

			//If both have activation compare their tstart
			if(t1->hasActivationTime && t2->hasActivationTime){
				bool compareByTstart= (t1->activation_time < t2->activation_time);
				if(t1->activation_time != t2->activation_time) return compareByTstart;
			}
		}
		
		//Second: compare priorities
		bool compareByPriority= (t1->priority > t2->priority);
		if(t1->priority!=t2->priority) return compareByPriority;

		//Third: compare by expiration time
		bool compareByExpiration= (t1->expiration_time < t2->expiration_time);
		if(t1->expiration_time != t2->expiration_time) return compareByExpiration;
					
		//Last chance: compare by task id & name
		bool compareByNameAndId= (
			std::tie(t1->cmd_name,t1->id) < std::tie(t2->cmd_name,t2->id)
		);
		return compareByNameAndId;

	}//close comparatorFcn()
	
	
};//close struct CompareTask

/** 
\brief Compare task by expiration time
*/
struct CompareTaskByExpirationTime {
	bool operator()( const TaskPtr& t1, const TaskPtr& t2 ) const {
  	return t1->expiration_time < t2->expiration_time;
  }
};


/** 
\brief Match task by name & id
*/
struct MatchTask {
	MatchTask(const std::string& id,const std::string& name) 
		: m_id(id), m_name(name) 
	{}
 	bool operator()(const TaskPtr obj) const {
  	return (obj->id==m_id && obj->cmd_name==m_name);
 	}
 	private:
  	const std::string& m_id;
		const std::string& m_name;
};

/** 
\brief Match task by name 
*/
struct MatchTaskByName {
	MatchTaskByName(const std::string& name) 
		: m_name(name) 
	{}
 	bool operator()(const TaskPtr obj) const {
  	return (obj->cmd_name==m_name);
 	}
 	private:
		const std::string& m_name;
};

/** 
\brief Match task by id
*/
struct MatchTaskById {
	MatchTaskById(const std::string& id) 
		: m_id(id) 
	{}
 	bool operator()(const TaskPtr obj) const {
  	return (obj->id==m_id);
 	}
 	private:
		const std::string& m_id;
};

/** 
\brief Match task by execution status
*/
struct MatchTaskByStatus {
	MatchTaskByStatus(const int& status) 
		: m_status(status) 
	{}
 	bool operator()(const TaskPtr obj) const {
  	return (obj->status==m_status);
 	}
 	private:
  	const int& m_status;
};

/** 
\brief Match task within time interval
*/
struct MatchOldTask {
	MatchOldTask(const int& filter_status,const yat::Time& current_time,const long int& historyTimeThr) 
		: m_filter_status(filter_status), m_currentTime(current_time), m_historyTimeThr(historyTimeThr) 
	{}
 	bool operator()(const TaskPtr obj) const {
		double tdiff= TangoUtils::GetYatTimeDiffSec(m_currentTime, obj->creation_time);
		bool isOld= (tdiff>=m_historyTimeThr) && (m_filter_status==-1 || obj->status!=m_filter_status);
  	return isOld;
 	}
 	private:
  	const int& m_filter_status;
		const yat::Time& m_currentTime;
		const long int& m_historyTimeThr;
};


/** 
\brief Match task by sequence id
*/
struct MatchTaskBySequenceId {
	MatchTaskBySequenceId(const std::string& seq_id) 
		: m_seq_id(seq_id) 
	{}
 	bool operator()(const TaskPtr obj) const {
		bool isInSameSequence= (
			(obj->is_in_sequence==true) && 			
			(obj->sequence_id==m_seq_id)
		);
  	return isInSameSequence;
 	}
 	private:
  	const std::string& m_seq_id;
};

/** 
\brief Match task by sequence name
*/
struct MatchTaskBySequenceName {
	MatchTaskBySequenceName(const std::string& seq_name) 
		: m_seq_name(seq_name) 
	{}
 	bool operator()(const TaskPtr obj) const {
		bool isInSameSequence= (
			(obj->is_in_sequence==true) && 			
			(obj->sequence_name==m_seq_name)
		);
  	return isInSameSequence;
 	}
 	private:
  	const std::string& m_seq_name;
};

/** 
\brief Match asynch task by watched attr name 
*/
struct MatchAsynchTaskByWatchedAttrName {
	MatchAsynchTaskByWatchedAttrName(const std::string& attr_name) 
		: m_attr_name(attr_name) 
	{}
 	bool operator()(const TaskPtr obj) const {
  	bool isMatching= (
			(obj->is_asynch==true) && 			
			(obj->status_attr_name==m_attr_name) &&
			(obj->status_attr_name!="")
		);
		return isMatching;
 	}
 	private:
		const std::string& m_attr_name;
};



// ============================================================================
// class: TaskSequence
// ============================================================================
class TaskSequence {

	public:
		/** 
		\brief Constructor
 		*/
		TaskSequence();
		/** 
		\brief Parametric constructor
 		*/
		TaskSequence(Tango::DeviceImpl* dev,std::string _sequence_name,bool useUTCTime=false);
		/** 
		\brief Destructor
 		*/
		~TaskSequence();

	public:	
		/** 
		\brief Add task in sequence
 		*/
		int AddTask(TaskPtr task);
		/** 
		\brief Get number of tasks in sequence
 		*/
		int GetNTasks(){return static_cast<int>(tasks.size());}

		/** 
		\brief Compute sequence task status
 		*/
		void ComputeStatus();

		/** 
		\brief Set output device status attr
 		*/
		int SetDevStatusAttr(std::string attr_name);

		/** 
		\brief Set task status
 		*/
		int SetTaskStatus(std::string task_name,int status,std::string status_info);
		/** 
		\brief Set asynch task status from watched attr name
 		*/
		int SetAsynchTaskStatus(std::string attr_name,int status,std::string status_info);
	
		/** 
		\brief Find task in sequence
 		*/
		TaskPtr FindTask(int& index,std::string name);

		/** 
		\brief Find task in sequence
 		*/
		TaskPtr FindTask(int& index,std::string id,std::string name);

		/** 
		\brief Find asynch task matching attr name
 		*/
		TaskPtr FindAsynchTaskByWatchedAttrName(int& index,std::string attr_name);
		
		/** 
		\brief Get number of running tasks
 		*/
		int GetNRunningTasks(){
			return std::count_if(tasks.begin(), tasks.end(),MatchTaskByStatus(Task::eRUNNING));
		}

		/** 
		\brief Get number of idle tasks
 		*/
		int GetNIdleTasks(){
			return std::count_if(tasks.begin(), tasks.end(),MatchTaskByStatus(Task::eIDLE));
		}
		
		/** 
		\brief Get number of failed tasks
 		*/
		int GetNFailedTasks(){
			return std::count_if(tasks.begin(), tasks.end(),MatchTaskByStatus(Task::eFAILED));
		}

		/** 
		\brief Get number of aborted tasks
 		*/
		int GetNAbortedTasks(){
			return std::count_if(tasks.begin(), tasks.end(),MatchTaskByStatus(Task::eABORTED));
		}
	
		/** 
		\brief Get number of expired tasks
 		*/
		int GetNExpiredTasks(){
			return std::count_if(tasks.begin(), tasks.end(),MatchTaskByStatus(Task::eEXPIRED));
		}
		/** 
		\brief Set dependency info in tasks
 		*/
		int SetTaskDependency(std::vector<std::vector<size_t>>& dep_indexes);

		/** 
		\brief Sort tasks according to dependency info 
 		*/
		int Sort(); 
		/** 
		\brief Find task sorting indexes given the dependency matrix
 		*/
		int FindSortingIndexes(std::vector<size_t>& order_index);

		/** 
		\brief Check if a task dependency matrix has cyclic dependencies
 		*/
		bool HasCyclicDependencies(std::vector<std::vector<size_t>>& dep_matrix); 

	private:
		/** 
		\brief Generate id
 		*/
		void GenerateId(){seq_id= CodeUtils::GenerateUUID();}		
		/** 
		\brief Set the task creation time
 		*/
		void SetCreationTime(){creation_time.set_current(useUTC);}

		
		/** 
		\brief Helper function called recursively by Sort()
 		*/	
		void Sorter(int index, std::vector<bool>& visited, std::stack<int>& Stack);
		
		/** 
		\brief Helper function called recursively by HasCyclicDependencies()
 		*/
		bool CheckCyclicDependencies(int index, std::vector<std::vector<size_t>>& dep_matrix, std::vector<bool>& visited, std::vector<bool>& recStack);
		
	public:
		//Task sequence info
		std::string seq_name;
		std::string seq_id;
		std::vector<TaskPtr> tasks;

		//Task sequence status
		int status;
		std::string status_info;
		std::vector<std::vector<size_t>> dependency_matrix;

		//Timestamp & timeout
		bool useUTC;
		yat::Time creation_time;

		//Device status attr
		bool has_device_status_attr;
		std::string status_attr_name;

	private:
		//Device instance
		Tango::DeviceImpl* m_dev;

};//close class TaskSequence
typedef std::shared_ptr<TaskSequence> TaskSequencePtr;


/** 
\brief Match task by sequence name
*/
struct MatchTaskSequenceByName {
	MatchTaskSequenceByName(const std::string& seq_name) 
		: m_seq_name(seq_name) 
	{}
 	bool operator()(const TaskSequencePtr obj) const {
		bool isInSameSequence= (
			(obj->seq_name==m_seq_name)
		);
  	return isInSameSequence;
 	}
 	private:
  	const std::string& m_seq_name;
};

/** 
\brief Match task by sequence id and name
*/
struct MatchTaskSequenceByIdAndName {
	MatchTaskSequenceByIdAndName(const std::string& seq_id,const std::string& seq_name) 
		: m_seq_id(seq_id), m_seq_name(seq_name) 
	{}
 	bool operator()(const TaskSequencePtr obj) const {
		bool isInSameSequence= (
			(obj->seq_name==m_seq_name) &&
			(obj->seq_id==m_seq_id)
		);
  	return isInSameSequence;
 	}
 	private:
		const std::string& m_seq_id;
  	const std::string& m_seq_name;
};

/** 
\brief Match task sequence with dev attr name
*/
struct MatchTaskSequenceByDevAttrName {
	MatchTaskSequenceByDevAttrName(const std::string& attr_name) 
		: m_attr_name(attr_name)
	{}
 	bool operator()(const TaskSequencePtr obj) const {
  	return (obj->has_device_status_attr && obj->status_attr_name==m_attr_name);
 	}
	private:
		const std::string& m_attr_name;
};

/** 
\brief Match sequence task within time interval
*/
struct MatchOldSequenceTask {
	MatchOldSequenceTask(const int& filter_status,const yat::Time& current_time,const long int& historyTimeThr,const bool& invert_filter) 
		: m_filter_status(filter_status), m_currentTime(current_time), m_historyTimeThr(historyTimeThr), m_invert_filter(invert_filter)
	{}
 	bool operator()(const TaskSequencePtr obj) const {
		double tdiff= TangoUtils::GetYatTimeDiffSec(obj->creation_time,m_currentTime);
		bool isOldTask= (tdiff>=m_historyTimeThr);
		bool isSelectedTask= (m_filter_status==-1 || (!m_invert_filter && obj->status==m_filter_status) || (m_invert_filter && obj->status!=m_filter_status) );
		bool isOld= (isOldTask && isSelectedTask);
		DEBUG_LOG("Task sequence "<<obj->seq_name<<", tdiff(s)="<<tdiff<<", historyTimeThr(s)="<<m_historyTimeThr<<", isOldTask? "<<isOldTask<<", taskStatus="<<obj->status<<", filterStatus="<<m_filter_status<<", isSelectedTask? "<<isSelectedTask<<", m_invert_filter? "<<m_invert_filter);
  	return isOld;
 	}
 	private:
  	const int& m_filter_status;
		const yat::Time& m_currentTime;
		const long int& m_historyTimeThr;
		const bool& m_invert_filter;
};

// ============================================================================
// class: DataValue
// ============================================================================
class DataValue {
	public:
		DataValue()
		{
			type= -1;
			format= -1;	
		}
		DataValue(int _type,int _format)
			: type(_type), format(_format)
		{}
		virtual ~DataValue(){}
	public: 
		int type;
		int format;
};

template <typename T>
class ScalarDataValue : public DataValue {
	public:
		ScalarDataValue()
			: DataValue()
		{
			value= T();
		} 
		ScalarDataValue(int _type,T& _value)
			: DataValue(type,Tango::SCALAR), value(_value)	
		{}

		const T& Get() const {return value;}

	public: 
		T value;
};

template <typename T>
struct SpectrumDataValue : public DataValue {
	public:
		SpectrumDataValue()
			: DataValue()
		{
			value.clear();
		} 
		SpectrumDataValue(int _type,std::vector<T>& _value)
			: DataValue(type,Tango::SPECTRUM), value(_value)	
		{}

		const std::vector<T>& Get() const {return value;}

	public: 
		std::vector<T> value;
};
typedef std::shared_ptr<DataValue> DataValuePtr;

// ============================================================================
// struct: TaskData
// ============================================================================
struct TaskData {

	public:
		TaskData(){
			data= 0;
			name= "";
			id= "";
			status= Task::eUNKNOWN;
			is_utc= false;
			priority= Task::eMEDIUM_PRIORITY;
			creation_time= "";
			has_activation_time= false;
			activation_time= "";
			has_expiration_time= false;
			expiration_time= "";
			data_type= -1;
			data_format= -1;
			has_data= false;
		}

	public:
		template<typename T> 
		int GetData(T& value){
			if(!has_data || !data) return -1;
			try {
				std::shared_ptr<ScalarDataValue<T>> data_casted= std::dynamic_pointer_cast<ScalarDataValue<T>>(data);
				value= data_casted->Get();
			}
			catch(...){
				WARN_LOG("Failed to cast data value to scalar to retrieve value!");
				return -1;
			}
			return 0;
		}//close GetData()
	
		template<typename T> 
		int GetData(std::vector<T>& values){
			if(!has_data || !data) return -1;
			try {
				std::shared_ptr<SpectrumDataValue<T>> data_casted= std::dynamic_pointer_cast<SpectrumDataValue<T>>(data);
				values= data_casted->Get();
			}
			catch(...){
				WARN_LOG("Failed to cast data value to spectrum to retrieve value!");
				return -1;
			}
			return 0;
		}//close GetData()

	public:
		std::string name;
		std::string id;
		int priority;
		int status;
		bool is_utc;
		std::string creation_time;
		bool has_activation_time;
		std::string activation_time;
		bool has_expiration_time;
		std::string expiration_time;
		int data_type;
		int data_format;
		bool has_data;
		DataValuePtr data;	
};

// ============================================================================
// class: TaskUtils
// ============================================================================

class TaskUtils {
	public:
		/** 
		\brief Constructor
		*/
		TaskUtils(){};
		/** 
		\brief Destructor
		*/
		~TaskUtils(){};

	public:
		/** 
		\brief Parse task data from json task data string
		*/
		static int ParseFromJsonString(TaskData& task_data,Tango::DeviceImpl* dev,std::string& json_string);			
	
		/** 
		\brief Parse task data from json task data object
		*/
		static int ParseFromJson(TaskData& task_data,Tango::DeviceImpl* dev,Json::Value& json_obj);			
	
		/** 
		\brief Parse task data value from json task data object
		*/
		static int ParseDataFieldFromJson(TaskData& task_data,Tango::DeviceImpl* dev,Json::Value& json_obj);

	private:
		static DataValuePtr CreateBoolDataFromJson(Json::Value& root,int data_type);
		static DataValuePtr CreateFloatDataFromJson(Json::Value& root,int data_type);
		static DataValuePtr CreateDoubleDataFromJson(Json::Value& root,int data_type);
		static DataValuePtr CreateIntDataFromJson(Json::Value& root,int data_type);
		static DataValuePtr CreateStringDataFromJson(Json::Value& root,int data_type);
		

};//close class TaskUtils




}//close namespace 

#endif

