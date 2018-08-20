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


#include <Test_Task.h>
#include <Task.h>
#include <TangoUtils.h>

#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

#include <tango.h>


namespace Utils_ns {


//===========================================
//==           DEFINE TESTS              ====
//===========================================

TEST_F(TaskTest, TestCreateTask)
{
	
	//Create task
	bool useUTC= false;
	std::string task_name= "task";
	int task_priority= Task::eHIGH_PRIORITY;
	TaskPtr task= std::make_shared<Task>(nullptr,task_name,task_priority,useUTC);

	//Ensure task has no argin set
	ASSERT_FALSE(task->hasArgin);	

	//Ensure task has no activation time
	ASSERT_FALSE(task->hasActivationTime);
	ASSERT_TRUE((task->activation_time).is_empty_unix());

	//Ensure task has expiration time set to default	
	double tdiff= TangoUtils::GetYatTimeDiffSec(task->creation_time, task->expiration_time); 
	long int tdiff_casted= static_cast<long int>(tdiff);

	cerr<<"Task tcreation="<<(task->creation_time).to_local_ISO8601()<<"("<<(task->creation_time).long_unix()<<"), tstop="<<(task->expiration_time).to_local_ISO8601()<<" ("<<(task->expiration_time).long_unix()<<"), tdiff="<<tdiff<<", tdiff_casted="<<tdiff_casted<<endl;
	
	ASSERT_TRUE(task->hasExpirationTime);
	ASSERT_EQ(tdiff_casted,task->default_max_duration/1000);

	//Ensure priority set
	ASSERT_EQ(task->priority,task_priority);
	
}//close TestCreateTask


TEST_F(TaskTest, TestCreateTaskWithCustomExpiration)
{

	//Create task
	bool useUTC= false;
	std::string task_name= "task";
	int task_priority= Task::eMEDIUM_PRIORITY;
	TaskPtr task= std::make_shared<Task>(nullptr,task_name,task_priority,useUTC);

	//Set expiration time duration
	int task_duration= 20000;//ms
	ASSERT_EQ(task->SetMaxDuration(task_duration),0);

	
	//Ensure task has expiration time set to default
	double tdiff= TangoUtils::GetYatTimeDiffSec(task->creation_time, task->expiration_time); 
	long int tdiff_casted= static_cast<long int>(tdiff);
	cerr<<"Task tcreation="<<(task->creation_time).to_local_ISO8601()<<"("<<(task->creation_time).long_unix()<<"), tstop="<<(task->expiration_time).to_local_ISO8601()<<" ("<<(task->expiration_time).long_unix()<<"), tdiff="<<tdiff<<", tdiff_casted="<<tdiff_casted<<endl;

	ASSERT_TRUE(task->hasExpirationTime);
	ASSERT_EQ(tdiff_casted,task_duration/1000);

}


TEST_F(TaskTest, TestCreateTaskSequenceWithDependencies)
{

	bool useUTC= false;

	//## Create tasks
	//- TaskSequence: 
	//     + task 1: depend on task2
	//     + task 2: depend on task4 & task 5
	//     + task 3: depend on task1
	//     + task 4: no dependency
	//     + task 5: no dependency
	std::string task1_name= "task1";
	int task1_duration= 20000;//ms
	TaskPtr task1= std::make_shared<Task>(nullptr,task1_name);
	task1->SetMaxDuration(task1_duration);

	std::string task2_name= "task2";
	int task2_duration= 5000;//ms
	TaskPtr task2= std::make_shared<Task>(nullptr,task2_name);
	task2->SetMaxDuration(task2_duration);

	std::string task3_name= "task3";
	int task3_duration= 10000;//ms
	TaskPtr task3= std::make_shared<Task>(nullptr,task3_name);
	task3->SetMaxDuration(task3_duration);

	std::string task4_name= "task4";
	int task4_delay= 50000;//ms
	int task4_duration= 20000;//ms
	TaskPtr task4= std::make_shared<Task>(nullptr,task4_name);
	task4->SetTimeDelayAndDuration(task4_delay,task4_duration);

	std::string task5_name= "task5";
	int task5_delay= 40000;//ms
	int task5_duration= 50000;//ms
	TaskPtr task5= std::make_shared<Task>(nullptr,task5_name);
	task5->SetTimeDelayAndDuration(task5_delay,task5_duration);

	std::string taskSeq_name= "TS";
	TaskSequencePtr taskSeq= std::make_shared<TaskSequence>(nullptr,taskSeq_name,useUTC);
	ASSERT_EQ(taskSeq->AddTask(task1),0);
	ASSERT_EQ(taskSeq->AddTask(task2),0);
	ASSERT_EQ(taskSeq->AddTask(task3),0);
	ASSERT_EQ(taskSeq->AddTask(task4),0);
	ASSERT_EQ(taskSeq->AddTask(task5),0);

	//## Test cyclic dependency matrix rejection
	std::vector<std::vector<size_t>> dep_indexes_cyclic { 
		{1},
		{3,4},
		{0},
		{},
		{0}
	};
	ASSERT_EQ(taskSeq->SetTaskDependency(dep_indexes_cyclic),-1);

	//## Test valid dependency matrix
	std::vector<std::vector<size_t>> dep_indexes { 
		{1},
		{3,4},
		{0},
		{},
		{}
	};
	ASSERT_EQ(taskSeq->SetTaskDependency(dep_indexes),0);

	//## Test sorting
	ASSERT_EQ(taskSeq->Sort(),0);
		
	std::vector<std::string> exp_task_names {
		task4_name,
		task5_name,
		task2_name,		
		task1_name,
		task3_name
	};

	std::stringstream ss;
	ss<<"task_names {";
	std::vector<std::string> task_names;
	for(unsigned int i=0;i<(taskSeq->tasks).size();i++){
		std::string task_name= (taskSeq->tasks)[i]->cmd_name;
		task_names.push_back(task_name);
		ss<<task_name<<" {tstart(?"<<(taskSeq->tasks)[i]->hasActivationTime<<")="<<((taskSeq->tasks)[i]->activation_time).long_unix()<<", tstart^exp(?"<<(taskSeq->tasks)[i]->depend_on_sequence_tasks<<")="<<((taskSeq->tasks)[i]->exp_max_activation_time).long_unix()<<", tstop="<<((taskSeq->tasks)[i]->expiration_time).long_unix()<<", tstop^exp(?"<<(taskSeq->tasks)[i]->depend_on_sequence_tasks<<")="<<((taskSeq->tasks)[i]->exp_max_expiration_time).long_unix()<<"}, ";
	}
	ss<<"}";
	cerr<<ss.str()<<endl;
	
	
	ASSERT_THAT(task_names, ::testing::ContainerEq(exp_task_names)) << "Tasks in sequence are ordered in a wrong way!";

	//## Test dependent task tstart/tstop
	//- Task2
	yat::Time task2_tstart_exp= task2->exp_max_activation_time;
	yat::Time task2_tstop_exp= task2->exp_max_expiration_time;
	yat::Time task2_dep_tstop= task4->expiration_time;
	if(task4->expiration_time<task5->expiration_time) task2_dep_tstop= task5->expiration_time;
	
	ASSERT_TRUE( (task2_tstart_exp==task2_dep_tstop) );

}//close TestCreateTaskSequenceWithDependencies

}//close namespace


