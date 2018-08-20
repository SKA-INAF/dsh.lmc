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
* @file TaskManager.cc
* @class TaskManager
* @brief TaskManager class
*
* TaskManager class
* @author S. Riggi
* @date 15/01/2016
*/

#include <TaskManager.h>

#ifndef _TASK_h
	#include <Task.h>
#endif

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


namespace Utils_ns {

bool TaskManager::m_wakeUpSignal;
bool TaskManager::m_endSignal;	

TaskManager::TaskManager(Tango::DeviceImpl* dev)
	: m_dev(dev)
{

	m_wakeUpSignal= false;
	m_endSignal= false;
	m_maxTasksInQueue= 30;
	m_maxTasksInCollection= 1000;
	m_defaultQueueWaitTime= 30;//seconds

}//close constructor

TaskManager::~TaskManager()
{

}//close destructor

int TaskManager::GetTaskStatusFromString(int& status,std::string status_str){

	if(status_str=="") return -1;
	
	status= -1;
	if(status_str=="UNKNOWN") status= Task::eUNKNOWN;
	else if(status_str=="INIT") status= Task::eINIT;
	else if(status_str=="IDLE") status= Task::eIDLE;
	else if(status_str=="RUNNING") status= Task::eRUNNING;
	else if(status_str=="COMPLETED") status= Task::eCOMPLETED;
	else if(status_str=="FAILED") status= Task::eFAILED;
	else if(status_str=="ABORTED") status= Task::eABORTED;
	else if(status_str=="EXPIRED") status= Task::eEXPIRED;
	else if(status_str=="CANCELED") status= Task::eCANCELED;
	else return -1;
	 
	return 0;

}//close GetTaskStatusFromString()


int TaskManager::GetTaskStatusString(std::string& status_str,int status){

	status_str= "";
	if(status==Task::eUNKNOWN) status_str= "UNKNOWN";
	else if(status==Task::eINIT) status_str= "INIT";
	else if(status==Task::eIDLE) status_str= "IDLE";
	else if(status==Task::eRUNNING) status_str= "RUNNING";
	else if(status==Task::eCOMPLETED) status_str= "COMPLETED";
	else if(status==Task::eFAILED) status_str= "FAILED";
	else if(status==Task::eABORTED) status_str= "ABORTED";
	else if(status==Task::eEXPIRED) status_str= "EXPIRED";
	else if(status==Task::eCANCELED) status_str= "CANCELED";
	else {
		return -1;
	}

	return 0;

}//close GetTaskStatusString()



void TaskManager::WakeUp(){
	
	std::lock_guard<std::mutex> lock(m_mutex);
	__DEBUG_LOG(m_dev,"Wake up signal throw...");
	m_wakeUpSignal = true;
	m_cond.notify_all();
	__DEBUG_LOG(m_dev,"Wake up done!");
	
}//close WakeUp()

void TaskManager::End(){

	std::lock_guard<std::mutex> lock(m_mutex);
	__DEBUG_LOG(m_dev,"End signal throw...");
	m_endSignal = true;
  m_cond.notify_all();
	__DEBUG_LOG(m_dev,"End signal done!");
	
}//close End()



void TaskManager::ExecuteTask(Task& task){

	//Execute task
	try{
		execute_task(task);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);

		//Set task status to FAILED (if not already done by the command internally) (mutex internally locked in SetStatus)
		std::string exception_reason(e.errors[0].reason.in());
		std::stringstream ss;
		ss<< "Tango exception occurred while executing "<<task.cmd_name<<" on device "<<m_dev->get_name()<<" (reason="<<exception_reason<<")!";
		__ERROR_LOG(m_dev,ss.str());

		//if(SetTaskStatus(task.id,task.cmd_name,Task::eFAILED,ss.str())<0){
		if(SetSequenceTaskStatus(task.id,task.cmd_name,Task::eFAILED,ss.str())<0){
			__WARN_LOG(m_dev,"Failed to set task status to FAILED!");
		}
		throw;
	}
	catch(...){
		//Set task status to FAILED (if not already done by the command internally) (mutex internally locked in SetStatus)
		std::stringstream ss;
		ss<< "Unknown exception occurred while executing "<<task.cmd_name<<" on device "<<m_dev->get_name()<<"!";
		__ERROR_LOG(m_dev,ss.str());

		//if(SetTaskStatus(task.id,task.cmd_name,Task::eFAILED,ss.str())<0){
		if(SetSequenceTaskStatus(task.id,task.cmd_name,Task::eFAILED,ss.str())<0){	
			__WARN_LOG(m_dev,"Failed to set task status to FAILED!");
		}
		throw;
	}

