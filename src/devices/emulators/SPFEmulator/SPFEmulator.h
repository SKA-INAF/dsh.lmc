/*----- PROTECTED REGION ID(SPFEmulator.h) ENABLED START -----*/

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
// * DSH.LMC is distributed in the hope that it will be useful, but 			  *
// * WITHOUT ANY WARRANTY; without even the implied warranty of             * 
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
// * See the GNU General Public License for more details. You should        * 
// * have received a copy of the GNU General Public License along with      * 
// * DSH.LMC. If not, see http://www.gnu.org/licenses/.                     *
// **************************************************************************

//=============================================================================
//
// file :        SPFEmulator.h
//
// description : Include file for the SPFEmulator class
//
// project :     SPFEmulator
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


#ifndef SPFEmulator_H
#define SPFEmulator_H

#include <tango.h>
#include <LMCDevice.h>

#include <yat4tango/InnerAppender.h>
#include <yat/memory/DataBuffer.h>
#include <SPFEmulatorTask.h>

#include <map>
#include <utility>

/*----- PROTECTED REGION END -----*/	//	SPFEmulator.h

/**
 *  SPFEmulator class description:
 *    Device server to emulate SPF-LMC interface
 */

namespace SPFEmulator_ns
{
enum _centralLoggerStatusEnum {
} ;
typedef _centralLoggerStatusEnum centralLoggerStatusEnum;

enum _elementLoggerStatusEnum {
} ;
typedef _elementLoggerStatusEnum elementLoggerStatusEnum;

enum _storageLoggerStatusEnum {
} ;
typedef _storageLoggerStatusEnum storageLoggerStatusEnum;

enum _fwdAttrInitStatusEnum {
} ;
typedef _fwdAttrInitStatusEnum fwdAttrInitStatusEnum;

/*----- PROTECTED REGION ID(SPFEmulator::Additional Class Declarations) ENABLED START -----*/

//	Additional Class Declarations
class SPFEmulatorTask;

/*----- PROTECTED REGION END -----*/	//	SPFEmulator::Additional Class Declarations

class SPFEmulator : public LMCDevice_ns::LMCDevice
{

/*----- PROTECTED REGION ID(SPFEmulator::Data Members) ENABLED START -----*/

//	Add your own data members
	protected:
		//- the device task
		SPFEmulatorTask* m_task;

		//- Mutex
		omni_mutex m_mutex;

/*----- PROTECTED REGION END -----*/	//	SPFEmulator::Data Members

//	Device property data members
public:
	//	StartupTime:	Average time for starting up SPF in seconds
	Tango::DevULong	startupTime;
	//	GoOperationalTime:	Average time taken for going operational
	Tango::DevULong	goOperationalTime;
	//	EnableStartupFailure:	Enable/disable simulation of startup failures
	Tango::DevBoolean	enableStartupFailure;
	//	StartupFailureProbability:	Startup failure probability in range (0,1)
	Tango::DevFloat	startupFailureProbability;
	//	EnableOperationalFailure:	Enable/disable simulation of failures during go operational step
	Tango::DevBoolean	enableOperationalFailure;
	//	OperationalFailureProbability:	Failure probability in range (0,1) for go operational task
	Tango::DevFloat	operationalFailureProbability;
	//	EnableAutoStartup:	if enabled automatically execute startup task
	Tango::DevBoolean	enableAutoStartup;
	//	GoStandbyLPTime:	Average time taken for going to STANDBY-LP
	Tango::DevULong	goStandbyLPTime;
	//	GoMaintenanceTime:	Average time taken for going to MIANTENANCE
	Tango::DevULong	goMaintenanceTime;
	//	EnableStandbyLPFailure:	Enable/disable simulation of failures during go standbyLP step
	Tango::DevBoolean	enableStandbyLPFailure;
	//	StandbyLPFailureProbability:	Failure probability in range (0,1) for go standbyLP task
	Tango::DevFloat	standbyLPFailureProbability;
	//	EnableMaintenanceFailure:	Enable/disable simulation of failures during go maintenance step
	Tango::DevBoolean	enableMaintenanceFailure;
	//	MaintenanceFailureProbability:	Failure probability in range (0,1) for go standbyLP task
	Tango::DevFloat	maintenanceFailureProbability;
	//	Rfe1TempSetPointDefault:	Default Rfe1 (Lna H/V) temperature setpoint
	Tango::DevShort	rfe1TempSetPointDefault;
	//	CalSourceTempSetPointDefault:	Default calibration source temp setpoint
	Tango::DevShort	calSourceTempSetPointDefault;
	//	Rfe2TempSetPointDefault:	Default Rfe2 temp setpoint
	Tango::DevShort	rfe2TempSetPointDefault;
	//	ExpectedOnlineDefault:	Default expected online flag
	Tango::DevBoolean	expectedOnlineDefault;

