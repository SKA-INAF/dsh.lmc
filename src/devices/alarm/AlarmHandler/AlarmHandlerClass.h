/*----- PROTECTED REGION ID(AlarmHandlerClass.h) ENABLED START -----*/
//=============================================================================
//
// file :        AlarmHandlerClass.h
//
// description : Include for the AlarmHandler root class.
//               This class is the singleton class for
//                the AlarmHandler device class.
//               It contains all properties and methods which the 
//               AlarmHandler requires only once e.g. the commands.
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


#ifndef AlarmHandlerClass_H
#define AlarmHandlerClass_H

#include <tango.h>
#include <LMCDeviceClass.h>
#include <AlarmHandler.h>


/*----- PROTECTED REGION END -----*/	//	AlarmHandlerClass.h


namespace AlarmHandler_ns
{
/*----- PROTECTED REGION ID(AlarmHandlerClass::classes for dynamic creation) ENABLED START -----*/


/*----- PROTECTED REGION END -----*/	//	AlarmHandlerClass::classes for dynamic creation

//=========================================
//	Define classes for attributes
//=========================================
//	Attribute audibleAlarm class definition
class audibleAlarmAttrib: public Tango::Attr
{
public:
	audibleAlarmAttrib():Attr("audibleAlarm",
			Tango::DEV_BOOLEAN, Tango::READ) {};
	~audibleAlarmAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_audibleAlarm(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_audibleAlarm_allowed(ty);}
};

//	Attribute StatisticsResetTime class definition
class StatisticsResetTimeAttrib: public Tango::Attr
{
public:
	StatisticsResetTimeAttrib():Attr("StatisticsResetTime",
			Tango::DEV_DOUBLE, Tango::READ) {};
	~StatisticsResetTimeAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_StatisticsResetTime(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_StatisticsResetTime_allowed(ty);}
};

//	Attribute alarm class definition
class alarmAttrib: public Tango::SpectrumAttr
{
public:
	alarmAttrib():SpectrumAttr("alarm",
			Tango::DEV_STRING, Tango::READ, 1024) {};
	~alarmAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_alarm(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_alarm_allowed(ty);}
};

//	Attribute normalAlarms class definition
class normalAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	normalAlarmsAttrib():SpectrumAttr("normalAlarms",
			Tango::DEV_STRING, Tango::READ, 10000) {};
	~normalAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_normalAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_normalAlarms_allowed(ty);}
};

//	Attribute unacknowledgedAlarms class definition
class unacknowledgedAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	unacknowledgedAlarmsAttrib():SpectrumAttr("unacknowledgedAlarms",
			Tango::DEV_STRING, Tango::READ, 10000) {};
	~unacknowledgedAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_unacknowledgedAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_unacknowledgedAlarms_allowed(ty);}
};

//	Attribute acknowledgedAlarms class definition
class acknowledgedAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	acknowledgedAlarmsAttrib():SpectrumAttr("acknowledgedAlarms",
			Tango::DEV_STRING, Tango::READ, 10000) {};
	~acknowledgedAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_acknowledgedAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_acknowledgedAlarms_allowed(ty);}
};

//	Attribute unacknowledgedNormalAlarms class definition
class unacknowledgedNormalAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	unacknowledgedNormalAlarmsAttrib():SpectrumAttr("unacknowledgedNormalAlarms",
			Tango::DEV_STRING, Tango::READ, 10000) {};
	~unacknowledgedNormalAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_unacknowledgedNormalAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_unacknowledgedNormalAlarms_allowed(ty);}
};

//	Attribute shelvedAlarms class definition
class shelvedAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	shelvedAlarmsAttrib():SpectrumAttr("shelvedAlarms",
			Tango::DEV_STRING, Tango::READ, 10000) {};
	~shelvedAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_shelvedAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_shelvedAlarms_allowed(ty);}
};

//	Attribute outOfServiceAlarms class definition
class outOfServiceAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	outOfServiceAlarmsAttrib():SpectrumAttr("outOfServiceAlarms",
			Tango::DEV_STRING, Tango::READ, 10000) {};
	~outOfServiceAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_outOfServiceAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_outOfServiceAlarms_allowed(ty);}
};

//	Attribute silencedAlarms class definition
class silencedAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	silencedAlarmsAttrib():SpectrumAttr("silencedAlarms",
			Tango::DEV_STRING, Tango::READ, 10000) {};
	~silencedAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_silencedAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_silencedAlarms_allowed(ty);}
};

//	Attribute listAlarms class definition
class listAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	listAlarmsAttrib():SpectrumAttr("listAlarms",
			Tango::DEV_STRING, Tango::READ, 10000) {};
	~listAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_listAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_listAlarms_allowed(ty);}
};

//	Attribute frequencyAlarms class definition
class frequencyAlarmsAttrib: public Tango::SpectrumAttr
{
public:
	frequencyAlarmsAttrib():SpectrumAttr("frequencyAlarms",
			Tango::DEV_DOUBLE, Tango::READ, 10000) {};
	~frequencyAlarmsAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_frequencyAlarms(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_frequencyAlarms_allowed(ty);}
};


