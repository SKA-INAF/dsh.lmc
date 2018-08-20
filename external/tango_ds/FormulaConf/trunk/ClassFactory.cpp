/*----- PROTECTED REGION ID(FormulaConf::ClassFactory.cpp) ENABLED START -----*/
static const char *RcsId = "$Id:  $";
//=============================================================================
//
// file :        ClassFactory.cpp
//
// description : C++ source for the class_factory method of the DServer
//               device class. This method is responsible for the creation of
//               all class singleton for a device server. It is called
//               at device server startup.
//
// project :     FormulaConf
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

#include <tango.h>
#include <FormulaConfClass.h>

//	Add class header files if needed


/**
 *	Create FormulaConf Class singleton and store it in DServer object.
 */

void Tango::DServer::class_factory()
{
	//	Add method class init if needed
	add_class(FormulaConf_ns::FormulaConfClass::init("FormulaConf"));
}
/*----- PROTECTED REGION END -----*/	//	FormulaConf::ClassFactory.cpp
