/*----- PROTECTED REGION ID(LMCDeviceClass.h) ENABLED START -----*/
//=============================================================================
//
// file :        LMCDeviceClass.h
//
// description : Include for the LMCDevice root class.
//               This class is the singleton class for
//                the LMCDevice device class.
//               It contains all properties and methods which the 
//               LMCDevice requires only once e.g. the commands.
//
// project :     LMCDevice
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


#ifndef LMCDeviceClass_H
#define LMCDeviceClass_H

#include <tango.h>
#include <LMCDevice.h>


/*----- PROTECTED REGION END -----*/	//	LMCDeviceClass.h


namespace LMCDevice_ns
{
/*----- PROTECTED REGION ID(LMCDeviceClass::classes for dynamic creation) ENABLED START -----*/


/*----- PROTECTED REGION END -----*/	//	LMCDeviceClass::classes for dynamic creation

//=========================================
//	Define classes for attributes
//=========================================
//	Attribute loggingLevelCentral class definition
class loggingLevelCentralAttrib: public Tango::Attr
{
public:
	loggingLevelCentralAttrib():Attr("loggingLevelCentral",
			Tango::DEV_LONG, Tango::READ_WRITE) {};
	~loggingLevelCentralAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_loggingLevelCentral(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_loggingLevelCentral(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_loggingLevelCentral_allowed(ty);}
};

//	Attribute loggingLevelElement class definition
class loggingLevelElementAttrib: public Tango::Attr
{
public:
	loggingLevelElementAttrib():Attr("loggingLevelElement",
			Tango::DEV_LONG, Tango::READ_WRITE) {};
	~loggingLevelElementAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_loggingLevelElement(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_loggingLevelElement(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_loggingLevelElement_allowed(ty);}
};

//	Attribute loggingLevelStorage class definition
class loggingLevelStorageAttrib: public Tango::Attr
{
public:
	loggingLevelStorageAttrib():Attr("loggingLevelStorage",
			Tango::DEV_LONG, Tango::READ_WRITE) {};
	~loggingLevelStorageAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_loggingLevelStorage(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_loggingLevelStorage(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_loggingLevelStorage_allowed(ty);}
};

//	Attribute loggingLevelConsole class definition
class loggingLevelConsoleAttrib: public Tango::Attr
{
public:
	loggingLevelConsoleAttrib():Attr("loggingLevelConsole",
			Tango::DEV_LONG, Tango::READ_WRITE) {};
	~loggingLevelConsoleAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_loggingLevelConsole(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_loggingLevelConsole(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_loggingLevelConsole_allowed(ty);}
};

//	Attribute buildState class definition
class buildStateAttrib: public Tango::Attr
{
public:
	buildStateAttrib():Attr("buildState",
			Tango::DEV_STRING, Tango::READ) {};
	~buildStateAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_buildState(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_buildState_allowed(ty);}
};

//	Attribute centralLoggerStatus class definition
class centralLoggerStatusAttrib: public Tango::Attr
{
public:
	centralLoggerStatusAttrib():Attr("centralLoggerStatus",
			Tango::DEV_ENUM, Tango::READ) {};
	~centralLoggerStatusAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_centralLoggerStatus(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_centralLoggerStatus_allowed(ty);}
	virtual bool same_type(const type_info &in_type) {return typeid(centralLoggerStatusEnum) == in_type;}
	virtual string get_enum_type() {return string("centralLoggerStatusEnum");}
};

//	Attribute elementLoggerStatus class definition
class elementLoggerStatusAttrib: public Tango::Attr
{
public:
	elementLoggerStatusAttrib():Attr("elementLoggerStatus",
			Tango::DEV_ENUM, Tango::READ) {};
	~elementLoggerStatusAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_elementLoggerStatus(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_elementLoggerStatus_allowed(ty);}
	virtual bool same_type(const type_info &in_type) {return typeid(elementLoggerStatusEnum) == in_type;}
	virtual string get_enum_type() {return string("elementLoggerStatusEnum");}
};

//	Attribute storageLoggerStatus class definition
class storageLoggerStatusAttrib: public Tango::Attr
{
public:
	storageLoggerStatusAttrib():Attr("storageLoggerStatus",
			Tango::DEV_ENUM, Tango::READ) {};
	~storageLoggerStatusAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_storageLoggerStatus(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_storageLoggerStatus_allowed(ty);}
	virtual bool same_type(const type_info &in_type) {return typeid(storageLoggerStatusEnum) == in_type;}
	virtual string get_enum_type() {return string("storageLoggerStatusEnum");}
};

//	Attribute eventRate class definition
class eventRateAttrib: public Tango::Attr
{
public:
	eventRateAttrib():Attr("eventRate",
			Tango::DEV_FLOAT, Tango::READ) {};
	~eventRateAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_eventRate(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_eventRate_allowed(ty);}
};

//	Attribute exceptionRate class definition
class exceptionRateAttrib: public Tango::Attr
{
public:
	exceptionRateAttrib():Attr("exceptionRate",
			Tango::DEV_FLOAT, Tango::READ) {};
	~exceptionRateAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_exceptionRate(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_exceptionRate_allowed(ty);}
};

//	Attribute fwdAttrInitStatus class definition
class fwdAttrInitStatusAttrib: public Tango::Attr
{
public:
	fwdAttrInitStatusAttrib():Attr("fwdAttrInitStatus",
			Tango::DEV_ENUM, Tango::READ) {};
	~fwdAttrInitStatusAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_fwdAttrInitStatus(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_fwdAttrInitStatus_allowed(ty);}
	virtual bool same_type(const type_info &in_type) {return typeid(fwdAttrInitStatusEnum) == in_type;}
	virtual string get_enum_type() {return string("fwdAttrInitStatusEnum");}
};

//	Attribute centralLoggerEnabled class definition
class centralLoggerEnabledAttrib: public Tango::Attr
{
public:
	centralLoggerEnabledAttrib():Attr("centralLoggerEnabled",
			Tango::DEV_BOOLEAN, Tango::READ_WRITE) {};
	~centralLoggerEnabledAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_centralLoggerEnabled(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_centralLoggerEnabled(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_centralLoggerEnabled_allowed(ty);}
};

//	Attribute elementLoggerEnabled class definition
class elementLoggerEnabledAttrib: public Tango::Attr
{
public:
	elementLoggerEnabledAttrib():Attr("elementLoggerEnabled",
			Tango::DEV_BOOLEAN, Tango::READ_WRITE) {};
	~elementLoggerEnabledAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_elementLoggerEnabled(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_elementLoggerEnabled(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_elementLoggerEnabled_allowed(ty);}
};

//	Attribute storageLoggerEnabled class definition
class storageLoggerEnabledAttrib: public Tango::Attr
{
public:
	storageLoggerEnabledAttrib():Attr("storageLoggerEnabled",
			Tango::DEV_BOOLEAN, Tango::READ_WRITE) {};
	~storageLoggerEnabledAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_storageLoggerEnabled(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_storageLoggerEnabled(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_storageLoggerEnabled_allowed(ty);}
};

//	Attribute loggingTargetCentral class definition
class loggingTargetCentralAttrib: public Tango::Attr
{
public:
	loggingTargetCentralAttrib():Attr("loggingTargetCentral",
			Tango::DEV_STRING, Tango::READ_WRITE) {};
	~loggingTargetCentralAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_loggingTargetCentral(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_loggingTargetCentral(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_loggingTargetCentral_allowed(ty);}
};

//	Attribute loggingTargetElement class definition
class loggingTargetElementAttrib: public Tango::Attr
{
public:
	loggingTargetElementAttrib():Attr("loggingTargetElement",
			Tango::DEV_STRING, Tango::READ_WRITE) {};
	~loggingTargetElementAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_loggingTargetElement(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_loggingTargetElement(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_loggingTargetElement_allowed(ty);}
};

//	Attribute loggingTargetStorage class definition
class loggingTargetStorageAttrib: public Tango::Attr
{
public:
	loggingTargetStorageAttrib():Attr("loggingTargetStorage",
			Tango::DEV_STRING, Tango::READ_WRITE) {};
	~loggingTargetStorageAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_loggingTargetStorage(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_loggingTargetStorage(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_loggingTargetStorage_allowed(ty);}
};

//	Attribute attrRandomizationEnabled class definition
class attrRandomizationEnabledAttrib: public Tango::Attr
{
public:
	attrRandomizationEnabledAttrib():Attr("attrRandomizationEnabled",
			Tango::DEV_BOOLEAN, Tango::READ_WRITE) {};
	~attrRandomizationEnabledAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_attrRandomizationEnabled(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_attrRandomizationEnabled(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_attrRandomizationEnabled_allowed(ty);}
};

//	Attribute attrRandomizationPeriod class definition
class attrRandomizationPeriodAttrib: public Tango::Attr
{
public:
	attrRandomizationPeriodAttrib():Attr("attrRandomizationPeriod",
			Tango::DEV_ULONG, Tango::READ_WRITE) {};
	~attrRandomizationPeriodAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<LMCDevice *>(dev))->read_attrRandomizationPeriod(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<LMCDevice *>(dev))->write_attrRandomizationPeriod(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<LMCDevice *>(dev))->is_attrRandomizationPeriod_allowed(ty);}
};


//=========================================
//	Define classes for commands
//=========================================
//	Command InitializeLoggers class definition
class InitializeLoggersClass : public Tango::Command
{
public:
	InitializeLoggersClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	InitializeLoggersClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~InitializeLoggersClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_InitializeLoggers_allowed(any);}
};

//	Command InitializeFwdAttrs class definition
class InitializeFwdAttrsClass : public Tango::Command
{
public:
	InitializeFwdAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	InitializeFwdAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~InitializeFwdAttrsClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_InitializeFwdAttrs_allowed(any);}
};

//	Command RandomizeDynAttrs class definition
class RandomizeDynAttrsClass : public Tango::Command
{
public:
	RandomizeDynAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	RandomizeDynAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~RandomizeDynAttrsClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_RandomizeDynAttrs_allowed(any);}
};

//	Command InitializeDevProxies class definition
class InitializeDevProxiesClass : public Tango::Command
{
public:
	InitializeDevProxiesClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	InitializeDevProxiesClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~InitializeDevProxiesClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_InitializeDevProxies_allowed(any);}
};

//	Command GetRegisteredProxies class definition
class GetRegisteredProxiesClass : public Tango::Command
{
public:
	GetRegisteredProxiesClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetRegisteredProxiesClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetRegisteredProxiesClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetRegisteredProxies_allowed(any);}
};

//	Command GetRegisteredEvents class definition
class GetRegisteredEventsClass : public Tango::Command
{
public:
	GetRegisteredEventsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetRegisteredEventsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetRegisteredEventsClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetRegisteredEvents_allowed(any);}
};

//	Command GetRegisteredFormulas class definition
class GetRegisteredFormulasClass : public Tango::Command
{
public:
	GetRegisteredFormulasClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetRegisteredFormulasClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetRegisteredFormulasClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetRegisteredFormulas_allowed(any);}
};

//	Command GetInitializedFormulas class definition
class GetInitializedFormulasClass : public Tango::Command
{
public:
	GetInitializedFormulasClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetInitializedFormulasClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetInitializedFormulasClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetInitializedFormulas_allowed(any);}
};

//	Command GetNRunningTasks class definition
class GetNRunningTasksClass : public Tango::Command
{
public:
	GetNRunningTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetNRunningTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetNRunningTasksClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetNRunningTasks_allowed(any);}
};

//	Command GetNQueuedTasks class definition
class GetNQueuedTasksClass : public Tango::Command
{
public:
	GetNQueuedTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetNQueuedTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetNQueuedTasksClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetNQueuedTasks_allowed(any);}
};

