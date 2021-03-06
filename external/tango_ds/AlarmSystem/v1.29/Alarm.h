//=============================================================================
//
// file :        Alarm.h
//
// description : Include for the Alarm class.
//
// project :	alarm
//
// $Author: graziano $
//
// $Revision: 1.16 $
//
// copyleft :    Sincrotrone Trieste S.C.p.A. di interesse nazionale
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
#ifndef _ALARM_H
#define _ALARM_H

#include <tango.h>
#include <mysql.h>

#include <boost/version.hpp>
#define _ACCESS_NODE_D 1
#if BOOST_VERSION  >= 103600
#define BOOST_VER_1_36_0 1
#endif

#include "alarm_table.h"
#include "event_table.h"

#define MAX_ALARMS	1024

#define _USE_ELETTRA_DB_RW

//using namespace Tango;

class alarm_thread;
class log_thread;
class update_thread;

/**
 * @author	$Author: graziano $
 * @version	$Revision: 1.16 $ $
 */

 //	Add your own constants definitions here.
 //-----------------------------------------------


namespace Alarm_ns
{

/**
 * Class Description:
 * 
 */

/*
 *	Device States Description:
 */


class Alarm: public Tango::Device_4Impl
{
public :
	//	Add your own data members here
	//-----------------------------------------

	//	Here is the Start of the automatic code generation part
	//-------------------------------------------------------------	
/**
 *	@name attributes
 *	Attributs member data.
 */
//@{
		Tango::DevString	*attr_alarm_read;
//@}

/**
 *	@name Device properties
 *	Device properties member data.
 */
//@{
/**
 *	Persistent storage of the alarms status
 */
	vector<string>	alarmStatus;
/**
 *	Labels for Group mask, first is for mask 0x00
 */
	vector<string>	groupNames;
/**
 *	Threshold for Tango error for being internal alarms
 */
	Tango::DevLong	errThreshold;
/**
 *	Host of the MySQL db
 */
	string	dbHost;
/**
 *	Username for the MySQL db
 */
	string	dbUser;
/**
 *	Password for the MySQL db
 */
	string	dbPasswd;
/**
 *	Db name for the MySQL db
 */
	string	dbName;
/**
 *	Port of the MySQL db
 */
	string	dbPort;
/**
 *	Name used to associate configured alarm rules to this instance
 */
	string	instanceName;
//@}

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device Name
 */
	Alarm(Tango::DeviceClass *cl,string &s);
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device Name
 */
	Alarm(Tango::DeviceClass *cl,const char *s);
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device name
 *	@param d	Device description.
 */
	Alarm(Tango::DeviceClass *cl,const char *s,const char *d);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */	
	~Alarm() {delete_device();};
/**
 *	will be called at device destruction or at init command.
 */
	void delete_device();
//@}

	
/**@name Miscellaneous methods */
//@{
/**
 *	Initialize the device
 */
	virtual void init_device();
/**
 *	Always executed method befor execution command method.
 */
	virtual void always_executed_hook();

//@}

/**
 * @name Alarm methods prototypes
 */

//@{
/**
 *	Hardware acquisition for attributes.
 */
	virtual void read_attr_hardware(vector<long> &attr_list);
/**
 *	Extract real attribute values for alarm acquisition result.
 */
	virtual void read_alarm(Tango::Attribute &attr);
/**
 *	Read/Write allowed for alarm attribute.
 */
	virtual bool is_alarm_allowed(Tango::AttReqType type);
/**
 *	Execution allowed for Ack command.
 */
	virtual bool is_Ack_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for Load command.
 */
	virtual bool is_Load_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for Remove command.
 */
	virtual bool is_Remove_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for Configured command.
 */
	virtual bool is_Configured_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for StopNew command.
 */
	virtual bool is_StopNew_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for Silence command.
 */
	virtual bool is_Silence_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for Modify command.
 */
	virtual bool is_Modify_allowed(const CORBA::Any &any);
/**
 * Alarm acknowledge
 *	@param	argin	String array containing the alarms to be acknowledged
 *	@exception DevFailed
 */
	void	ack(const Tango::DevVarStringArray *);
/**
 * Load a new alarm.
 *	@param	argin	Alarm entry
 *	@exception DevFailed
 */
	void	load(Tango::DevString);
/**
 * Remove alarm.
 *	@param	argin	Alarm name
 *	@exception DevFailed
 */
	void	remove(Tango::DevString);
/**
 * Alarms configured
 *	@param	argin	String containing a filter for output, if empty return all alarms
 *	@return	Alarms configured
 *	@exception DevFailed
 */
	Tango::DevVarStringArray	*configured(Tango::DevString);
/**
 * Remove "NEW" field from alarm string (so alarm panel stop sound)
 *	@exception DevFailed
 */
	void	stop_new();
/**
 * Alarm temporarily silence
 *	@param	argin	String array containing the alarms to be silenced
 *	@exception DevFailed
 */
	void	silence(const Tango::DevVarStringArray *);
/**
 * Modify an existing alarm.
 *	@param	argin	Alarm entry
 *	@exception DevFailed
 */
	void	modify(Tango::DevString);

/**
 *	Read the device properties from database
 */
	 void get_device_property();
//@}