	//If task is executed with success update status
	//Case (synch task): task is completed so set status to COMPLETED
	//Case (asynch task): task started/was scheduled so set status to RUNNING
	if(task.is_asynch){
		std::stringstream ss;
		ss<< "Task "<<task.cmd_name<<" commanded in device "<<m_dev->get_name()<<" started with success";
		__DEBUG_LOG(m_dev,ss.str());

		//if(SetTaskStatus(task.id,task.cmd_name,Task::eRUNNING,ss.str())<0){	
		if(SetSequenceTaskStatus(task.id,task.cmd_name,Task::eRUNNING,ss.str())<0){
			__WARN_LOG(m_dev,"Failed to set task "<<task.cmd_name<<" status to RUNNING!");
		}
	}//close if
	else{
		std::stringstream ss;
		ss<< "Task "<<task.cmd_name<<" commanded in device "<<m_dev->get_name()<<" completed with success";	
		__DEBUG_LOG(m_dev,ss.str());
		//if(SetTaskStatus(task.id,task.cmd_name,Task::eCOMPLETED,ss.str())<0){
		if(SetSequenceTaskStatus(task.id,task.cmd_name,Task::eCOMPLETED,ss.str())<0){
			__WARN_LOG(m_dev,"Failed to set task "<<task.cmd_name<<" status to COMPLETED!");
		}
	}

}//close ExecuteTask()



int TaskManager::AddTaskSequence(TaskSequencePtr taskSequence,bool addToQueue){

	std::lock_guard<std::mutex> lock(m_mutex);

	//Check task sequence
	if(!taskSequence){
		__DEBUG_LOG(m_dev,"Null ptr to given task sequence, cannot add it!");
		return -1;
	}
		
	//Check if queue quota will not be exceeded
	int nTasksInSequence= taskSequence->GetNTasks();
	if(nTasksInSequence<=0){
		__DEBUG_LOG(m_dev,"No tasks present in sequence, nothing to be added!");
		return -1;
	}

	int nQueuedTasks= static_cast<int>(m_taskQueue.size());//Number of tasks present in the queue
	int nTasks= get_n_sequence_tasks();//Total number of tasks stored in collection (example: seq1 (2 task), seq2 (3 task) will return 5 tasks)
	//bool isQuotaReached= ( (nQueuedTasks+nTasksInSequence>m_maxTasksInQueue ) || (nTasks+1>m_maxTasksInCollection) );
	bool isQuotaReached= (nQueuedTasks+nTasksInSequence>m_maxTasksInQueue);
	if(isQuotaReached){
		__WARN_LOG(m_dev,"Maximum number of tasks in queue and/or in collection is already reached (or would be exceeded with this sequence), cannot add sequence!");
		return -1;
	}

	//Add each task present in sequence to the queue 
	if(addToQueue){
		for(int i=0;i<nTasksInSequence;i++){
			TaskPtr task= (taskSequence->tasks)[i];
			__DEBUG_LOG(m_dev,"Adding task "<<i+1<<" in sequence (id="<<task->id<<", name="<<task->cmd_name<<") to queue...");
	
			//In case of asynch attr check if progress attr is effectively present in device
			if( task->is_asynch && !TangoUtils::HasAttribute(m_dev,task->status_attr_name) ){
				__WARN_LOG(m_dev,"Device "<<m_dev->get_name()<<" has no task progress attribute "<<task->status_attr_name<<" expected for asynch task!");	
				return -1;
			}

			//Add in queue
			try {	
				m_taskQueue.insert(task);
			}
			catch(std::exception& e){
				__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while adding task in queue!");
				task->status= Task::eABORTED;
				return -1;
			}	
			catch(...){
				__WARN_LOG(m_dev,"Unknown exception occurred while adding task in queue!");
				task->status= Task::eABORTED;
				return -1;
			}	
		
			//Set task status to IDLE if everything if fine
			task->status= Task::eIDLE;	
	
			__DEBUG_LOG(m_dev,"Added task "<<task->cmd_name<<" to queue...");
			
		}//end loop tasks in sequence
	}//close if

	//Add task sequence in collection
	try {	
		m_taskSequences.push_back(taskSequence);
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while adding task sequence in collection!");
		return -1;
	}	
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while adding task sequence in collection!");
		return -1;
	}	


	//Emit signal to wake up any the waiting thread
	m_wakeUpSignal = true;
  m_cond.notify_all();

	__DEBUG_LOG(m_dev,"Queue size is "<<m_taskQueue.size());

	return 0;

}//close AddTaskSequence()