//	Command GetNIdleTasks class definition
class GetNIdleTasksClass : public Tango::Command
{
public:
	GetNIdleTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetNIdleTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetNIdleTasksClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetNIdleTasks_allowed(any);}
};

//	Command GetNTasks class definition
class GetNTasksClass : public Tango::Command
{
public:
	GetNTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetNTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetNTasksClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetNTasks_allowed(any);}
};

//	Command GetNFailedTasks class definition
class GetNFailedTasksClass : public Tango::Command
{
public:
	GetNFailedTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetNFailedTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetNFailedTasksClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetNFailedTasks_allowed(any);}
};

//	Command GetNTotFailedTasks class definition
class GetNTotFailedTasksClass : public Tango::Command
{
public:
	GetNTotFailedTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetNTotFailedTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetNTotFailedTasksClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetNTotFailedTasks_allowed(any);}
};

//	Command IsTaskQuotaReached class definition
class IsTaskQuotaReachedClass : public Tango::Command
{
public:
	IsTaskQuotaReachedClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	IsTaskQuotaReachedClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~IsTaskQuotaReachedClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_IsTaskQuotaReached_allowed(any);}
};

//	Command FlushTaskQueue class definition
class FlushTaskQueueClass : public Tango::Command
{
public:
	FlushTaskQueueClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	FlushTaskQueueClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~FlushTaskQueueClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_FlushTaskQueue_allowed(any);}
};

