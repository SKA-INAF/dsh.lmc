//=============================================================================
//
// file :         AlarmClass.h
//
// description :  Include for the AlarmClass root class.
//                This class is represents the singleton class for
//                the Alarm device class.
//                It contains all properties and methods which the 
//                Alarm requires only once e.g. the commands.
//			
// project :      TANGO Device Server
//
// $Author: graziano $
//
// $Revision: 1.5 $
//
// copyleft :     Sincrotrone Trieste S.C.p.A. di interesse nazionale
//                Strada Statale 14 - km 163,5 in AREA Science Park
//                34012 Basovizza, Trieste ITALY
//                Tel. +39 040 37581
//                Fax. +39 040 9380902
//
//=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - ELETTRA U.O. Controlli
//=============================================================================

#ifndef _ALARMCLASS_H
#define _ALARMCLASS_H

#include <tango.h>
#include <Alarm.h>


namespace Alarm_ns
{
//=====================================
//	Define classes for attributes
//=====================================
class alarmAttrib: public Tango::SpectrumAttr
{
public:
	alarmAttrib():SpectrumAttr("alarm", Tango::DEV_STRING, Tango::READ, 1024) {};
	~alarmAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<Alarm *>(dev))->read_alarm(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<Alarm *>(dev))->is_alarm_allowed(ty);}
};

//=========================================
//	Define classes for commands
//=========================================
class ModifyCmd : public Tango::Command
{
public:
	ModifyCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ModifyCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ModifyCmd() {};

	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Alarm *>(dev))->is_Modify_allowed(any);}
};

class SilenceCmd : public Tango::Command
{
public:
	SilenceCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	SilenceCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~SilenceCmd() {};

	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Alarm *>(dev))->is_Silence_allowed(any);}
};


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
	{return (static_cast<Alarm *>(dev))->is_StopNew_allowed(any);}
};



class ConfiguredCmd : public Tango::Command
{
public:
	ConfiguredCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ConfiguredCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ConfiguredCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Alarm *>(dev))->is_Configured_allowed(any);}
};



class RemoveCmd : public Tango::Command
{
public:
	RemoveCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	RemoveCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~RemoveCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Alarm *>(dev))->is_Remove_allowed(any);}
};



class LoadCmd : public Tango::Command
{
public:
	LoadCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	LoadCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~LoadCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Alarm *>(dev))->is_Load_allowed(any);}
};



class AckCmd : public Tango::Command
{
public:
	AckCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	AckCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~AckCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Alarm *>(dev))->is_Ack_allowed(any);}
};



//
// The AlarmClass singleton definition
//

class
#ifdef WIN32
	__declspec(dllexport)
#endif
	AlarmClass : public Tango::DeviceClass
{
public:
//	properties member data

//	add your own data members here
//------------------------------------

public:
	Tango::DbData	cl_prop;
	Tango::DbData	cl_def_prop;
	Tango::DbData	dev_def_prop;

//	Method prototypes
	static AlarmClass *init(const char *);
	static AlarmClass *instance();
	~AlarmClass();
	Tango::DbDatum	get_class_property(string &);
	Tango::DbDatum	get_default_device_property(string &);
	Tango::DbDatum	get_default_class_property(string &);
	
protected:
	AlarmClass(string &);
	static AlarmClass *_instance;
	void command_factory();
	void get_class_property();
	void attribute_factory(vector<Tango::Attr *> &);
	void write_class_property();
	void set_default_property();

private:
	void device_factory(const Tango::DevVarStringArray *);
};


}	//	namespace Alarm_ns

#endif // _ALARMCLASS_H