int TaskManager::RemoveTaskSequence(std::string name){

	std::unique_lock<std::mutex> mlock(m_mutex);

	//Remove task from queue 
	if(remove_sequence_tasks_in_queue(name)<0){
		__WARN_LOG(m_dev,"Failed to remove sequence task (seq_name="<<name<<") from queue!");
		return -1;
	}

	//Remove task from list
	if(remove_sequence_tasks(name)<0){
		__WARN_LOG(m_dev,"Failed to remove task sequence with name "<<name<<" from list!");
		return -1;
	}

	return 0;

}//close RemoveTaskSequence()


int TaskManager::RevokeTaskSequence(std::string name){

	std::unique_lock<std::mutex> mlock(m_mutex);

	//Remove tasks from the queue
	//NB: If they are in queue they are not still executed
	if(remove_sequence_tasks_in_queue(name)<0){
		__WARN_LOG(m_dev,"Failed to remove sequence task (seq_name="<<name<<") from queue!");
		return -1;
	}
	 
	//Revoke tasks in list
	//NB: This sets IDLE tasks to CANCELED and does nothing to RUNNING or FAILED tasks
	if(revoke_sequence_tasks(name)<0){
		__WARN_LOG(m_dev,"Failed to revoke task sequence with name "<<name<<" from list!");
		return -1;
	}	

	return 0;

}//close RevokeTaskSequence()


int TaskManager::RevokeTask(std::string name){

	std::unique_lock<std::mutex> mlock(m_mutex);

	//Remove tasks from the queue
	//NB: If they are in queue they are not still executed
	if(remove_tasks_in_queue(name)<0){
		__WARN_LOG(m_dev,"Failed to remove task (name="<<name<<") from queue!");
		return -1;
	}
	 
	//Revoke tasks in list
	//NB: This sets IDLE tasks to CANCELED and does nothing to RUNNING or FAILED tasks
	if(revoke_tasks(name)<0){
		__WARN_LOG(m_dev,"Failed to revoke task with name "<<name<<" from list!");
		return -1;
	}	

	return 0;

}//close RevokeTask()