//	Command GetQueuedTaskInfo class definition
class GetQueuedTaskInfoClass : public Tango::Command
{
public:
	GetQueuedTaskInfoClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetQueuedTaskInfoClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetQueuedTaskInfoClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetQueuedTaskInfo_allowed(any);}
};

//	Command GetTaskInfo class definition
class GetTaskInfoClass : public Tango::Command
{
public:
	GetTaskInfoClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetTaskInfoClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetTaskInfoClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetTaskInfo_allowed(any);}
};

//	Command ClearTasks class definition
class ClearTasksClass : public Tango::Command
{
public:
	ClearTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ClearTasksClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ClearTasksClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_ClearTasks_allowed(any);}
};

//	Command UpdateFormulaAttrs class definition
class UpdateFormulaAttrsClass : public Tango::Command
{
public:
	UpdateFormulaAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	UpdateFormulaAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~UpdateFormulaAttrsClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_UpdateFormulaAttrs_allowed(any);}
};

//	Command UpdateStatusAttrs class definition
class UpdateStatusAttrsClass : public Tango::Command
{
public:
	UpdateStatusAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	UpdateStatusAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~UpdateStatusAttrsClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_UpdateStatusAttrs_allowed(any);}
};

//	Command MonitorProxies class definition
class MonitorProxiesClass : public Tango::Command
{
public:
	MonitorProxiesClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	MonitorProxiesClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~MonitorProxiesClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_MonitorProxies_allowed(any);}
};