//=========================================
//	Define classes for dynamic attributes
//=========================================
//	Attribute AlarmState class definition
class AlarmStateAttrib: public Tango::Attr
{
public:
	AlarmStateAttrib(const string &att_name):Attr(att_name.c_str(), 
			Tango::DEV_ENUM, Tango::READ) {};
	~AlarmStateAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_AlarmState(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_AlarmState_allowed(ty);}
	virtual bool same_type(const type_info &in_type) {return typeid(AlarmStateEnum) == in_type;}
	virtual string get_enum_type() {return string("AlarmStateEnum");}
};

//	Attribute AlarmFormula class definition
class AlarmFormulaAttrib: public Tango::Attr
{
public:
	AlarmFormulaAttrib(const string &att_name):Attr(att_name.c_str(), 
			Tango::DEV_STRING, Tango::READ) {};
	~AlarmFormulaAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<AlarmHandler *>(dev))->read_AlarmFormula(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<AlarmHandler *>(dev))->is_AlarmFormula_allowed(ty);}
};


//=========================================
//	Define classes for commands
//=========================================
//	Command Ack class definition
class AckClass : public Tango::Command
{
public:
	AckClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	AckClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~AckClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_Ack_allowed(any);}
};

//	Command Load class definition
class LoadClass : public Tango::Command
{
public:
	LoadClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	LoadClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~LoadClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_Load_allowed(any);}
};

//	Command Remove class definition
class RemoveClass : public Tango::Command
{
public:
	RemoveClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	RemoveClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~RemoveClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_Remove_allowed(any);}
};

//	Command SearchAlarm class definition
class SearchAlarmClass : public Tango::Command
{
public:
	SearchAlarmClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	SearchAlarmClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~SearchAlarmClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_SearchAlarm_allowed(any);}
};

//	Command StopAudible class definition
class StopAudibleClass : public Tango::Command
{
public:
	StopAudibleClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	StopAudibleClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~StopAudibleClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_StopAudible_allowed(any);}
};

//	Command Silence class definition
class SilenceClass : public Tango::Command
{
public:
	SilenceClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	SilenceClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~SilenceClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_Silence_allowed(any);}
};

//	Command Modify class definition
class ModifyClass : public Tango::Command
{
public:
	ModifyClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ModifyClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ModifyClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_Modify_allowed(any);}
};

//	Command Shelve class definition
class ShelveClass : public Tango::Command
{
public:
	ShelveClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ShelveClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ShelveClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_Shelve_allowed(any);}
};

//	Command Enable class definition
class EnableClass : public Tango::Command
{
public:
	EnableClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	EnableClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~EnableClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_Enable_allowed(any);}
};

//	Command Disable class definition
class DisableClass : public Tango::Command
{
public:
	DisableClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	DisableClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~DisableClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_Disable_allowed(any);}
};

//	Command ResetStatistics class definition
class ResetStatisticsClass : public Tango::Command
{
public:
	ResetStatisticsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ResetStatisticsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ResetStatisticsClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_ResetStatistics_allowed(any);}
};

//	Command StopNew class definition
class StopNewClass : public Tango::Command
{
public:
	StopNewClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	StopNewClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~StopNewClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_StopNew_allowed(any);}
};

//	Command GetAlarmInfo class definition
class GetAlarmInfoClass : public Tango::Command
{
public:
	GetAlarmInfoClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetAlarmInfoClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetAlarmInfoClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_GetAlarmInfo_allowed(any);}
};

//	Command RemoveAlarmAttrs class definition
class RemoveAlarmAttrsClass : public Tango::Command
{
public:
	RemoveAlarmAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	RemoveAlarmAttrsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~RemoveAlarmAttrsClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<AlarmHandler *>(dev))->is_RemoveAlarmAttrs_allowed(any);}
};


/**
 *	The AlarmHandlerClass singleton definition
 */

#ifdef _TG_WINDOWS_
class __declspec(dllexport)  AlarmHandlerClass : public LMCDevice_ns::LMCDeviceClass
#else
class AlarmHandlerClass : public LMCDevice_ns::LMCDeviceClass
#endif
{
	/*----- PROTECTED REGION ID(AlarmHandlerClass::Additionnal DServer data members) ENABLED START -----*/
	
	
	/*----- PROTECTED REGION END -----*/	//	AlarmHandlerClass::Additionnal DServer data members

	public:
		//	write class properties data members
		Tango::DbData	cl_prop;
		Tango::DbData	cl_def_prop;
		Tango::DbData	dev_def_prop;
	
		//	Method prototypes
		static AlarmHandlerClass *init(const char *);
		static AlarmHandlerClass *instance();
		~AlarmHandlerClass();
		Tango::DbDatum	get_class_property(string &);
		Tango::DbDatum	get_default_device_property(string &);
		Tango::DbDatum	get_default_class_property(string &);
	
	protected:
		AlarmHandlerClass(string &);
		static AlarmHandlerClass *_instance;
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

#endif   //	AlarmHandler_H