int TaskManager::PopTask(Task& task) {
	
	std::unique_lock<std::mutex> mlock(m_mutex);

	//## If queue is empty wait until signalled
	__DEBUG_LOG(m_dev,"Waiting until signaled (queue size="<<m_taskQueue.size()<<", m_wakeUpSignal? "<<m_wakeUpSignal<<", m_endSignal? "<<m_endSignal<<")...");
	m_cond.wait(mlock,[this]{return (!m_taskQueue.empty() || m_wakeUpSignal || m_endSignal);});
	__DEBUG_LOG(m_dev,"Task manager awake...");
  	
	m_wakeUpSignal = false;

	if(m_endSignal){
		__DEBUG_LOG(m_dev,"End signal catched, exiting...");
		return -1;
	}
	if(m_taskQueue.empty()){
		__DEBUG_LOG(m_dev,"Empty queue...");
		return -1;
	}
	

	//## Remove all expired tasks from the queue
	if(remove_expired_tasks_in_queue()<0){
		__DEBUG_LOG(m_dev,"Failed to remove expired tasks present in queue...");
		return -1;
	}

	//## Get first executable task
	auto it= m_taskQueue.begin();
	auto topWaitingTaskIter= m_taskQueue.begin();
	auto runnableTaskIter= m_taskQueue.begin();
	bool foundRunnableTask= false;
	bool foundTaskWithActivationTimeout= false;
	

	for (it = m_taskQueue.begin(); it != m_taskQueue.end(); ++it) {
				
		//Check if task is runnable
		TaskPtr thisTask= *it;

		__DEBUG_LOG(m_dev,"Check if task "<<thisTask->cmd_name<<" can be popped from the queue...");

		bool isSequenceTaskRunnable= is_sequence_task_runnable(thisTask);
		bool isRunnable= (
			thisTask->IsRunnable() &&
			isSequenceTaskRunnable	
		);
		if(isRunnable){
			foundRunnableTask= true;
			runnableTaskIter= it;//Store task iterator
			break;
		} 
		__DEBUG_LOG(m_dev,"Task "<<thisTask->cmd_name<<" isRunnable? "<<thisTask->IsRunnable()<<" isSequenceTaskRunnable? "<<isSequenceTaskRunnable<<"...");
 

		//Check if task is timed out
		bool isTimedOut= thisTask->IsExpirationReached();
		if(isTimedOut) {//Mark task as expired and to be removed from the queue
			std::stringstream ss;
			ss<<"Task "<<thisTask->cmd_name<<" is timed-out, tag as EXPIRED and skip to next task in queue...";
			__WARN_LOG(m_dev,ss.str());
			thisTask->SetStatus(Task::eEXPIRED,"Task is found timed-out while processing the queue");
			continue;//skip to next task in queue
		}

		//Check if task has activation time
		bool isWaitingForActivation= thisTask->IsWaitingForActivation();
		if(isWaitingForActivation && !foundTaskWithActivationTimeout){//store first activation time
			topWaitingTaskIter= it;//Store task iterator
			foundTaskWithActivationTimeout= true;
			continue;//skip to next task in queue
		}  

	}//end loop tasks in queue

	__DEBUG_LOG(m_dev,"foundRunnableTask="<<foundRunnableTask<<", foundTaskWithActivationTimeout="<<foundTaskWithActivationTimeout);

	//## Return popped task if a runnable is found 
	if(foundRunnableTask){	
		task= *(*runnableTaskIter);
		__DEBUG_LOG(m_dev,"Task (name="<<task.cmd_name<<", id="<<task.id<<", status="<<task.status<<", hasActivation? "<<task.hasActivationTime<<", hasExpiration? "<<task.hasExpirationTime<<") popped from the queue...");
		
		m_taskQueue.erase(runnableTaskIter);
		return 0;
	}

	//## Wait until first activation time or until any signal (wakeup/end)
	//## If no activation time was found return immediately (with a failure code)
	if(foundTaskWithActivationTimeout){

		double time_to_expiration_sec= ((*topWaitingTaskIter)->activation_timeout)->time_to_expiration();
		auto tdiff= std::chrono::duration<double,std::milli>( time_to_expiration_sec*1000. );

		if( m_cond.wait_for(mlock,tdiff, [](){return (m_wakeUpSignal || m_endSignal);}) ){
			__DEBUG_LOG(m_dev,"Finished waiting (signaled)...");
			if(m_wakeUpSignal) m_wakeUpSignal= false;
		}
		else {
			__DEBUG_LOG(m_dev,"Finished waiting (Timeout reached)...");
		}
	
		__DEBUG_LOG(m_dev,"Awake after timeout...");
	}//close if

	__DEBUG_LOG(m_dev,"No task popped from the queue!");

	return -1;

}//close PopTask()



int TaskManager::FlushQueue(){
	
	std::unique_lock<std::mutex> mlock(m_mutex);
	return flush_queue();

}//close FlushQueue()



int TaskManager::SetSequenceTaskStatus(std::string id,std::string name,int status,std::string status_info){

	std::unique_lock<std::mutex> mlock(m_mutex);
	return set_sequence_task_status(id,name,status,status_info);

}//close SetTaskStatus()

int TaskManager::SetAsynchTaskStatus(std::string attr_name,int status,std::string status_info){

	std::unique_lock<std::mutex> mlock(m_mutex);

	//Iterate over tasks sequence and set asynch status
	//NB: If no task is found with matching name do nothing
	for(unsigned int i=0;i<m_taskSequences.size();i++){
		m_taskSequences[i]->SetAsynchTaskStatus(attr_name,status,status_info);
	}

	return 0;

}//close SetAsynchTaskStatus()


int TaskManager::GetQueuedTaskInfo(std::vector<std::string>& info){
			
	std::lock_guard<std::mutex> lock(m_mutex);
	for(auto it : m_taskQueue) {
 		std::string task_info_str= "";
		if(it->EncodeToJsonString(task_info_str)<0){
			__WARN_LOG(m_dev,"Failed to get stringified task info!");
			return -1;
		}
		info.push_back(task_info_str);
	}//end loop tasks in queue 

	return 0;

}//close GetQueuedTaskInfo()



int TaskManager::GetSequenceTaskInfo(std::vector<std::string>& info){
			
	std::lock_guard<std::mutex> lock(m_mutex);
	for(unsigned int j=0;j<m_taskSequences.size();j++) {
		TaskSequencePtr thisSequencePtr= m_taskSequences[j];
		for(int i=0;i<thisSequencePtr->GetNTasks();i++){
 			std::string task_info_str= "";
			if( (thisSequencePtr->tasks)[i]->EncodeToJsonString(task_info_str)<0){
				__WARN_LOG(m_dev,"Failed to get stringified task info!");
				return -1;
			}
			info.push_back(task_info_str);
		}//end loop tasks in sequence
	}//end loop task sequences

	return 0;

}//close GetSequenceTaskInfo()


