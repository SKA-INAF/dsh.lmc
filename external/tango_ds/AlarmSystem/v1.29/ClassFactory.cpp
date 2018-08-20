static const char *RcsId = "$Header: /home/cvsadm/cvsroot/utils/alarm/tango/server/src/ClassFactory.cpp,v 1.2 2008-03-27 09:25:29 graziano Exp $";
//+=============================================================================
//
// file :        ClassFactory.cpp
//
// description : C++ source for the class_factory method of the DServer
//               device class. This method is responsible to create
//               all class singletin for a device server. It is called
//               at device server startup
//
// project :     TANGO Device Server
//
// $Author: graziano $
//
// $Revision: 1.2 $
//
// copyleft :    Sincrotrone Trieste S.C.p.A. di interesse nazionale
//                Strada Statale 14 - km 163,5 in AREA Science Park
//                34012 Basovizza, Trieste ITALY
//                Tel. +39 040 37581
//                Fax. +39 040 9380902
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - ELETTRA U.O. Controlli
//=============================================================================


#include <tango.h>
#include <AlarmClass.h>

/**
 *	Create AlarmClass singleton and store it in DServer object.
 *
 * @author	$Author: graziano $
 * @version	$Revision: 1.2 $ $
 */

void Tango::DServer::class_factory()
{

	add_class(Alarm_ns::AlarmClass::init("Alarm"));

}