	bool	mandatoryNotDefined;

//	Attribute data members
public:
	Tango::DevString	*attr_setB1FeedOperateModeProgress_read;
	Tango::DevString	*attr_setB1FeedRegenerationModeProgress_read;
	Tango::DevString	*attr_setB2FeedOperateModeProgress_read;
	Tango::DevString	*attr_setB2FeedRegenerationModeProgress_read;
	Tango::DevString	*attr_setB3FeedOperateModeProgress_read;
	Tango::DevString	*attr_setB3FeedRegenerationModeProgress_read;
	Tango::DevString	*attr_setB4FeedOperateModeProgress_read;
	Tango::DevString	*attr_setB4FeedRegenerationModeProgress_read;
	Tango::DevString	*attr_setB5FeedOperateModeProgress_read;
	Tango::DevString	*attr_setB5FeedRegenerationModeProgress_read;
	Tango::DevString	*attr_setOperateModeProgress_read;
	Tango::DevString	*attr_setStandbyLPModeProgress_read;
	Tango::DevString	*attr_setMaintenanceModeProgress_read;
	Tango::DevString	*attr_setB1FeedStandbyLPModeProgress_read;
	Tango::DevString	*attr_setB2FeedStandbyLPModeProgress_read;
	Tango::DevString	*attr_setB3FeedStandbyLPModeProgress_read;
	Tango::DevString	*attr_setB4FeedStandbyLPModeProgress_read;
	Tango::DevString	*attr_setB5FeedStandbyLPModeProgress_read;

//	Constructors and destructors
public:
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	SPFEmulator(Tango::DeviceClass *cl,string &s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	SPFEmulator(Tango::DeviceClass *cl,const char *s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device name
	 *	@param d	Device description.
	 */
	SPFEmulator(Tango::DeviceClass *cl,const char *s,const char *d);
	/**
	 * The device object destructor.
	 */
	~SPFEmulator() {delete_device();};


//	Miscellaneous methods
public:
	/*
	 *	will be called at device destruction or at init command.
	 */
	void delete_device();
	/*
	 *	Initialize the device
	 */
	virtual void init_device();
	/*
	 *	Read the device properties from database
	 */
	void get_device_property();
	/*
	 *	Always executed method before execution command method.
	 */
	virtual void always_executed_hook();

	/*
	 *	Check if mandatory property has been set
	 */
	 void check_mandatory_property(Tango::DbDatum &class_prop, Tango::DbDatum &dev_prop);

//	Attribute methods
public:
	//--------------------------------------------------------
	/*
	 *	Method      : SPFEmulator::read_attr_hardware()
	 *	Description : Hardware acquisition for attributes.
	 */
	//--------------------------------------------------------
	virtual void read_attr_hardware(vector<long> &attr_list);
	//--------------------------------------------------------
	/*
	 *	Method      : SPFEmulator::write_attr_hardware()
	 *	Description : Hardware writing for attributes.
	 */
	//--------------------------------------------------------
	virtual void write_attr_hardware(vector<long> &attr_list);

/**
 *	Attribute setB1FeedOperateModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB1FeedOperateModeProgress(Tango::Attribute &attr);
	virtual bool is_setB1FeedOperateModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB1FeedRegenerationModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB1FeedRegenerationModeProgress(Tango::Attribute &attr);
	virtual bool is_setB1FeedRegenerationModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB2FeedOperateModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB2FeedOperateModeProgress(Tango::Attribute &attr);
	virtual bool is_setB2FeedOperateModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB2FeedRegenerationModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB2FeedRegenerationModeProgress(Tango::Attribute &attr);
	virtual bool is_setB2FeedRegenerationModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB3FeedOperateModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB3FeedOperateModeProgress(Tango::Attribute &attr);
	virtual bool is_setB3FeedOperateModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB3FeedRegenerationModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB3FeedRegenerationModeProgress(Tango::Attribute &attr);
	virtual bool is_setB3FeedRegenerationModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB4FeedOperateModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB4FeedOperateModeProgress(Tango::Attribute &attr);
	virtual bool is_setB4FeedOperateModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB4FeedRegenerationModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB4FeedRegenerationModeProgress(Tango::Attribute &attr);
	virtual bool is_setB4FeedRegenerationModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB5FeedOperateModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB5FeedOperateModeProgress(Tango::Attribute &attr);
	virtual bool is_setB5FeedOperateModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB5FeedRegenerationModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB5FeedRegenerationModeProgress(Tango::Attribute &attr);
	virtual bool is_setB5FeedRegenerationModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setOperateModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setOperateModeProgress(Tango::Attribute &attr);
	virtual bool is_setOperateModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setStandbyLPModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setStandbyLPModeProgress(Tango::Attribute &attr);
	virtual bool is_setStandbyLPModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setMaintenanceModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setMaintenanceModeProgress(Tango::Attribute &attr);
	virtual bool is_setMaintenanceModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB1FeedStandbyLPModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB1FeedStandbyLPModeProgress(Tango::Attribute &attr);
	virtual bool is_setB1FeedStandbyLPModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB2FeedStandbyLPModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB2FeedStandbyLPModeProgress(Tango::Attribute &attr);
	virtual bool is_setB2FeedStandbyLPModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB3FeedStandbyLPModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB3FeedStandbyLPModeProgress(Tango::Attribute &attr);
	virtual bool is_setB3FeedStandbyLPModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB4FeedStandbyLPModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB4FeedStandbyLPModeProgress(Tango::Attribute &attr);
	virtual bool is_setB4FeedStandbyLPModeProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute setB5FeedStandbyLPModeProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_setB5FeedStandbyLPModeProgress(Tango::Attribute &attr);
	virtual bool is_setB5FeedStandbyLPModeProgress_allowed(Tango::AttReqType type);


	//--------------------------------------------------------
	/**
	 *	Method      : SPFEmulator::add_dynamic_attributes()
	 *	Description : Add dynamic attributes if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_attributes();




//	Command related methods
public:
	/**
	 *	Command SetLNAHPowerState related method
	 *	Description: Switch the H-channel LNA power on/off.
	 *               
	 *               == Allowed mode/state ==
	 *               spf_mode= {OPERATE,MAINTENANCE (TBD)}
	 *               
	 *               == Expected behavior ==
	 *               1) Depending on command bandId argument set value of device 
	 *               attributes: 
	 *               b1_lna_h_powerState, ..., b5_lna_h_powerState
	 *               
	 *               2) Switch SPF power state to FULL-POWER?
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin [0]: bandId
	 *               [1]: 0/1: enable/disable
	 */
	virtual void set_lnahpower_state(const Tango::DevVarShortArray *argin);
	virtual bool is_SetLNAHPowerState_allowed(const CORBA::Any &any);
	/**
	 *	Command SetLNAVPowerState related method
	 *	Description: Switch the V-channel LNA power on/off
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin [0]: bandId
	 *               [1]: 0/1: enable/disable
	 */
	virtual void set_lnavpower_state(const Tango::DevVarShortArray *argin);
	virtual bool is_SetLNAVPowerState_allowed(const CORBA::Any &any);
	/**
	 *	Command SetAmp2HPowerState related method
	 *	Description: Switch the 2nd stage H-channel amplifier power on/off
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin [0]: majorComponentBand
	 *               [1]: enable/disable
	 */
	virtual void set_amp2_hpower_state(const Tango::DevVarShortArray *argin);
	virtual bool is_SetAmp2HPowerState_allowed(const CORBA::Any &any);
	/**
	 *	Command SetAmp2VPowerState related method
	 *	Description: Switch the 2nd stage V-channel amplifier power on/off.
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin [0]: majorComponentBand
	 *               [1]: enable/disable
	 */
	virtual void set_amp2_vpower_state(const Tango::DevVarShortArray *argin);
	virtual bool is_SetAmp2VPowerState_allowed(const CORBA::Any &any);
	/**
	 *	Command SetRFE1TempSetPoint related method
	 *	Description: Change the current RFE 1st stage temperature control 
	 *               setpoint. This will not influence the default value. 
	 *               An extra parameter ?warm? is included in these service 
	 *               mode messages.
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin [0]: Major component band {B1,B2,B3,B4,B5}
	 *               [1]: temp set point 
	 *               {?disable? | ?sp1? | ?sp2? | ?sp3? | ?sp4? | ?sp5? | ?sp6? | ?warm?}
	 */
	virtual void set_rfe1_temp_set_point(const Tango::DevVarShortArray *argin);
	virtual bool is_SetRFE1TempSetPoint_allowed(const CORBA::Any &any);
	/**
	 *	Command SetRFE2TempCntrState related method
	 *	Description: Enable or disable the RFE 2nd stage temperature 
	 *               controller.
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin [0]: Major component band {B1,B2,B3,B4,B5,B6}
	 *               [1]: Enable/disable
	 */
	virtual void set_rfe2_temp_cntr_state(const Tango::DevVarShortArray *argin);
	virtual bool is_SetRFE2TempCntrState_allowed(const CORBA::Any &any);
	/**
	 *	Command SetCryoMotorState related method
	 *	Description: Enable or disable the cryocooler (motor)
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin [0]: cryoStat {B1,B2,B345}
	 *               [1]: enable/disable
	 */
	virtual void set_cryo_motor_state(const Tango::DevVarShortArray *argin);
	virtual bool is_SetCryoMotorState_allowed(const CORBA::Any &any);
	/**
	 *	Command SetCryoMotorSpeed related method
	 *	Description: Set the motor speed in rpm [45 - 90].
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin [0]: cryo stat {B1,B2,B345}
	 *               [1]: Speed in rpm {45,46,...90}
	 */
	virtual void set_cryo_motor_speed(const Tango::DevVarShortArray *argin);
	virtual bool is_SetCryoMotorSpeed_allowed(const CORBA::Any &any);
	/**
	 *	Command SetVaValveState related method
	 *	Description: Open/close the vacuum valve.
	 *               
	 *               NB: This command is deprecated from ICD release 2.
	 *               Replaced by writable attributes.
	 *
	 *	@param argin Enable/disable
	 */
	virtual void set_va_valve_state(Tango::DevBoolean argin);
	virtual bool is_SetVaValveState_allowed(const CORBA::Any &any);
	/**
	 *	Command SetSubElementMode related method
	 *	Description: Set the sub-element mode.
	 *               
	 *               NB: This command was deprecated from ICD v2. 
	 *               Replaced by single commands for each operating mode.
	 *
	 *	@param argin spfMode to be set. Allowed values are:
	 *               {Operate | Standby_LP | Maintenance}
	 */
	virtual void set_sub_element_mode(Tango::DevShort argin);
	virtual bool is_SetSubElementMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetFeedMode related method
	 *	Description: Request feed to go to a different state. 
	 *               Each receiver keeps track of its own errors in an 
	 *               error-register when it enters its error state. An FMECA 
	 *               sensor will indicate the fault to be corrected. 
	 *               After the fault is corrected by a technician 
	 *               the ``clear-errors-and-restart`` request should be sent to 
	 *               clear the errors in the feed error register in order to 
	 *               restart the feed.
	 *               
	 *               Valid values for mcBand argument are: {1-5}.
	 *               Valid values for feedMode argument are: {0,2}, 
	 *               where each enum code corresponds to:
	 *               
	 *               0= STANDBY-LP
	 *               1= OPERATE
	 *               2= MAINTENANCE
	 *
	 *	@param argin [0]: Major component band {B1,B2,B3,B4,B5}
	 *               [1]: seFeedMode {0,1,2}
	 *               
	 *               0= STANDBY-LP
	 *               1= OPERATE
	 *               2= MAINTENANCE
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *set_feed_mode(const Tango::DevVarShortArray *argin);
	virtual bool is_SetFeedMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SendFeedCommand related method
	 *	Description: Send a once-off command directly to the feed. 
	 *               Each feed can have different commands defined, and 
	 *               currently they are all defined in the same manner.
	 *               
	 *               Command is valid in spf mode {STANDBY-LP, OPERATE, MAINTENANCE}
	 *               
	 *               
	 *               Valid values for mcBand argument are: {1-5}.
	 *               Valid values for feedComamnd argument are: {0,1}, 
	 *               where each enum code corresponds to:
	 *               
	 *               0 = REGENERATE
	 *               1 = CLEAR-ERRORS
	 *               
	 *               Other additional commands defined in the future 
	 *               will follow from the above list into separate lists for 
	 *               each feed.
	 *
	 *	@param argin [0]: mcBand
	 *               [1]: feedCommand
	 */
	virtual void send_feed_command(const Tango::DevVarShortArray *argin);
	virtual bool is_SendFeedCommand_allowed(const CORBA::Any &any);
	/**
	 *	Command SetVacuumMode related method
	 *	Description: The SPF Vacuum service will nominally operate from its 
	 *               STANDBY-LP mode and will be mainly controlled by the 
	 *               feeds. In the case where lower level maintenance 
	 *               commands need to be performed, the vacuum service 
	 *               can be switched to maintenance mode. It will then be 
	 *               unavailable for use by a feed.
	 *               
	 *               Allowed only in MAINTENANCE mode
	 *
	 *	@param argin vaMode {STANDBY-LP, MAINTENANCE}
	 */
	virtual void set_vacuum_mode(Tango::DevShort argin);
	virtual bool is_SetVacuumMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SendVacuumCommand related method
	 *	Description: Send a once-off command directly to the SPF Vacuum 
	 *               service. Other additional commands defined 
	 *               in the future will follow from the above list.
	 *               
	 *               Allowed only in MAINTENANCE mode
	 *
	 *	@param argin vaCommand {RESET, CLEAR-ERRORS}
	 */
	virtual void send_vacuum_command(Tango::DevShort argin);
	virtual bool is_SendVacuumCommand_allowed(const CORBA::Any &any);
	/**
	 *	Command SetHeliumMode related method
	 *	Description: The SPF Helium service will nominally operate from 
	 *               its STANDBY-LP mode and will be mainly controlled 
	 *               by the feeds. In the case where lower level maintenance 
	 *               commands need to be performed, the helium service 
	 *               can be switched to maintenance mode. It will then be 
	 *               unavailable for use by a feed.
	 *               
	 *               Allowed only in MAINTENANCE mode
	 *
	 *	@param argin heMode
	 *               {STANDBY-LP, MAINTENANCE}
	 */
	virtual void set_helium_mode(Tango::DevShort argin);
	virtual bool is_SetHeliumMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SendHeliumCommand related method
	 *	Description: Send a once-off command directly to the SPF Helium 
	 *               service. 
	 *               Other additional commands defined in the future will 
	 *               follow from the above list.
	 *               
	 *               Command allowed only in MAINTENANCE mode
	 *
	 *	@param argin heCommand {RESET, CLEAR-ERRORS}
	 */
	virtual void send_helium_command(Tango::DevShort argin);
	virtual bool is_SendHeliumCommand_allowed(const CORBA::Any &any);
	/**
	 *	Command SetMCStartState related method
	 *	Description: Set a major component default start-up state.
	 *
	 *	@param argin [0]: majorComponent {B1, B2, B3, B4, B5, He, Va, SPFC}
	 *               [1]: mcStartupState
	 */
	virtual void set_mcstart_state(const Tango::DevVarShortArray *argin);
	virtual bool is_SetMCStartState_allowed(const CORBA::Any &any);
	/**
	 *	Command SetStandbyLPMode related method
	 *	Description: Set SPF in STANDBY-LP mode
	 *
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *set_standby_lpmode();
	virtual bool is_SetStandbyLPMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetOperateMode related method
	 *	Description: Set the SPF sub-element to OPERATE mode. 
	 *               This command is expected to last 30 seconds 
	 *               at maximum by requirements (TBC) and uses 
	 *               the pattern 3 in ICD for reply.
	 *               Corresponding command progress status attribute 
	 *               is named operateStatus.
	 *               
	 *               Expected actions on SPF internal components:
	 *               
	 *               - Close the vacuum valve (if not closed)
	 *               - Set the temp set point for all bands (setpoint is set to the default)
	 *               - Enable temp controllers for all bands (if not enabled)
	 *               - Enable the cryocooler (if not enabled, motor speed set to default)
	 *               - Enable LNAs (TBC)
	 *               - Set mode attribute to OPERATE
	 *
	 *	@returns == Long field ==
	 *           [0]: return code  {0=OK, 1=QUEUED, 2=FAILED}
	 *           
	 *           == String field ==
	 *           [0]: informative message or description of error/fault
	 */
	virtual Tango::DevVarLongStringArray *set_operate_mode();
	virtual bool is_SetOperateMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetMaintenanceMode related method
	 *	Description: Set SPF in MAINTENANCE mode
	 *
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *set_maintenance_mode();
	virtual bool is_SetMaintenanceMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetSubElementModeStr related method
	 *	Description: 
	 *
	 *	@param argin String mode
	 */
	virtual void set_sub_element_mode_str(Tango::DevString argin);
	virtual bool is_SetSubElementModeStr_allowed(const CORBA::Any &any);
	/**
	 *	Command Halt related method
	 *	Description: Terminate the SPF components, excluding this 
	 *               SPFController, in a graceful way and hand over the 
	 *               builtin watchdog timer (WDT) to the Linux OS 
	 *               so that it will not reset the hardware.
	 *               
	 *               NB: Once invoked the system cannot be 
	 *               started again from the SPFController device. 
	 *               For this a manual command over an SSH login terminal 
	 *               is required.
	 *
	 */
	virtual void halt();
	virtual bool is_Halt_allowed(const CORBA::Any &any);
	/**
	 *	Command Shutdown related method
	 *	Description: Shutdown the SPF components, including the 
	 *               SPFController. Only invoked from a STANDBY-LP mode. 
	 *               This command has to be used when a power cut occurs 
	 *               in the Dish. (TBC)
	 *
	 */
	virtual void shutdown();
	virtual bool is_Shutdown_allowed(const CORBA::Any &any);
	/**
	 *	Command Restart related method
	 *	Description: Performs a complete restart of SPF software components 
	 *               excluding the SPFController Tango device itself. 
	 *               To restart the SPFController device the corresponding 
	 *               Tango adm commands are to be used.
	 *               
	 *               NB: The command will also cause a hardware reset 
	 *               of the SPFC using the built-in watchdog timer (WDT) of 
	 *               the ARM processor.
	 *
	 */
	virtual void restart();
	virtual bool is_Restart_allowed(const CORBA::Any &any);
	/**
	 *	Command SetFeedOperateMode related method
	 *	Description: Set feed mode to OPERATE mode
	 *
	 *	@param argin Band id
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *set_feed_operate_mode(Tango::DevShort argin);
	virtual bool is_SetFeedOperateMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetFeedStandbyLPMode related method
	 *	Description: Set feed to STANDBY-LP mode
	 *
	 *	@param argin Band id
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *set_feed_standby_lpmode(Tango::DevShort argin);
	virtual bool is_SetFeedStandbyLPMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetFeedMaintenanceMode related method
	 *	Description: Set feed to MAINTENANCE mode
	 *
	 *	@param argin bandId
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *set_feed_maintenance_mode(Tango::DevShort argin);
	virtual bool is_SetFeedMaintenanceMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetFeedClearErrorsAndRestartMode related method
	 *	Description: Set feed to ERROR mode
	 *               
	 *               NB: Deprecated after ICD update in July 2017
	 *
	 *	@param argin bandId
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *set_feed_clear_errors_and_restart_mode(Tango::DevShort argin);
	virtual bool is_SetFeedClearErrorsAndRestartMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetFeedRegenerationMode related method
	 *	Description: Set feed to REGENERATION mode
	 *               
	 *               NB: Deprecated after ICD update in July 2017
	 *
	 *	@param argin bandId
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *set_feed_regeneration_mode(Tango::DevShort argin);
	virtual bool is_SetFeedRegenerationMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetCalSourceTempSetPointsToDefault related method
	 *	Description: Set calibration source temperature setpoints for all bands to default values
	 *
	 */
	virtual void set_cal_source_temp_set_points_to_default();
	virtual bool is_SetCalSourceTempSetPointsToDefault_allowed(const CORBA::Any &any);
	/**
	 *	Command SetTempSetPointsToDefault related method
	 *	Description: Set temperature set points to default values
	 *
	 */
	virtual void set_temp_set_points_to_default();
	virtual bool is_SetTempSetPointsToDefault_allowed(const CORBA::Any &any);

//	Dynamic commands methods
public:
	/**
	 *	Command dummyDynCmd related method
	 *	Description: 
	 *
	 */
	virtual void dummy_dyn_cmd(Tango::Command &command);
	virtual bool is_dummyDynCmd_allowed(const CORBA::Any &any);
	void add_dummyDynCmd_dynamic_command(string cmdname, bool device);
	void remove_dummyDynCmd_dynamic_command(string cmdname);

	//--------------------------------------------------------
	/**
	 *	Method      : SPFEmulator::add_dynamic_commands()
	 *	Description : Add dynamic commands if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_commands();

/*----- PROTECTED REGION ID(SPFEmulator::Additional Method prototypes) ENABLED START -----*/

//	Additional Method prototypes
	protected:
		
		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::InitCmdAttrStateMachine()
	 	*	Description : Initialize cmd & attr state machine
	 	*/
		//--------------------------------------------------------
		virtual int InitCmdAttrStateMachine();

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::InitTask()
	 	*	Description : Initialize emulator yat4tango task
	 	*/
		//--------------------------------------------------------
		virtual int InitTask();

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::ClearTask()
	 	*	Description : Clearup emulator yat4tango task
	 	*/
		//--------------------------------------------------------
		virtual int ClearTask();

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::Startup()
	 	*	Description : Emulate SPF startup 
	 	*/
		//--------------------------------------------------------
		virtual int Startup();

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::SetOperateStatus()
	 	*	Description : Set operate mode task status
	 	*/
		//--------------------------------------------------------
		void SetOperateStatus(std::string status,std::string info,bool clear=true);

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::SetMaintenanceStatus()
	 	*	Description : Set maintenance mode task status
	 	*/
		//--------------------------------------------------------
		void SetMaintenanceStatus(std::string status,std::string info,bool clear=true);

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::SetStandbyLPStatus()
	 	*	Description : Set standbyLP mode task status
	 	*/
		//--------------------------------------------------------
		void SetStandbyLPStatus(std::string status,std::string info,bool clear=true);

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::SetFeedOperateStatus()
	 	*	Description : Set feed to OPERATE mode progress status attr
	 	*/
		//--------------------------------------------------------
		void SetFeedOperateStatus(int bandId,std::string status,std::string info,bool clear=true);

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::SetFeedRegenerationStatus()
	 	*	Description : Set feed to REGENERATION mode progress status attr
	 	*/
		//--------------------------------------------------------
		void SetFeedRegenerationStatus(int bandId,std::string status,std::string info,bool clear=true);

		//--------------------------------------------------------
		/**
	 	*	Method      : SPFEmulator::SetFeedStandbyLPStatus()
	 	*	Description : Set feed to STANDBY-LP mode progress status attr
	 	*/
		//--------------------------------------------------------
		void SetFeedStandbyLPStatus(int bandId,std::string status,std::string info,bool clear=true);

	friend class SPFEmulatorClass;
	friend class SPFEmulatorTask;

/*----- PROTECTED REGION END -----*/	//	SPFEmulator::Additional Method prototypes
};

/*----- PROTECTED REGION ID(SPFEmulator::Additional Classes Definitions) ENABLED START -----*/

//	Additional Classes Definitions

/*----- PROTECTED REGION END -----*/	//	SPFEmulator::Additional Classes Definitions

}	//	End of namespace

#endif   //	SPFEmulator_H