int TaskManager::ClearAllSequenceTasks(){
		
	std::lock_guard<std::mutex> lock(m_mutex);
			
	if(m_taskSequences.empty()) return 0;
			
	try {
		m_taskSequences.erase( m_taskSequences.begin(), m_taskSequences.end() );
	}
	catch(...){
		__WARN_LOG(m_dev,"Exception while deleting task sequences from list!");
		return -1;
	}
			
	return 0;

}//close ClearAllSequenceTasks()


int TaskManager::ClearSequenceTasks(long int historyTimeThr,bool keepRunningTasks, bool useUTC){
		
	std::lock_guard<std::mutex> lock(m_mutex);
			
	if(m_taskSequences.empty()) return 0;
			
	yat::Time now;
	now.set_current(useUTC);

	try {
		if(keepRunningTasks) {//preserve RUNNING tasks
			m_taskSequences.erase( std::remove_if(m_taskSequences.begin(), m_taskSequences.end(), MatchOldSequenceTask(Task::eRUNNING,now,historyTimeThr,true) ), m_taskSequences.end() );
		}
		else {
			m_taskSequences.erase( std::remove_if(m_taskSequences.begin(), m_taskSequences.end(), MatchOldSequenceTask(-1,now,historyTimeThr,false) ), m_taskSequences.end() );
		}
	}
	catch(...){
		__WARN_LOG(m_dev,"Exception while deleting task from list!");
		return -1;
	}
			
	return 0;

}//close ClearSequenceTasks()


int TaskManager::ClearCompletedSequenceTasks(long int historyTimeThr,bool useUTC){
		
	std::lock_guard<std::mutex> lock(m_mutex);
			
	if(m_taskSequences.empty()) return 0;
			
	yat::Time now;
	now.set_current(useUTC);

	try {
		m_taskSequences.erase( std::remove_if(m_taskSequences.begin(), m_taskSequences.end(), MatchOldSequenceTask(Task::eCOMPLETED,now,historyTimeThr,true) ), m_taskSequences.end() );
	}
	catch(...){
		__WARN_LOG(m_dev,"Exception while deleting task from list!");
		return -1;
	}
			
	return 0;

}//close ClearCompletedSequenceTasks()


int TaskManager::GetTaskSequenceStatusInfo(std::vector<TaskInfo>& status_info, bool useDevAttrFilter, std::string dev_attr_name){

	std::lock_guard<std::mutex> lock(m_mutex);

	status_info.clear();
	if(useDevAttrFilter){
		std::vector<TaskSequencePtr> matched_task_sequence;
		if(find_task_sequences_by_dev_attr(matched_task_sequence,dev_attr_name)<0){
			__WARN_LOG(m_dev,"Failed to retrieve task sequence with dev attr name specified!");
			return -1;
		}
		
		for(unsigned int i=0;i<matched_task_sequence.size();i++){
			double creation_timestamp= (matched_task_sequence[i]->creation_time).double_unix();
			std::string seq_name= matched_task_sequence[i]->seq_name;
			int status= matched_task_sequence[i]->status;
			std::string info= matched_task_sequence[i]->status_info;
			status_info.push_back( TaskInfo(seq_name,status,info,creation_timestamp) );
		}//end loop matched sequence

	}//close if
	else{
		for(unsigned int i=0;i<m_taskSequences.size();i++){
			double creation_timestamp= (m_taskSequences[i]->creation_time).double_unix();
			std::string seq_name= m_taskSequences[i]->seq_name;
			int status= m_taskSequences[i]->status;
			std::string info= m_taskSequences[i]->status_info;

			status_info.push_back(TaskInfo(seq_name,status,info,creation_timestamp) ); 
		}//end loop matched sequence
	}//close else

	return 0;

}//close GetTaskSequenceStatusInfo()

int TaskManager::FindTasksInQueue(std::vector<TaskPtr>& matched_tasks,std::string id,std::string name){

	std::lock_guard<std::mutex> lock(m_mutex);
	return find_tasks_in_queue(matched_tasks,id,name);

}//close FindTasksInQueue()

