/*----- PROTECTED REGION ID(RxEmulator.h) ENABLED START -----*/
//=============================================================================
//
// file :        RxEmulator.h
//
// description : Include file for the RxEmulator class
//
// project :     RxEmulator
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


#ifndef RxEmulator_H
#define RxEmulator_H

#include <tango.h>
#include <LMCDevice.h>

#include <yat4tango/InnerAppender.h>
#include <yat/memory/DataBuffer.h>
#include <RxEmulatorTask.h>

#include <map>
#include <utility>

/*----- PROTECTED REGION END -----*/	//	RxEmulator.h

/**
 *  RxEmulator class description:
 *    RxEmulator
 */

namespace RxEmulator_ns
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

/*----- PROTECTED REGION ID(RxEmulator::Additional Class Declarations) ENABLED START -----*/

//	Additional Class Declarations
class RxEmulatorTask;

/*----- PROTECTED REGION END -----*/	//	RxEmulator::Additional Class Declarations

class RxEmulator : public LMCDevice_ns::LMCDevice
{

/*----- PROTECTED REGION ID(RxEmulator::Data Members) ENABLED START -----*/

//	Add your own data members
	protected:
		//- the device task
		RxEmulatorTask* m_task;

		//- Mutex
		omni_mutex m_mutex;

/*----- PROTECTED REGION END -----*/	//	RxEmulator::Data Members

//	Device property data members
public:
	//	StartupTime:	Average time for starting up Rx in seconds
	Tango::DevULong	startupTime;
	//	ConfigureTime:	Average time taken for configuring a band
	Tango::DevULong	configureTime;
	//	EnableStartupFailure:	Enable/disable simulation of startup failures
	Tango::DevBoolean	enableStartupFailure;
	//	StartupFailureProbability:	Startup failure probability in range (0,1)
	Tango::DevFloat	startupFailureProbability;
	//	EnableConfigureFailure:	Enable/disable simulation of configure failures during configure step
	Tango::DevBoolean	enableConfigureFailure;
	//	ConfigureFailureProbability:	Failure probability in range (0,1) for configure stage
	Tango::DevFloat	configureFailureProbability;
	//	B1NominalGain:	Default gain (in dB) expected to be achieved when the 
	//  Nominal Attenuation is applied to the SPFRx.  These gain 
	//  values are on a per band basis.
	//  The purpose of this value is for informational purposes only.  
	//  This will be configured at build/integration for each device.
	//  
	//  B1 range: 0-12 dB, default=TBD
	Tango::DevFloat	b1NominalGain;
	//	B2NominalGain:	Default gain (in dB) expected to be achieved when the 
	//  Nominal Attenuation is applied to the SPFRx.  These gain 
	//  values are on a per band basis.
	//  The purpose of this value is for informational purposes only.  
	//  This will be configured at build/integration for each device.
	//  
	//  B2 range 0-8 dB, default=TBD
	Tango::DevFloat	b2NominalGain;
	//	B3NominalGain:	Default gain (in dB) expected to be achieved when the 
	//  Nominal Attenuation is applied to the SPFRx.  These gain 
	//  values are on a per band basis.
	//  The purpose of this value is for informational purposes only.  
	//  This will be configured at build/integration for each device.
	//  
	//  B3 range 0-8 dB, default=TBD
	Tango::DevFloat	b3NominalGain;
	//	B4NominalGain:	Default gain (in dB) expected to be achieved when the 
	//  Nominal Attenuation is applied to the SPFRx.  These gain 
	//  values are on a per band basis.
	//  The purpose of this value is for informational purposes only.  
	//  This will be configured at build/integration for each device.
	//  
	//  B4: TBD
	Tango::DevFloat	b4NominalGain;
	//	B5aNominalGain:	Default gain (in dB) expected to be achieved when the 
	//  Nominal Attenuation is applied to the SPFRx.  These gain 
	//  values are on a per band basis.
	//  The purpose of this value is for informational purposes only.  
	//  This will be configured at build/integration for each device.
	//  
	//  B5a: TBD
	Tango::DevFloat	b5aNominalGain;
	//	B5bNominalGain:	Default gain (in dB) expected to be achieved when the 
	//  Nominal Attenuation is applied to the SPFRx.  These gain 
	//  values are on a per band basis.
	//  The purpose of this value is for informational purposes only.  
	//  This will be configured at build/integration for each device.
	//  
	//  B5b: TBD
	Tango::DevFloat	b5bNominalGain;
	//	B1MaxGain:	The Maximum Gain value (in dB) possible by the hardware 
	//  for each band.  This is determined experimentally for each 
	//  digitizer and will be unlikely to change unless hardware 
	//  characterization changes.  It will be configured in the system 
	//  at deployment for each receiver. 
	//  
	//  B1: by up to 46 dB, default=TBD
	Tango::DevFloat	b1MaxGain;
	//	B2MaxGain:	The Maximum Gain value (in dB) possible by the hardware 
	//  for each band.  This is determined experimentally for each 
	//  digitizer and will be unlikely to change unless hardware 
	//  characterization changes.  It will be configured in the system 
	//  at deployment for each receiver. 
	//  
	//  B2: by up to 46 dB, default=TBD
	Tango::DevFloat	b2MaxGain;
	//	B3MaxGain:	The Maximum Gain value (in dB) possible by the hardware 
	//  for each band.  This is determined experimentally for each 
	//  digitizer and will be unlikely to change unless hardware 
	//  characterization changes.  It will be configured in the system 
	//  at deployment for each receiver. 
	//  
	//  B3: by up to 42 dB, default=TBD
	Tango::DevFloat	b3MaxGain;
	//	B4MaxGain:	The Maximum Gain value (in dB) possible by the hardware 
	//  for each band.  This is determined experimentally for each 
	//  digitizer and will be unlikely to change unless hardware 
	//  characterization changes.  It will be configured in the system 
	//  at deployment for each receiver. 
	//  
	//  B4: TBD
	Tango::DevFloat	b4MaxGain;
	//	B5aMaxGain:	The Maximum Gain value (in dB) possible by the hardware 
	//  for each band.  This is determined experimentally for each 
	//  digitizer and will be unlikely to change unless hardware 
	//  characterization changes.  It will be configured in the system 
	//  at deployment for each receiver. 
	//  
	//  B5: TBD
	Tango::DevFloat	b5aMaxGain;
	//	B5bMaxGain:	The Maximum Gain value (in dB) possible by the hardware 
	//  for each band.  This is determined experimentally for each 
	//  digitizer and will be unlikely to change unless hardware 
	//  characterization changes.  It will be configured in the system 
	//  at deployment for each receiver. 
	//  
	//  B5: TBD
	Tango::DevFloat	b5bMaxGain;
	//	B1PolHNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	//  
	//  Default value is determined at build/integration time and is 
	//  device dependent.
	Tango::DevFloat	b1PolHNominalAttenuation;
	//	B2PolHNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b2PolHNominalAttenuation;
	//	B3PolHNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b3PolHNominalAttenuation;
	//	B4PolHNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b4PolHNominalAttenuation;
	//	B5aPolHNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b5aPolHNominalAttenuation;
	//	B5bPolHNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b5bPolHNominalAttenuation;
	//	B1PolVNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b1PolVNominalAttenuation;
	//	B2PolVNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b2PolVNominalAttenuation;
	//	B3PolVNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b3PolVNominalAttenuation;
	//	B4PolVNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b4PolVNominalAttenuation;
	//	B5aPolVNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b5aPolVNominalAttenuation;
	//	B5bPolVNominalAttenuation:	The default attenuation in dB that is expected to be applied to 
	//  the receiver by default when capturing data.  The attenuation is 
	//  defined on per band (1...5b) and on a per polarization 
	//  (horizontal and vertical) basis and is the attenuation required 
	//  to achieve the nominal gain.
	Tango::DevFloat	b5bPolVNominalAttenuation;
	//	NoiseDiodeStateDefault:	Default noise diode generation model 
	//  (0=OFF, 1=PERIODIC, 2=PSEUDO-RANDOM)
	Tango::DevShort	noiseDiodeStateDefault;
	//	PeriodicNoiseDiodeParsDefault:	Default noise source config parameters 
	//  (start time, period, on time, off time) for periodic model.
	//  Default values are TBD
	vector<Tango::DevFloat>	periodicNoiseDiodeParsDefault;
	//	PseudoRandomNoiseDiodeParsDefault:	Default noise source config parameters 
	//  (start time, on time, off time, seed) for pseudo-random model.
	//  Default values are TBD
	vector<Tango::DevFloat>	pseudoRandomNoiseDiodeParsDefault;
	//	B1NominalSamplingClockFrequency:	Nominal band sampling clock frequency.
	//  These will define a nominal frequency for the sampling clock that will be used when no other Sampling Clock Frequency has been defined. These can be configured at integration time at each dish
	Tango::DevLong	b1NominalSamplingClockFrequency;
	//	B2NominalSamplingClockFrequency:	Nominal band sampling clock frequency.
	//  These will define a nominal frequency for the sampling clock that will be used when no other Sampling Clock Frequency has been defined. These can be configured at integration time at each dish.
	Tango::DevLong	b2NominalSamplingClockFrequency;
	//	B3NominalSamplingClockFrequency:	Nominal band sampling clock frequency.
	//  These will define a nominal frequency for the sampling clock that will be used when no other Sampling Clock Frequency has been defined. These can be configured at integration time at each dish.
	Tango::DevLong	b3NominalSamplingClockFrequency;
	//	B4NominalSamplingClockFrequency:	Nominal band sampling clock frequency.
	//  These will define a nominal frequency for the sampling clock that will be used when no other Sampling Clock Frequency has been defined. These can be configured at integration time at each dish.
	Tango::DevLong	b4NominalSamplingClockFrequency;
	//	B5aNominalSamplingClockFrequency:	Nominal band sampling clock frequency.
	//  These will define a nominal frequency for the sampling clock that will be used when no other Sampling Clock Frequency has been defined. These can be configured at integration time at each dish
	Tango::DevLong	b5aNominalSamplingClockFrequency;
	//	B5bNominalSamplingClockFrequency:	Nominal band sampling clock frequency.
	//  These will define a nominal frequency for the sampling clock that will be used when no other Sampling Clock Frequency has been defined. These can be configured at integration time at each dish.
	Tango::DevLong	b5bNominalSamplingClockFrequency;
	//	ConfiguredBandDefault:	Default configured band
	Tango::DevShort	configuredBandDefault;
	//	ExpectedOnlineDefault:	Default value for expected online flags
	Tango::DevBoolean	expectedOnlineDefault;

