/*----- PROTECTED REGION ID(DSManager::DynAttrUtils.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        DSManagerDynAttrUtils.cpp
//
// description : Dynamic attributes utilities file for the DSManager class
//
// project :     DSManager
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


#include <DSManager.h>
#include <DSManagerClass.h>

/*----- PROTECTED REGION END -----*/	//	DSManager::DynAttrUtils.cpp

//================================================================
//  Attributes managed is:
//================================================================
//================================================================

//	For compatibility reason, this file (DSManagerDynAttrUtils)
//	manage also the dynamic command utilities.
//================================================================
//  The following table gives the correspondence
//  between command and method names.
//
//  Command name  |  Method name
//================================================================
//  dummyDynCmd   |  dummy_dyn_cmd
//================================================================

namespace DSManager_ns
{
//=============================================================
//	Add/Remove dynamic attribute methods
//=============================================================



//============================================================
//	Tool methods to get pointer on attribute data buffer 
//============================================================


//=============================================================
//	Add/Remove dynamic command methods
//=============================================================

//--------------------------------------------------------
/**
 *	Add a dummyDynCmd dynamic command.
 *
 *  parameter cmdname: command name to be cretated and added.
 *  parameter device:  Set this flag to true if the command must be added for only this device.
 */
//--------------------------------------------------------
void DSManager::add_dummyDynCmd_dynamic_command(string cmdname, bool device)
{
	dummyDynCmdClass	*pdummyDynCmdCmd =
		new dummyDynCmdClass(cmdname.c_str(),
			Tango::DEV_VOID, Tango::DEV_VOID,
			"",
			"",
			Tango::OPERATOR);
	add_command(pdummyDynCmdCmd, device);
}
//--------------------------------------------------------
/**
 *	remove a dummyDynCmd dynamic command.
 *
 *  parameter cmdname: command name to be removed.
 */
//--------------------------------------------------------
void DSManager::remove_dummyDynCmd_dynamic_command(string cmdname)
{
	remove_command(cmdname, true);
}

} //	namespace