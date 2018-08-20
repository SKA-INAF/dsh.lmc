/*----- PROTECTED REGION ID(ElementLoggerStateMachine.cpp) ENABLED START -----*/
static const char *RcsId = "$Id:  $";
//=============================================================================
//
// file :        ElementLoggerStateMachine.cpp
//
// description : State machine file for the ElementLogger class
//
// project :     ElementLogger
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
// $Author:  $
//
// $Revision:  $
// $Date:  $
//
// $HeadURL:  $
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================

#include <ElementLogger.h>

/*----- PROTECTED REGION END -----*/	//	ElementLogger::ElementLoggerStateMachine.cpp

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


namespace ElementLogger_ns
{
//=================================================
//		Attributes Allowed Methods
//=================================================


//=================================================
//		Commands Allowed Methods
//=================================================

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_log_allowed()
 *	Description : Execution allowed for log attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_log_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for log command.
	/*----- PROTECTED REGION ID(ElementLogger::logStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::logStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_TestLog_allowed()
 *	Description : Execution allowed for TestLog attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_TestLog_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for TestLog command.
	/*----- PROTECTED REGION ID(ElementLogger::TestLogStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::TestLogStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_AddLogSource_allowed()
 *	Description : Execution allowed for AddLogSource attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_AddLogSource_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for AddLogSource command.
	/*----- PROTECTED REGION ID(ElementLogger::AddLogSourceStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::AddLogSourceStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_RemoveLogSource_allowed()
 *	Description : Execution allowed for RemoveLogSource attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_RemoveLogSource_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for RemoveLogSource command.
	/*----- PROTECTED REGION ID(ElementLogger::RemoveLogSourceStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::RemoveLogSourceStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_InitializeLogSources_allowed()
 *	Description : Execution allowed for InitializeLogSources attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_InitializeLogSources_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for InitializeLogSources command.
	/*----- PROTECTED REGION ID(ElementLogger::InitializeLogSourcesStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::InitializeLogSourcesStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_InitializeSELogSources_allowed()
 *	Description : Execution allowed for InitializeSELogSources attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_InitializeSELogSources_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for InitializeSELogSources command.
	/*----- PROTECTED REGION ID(ElementLogger::InitializeSELogSourcesStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::InitializeSELogSourcesStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_AddSELogSource_allowed()
 *	Description : Execution allowed for AddSELogSource attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_AddSELogSource_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for AddSELogSource command.
	/*----- PROTECTED REGION ID(ElementLogger::AddSELogSourceStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::AddSELogSourceStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_RemoveSELogSource_allowed()
 *	Description : Execution allowed for RemoveSELogSource attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_RemoveSELogSource_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for RemoveSELogSource command.
	/*----- PROTECTED REGION ID(ElementLogger::RemoveSELogSourceStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::RemoveSELogSourceStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetLoggingLevelCentral_allowed()
 *	Description : Execution allowed for SetLoggingLevelCentral attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetLoggingLevelCentral_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetLoggingLevelCentral command.
	/*----- PROTECTED REGION ID(ElementLogger::SetLoggingLevelCentralStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetLoggingLevelCentralStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetLoggingLevelElement_allowed()
 *	Description : Execution allowed for SetLoggingLevelElement attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetLoggingLevelElement_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetLoggingLevelElement command.
	/*----- PROTECTED REGION ID(ElementLogger::SetLoggingLevelElementStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetLoggingLevelElementStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetLoggingLevelStorage_allowed()
 *	Description : Execution allowed for SetLoggingLevelStorage attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetLoggingLevelStorage_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetLoggingLevelStorage command.
	/*----- PROTECTED REGION ID(ElementLogger::SetLoggingLevelStorageStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetLoggingLevelStorageStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetSPFLoggingLevel_allowed()
 *	Description : Execution allowed for SetSPFLoggingLevel attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetSPFLoggingLevel_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetSPFLoggingLevel command.
	/*----- PROTECTED REGION ID(ElementLogger::SetSPFLoggingLevelStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetSPFLoggingLevelStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetRxLoggingLevel_allowed()
 *	Description : Execution allowed for SetRxLoggingLevel attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetRxLoggingLevel_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetRxLoggingLevel command.
	/*----- PROTECTED REGION ID(ElementLogger::SetRxLoggingLevelStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetRxLoggingLevelStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetSPFLoggingTarget_allowed()
 *	Description : Execution allowed for SetSPFLoggingTarget attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetSPFLoggingTarget_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetSPFLoggingTarget command.
	/*----- PROTECTED REGION ID(ElementLogger::SetSPFLoggingTargetStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetSPFLoggingTargetStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetRxLoggingTarget_allowed()
 *	Description : Execution allowed for SetRxLoggingTarget attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetRxLoggingTarget_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetRxLoggingTarget command.
	/*----- PROTECTED REGION ID(ElementLogger::SetRxLoggingTargetStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetRxLoggingTargetStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_EnableCentralLogger_allowed()
 *	Description : Execution allowed for EnableCentralLogger attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_EnableCentralLogger_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for EnableCentralLogger command.
	/*----- PROTECTED REGION ID(ElementLogger::EnableCentralLoggerStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::EnableCentralLoggerStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_DisableCentralLogger_allowed()
 *	Description : Execution allowed for DisableCentralLogger attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_DisableCentralLogger_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for DisableCentralLogger command.
	/*----- PROTECTED REGION ID(ElementLogger::DisableCentralLoggerStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::DisableCentralLoggerStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_EnableElementLogger_allowed()
 *	Description : Execution allowed for EnableElementLogger attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_EnableElementLogger_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for EnableElementLogger command.
	/*----- PROTECTED REGION ID(ElementLogger::EnableElementLoggerStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::EnableElementLoggerStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_DisableElementLogger_allowed()
 *	Description : Execution allowed for DisableElementLogger attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_DisableElementLogger_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for DisableElementLogger command.
	/*----- PROTECTED REGION ID(ElementLogger::DisableElementLoggerStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::DisableElementLoggerStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_EnableStorageLogger_allowed()
 *	Description : Execution allowed for EnableStorageLogger attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_EnableStorageLogger_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for EnableStorageLogger command.
	/*----- PROTECTED REGION ID(ElementLogger::EnableStorageLoggerStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::EnableStorageLoggerStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_DisableStorageLogger_allowed()
 *	Description : Execution allowed for DisableStorageLogger attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_DisableStorageLogger_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for DisableStorageLogger command.
	/*----- PROTECTED REGION ID(ElementLogger::DisableStorageLoggerStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::DisableStorageLoggerStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetLoggingTargetCentral_allowed()
 *	Description : Execution allowed for SetLoggingTargetCentral attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetLoggingTargetCentral_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetLoggingTargetCentral command.
	/*----- PROTECTED REGION ID(ElementLogger::SetLoggingTargetCentralStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetLoggingTargetCentralStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetLoggingTargetElement_allowed()
 *	Description : Execution allowed for SetLoggingTargetElement attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetLoggingTargetElement_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetLoggingTargetElement command.
	/*----- PROTECTED REGION ID(ElementLogger::SetLoggingTargetElementStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetLoggingTargetElementStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_SetLoggingTargetStorage_allowed()
 *	Description : Execution allowed for SetLoggingTargetStorage attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_SetLoggingTargetStorage_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetLoggingTargetStorage command.
	/*----- PROTECTED REGION ID(ElementLogger::SetLoggingTargetStorageStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::SetLoggingTargetStorageStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : ElementLogger::is_dummyDynCmd_allowed()
 *	Description : Execution allowed for dummyDynCmd attribute
 */
//--------------------------------------------------------
bool ElementLogger::is_dummyDynCmd_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for dummyDynCmd command.
	/*----- PROTECTED REGION ID(ElementLogger::dummyDynCmdStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	ElementLogger::dummyDynCmdStateAllowed
	return true;
}


/*----- PROTECTED REGION ID(ElementLogger::ElementLoggerStateAllowed.AdditionalMethods) ENABLED START -----*/

//	Additional Methods

/*----- PROTECTED REGION END -----*/	//	ElementLogger::ElementLoggerStateAllowed.AdditionalMethods

}	//	End of namespace