//	Command RevokeTask class definition
class RevokeTaskClass : public Tango::Command
{
public:
	RevokeTaskClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	RevokeTaskClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~RevokeTaskClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_RevokeTask_allowed(any);}
};

//	Command RevokeTaskSequence class definition
class RevokeTaskSequenceClass : public Tango::Command
{
public:
	RevokeTaskSequenceClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	RevokeTaskSequenceClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~RevokeTaskSequenceClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_RevokeTaskSequence_allowed(any);}
};

//	Command GetFormulaData class definition
class GetFormulaDataClass : public Tango::Command
{
public:
	GetFormulaDataClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetFormulaDataClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetFormulaDataClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetFormulaData_allowed(any);}
};

//	Command SendTestLogMsg class definition
class SendTestLogMsgClass : public Tango::Command
{
public:
	SendTestLogMsgClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	SendTestLogMsgClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~SendTestLogMsgClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_SendTestLogMsg_allowed(any);}
};

//	Command ClearTaskHistory class definition
class ClearTaskHistoryClass : public Tango::Command
{
public:
	ClearTaskHistoryClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ClearTaskHistoryClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ClearTaskHistoryClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_ClearTaskHistory_allowed(any);}
};

//	Command GetLogAppenderInfo class definition
class GetLogAppenderInfoClass : public Tango::Command
{
public:
	GetLogAppenderInfoClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetLogAppenderInfoClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetLogAppenderInfoClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_GetLogAppenderInfo_allowed(any);}
};


//=========================================
//	Define classes for dynamic commands
//=========================================
//	Command dummyDynCmd class definition
class dummyDynCmdClass : public Tango::Command
{
public:
	dummyDynCmdClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	dummyDynCmdClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~dummyDynCmdClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<LMCDevice *>(dev))->is_dummyDynCmd_allowed(any);}
};


/**
 *	The LMCDeviceClass singleton definition
 */

#ifdef _TG_WINDOWS_
class __declspec(dllexport)  LMCDeviceClass : public Tango::DeviceClass
#else
class LMCDeviceClass : public Tango::DeviceClass
#endif
{
	/*----- PROTECTED REGION ID(LMCDeviceClass::Additionnal DServer data members) ENABLED START -----*/
	
	
	/*----- PROTECTED REGION END -----*/	//	LMCDeviceClass::Additionnal DServer data members

	public:
		//	write class properties data members
		Tango::DbData	cl_prop;
		Tango::DbData	cl_def_prop;
		Tango::DbData	dev_def_prop;
	
		//	Method prototypes
		static LMCDeviceClass *init(const char *);
		static LMCDeviceClass *instance();
		~LMCDeviceClass();
		Tango::DbDatum	get_class_property(string &);
		Tango::DbDatum	get_default_device_property(string &);
		Tango::DbDatum	get_default_class_property(string &);
	
	protected:
		LMCDeviceClass(string &);
		static LMCDeviceClass *_instance;
		void command_factory();
		void attribute_factory(vector<Tango::Attr *> &);
		void pipe_factory();
		void write_class_property();
		void set_default_property();
		void get_class_property();
		string get_cvstag();
		string get_cvsroot();
	
	private:
		void device_factory(const Tango::DevVarStringArray *);
		void create_static_attribute_list(vector<Tango::Attr *> &);
		void erase_dynamic_attributes(const Tango::DevVarStringArray *,vector<Tango::Attr *> &);
		vector<string>	defaultAttList;
		Tango::Attr *get_attr_object_by_name(vector<Tango::Attr *> &att_list, string attname);
};

}	//	End of namespace

#endif   //	LMCDevice_H