	//	Here is the end of the automatic code generation part
	//-------------------------------------------------------------	

	friend class alarm_thread;

protected :	
	//	Add your own data members here
	//-----------------------------------------
	
private:
	vector<alarm_t> stored;		/* property stored alarms (on exit) */
	alarm_table alarms;
	event_table* events;
//	event_list evlist;				/* producer/consumer events list */		//gcc 4 problem??
	EventCallBack ecb;				/* callback handles */
	alarm_thread *almloop;
	update_thread *updateloop;
	vector<alarm_t> alarmed;
	ReadersWritersLock *alarmedlock;
	vector<alarm_t> internal;
	ReadersWritersLock *internallock;
	ReadersWritersLock *dslock;
	
	static int instanceCounter;
	
	int internal_counter;
	int ds_num;
	Tango::DevString ds[MAX_ALARMS];
	char dss[MAX_ALARMS][10124];

	void init_events(vector<string> &evn);
	void init_alarms(map< string,vector<string> > &alarm_events);
	void add_alarm(alarm_t& a) throw(string&);
	void add_event(alarm_t& a, vector<string> &evn) throw(string&);
	void subscribe_event(alarm_t& a, EventCallBack& ecb, vector<string> &evn) throw(string&);
//	void do_alarm(bei_t& e);											//gcc 4 problem??
	bool remove_alarm(string& s) throw(string&);
	//void add_to_database(alarm_t& a) throw(string&);
	void set_internal_alarm(string name, Tango::TimeVal t, string msg, unsigned int count=1);
	
	void load_alarm(string alarm_string, alarm_t &alm, vector<string> &evn);

	double eval_formula(tree_parse_info_t tree, string &attr_values);
	void find_event_formula(tree_parse_info_t tree, vector<string> &);
	
	double eval_expression(iter_t const& i, string &attr_values, int ev_ind=0);			//recursive tree node evaluation	
	void eval_node_event(iter_t const& i, vector<string> & ev);		//recursive tree node evaluation		

	void prepare_alarm_attr();	//for read attribute alarm and push_change_event

public:
	void do_alarm(bei_t& e);											//public instead of protected for gcc 4 problem??
	void timer_update();												//public instead of protected for gcc 4 problem??
	event_list evlist;				/* producer/consumer events list */		//public instead of protected for gcc 4 problem??
	bool abortflag;
	
#ifdef _USE_ELETTRA_DB_RW
private:
	string host_rw;
	long port_rw;
#endif
};

}	// namespace

#endif	// _ALARM_H