	bool	mandatoryNotDefined;

//	Attribute data members
public:
	Tango::DevString	*attr_configureBandProgress_read;
	Tango::DevString	*attr_firmware0UpdateProgress_read;
	Tango::DevString	*attr_firmware1UpdateProgress_read;
	Tango::DevString	*attr_firmware2UpdateProgress_read;
	Tango::DevString	*attr_firmware3UpdateProgress_read;

//	Constructors and destructors
public:
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	RxEmulator(Tango::DeviceClass *cl,string &s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	RxEmulator(Tango::DeviceClass *cl,const char *s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device name
	 *	@param d	Device description.
	 */
	RxEmulator(Tango::DeviceClass *cl,const char *s,const char *d);
	/**
	 * The device object destructor.
	 */
	~RxEmulator() {delete_device();};


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
	 *	Method      : RxEmulator::read_attr_hardware()
	 *	Description : Hardware acquisition for attributes.
	 */
	//--------------------------------------------------------
	virtual void read_attr_hardware(vector<long> &attr_list);
	//--------------------------------------------------------
	/*
	 *	Method      : RxEmulator::write_attr_hardware()
	 *	Description : Hardware writing for attributes.
	 */
	//--------------------------------------------------------
	virtual void write_attr_hardware(vector<long> &attr_list);

/**
 *	Attribute configureBandProgress related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 3
 */
	virtual void read_configureBandProgress(Tango::Attribute &attr);
	virtual bool is_configureBandProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute firmware0UpdateProgress related methods
 *	Description: Firmware update command status progress.  Will indicate the progress of a firmware update to update the FPGA firmware code at the designated location.
 *               
 *               [0]: status label {?COMPLETED?,?RUNNING?,?FAILED?,...}
 *               [1] : Firmware location { ?0?, ?1?, ?2?, ?3? }
 *               [2]: Firmware version information
 *               [3]: Informational information regarding status.
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 4
 */
	virtual void read_firmware0UpdateProgress(Tango::Attribute &attr);
	virtual bool is_firmware0UpdateProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute firmware1UpdateProgress related methods
 *	Description: .Firmware update command status progress.  Will indicate the progress of a firmware update to update the FPGA firmware code at the designated location.
 *               
 *               [0]: status label {?COMPLETED?,?RUNNING?,?FAILED?,...}
 *               [1] : Firmware location { ?0?, ?1?, ?2?, ?3? }
 *               [2]: Firmware version information
 *               [3]: Informational information regarding status.
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 4
 */
	virtual void read_firmware1UpdateProgress(Tango::Attribute &attr);
	virtual bool is_firmware1UpdateProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute firmware2UpdateProgress related methods
 *	Description: Firmware update command status progress.  Will indicate the progress of a firmware update to update the FPGA firmware code at the designated location.
 *               
 *               [0]: status label {?COMPLETED?,?RUNNING?,?FAILED?,...}
 *               [1] : Firmware location { ?0?, ?1?, ?2?, ?3? }
 *               [2]: Firmware version information
 *               [3]: Informational information regarding status.
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 4
 */
	virtual void read_firmware2UpdateProgress(Tango::Attribute &attr);
	virtual bool is_firmware2UpdateProgress_allowed(Tango::AttReqType type);
/**
 *	Attribute firmware3UpdateProgress related methods
 *	Description: Firmware update command status progress.  Will indicate the progress of a firmware update to update the FPGA firmware code at the designated location.
 *               
 *               [0]: status label {?COMPLETED?,?RUNNING?,?FAILED?,...}
 *               [1] : Firmware location { ?0?, ?1?, ?2?, ?3? }
 *               [2]: Firmware version information
 *               [3]: Informational information regarding status.
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 4
 */
	virtual void read_firmware3UpdateProgress(Tango::Attribute &attr);
	virtual bool is_firmware3UpdateProgress_allowed(Tango::AttReqType type);


	//--------------------------------------------------------
	/**
	 *	Method      : RxEmulator::add_dynamic_attributes()
	 *	Description : Add dynamic attributes if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_attributes();




//	Command related methods
public:
	/**
	 *	Command Synchronise related method
	 *	Description: Reset configured band sample counters.
	 *               Command only valid in DATA-CAPTURE mode.
	 *               
	 *               NB: Run over the selected band only
	 *
	 */
	virtual void synchronise();
	virtual bool is_Synchronise_allowed(const CORBA::Any &any);
	/**
	 *	Command ConfigureBand related method
	 *	Description: Set designated band and load the necessary configuration 
	 *               into the FPGA. This will prepare the FPGA so that on 
	 *               completion of this command, the FPGA 
	 *               and sub-element will begin capturing data.
	 *               The command automatically transition to 
	 *               DATA-CAPTURE in case of success, otherwise revert back
	 *               to STANDBY-FP (MAINTENANCE?).
	 *               
	 *               Allowed only in Rx modes:{STANDBY-FP,DATA-CAPTURE,MAINTENANCE}
	 *
	 *	@param argin 
	 */
	virtual void configure_band(Tango::DevShort argin);
	virtual bool is_ConfigureBand_allowed(const CORBA::Any &any);
	/**
	 *	Command ConfigureBand1 related method
	 *	Description: Set designated band and load the necessary configuration 
	 *               into the FPGA. This will prepare the FPGA so that on 
	 *               completion of this command, the FPGA 
	 *               and sub-element will begin capturing data.
	 *               The command automatically transition to 
	 *               DATA-CAPTURE in case of success, otherwise revert back
	 *               to STANDBY-FP (MAINTENANCE?).
	 *               
	 *               Allowed only in SPFRx modes:{STANDBY-FP,DATA-CAPTURE,MAINTENANCE}
	 *
	 */
	virtual void configure_band1();
	virtual bool is_ConfigureBand1_allowed(const CORBA::Any &any);
	/**
	 *	Command ConfigureBand2 related method
	 *	Description: Set designated band and load the necessary configuration 
	 *               into the FPGA. This will prepare the FPGA so that on 
	 *               completion of this command, the FPGA 
	 *               and sub-element will begin capturing data.
	 *               The command automatically transition to 
	 *               DATA-CAPTURE in case of success, otherwise revert back
	 *               to STANDBY-FP (MAINTENANCE?).
	 *               
	 *               Allowed only in SPFRx modes:{STANDBY-FP,DATA-CAPTURE,MAINTENANCE}
	 *
	 */
	virtual void configure_band2();
	virtual bool is_ConfigureBand2_allowed(const CORBA::Any &any);
	/**
	 *	Command ConfigureBand3 related method
	 *	Description: Set designated band and load the necessary configuration 
	 *               into the FPGA. This will prepare the FPGA so that on 
	 *               completion of this command, the FPGA 
	 *               and sub-element will begin capturing data.
	 *               The command automatically transition to 
	 *               DATA-CAPTURE in case of success, otherwise revert back
	 *               to STANDBY-FP (MAINTENANCE?).
	 *               
	 *               Allowed only in SPFRx modes:{STANDBY-FP,DATA-CAPTURE,MAINTENANCE}
	 *
	 */
	virtual void configure_band3();
	virtual bool is_ConfigureBand3_allowed(const CORBA::Any &any);
	/**
	 *	Command ConfigureBand4 related method
	 *	Description: Set designated band and load the necessary configuration 
	 *               into the FPGA. This will prepare the FPGA so that on 
	 *               completion of this command, the FPGA 
	 *               and sub-element will begin capturing data.
	 *               The command automatically transition to 
	 *               DATA-CAPTURE in case of success, otherwise revert back
	 *               to STANDBY-FP (MAINTENANCE?).
	 *               
	 *               Allowed only in SPFRx modes:{STANDBY-FP,DATA-CAPTURE,MAINTENANCE}
	 *
	 */
	virtual void configure_band4();
	virtual bool is_ConfigureBand4_allowed(const CORBA::Any &any);
	/**
	 *	Command ConfigureBand5a related method
	 *	Description: Set designated band and load the necessary configuration 
	 *               into the FPGA. This will prepare the FPGA so that on 
	 *               completion of this command, the FPGA 
	 *               and sub-element will begin capturing data.
	 *               The command automatically transition to 
	 *               DATA-CAPTURE in case of success, otherwise revert back
	 *               to STANDBY-FP (MAINTENANCE?).
	 *               
	 *               Allowed only in SPFRx modes:{STANDBY-FP,DATA-CAPTURE,MAINTENANCE}
	 *
	 */
	virtual void configure_band5a();
	virtual bool is_ConfigureBand5a_allowed(const CORBA::Any &any);
	/**
	 *	Command ConfigureBand5b related method
	 *	Description: Set designated band and load the necessary configuration 
	 *               into the FPGA. This will prepare the FPGA so that on 
	 *               completion of this command, the FPGA 
	 *               and sub-element will begin capturing data.
	 *               The command automatically transition to 
	 *               DATA-CAPTURE in case of success, otherwise revert back
	 *               to STANDBY-FP (MAINTENANCE?).
	 *               
	 *               Allowed only in SPFRx modes:{STANDBY-FP,DATA-CAPTURE,MAINTENANCE}
	 *
	 */
	virtual void configure_band5b();
	virtual bool is_ConfigureBand5b_allowed(const CORBA::Any &any);
	/**
	 *	Command ConfigureBandAsync related method
	 *	Description: Configure receiver band. 
	 *               This command Instructs the receiver to select the 
	 *               Set designated band and load the necessary configuration 
	 *               into the FPGA. This will prepare the FPGA so that on 
	 *               completion of this command, the FPGA 
	 *               and sub-element will begin capturing data.
	 *               The command automatically transition to 
	 *               DATA-CAPTURE in case of success, otherwise revert back
	 *               to STANDBY-FP (MAINTENANCE?).
	 *               
	 *               Allowed only in Rx modes:{STANDBY-FP,DATA-CAPTURE,MAINTENANCE}
	 *
	 *	@param argin Band id
	 *	@returns 
	 */
	virtual Tango::DevVarLongStringArray *configure_band_async(Tango::DevShort argin);
	virtual bool is_ConfigureBandAsync_allowed(const CORBA::Any &any);
	/**
	 *	Command CaptureData related method
	 *	Description: Start/stop capture on the configured band.
	 *               Command only valid in {DATA-CAPTURE, STANDBY-FP} modes
	 *
	 *	@param argin Flag value start/stop (0/1)
	 */
	virtual void capture_data(Tango::DevBoolean argin);
	virtual bool is_CaptureData_allowed(const CORBA::Any &any);
	/**
	 *	Command SetSubElementMode related method
	 *	Description: Set the sub-element mode.
	 *               Allowed set modes are: 
	 *               
	 *               Data_Capture
	 *               Standby_FP 
	 *               Standby_LP 
	 *               Maintenance
	 *
	 *	@param argin Operational mode 
	 *               {Data_Capture | Standby_FP | Standby_LP | Maintenance}
	 */
	virtual void set_sub_element_mode(Tango::DevShort argin);
	virtual bool is_SetSubElementMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetSubElementModeStr related method
	 *	Description: Set Rx mode
	 *
	 *	@param argin Requested mode. Allowed values are:
	 *               
	 *               {``STANDBY-LP``,``STANDBY-FP``,``DATA-CAPTURE``,``MAINTENANCE``}
	 */
	virtual void set_sub_element_mode_str(Tango::DevString argin);
	virtual bool is_SetSubElementModeStr_allowed(const CORBA::Any &any);
	/**
	 *	Command SetStandbyLPMode related method
	 *	Description: Activate STANDBY-LP mode
	 *               
	 *               Allowed only in mode: {STANDBY-LP, STANDBY-FP, MAINTENANCE}
	 *
	 */
	virtual void set_standby_lpmode();
	virtual bool is_SetStandbyLPMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetStandbyFPMode related method
	 *	Description: Activate STANDBY-FP mode
	 *               
	 *               Allowed only in mode: {STANDBY-LP,STANDBY-FP,MAINTENANCE,CONFIGURE,DATA-CAPTURE}
	 *
	 */
	virtual void set_standby_fpmode();
	virtual bool is_SetStandbyFPMode_allowed(const CORBA::Any &any);
	/**
	 *	Command SetMaintenanceMode related method
	 *	Description: Activate MAINTENANCE mode. 
	 *               Allowed only in mode: {STANDBY-LP, STANDBY-FP, MAINTENANCE, DATA-CAPTURE?}
	 *
	 */
	virtual void set_maintenance_mode();
	virtual bool is_SetMaintenanceMode_allowed(const CORBA::Any &any);
	/**
	 *	Command EnableEngineeringMode related method
	 *	Description: Allows the caller to enable engineering mode.  
	 *               When in engineering mode, many of the features become 
	 *               available that may be resticted when running in 
	 *               regular operational mode.  This is considered an expert 
	 *               mode and should only be enabled by expert users.
	 *
	 *	@param argin Ture to enable engineering mode.  False to return to regular operations mode.
	 */
	virtual void enable_engineering_mode(Tango::DevBoolean argin);
	virtual bool is_EnableEngineeringMode_allowed(const CORBA::Any &any);
	/**
	 *	Command UpdateFirmware related method
	 *	Description: his command instructs the SPFRx to update the firmware to the latest loaded on the controller.  Firmware file must be pre-loaded into the correct location on the SPFRxSW hardware.
	 *
	 *	@param argin Firmware Position to Update
	 */
	virtual void update_firmware(Tango::DevShort argin);
	virtual bool is_UpdateFirmware_allowed(const CORBA::Any &any);
	/**
	 *	Command SetHighPowerState related method
	 *	Description: True for enabling High Power Mode.  
	 *               False for Low Power Mode
	 *
	 *	@param argin 
	 */
	virtual void set_high_power_state(Tango::DevBoolean argin);
	virtual bool is_SetHighPowerState_allowed(const CORBA::Any &any);
	/**
	 *	Command AdjustPolHAttenuation related method
	 *	Description: Allows the adjustment of the currently set attenuation.  
	 *               This adjusts the attenuation a + or - value from the 
	 *               current offset (TBD: this may be from nominal).  
	 *               TBD: Can only be run when in engineering mode.
	 *               
	 *               Adjustments to the attenuation and not preserved 
	 *               across power-up or changing bands.
	 *               
	 *               Command allowed only in modes 
	 *               {STANDBY-LP, STANDBY-FP,MAINTENANCE,DATA-CAPTURE}
	 *
	 *	@param argin attenuationOffset
	 */
	virtual void adjust_pol_hattenuation(Tango::DevFloat argin);
	virtual bool is_AdjustPolHAttenuation_allowed(const CORBA::Any &any);
	/**
	 *	Command AdjustPolVAttenuation related method
	 *	Description: This adjusts the attenuation a + or - value from the 
	 *               current offset (TBD: this may be from nominal).  
	 *               TBD: Can only be run when in engineering mode.
	 *               
	 *               Adjustments to the attenuation and not preserved 
	 *               across power-up or changing bands.
	 *               
	 *               Command allowed only in modes 
	 *               {STANDBY-LP, STANDBY-FP,MAINTENANCE,DATA-CAPTURE}
	 *
	 *	@param argin attenuationOffset
	 */
	virtual void adjust_pol_vattenuation(Tango::DevFloat argin);
	virtual bool is_AdjustPolVAttenuation_allowed(const CORBA::Any &any);
	/**
	 *	Command Shutdown related method
	 *	Description: Shutdown RXPU
	 *
	 */
	virtual void shutdown();
	virtual bool is_Shutdown_allowed(const CORBA::Any &any);

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
	 *	Method      : RxEmulator::add_dynamic_commands()
	 *	Description : Add dynamic commands if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_commands();

/*----- PROTECTED REGION ID(RxEmulator::Additional Method prototypes) ENABLED START -----*/

//	Additional Method prototypes
	protected:
		
		//--------------------------------------------------------
		/**
	 	*	Method      : RxEmulator::InitCmdAttrStateMachine()
	 	*	Description : Initialize cmd & attr state machine
	 	*/
		//--------------------------------------------------------
		virtual int InitCmdAttrStateMachine();

		//--------------------------------------------------------
		/**
	 	*	Method      : RxEmulator::InitTask()
	 	*	Description : Initialize emulator yat4tango task
	 	*/
		//--------------------------------------------------------
		virtual int InitTask();

		//--------------------------------------------------------
		/**
	 	*	Method      : RxEmulator::ClearTask()
	 	*	Description : Clearup emulator yat4tango task
	 	*/
		//--------------------------------------------------------
		virtual int ClearTask();

		//--------------------------------------------------------
		/**
	 	*	Method      : RxEmulator::Startup()
	 	*	Description : Emulate SPF startup 
	 	*/
		//--------------------------------------------------------
		virtual int Startup();

		//--------------------------------------------------------
		/**
	 	*	Method      : RxEmulator::SetConfigureStatus()
	 	*	Description : Set configure status progress attr
	 	*/
		//--------------------------------------------------------
		void SetConfigureStatus(std::string status,std::string info,bool clear=true);

		//--------------------------------------------------------
		/**
	 	*	Method      : RxEmulator::SetUpdateFirmwareStatus()
	 	*	Description : Set update firmware status progress attr
	 	*/
		//--------------------------------------------------------
		void SetUpdateFirmwareStatus(std::string status,std::string info,int firmwareId,std::string firmwareVersInfo="",bool clear=true);
	
		//--------------------------------------------------------
		/**
	 	*	Method      : RxEmulator::InitAttrsToDefault()
	 	*	Description : Initialize attribute to their default values
	 	*/
		//--------------------------------------------------------
		int InitAttrsToDefault();

		//--------------------------------------------------------
		/**
	 	*	Method      : RxEmulator::ConfigureBand()
	 	*	Description : Configure receiver band
	 	*/
		//--------------------------------------------------------
		void ConfigureBand(Tango::DevShort bandId);

	friend class RxEmulatorClass;
	friend class RxEmulatorTask;

/*----- PROTECTED REGION END -----*/	//	RxEmulator::Additional Method prototypes
};

/*----- PROTECTED REGION ID(RxEmulator::Additional Classes Definitions) ENABLED START -----*/

//	Additional Classes Definitions

/*----- PROTECTED REGION END -----*/	//	RxEmulator::Additional Classes Definitions

}	//	End of namespace

#endif   //	RxEmulator_H
