/*----- PROTECTED REGION ID(AlarmHandlerStateMachine.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        AlarmHandlerStateMachine.cpp
//
// description : State machine file for the AlarmHandler class
//
// project :     AlarmHandler
//
// This file is part of Tango device class.
// 
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
// 
//
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================

#include <AlarmHandler.h>

/*----- PROTECTED REGION END -----*/	//	AlarmHandler::AlarmHandlerStateMachine.cpp

//================================================================
//  States   |  Description
//================================================================
//  ON       |  
//  OFF      |  
//  FAULT    |  
//  INIT     |  
//  RUNNING  |  
//  ALARM    |  
//  UNKNOWN  |  
//  MOVING   |  


namespace AlarmHandler_ns
{
//=================================================
//		Attributes Allowed Methods
//=================================================

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmAudible_allowed()
 *	Description : Execution allowed for alarmAudible attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmAudible_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmAudible attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmAudibleStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmAudibleStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_StatisticsResetTime_allowed()
 *	Description : Execution allowed for StatisticsResetTime attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_StatisticsResetTime_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for StatisticsResetTime attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::StatisticsResetTimeStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::StatisticsResetTimeStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarm_allowed()
 *	Description : Execution allowed for alarm attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarm_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarm attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmNormal_allowed()
 *	Description : Execution allowed for alarmNormal attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmNormal_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmNormal attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmNormalStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmNormalStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmUnacknowledged_allowed()
 *	Description : Execution allowed for alarmUnacknowledged attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmUnacknowledged_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmUnacknowledged attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmUnacknowledgedStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmUnacknowledgedStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmAcknowledged_allowed()
 *	Description : Execution allowed for alarmAcknowledged attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmAcknowledged_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmAcknowledged attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmAcknowledgedStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmAcknowledgedStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmUnacknowledgedNormal_allowed()
 *	Description : Execution allowed for alarmUnacknowledgedNormal attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmUnacknowledgedNormal_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmUnacknowledgedNormal attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmUnacknowledgedNormalStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmUnacknowledgedNormalStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmShelved_allowed()
 *	Description : Execution allowed for alarmShelved attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmShelved_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmShelved attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmShelvedStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmShelvedStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmOutOfService_allowed()
 *	Description : Execution allowed for alarmOutOfService attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmOutOfService_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmOutOfService attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmOutOfServiceStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmOutOfServiceStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmSilenced_allowed()
 *	Description : Execution allowed for alarmSilenced attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmSilenced_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmSilenced attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmSilencedStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmSilencedStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmList_allowed()
 *	Description : Execution allowed for alarmList attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmList_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmList attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmListStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmListStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmFrequency_allowed()
 *	Description : Execution allowed for alarmFrequency attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmFrequency_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmFrequency attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmFrequencyStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmFrequencyStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_alarmSummary_allowed()
 *	Description : Execution allowed for alarmSummary attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_alarmSummary_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for alarmSummary attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::alarmSummaryStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::alarmSummaryStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_AlarmState_allowed()
 *	Description : Execution allowed for AlarmState attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_AlarmState_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for AlarmState attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::AlarmStateStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::AlarmStateStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_AlarmFormula_allowed()
 *	Description : Execution allowed for AlarmFormula attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_AlarmFormula_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for AlarmFormula attribute in read access.
	/*----- PROTECTED REGION ID(AlarmHandler::AlarmFormulaStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::AlarmFormulaStateAllowed_READ
	return true;
}


//=================================================
//		Commands Allowed Methods
//=================================================

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_Ack_allowed()
 *	Description : Execution allowed for Ack attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_Ack_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Ack command.
	/*----- PROTECTED REGION ID(AlarmHandler::AckStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::AckStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_Load_allowed()
 *	Description : Execution allowed for Load attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_Load_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Load command.
	/*----- PROTECTED REGION ID(AlarmHandler::LoadStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::LoadStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_Remove_allowed()
 *	Description : Execution allowed for Remove attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_Remove_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Remove command.
	/*----- PROTECTED REGION ID(AlarmHandler::RemoveStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::RemoveStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_SearchAlarm_allowed()
 *	Description : Execution allowed for SearchAlarm attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_SearchAlarm_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SearchAlarm command.
	/*----- PROTECTED REGION ID(AlarmHandler::SearchAlarmStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::SearchAlarmStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_StopAudible_allowed()
 *	Description : Execution allowed for StopAudible attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_StopAudible_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for StopAudible command.
	/*----- PROTECTED REGION ID(AlarmHandler::StopAudibleStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::StopAudibleStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_Silence_allowed()
 *	Description : Execution allowed for Silence attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_Silence_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Silence command.
	/*----- PROTECTED REGION ID(AlarmHandler::SilenceStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::SilenceStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_Modify_allowed()
 *	Description : Execution allowed for Modify attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_Modify_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Modify command.
	/*----- PROTECTED REGION ID(AlarmHandler::ModifyStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::ModifyStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_Shelve_allowed()
 *	Description : Execution allowed for Shelve attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_Shelve_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Shelve command.
	/*----- PROTECTED REGION ID(AlarmHandler::ShelveStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::ShelveStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_Enable_allowed()
 *	Description : Execution allowed for Enable attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_Enable_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Enable command.
	/*----- PROTECTED REGION ID(AlarmHandler::EnableStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::EnableStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_Disable_allowed()
 *	Description : Execution allowed for Disable attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_Disable_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Disable command.
	/*----- PROTECTED REGION ID(AlarmHandler::DisableStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::DisableStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_ResetStatistics_allowed()
 *	Description : Execution allowed for ResetStatistics attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_ResetStatistics_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for ResetStatistics command.
	/*----- PROTECTED REGION ID(AlarmHandler::ResetStatisticsStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::ResetStatisticsStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_StopNew_allowed()
 *	Description : Execution allowed for StopNew attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_StopNew_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for StopNew command.
	/*----- PROTECTED REGION ID(AlarmHandler::StopNewStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::StopNewStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_GetAlarmInfo_allowed()
 *	Description : Execution allowed for GetAlarmInfo attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_GetAlarmInfo_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for GetAlarmInfo command.
	/*----- PROTECTED REGION ID(AlarmHandler::GetAlarmInfoStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::GetAlarmInfoStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : AlarmHandler::is_RemoveAlarmAttrs_allowed()
 *	Description : Execution allowed for RemoveAlarmAttrs attribute
 */
//--------------------------------------------------------
bool AlarmHandler::is_RemoveAlarmAttrs_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for RemoveAlarmAttrs command.
	/*----- PROTECTED REGION ID(AlarmHandler::RemoveAlarmAttrsStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandler::RemoveAlarmAttrsStateAllowed
	return true;
}


/*----- PROTECTED REGION ID(AlarmHandler::AlarmHandlerStateAllowed.AdditionalMethods) ENABLED START -----*/

//	Additional Methods

/*----- PROTECTED REGION END -----*/	//	AlarmHandler::AlarmHandlerStateAllowed.AdditionalMethods

}	//	End of namespace
