/*----- PROTECTED REGION ID(DSEmulatorStateMachine.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        DSEmulatorStateMachine.cpp
//
// description : State machine file for the DSEmulator class
//
// project :     DSEmulator
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

#include <DSEmulator.h>

/*----- PROTECTED REGION END -----*/	//	DSEmulator::DSEmulatorStateMachine.cpp

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
//  DISABLE  |  
//  STANDBY  |  


namespace DSEmulator_ns
{
//=================================================
//		Attributes Allowed Methods
//=================================================

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_powerCut_allowed()
 *	Description : Execution allowed for powerCut attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_powerCut_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for powerCut attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::powerCutStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::powerCutStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_powerRestored_allowed()
 *	Description : Execution allowed for powerRestored attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_powerRestored_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for powerRestored attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::powerRestoredStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::powerRestoredStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_stowProgress_allowed()
 *	Description : Execution allowed for stowProgress attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_stowProgress_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for stowProgress attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::stowProgressStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::stowProgressStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_indexingProgress_allowed()
 *	Description : Execution allowed for indexingProgress attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_indexingProgress_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for indexingProgress attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::indexingProgressStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::indexingProgressStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_achievedPointing_allowed()
 *	Description : Execution allowed for achievedPointing attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_achievedPointing_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for achievedPointing attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::achievedPointingStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::achievedPointingStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_desiredPointing_allowed()
 *	Description : Execution allowed for desiredPointing attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_desiredPointing_allowed(TANGO_UNUSED(Tango::AttReqType type))
{
	//	Not any excluded states for desiredPointing attribute in Write access.
	/*----- PROTECTED REGION ID(DSEmulator::desiredPointingStateAllowed_WRITE) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::desiredPointingStateAllowed_WRITE

	//	Not any excluded states for desiredPointing attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::desiredPointingStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::desiredPointingStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_setStandbyLPModeProgress_allowed()
 *	Description : Execution allowed for setStandbyLPModeProgress attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_setStandbyLPModeProgress_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for setStandbyLPModeProgress attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::setStandbyLPModeProgressStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::setStandbyLPModeProgressStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_setStandbyFPModeProgress_allowed()
 *	Description : Execution allowed for setStandbyFPModeProgress attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_setStandbyFPModeProgress_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for setStandbyFPModeProgress attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::setStandbyFPModeProgressStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::setStandbyFPModeProgressStateAllowed_READ
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_setPointModeProgress_allowed()
 *	Description : Execution allowed for setPointModeProgress attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_setPointModeProgress_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for setPointModeProgress attribute in read access.
	/*----- PROTECTED REGION ID(DSEmulator::setPointModeProgressStateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::setPointModeProgressStateAllowed_READ
	return true;
}


//=================================================
//		Commands Allowed Methods
//=================================================

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_SetStandbyLPMode_allowed()
 *	Description : Execution allowed for SetStandbyLPMode attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_SetStandbyLPMode_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetStandbyLPMode command.
	/*----- PROTECTED REGION ID(DSEmulator::SetStandbyLPModeStateAllowed) ENABLED START -----*/
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "operatingMode";

	//=====================================================
	//==         CHECK IF CMD ALLOWED
	//=====================================================	
	_DEBUG_LOG("Check if cmd "<<__FUNCTION__<<" is allowed...");
	bool check_cmd= true;
	try {
		IsCmdAllowed(__FUNCTION__,check_cmd);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		throw;
	}


	/*----- PROTECTED REGION END -----*/	//	DSEmulator::SetStandbyLPModeStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_SetStandbyFPMode_allowed()
 *	Description : Execution allowed for SetStandbyFPMode attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_SetStandbyFPMode_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetStandbyFPMode command.
	/*----- PROTECTED REGION ID(DSEmulator::SetStandbyFPModeStateAllowed) ENABLED START -----*/
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "operatingMode";

	//=====================================================
	//==         CHECK IF CMD ALLOWED
	//=====================================================	
	_DEBUG_LOG("Check if cmd "<<__FUNCTION__<<" is allowed...");
	bool check_cmd= true;
	try {
		IsCmdAllowed(__FUNCTION__,check_cmd);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		throw;
	}

	/*----- PROTECTED REGION END -----*/	//	DSEmulator::SetStandbyFPModeStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_SetPointMode_allowed()
 *	Description : Execution allowed for SetPointMode attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_SetPointMode_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetPointMode command.
	/*----- PROTECTED REGION ID(DSEmulator::SetPointModeStateAllowed) ENABLED START -----*/
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "operatingMode";

	//=====================================================
	//==         CHECK IF CMD ALLOWED
	//=====================================================	
	_DEBUG_LOG("Check if cmd "<<__FUNCTION__<<" is allowed...");
	bool check_cmd= true;
	try {
		IsCmdAllowed(__FUNCTION__,check_cmd);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		throw;
	}

	/*----- PROTECTED REGION END -----*/	//	DSEmulator::SetPointModeStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_Stow_allowed()
 *	Description : Execution allowed for Stow attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_Stow_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for Stow command.
	/*----- PROTECTED REGION ID(DSEmulator::StowStateAllowed) ENABLED START -----*/
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "operatingMode";

	//=====================================================
	//==         CHECK IF CMD ALLOWED
	//=====================================================	
	_DEBUG_LOG("Check if cmd "<<__FUNCTION__<<" is allowed...");
	bool check_cmd= true;
	try {
		IsCmdAllowed(__FUNCTION__,check_cmd);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		throw;
	}

	/*----- PROTECTED REGION END -----*/	//	DSEmulator::StowStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_SetMaintenanceMode_allowed()
 *	Description : Execution allowed for SetMaintenanceMode attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_SetMaintenanceMode_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetMaintenanceMode command.
	/*----- PROTECTED REGION ID(DSEmulator::SetMaintenanceModeStateAllowed) ENABLED START -----*/
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "operatingMode";
	
	//=====================================================
	//==         CHECK IF CMD ALLOWED
	//=====================================================	
	_DEBUG_LOG("Check if cmd "<<__FUNCTION__<<" is allowed...");
	
	bool check_cmd= true;
	try {
		IsCmdAllowed(__FUNCTION__,check_cmd);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		throw;
	}

	/*----- PROTECTED REGION END -----*/	//	DSEmulator::SetMaintenanceModeStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_SetIndexPosition_allowed()
 *	Description : Execution allowed for SetIndexPosition attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_SetIndexPosition_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SetIndexPosition command.
	/*----- PROTECTED REGION ID(DSEmulator::SetIndexPositionStateAllowed) ENABLED START -----*/
	
	//###########################################################################
	//##    HARD-CODED PARAMETERS (TO BE CHANGED IF ICD CHANGE OR USE ALIASES?)
	//###########################################################################
	std::string attr_name= "indexerPosition";

	//=====================================================
	//==         CHECK IF CMD ALLOWED
	//=====================================================	
	_DEBUG_LOG("Check if cmd "<<__FUNCTION__<<" is allowed...");
	bool check_cmd= true;
	try {
		IsCmdAllowed(__FUNCTION__,check_cmd);
	}
	catch(Tango::DevFailed& e){
		Tango::Except::print_exception(e);
		throw;
	}

	//=====================================================
	//==         ARGIN CHECK
	//=====================================================
	//## Extract argin (DevShort)
	Tango::DevShort argin;
	if ((any>>=argin)== false) {
		std::stringstream ss;
		ss<<"Failed to extract cmd input args!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_INDEX_POSITION_FAILED",ss.str());	
	}
	Tango::DevShort indexerPositionId= argin-1;//Arguments are given in [1-5] but enum are starting from 0
	
	
	//## Retrieve indexer position label to check if this is a valid argin
	std::string indexerPos_str= ""; 
	if(Utils_ns::TangoUtils::GetEnumAttrLabelValue(indexerPos_str,this,attr_name,indexerPositionId)<0){
		std::stringstream ss;
		ss<<"Failed to get enum label for "<<attr_name<<" corresponding to value "<<indexerPositionId<<"!";
		_ERROR_LOG(ss.str());
		_THROW_TANGO_EXCEPTION("SET_INDEX_POSITION_FAILED",ss.str());	
	}
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::SetIndexPositionStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_SimulatePowerCut_allowed()
 *	Description : Execution allowed for SimulatePowerCut attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_SimulatePowerCut_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SimulatePowerCut command.
	/*----- PROTECTED REGION ID(DSEmulator::SimulatePowerCutStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::SimulatePowerCutStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_SimulatePowerRestore_allowed()
 *	Description : Execution allowed for SimulatePowerRestore attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_SimulatePowerRestore_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SimulatePowerRestore command.
	/*----- PROTECTED REGION ID(DSEmulator::SimulatePowerRestoreStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::SimulatePowerRestoreStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_SimulateESTOP_allowed()
 *	Description : Execution allowed for SimulateESTOP attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_SimulateESTOP_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for SimulateESTOP command.
	/*----- PROTECTED REGION ID(DSEmulator::SimulateESTOPStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::SimulateESTOPStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_GetTrackProgram_allowed()
 *	Description : Execution allowed for GetTrackProgram attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_GetTrackProgram_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for GetTrackProgram command.
	/*----- PROTECTED REGION ID(DSEmulator::GetTrackProgramStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::GetTrackProgramStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_FlushTrackProgram_allowed()
 *	Description : Execution allowed for FlushTrackProgram attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_FlushTrackProgram_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for FlushTrackProgram command.
	/*----- PROTECTED REGION ID(DSEmulator::FlushTrackProgramStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::FlushTrackProgramStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_PauseTracking_allowed()
 *	Description : Execution allowed for PauseTracking attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_PauseTracking_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for PauseTracking command.
	/*----- PROTECTED REGION ID(DSEmulator::PauseTrackingStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::PauseTrackingStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_ResumeTracking_allowed()
 *	Description : Execution allowed for ResumeTracking attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_ResumeTracking_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for ResumeTracking command.
	/*----- PROTECTED REGION ID(DSEmulator::ResumeTrackingStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::ResumeTrackingStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_ResetProgressAttrs_allowed()
 *	Description : Execution allowed for ResetProgressAttrs attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_ResetProgressAttrs_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for ResetProgressAttrs command.
	/*----- PROTECTED REGION ID(DSEmulator::ResetProgressAttrsStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::ResetProgressAttrsStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_ResetPowerCutAttrs_allowed()
 *	Description : Execution allowed for ResetPowerCutAttrs attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_ResetPowerCutAttrs_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for ResetPowerCutAttrs command.
	/*----- PROTECTED REGION ID(DSEmulator::ResetPowerCutAttrsStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::ResetPowerCutAttrsStateAllowed
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : DSEmulator::is_dummyDynCmd_allowed()
 *	Description : Execution allowed for dummyDynCmd attribute
 */
//--------------------------------------------------------
bool DSEmulator::is_dummyDynCmd_allowed(TANGO_UNUSED(const CORBA::Any &any))
{
	//	Not any excluded states for dummyDynCmd command.
	/*----- PROTECTED REGION ID(DSEmulator::dummyDynCmdStateAllowed) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	DSEmulator::dummyDynCmdStateAllowed
	return true;
}


/*----- PROTECTED REGION ID(DSEmulator::DSEmulatorStateAllowed.AdditionalMethods) ENABLED START -----*/

//	Additional Methods

/*----- PROTECTED REGION END -----*/	//	DSEmulator::DSEmulatorStateAllowed.AdditionalMethods

}	//	End of namespace