// ============================================================================
//                    PRIVATE METHODS
// ============================================================================
void TaskManager::execute_task(Task& task){

	//## Execute handler command
	CORBA::Any* argout= 0;
	try {
		argout= task.Run();
	}	
	catch(...){
		throw;
	}
 	
	//## Process cmd response
	if(argout){
		//...
		//...
	}

}//close execute_task()



TaskPtr TaskManager::find_task_in_queue(std::string id,std::string name){

	if(m_taskQueue.empty()) return nullptr;
	std::set<TaskPtr>::iterator it= std::find_if(m_taskQueue.begin(),m_taskQueue.end(), MatchTask(id,name));
	if(it==m_taskQueue.end()) return nullptr;//not found in collection 
	return (*it);

}//close find_task_in_queue()


int TaskManager::find_tasks_in_queue(std::vector<TaskPtr>& matched_tasks,std::string id,std::string name){

	matched_tasks.clear();
	try {
		std::copy_if(m_taskQueue.begin(), m_taskQueue.end(), std::back_inserter(matched_tasks), MatchTask(id,name));
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while finding all matching task occurrences in queue!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while finding all matching task occurrences in queue!");
		return -1;
	}
	return 0;

}//close find_tasks_in_queue()


int TaskManager::remove_sequence_tasks(std::string name){

	if(m_taskSequences.empty()) return 0;
	try {
		m_taskSequences.erase(std::remove_if(m_taskSequences.begin(), m_taskSequences.end(), MatchTaskSequenceByName(name)), m_taskSequences.end());
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while removing all matching task sequence occurrences!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while removing all matching task sequence occurrences!");
		return -1;
	}

	return 0;
	
}//close remove_sequence_tasks()


int TaskManager::revoke_sequence_tasks(std::string name){

	for(size_t i=0;i<m_taskSequences.size();i++){
		std::string seq_name= m_taskSequences[i]->seq_name;
		if(seq_name!=name) continue;
		
		//Set IDLE tasks to CANCELED
		bool hasCanceledTasks= false;
		for(size_t j=0;j<(m_taskSequences[i]->tasks).size();j++){
			int status= ((m_taskSequences[i]->tasks)[j])->status;
			if(status!=Task::eIDLE) continue;

			//Set status info
			((m_taskSequences[i]->tasks)[j])->SetStatus(Task::eCANCELED,"Task was canceled");
			hasCanceledTasks= true;

		}//end loop tasks in sequence
	
		//Update sequence status
		if(hasCanceledTasks){
			m_taskSequences[i]->ComputeStatus();
		}

	}//end loop sequence tasks

	return 0;

}//close revoke_sequence_tasks()


int TaskManager::revoke_tasks(std::string name){

	for(size_t i=0;i<m_taskSequences.size();i++){
		std::string seq_name= m_taskSequences[i]->seq_name;
		
		//Set IDLE tasks to CANCELED
		bool hasCanceledTasks= false;
		for(size_t j=0;j<(m_taskSequences[i]->tasks).size();j++){
			int status= ((m_taskSequences[i]->tasks)[j])->status;
			std::string task_name= ((m_taskSequences[i]->tasks)[j])->cmd_name;
			if(status!=Task::eIDLE || task_name!=name) continue;

			//Set status info
			((m_taskSequences[i]->tasks)[j])->SetStatus(Task::eCANCELED,"Task was canceled");
			hasCanceledTasks= true;

		}//end loop tasks in sequence
	
		//Update sequence status
		if(hasCanceledTasks){
			m_taskSequences[i]->ComputeStatus();
		}

	}//end loop sequence tasks

	return 0;

}//close revoke_tasks()



int TaskManager::remove_tasks_in_queue(std::string id,std::string name){

	try {
		std::set<TaskPtr>::iterator it = m_taskQueue.begin();
		while ((it = std::find_if(it, m_taskQueue.end(), MatchTask(id,name))) != m_taskQueue.end()){
    	it = m_taskQueue.erase(it);
		}
		//m_taskQueue.erase(std::remove_if(m_taskQueue.begin(), m_taskQueue.end(), MatchTask(id,name)), m_taskQueue.end());//This cannot be used as set is ordered!!!
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while removing all matching task occurrences in queue!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while removing all matching task occurrences in queue!");
		return -1;
	}

	return 0;

}//close remove_tasks_in_queue()


