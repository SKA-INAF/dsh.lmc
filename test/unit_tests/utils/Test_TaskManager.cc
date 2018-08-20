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
* @file Test_TaskManager.cc
* @class Test_TaskManager
* @brief Class for testing TaskManager class
*
* Test class for TaskManager class
* @author S. Riggi
* @date 15/01/2016
*/


#include <Test_TaskManager.h>
#include <TaskManager.h>
#include <Task.h>

#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

#include <tango.h>


namespace Utils_ns {

TaskManager* TaskManagerTest::m_taskManager = nullptr;
bool TaskManagerTest::m_setUpFailed= false;

//===========================================
//==           DEFINE TESTS              ====
//===========================================
TEST_F(TaskManagerTest, TestAddTaskToQueueOnce)
{
	//Check task manager instance
	if(!m_taskManager){
		FAIL() << "TaskManager is not instantiated in test class, cannot execute test!";
	}
	if(m_taskManager->GetNTasksInQueue()!=0){
		FAIL() << "Task queue is not empty!";
	}

	bool useUTC= false;
	int priority= Utils_ns::Task::eMEDIUM_PRIORITY;

	//Create two equal tasks
	//- No activation times
	//- Same name & id
	//- Same expiration time
	std::string task_name= "MyTask";
	std::string task_id= "abscd";	
	long int task_duration= 30;
	yat::Time task_expirationTime;
	task_expirationTime.set_current(useUTC);
	task_expirationTime+= static_cast<double>(task_duration);

	TaskPtr task1= std::make_shared<Task>(nullptr,task_name,priority,useUTC);
	task1->id= task_id; 
	task1->SetExpirationTime(task_expirationTime);

	TaskPtr task2= std::make_shared<Task>(nullptr,task_name,priority,useUTC);
	task2->id= task_id; 
	task2->SetExpirationTime(task_expirationTime);
	
	//Create task sequences with the two tasks
	TaskSequencePtr taskSeq1= std::make_shared<TaskSequence>(nullptr,"TaskSequence",useUTC);
	taskSeq1->AddTask(task1);

	TaskSequencePtr taskSeq2= std::make_shared<TaskSequence>(nullptr,"TaskSequence",useUTC);
	taskSeq2->AddTask(task2);

	//Test task comparators
	//- priorities
	ASSERT_EQ(task1->priority,task2->priority);
	ASSERT_EQ( (task1->priority > task2->priority), (task2->priority > task1->priority) );

	//- expiration times
	bool areExpTimeEqual= (task1->expiration_time == task2->expiration_time);
	ASSERT_TRUE(areExpTimeEqual);
	ASSERT_EQ( (task1->expiration_time < task2->expiration_time), (task2->expiration_time < task1->expiration_time) );

	//- name & id
	bool compareByNameAndId_12= (std::tie(task1->cmd_name,task1->id) < std::tie(task2->cmd_name,task2->id));
	bool compareByNameAndId_21= (std::tie(task2->cmd_name,task2->id) < std::tie(task1->cmd_name,task1->id));
	ASSERT_EQ(compareByNameAndId_12,compareByNameAndId_21);

	//Add task sequences to manager
	bool addToQueue= true;
	m_taskManager->AddTaskSequence(taskSeq1,addToQueue);
	m_taskManager->AddTaskSequence(taskSeq2,addToQueue);
	
	//Only one task should be present in queue
	std::vector<TaskPtr> matched_tasks;
	m_taskManager->FindTasksInQueue(matched_tasks,task_id,task_name);
	int nFoundTasks= static_cast<int>(matched_tasks.size());	

	EXPECT_EQ(nFoundTasks,1);
	
}//close test

TEST_F(TaskManagerTest, TestSameSequenceTaskOrderingInQueue)
{
	//Check task manager instance
	if(!m_taskManager){
		FAIL() << "TaskManager is not instantiated in test class, cannot execute test!";
	}
	if(m_taskManager->GetNTasksInQueue()!=0){
		FAIL() << "Task queue is not empty!";
	}

	bool useUTC= false;

	//## Create tasks
	//- TaskSequence: 
	//     + task 1: tstart= 30s (max duration=20s)
	//     + task 2: depend on task 1 (max duration=5s)
	//     + task 3: priority=LOW (max duration=20s)
	//     + task 4: depend on task 2 (max duration=20s)
	//     + task 5: tstart= 120 (max duration=20s)
	//     + task 6: depend on task 5
	//     + task 7: priority=HIGH (max duration=20s)
	//     + task 8: priority=MEDIUM (max duration=20s)
	//		 + task 9: depend on task 1 (max duration=20s)
	std::string task1_name= "task1";
	int task1_priority= Task::eMEDIUM_PRIORITY;
	int task1_delay= 30000;//ms
	int task1_duration= 20000;//ms
	TaskPtr task1= std::make_shared<Task>(nullptr,task1_name,task1_priority,useUTC);
	task1->SetTimeDelayAndDuration(task1_delay,task1_duration);

	std::string task2_name= "task2";
	int task2_priority= Task::eMEDIUM_PRIORITY;
	int task2_duration= 80000;//ms
	TaskPtr task2= std::make_shared<Task>(nullptr,task2_name,task2_priority,useUTC);
	task2->SetMaxDuration(task2_duration);

	std::string task3_name= "task3";
	int task3_priority= Task::eLOW_PRIORITY;
	int task3_duration= 20000;//ms
	TaskPtr task3= std::make_shared<Task>(nullptr,task3_name,task3_priority,useUTC);
	task3->SetMaxDuration(task3_duration);

	std::string task4_name= "task4";
	int task4_priority= Task::eMEDIUM_PRIORITY;
	int task4_duration= 20000;//ms
	TaskPtr task4= std::make_shared<Task>(nullptr,task4_name,task4_priority,useUTC);
	task4->SetMaxDuration(task4_duration);

	std::string task5_name= "task5";
	int task5_priority= Task::eMEDIUM_PRIORITY;
	int task5_delay= 120000;//ms
	int task5_duration= 20000;//ms
	TaskPtr task5= std::make_shared<Task>(nullptr,task5_name,task5_priority,useUTC);
	task5->SetTimeDelayAndDuration(task5_delay,task5_duration);

	std::string task6_name= "task6";
	int task6_priority= Task::eMEDIUM_PRIORITY;
	int task6_duration= 20000;//ms
	TaskPtr task6= std::make_shared<Task>(nullptr,task6_name,task6_priority,useUTC);
	task6->SetMaxDuration(task6_duration);
	
	std::string task7_name= "task7";
	int task7_priority= Task::eHIGH_PRIORITY;
	int task7_duration= 20000;//ms
	TaskPtr task7= std::make_shared<Task>(nullptr,task7_name,task7_priority,useUTC);
	task7->SetMaxDuration(task7_duration);

	std::string task8_name= "task8";
	int task8_priority= Task::eMEDIUM_PRIORITY;
	int task8_duration= 20000;//ms
	TaskPtr task8= std::make_shared<Task>(nullptr,task8_name,task8_priority,useUTC);
	task8->SetMaxDuration(task8_duration);

	std::string task9_name= "task9";
	int task9_priority= Task::eMEDIUM_PRIORITY;
	int task9_duration= 20000;//ms
	TaskPtr task9= std::make_shared<Task>(nullptr,task9_name,task9_priority,useUTC);
	task9->SetMaxDuration(task9_duration);
	
	std::string taskSeq_name= "TS";
	TaskSequencePtr taskSeq= std::make_shared<TaskSequence>(nullptr,taskSeq_name,useUTC);
	ASSERT_EQ(taskSeq->AddTask(task1),0);
	ASSERT_EQ(taskSeq->AddTask(task2),0);
	ASSERT_EQ(taskSeq->AddTask(task3),0);
	ASSERT_EQ(taskSeq->AddTask(task4),0);
	ASSERT_EQ(taskSeq->AddTask(task5),0);
	ASSERT_EQ(taskSeq->AddTask(task6),0);
	ASSERT_EQ(taskSeq->AddTask(task7),0);
	ASSERT_EQ(taskSeq->AddTask(task8),0);
	ASSERT_EQ(taskSeq->AddTask(task9),0);

	//## Set dependent indexes
	std::vector<std::vector<size_t>> dep_indexes { 
		{},
		{0},
		{},
		{1},
		{},
		{4}, 
		{},
		{},
		{0}
	};
	ASSERT_EQ(taskSeq->SetTaskDependency(dep_indexes),0);

	//## Add task sequences to manager
	bool addToQueue= true;
	ASSERT_EQ(m_taskManager->AddTaskSequence(taskSeq,addToQueue),0);
	
	//## Expect task added according to precise order
	TaskQueue task_queue= m_taskManager->GetTaskQueue();
	std::vector<std::string> task_names;
	std::vector<std::string> exp_task_names {
		task7_name,
		task8_name,
		task3_name,		
		task1_name,
		task9_name,
		task2_name,
		task5_name,	
		task4_name,
		task6_name
	};
	
	//::testing::internal::CaptureStderr();
	std::stringstream ss;
	ss<<"task_names {";
	for(auto it : task_queue) {
  	std::string task_name= it->cmd_name;
		task_names.push_back(task_name);
		ss<<task_name<<" {tstart(?"<<it->hasActivationTime<<")="<<(it->activation_time).long_unix()<<", tstart^exp(?"<<it->depend_on_sequence_tasks<<")="<<(it->exp_max_activation_time).long_unix()<<", tstop="<<(it->expiration_time).long_unix()<<", tstop^exp(?"<<it->depend_on_sequence_tasks<<")="<<(it->exp_max_expiration_time).long_unix()<<"}, ";
	}  
	ss<<"}";
	cerr<<ss.str()<<endl;
	//std::string out_msg = ::testing::internal::GetCapturedStderr();
	
	EXPECT_THAT(task_names, ::testing::ContainerEq(exp_task_names)) << "Tasks are added to queue in a wrong order!";


}//close TestSameSequenceTaskOrderingInQueue 


TEST_F(TaskManagerTest, TestAddTaskToQueueOrdering)
{
	//Check task manager instance
	if(!m_taskManager){
		FAIL() << "TaskManager is not instantiated in test class, cannot execute test!";
	}
	if(m_taskManager->GetNTasksInQueue()!=0){
		FAIL() << "Task queue is not empty!";
	}

	bool useUTC= false;
	yat::Time time_ref;
	time_ref.set_current(useUTC);
	
	//## Create tasks
	//- TaskSequence 1: 
	//     + task 1: No activation
	//     + task 2: with activation delay
	std::string task11_name= "task11";
	int task11_priority= Task::eMEDIUM_PRIORITY;
	int task11_duration= 30000;//ms
	TaskPtr task11= std::make_shared<Task>(nullptr,task11_name,task11_priority,useUTC);
	task11->SetMaxDuration(task11_duration,Task::eEXTERNAL_TIME,time_ref);

	std::string task12_name= "task12";
	int task12_priority= Task::eMEDIUM_PRIORITY;
	int task12_duration= 30000;//ms
	int task12_delay= 10000;//ms
	TaskPtr task12= std::make_shared<Task>(nullptr,task12_name,task12_priority,useUTC);
	task12->SetTimeDelayAndDuration(task12_delay,task12_duration,Task::eEXTERNAL_TIME,time_ref);
	
	std::string taskSeq1_name= "TS1";
	TaskSequencePtr taskSeq1= std::make_shared<TaskSequence>(nullptr,taskSeq1_name,useUTC);
	ASSERT_EQ(taskSeq1->AddTask(task11),0);
	ASSERT_EQ(taskSeq1->AddTask(task12),0);

	//- TaskSequence 2: 
	//     + task 1: No activation, high priority
	std::string task21_name= "task21";
	int task21_priority= Task::eHIGH_PRIORITY;
	int task21_duration= 60000;//ms
	TaskPtr task21= std::make_shared<Task>(nullptr,task21_name,task21_priority,useUTC);
	task21->SetMaxDuration(task21_duration,Task::eEXTERNAL_TIME,time_ref);

	std::string taskSeq2_name= "TS2";
	TaskSequencePtr taskSeq2= std::make_shared<TaskSequence>(nullptr,taskSeq2_name,useUTC);
	ASSERT_EQ(taskSeq2->AddTask(task21),0);

	//- TaskSequence 3: 
	//     + task 1: no activation 
	//     + task 2: with activation delay
	//     + task 3: with activation delay
	std::string task31_name= "task31";
	int task31_priority= Task::eLOW_PRIORITY;
	int task31_duration= 30000;//ms
	TaskPtr task31= std::make_shared<Task>(nullptr,task31_name,task31_priority,useUTC);
	task31->SetMaxDuration(task31_duration,Task::eEXTERNAL_TIME,time_ref);

	std::string task32_name= "task32";
	int task32_priority= Task::eMEDIUM_PRIORITY;
	int task32_duration= 10000;//ms
	int task32_delay= 10000;//ms
	TaskPtr task32= std::make_shared<Task>(nullptr,task32_name,task32_priority,useUTC);
	task32->SetTimeDelayAndDuration(task32_delay,task32_duration,Task::eEXTERNAL_TIME,time_ref);
	
	std::string task33_name= "task33";
	int task33_priority= Task::eMEDIUM_PRIORITY;
	int task33_duration= 60000;//ms
	int task33_delay= 10000;//ms
	TaskPtr task33= std::make_shared<Task>(nullptr,task33_name,task33_priority,useUTC);
	task33->SetTimeDelayAndDuration(task33_delay,task33_duration,Task::eEXTERNAL_TIME,time_ref);
	
	std::string taskSeq3_name= "TS3";
	TaskSequencePtr taskSeq3= std::make_shared<TaskSequence>(nullptr,taskSeq3_name,useUTC);
	ASSERT_EQ(taskSeq3->AddTask(task31),0);	
	ASSERT_EQ(taskSeq3->AddTask(task32),0);
	ASSERT_EQ(taskSeq3->AddTask(task33),0);

	/*
	std::stringstream msg_stream;
	msg_stream<<"Task3 indexes {";
	for(unsigned int i=0;i<(taskSeq3->tasks).size();i++){
		int task_index= (taskSeq3->tasks)[i]->sequence_index;
		msg_stream<<task_index<<", ";
	}
	msg_stream<<"}";
	cerr<<msg_stream.str()<<endl;
	*/

	//## Add task sequences to manager
	bool addToQueue= true;
	ASSERT_EQ(m_taskManager->AddTaskSequence(taskSeq1,addToQueue),0);
	ASSERT_EQ(m_taskManager->AddTaskSequence(taskSeq2,addToQueue),0);
	ASSERT_EQ(m_taskManager->AddTaskSequence(taskSeq3,addToQueue),0);
	
	//## Expect task added according to precise order
	TaskQueue task_queue= m_taskManager->GetTaskQueue();
	std::vector<std::string> task_names;
	std::vector<std::string> exp_task_names {
		task21_name,
		task11_name,
		task31_name,		
		task32_name,
		task12_name,
		task33_name
	};
	
	//::testing::internal::CaptureStderr();
	
	
	std::stringstream ss;
	ss<<"task_names {";
	for(auto it : task_queue) {
  	std::string task_name= it->cmd_name;
		task_names.push_back(task_name);
		ss<<task_name<<", ";
	}  
	ss<<"}";
	//cerr<<ss.str()<<endl;
	//std::string out_msg = ::testing::internal::GetCapturedStderr();
	

	EXPECT_THAT(task_names, ::testing::ContainerEq(exp_task_names)) << "Tasks are added to queue in a wrong order!";

}//close TestAddTaskToQueueOrdering



}//close namespace