int TaskManager::remove_tasks_in_queue(std::string name){

	try {
		std::set<TaskPtr>::iterator it = m_taskQueue.begin();
		while ((it = std::find_if(it, m_taskQueue.end(), MatchTaskByName(name))) != m_taskQueue.end()){
    	it = m_taskQueue.erase(it);
		}
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while removing all matching task occurrences in queue!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while removing all matching task occurrences in queue!");
		return -1;
	}

	return 0;

}//close remove_tasks_in_queue()


int TaskManager::remove_sequence_tasks_in_queue(std::string name){

	try {
		std::set<TaskPtr>::iterator it = m_taskQueue.begin();
		while ((it = std::find_if(it, m_taskQueue.end(), MatchTaskBySequenceName(name))) != m_taskQueue.end()){
    	it = m_taskQueue.erase(it);
		}
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while removing all matching sequence task occurrences in queue!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while removing all matching sequence task occurrences in queue!");
		return -1;
	}

	return 0;

}//close remove_sequence_tasks_in_queue()



int TaskManager::remove_expired_tasks_in_queue(){

	__DEBUG_LOG(m_dev,"Removing expired tasks in queue...");
	int nTasksRemoved= 0;
	try {
		std::set<TaskPtr>::iterator it = m_taskQueue.begin();
		while ((it = std::find_if(it, m_taskQueue.end(), MatchTaskByStatus(Task::eEXPIRED))) != m_taskQueue.end()){
    	it = m_taskQueue.erase(it);
			nTasksRemoved++;
		}
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while removing all matching EXPIRED task occurrences in queue!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while removing all matching EXPIRED task occurrences in queue!");
		return -1;
	}

	__DEBUG_LOG(m_dev,"Removed "<<nTasksRemoved<<" expired tasks in queue...");

	return 0;

}//close remove_expired_tasks_in_queue()



int TaskManager::flush_queue(){
	
	//Erase all tasks in queue
	try{
		m_taskQueue.erase(m_taskQueue.begin(),m_taskQueue.end());
		//m_monitorQueue.erase(m_monitorQueue.begin(),m_monitorQueue.end());
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while removing all items in queues!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while removing all items in queues!");
		return -1;
	}

	/*
	//Erase all IDLE tasks in list
	if(remove_idle_tasks()<0){
		__WARN_LOG(m_dev,"Failed to remove IDLE tasks from list!");
		return -1;
	}
	*/

	return 0;

}//close flush_queue()


bool TaskManager::is_task_quota_reached(){

	int nQueuedTasks= static_cast<int>(m_taskQueue.size());//Number of tasks present in the queue
	int nTasks= get_n_sequence_tasks();//Total number of tasks stored in collection (example: seq1 (2 task), seq2 (3 task) will return 5 tasks)
	bool isQuotaReached= ( (nQueuedTasks>m_maxTasksInQueue ) || (nTasks>m_maxTasksInCollection) );

	return isQuotaReached;
	
}//close is_task_quota_reached()


int TaskManager::set_sequence_task_status(std::string id,std::string name,int status,std::string status_info){

	//Find tasks in sequence
	bool isFound= false;
	for(unsigned int i=0;i<m_taskSequences.size();i++){
		int index= -1;
		TaskPtr task= m_taskSequences[i]->FindTask(index,id,name);
		if(!task) continue;

		//Set task found
		isFound= true;
		
		//Set status info
		task->SetStatus(status,status_info);

		//Update sequence status
		bool is_in_sequence= task->is_in_sequence;
		if(is_in_sequence){
			m_taskSequences[i]->ComputeStatus();
		}

	}//end loop sequence tasks
	
	//Check if found
	if(!isFound){
		__WARN_LOG(m_dev,"Cannot find task "<<name<<" (id="<<id<<") in list!");
		return -1;
	}

	return 0;

}//close set_sequence_task_status()

int TaskManager::find_sequence_tasks_in_queue(std::vector<TaskPtr>& matched_tasks,std::string seq_name){

	matched_tasks.clear();
	try {
		std::copy_if(m_taskQueue.begin(), m_taskQueue.end(), std::back_inserter(matched_tasks), MatchTaskBySequenceName(seq_name));
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while finding all matching task occurrences in queue!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while finding all matching task occurrences in queue!");
		return -1;
	}
	return 0;

}//close find_sequence_tasks_in_queue()



TaskSequencePtr TaskManager::find_task_sequence(int& index,std::string seq_id,std::string seq_name){

	index= -1;
	if(m_taskSequences.empty()) return nullptr;
	auto it = std::find_if(m_taskSequences.begin(), m_taskSequences.end(), MatchTaskSequenceByIdAndName(seq_id,seq_name));
	if (it==m_taskSequences.end()) return nullptr;//not found in collection
	index = static_cast<int>(it-m_taskSequences.begin());
	return m_taskSequences[index];

}//close find_task_sequence()


TaskPtr TaskManager::find_task_in_sequence(std::string id,std::string name,std::string seq_id,std::string seq_name){

	//Find task sequence
	int index= -1;
	TaskSequencePtr taskSequence= find_task_sequence(index,seq_id,seq_name);
	if(!taskSequence){
		__WARN_LOG(m_dev,"Cannot find task sequence (seq_id="<<seq_id<<", seq_name="<<seq_name<<")!");
		return nullptr;
	}

	//Find task in sequence
	int index_in_sequence= -1;
	return taskSequence->FindTask(index_in_sequence,id,name);

}//close find_task_in_sequence()

int TaskManager::find_task_sequences_by_dev_attr(std::vector<TaskSequencePtr>& matched_task_sequences,std::string dev_attr_name){

	matched_task_sequences.clear();
	try {
		std::copy_if(m_taskSequences.begin(), m_taskSequences.end(), std::back_inserter(matched_task_sequences), MatchTaskSequenceByDevAttrName(dev_attr_name));
	}
	catch(std::exception& e){
		__WARN_LOG(m_dev,"C++ exception (err="<<e.what()<<") occurred while finding all matching task sequences having a dev attr name!");
		return -1;
	}
	catch(...){
		__WARN_LOG(m_dev,"Unknown exception occurred while finding all matching task sequences having a dev attr name!");
		return -1;
	}
	return 0;

}//close find_task_sequences_by_dev_attr()


bool TaskManager::is_sequence_task_runnable(TaskPtr aTask){

	if(!aTask) return false;
	if(!aTask->is_in_sequence) {
		__DEBUG_LOG(m_dev,"Task runnable as not a sequence task!");	
		return true;
	}
	if(!aTask->depend_on_sequence_tasks || aTask->dep_task_seq_indexes.empty()) {
		__DEBUG_LOG(m_dev,"Task runnable as not depending on other tasks in sequence!");	
		return true;
	}
	
	//Find task sequence
	int index= -1;
	TaskSequencePtr taskSequence= find_task_sequence(index,aTask->sequence_id, aTask->sequence_name);
	if(!taskSequence){
		__ERROR_LOG(m_dev,"Failed to retrieve task sequence (id="<<aTask->sequence_id<<", name="<<aTask->sequence_name<<") in collection (not expected error)!");
		return false;
	}

	//Check if dependent tasks are already completed
	bool isRunnable= true;
	int nTasksInSequence= taskSequence->GetNTasks();
	int nDepTasks= static_cast<int>( (aTask->dep_task_seq_indexes).size() );
	__DEBUG_LOG(m_dev,"Task "<<aTask->cmd_name<<" depends on "<<nDepTasks<<" sequence tasks");
	
	for(unsigned int i=0;i<(aTask->dep_task_seq_indexes).size();i++){
		int dep_task_seq_index= (aTask->dep_task_seq_indexes)[i];
		if(dep_task_seq_index<0 || dep_task_seq_index>=nTasksInSequence){
			__ERROR_LOG(m_dev,"Invalid dependent task index (valid range is [0,"<<nTasksInSequence-1<<"]...check how you created the task sequence!!!");
			isRunnable= false;
			break;
		}

		int dep_task_status= ((taskSequence->tasks)[dep_task_seq_index])->status;
		
		if(dep_task_status!=Task::eCOMPLETED) {
			__DEBUG_LOG(m_dev,"Task "<<aTask->cmd_name<<" cannot be run as it depends on task n. "<<dep_task_seq_index<<" in sequence with status "<<dep_task_status);
			isRunnable= false;
			break;
		}
	}//end loop dependend tasks

	return isRunnable;

}//close is_sequence_task_runnable()


int TaskManager::find_tasks_by_watched_attr_name(std::vector<TaskPtr>& matching_tasks,std::string attr_name){
	
	//Loop over task sequence and find asynch tasks with watched attr name 
	matching_tasks.clear();
	for(unsigned int i=0;i<m_taskSequences.size();i++){
		int index= -1;
		TaskPtr task= m_taskSequences[i]->FindAsynchTaskByWatchedAttrName(index,attr_name);
		if(!task) continue;
		matching_tasks.push_back(task);
	}//end loop sequence tasks

	return 0;

}//close find_tasks_by_watched_attr_name()


}//close namespace


