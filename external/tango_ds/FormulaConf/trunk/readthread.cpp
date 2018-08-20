//
// file :         readthread.cpp
//
// description :  reading thread
//
// project :      TANGO Device Server
//
// $Author: graziano $ 
//
// $Revision: 1.13 $
//
// $Log: readthread.cpp,v $
// Revision 1.13  2016-06-09 11:59:01  graziano
// added quality
//
// Revision 1.12  2014-04-11 07:57:59  graziano
// added sin, cos
//
// Revision 1.11  2013-08-06 08:52:04  graziano
// added type long
//
// Revision 1.10  2013-05-31 09:22:05  graziano
// fixed
//
// Revision 1.9  2013-05-29 15:52:00  graziano
// fixed
//
// Revision 1.8  2013-05-28 16:02:50  graziano
// bugs fixed
//
// Revision 1.7  2013-05-20 14:27:52  graziano
// added push events
//
// Revision 1.6  2013-05-09 15:08:47  graziano
// fixed log
//
// Revision 1.5  2013-05-09 14:23:58  graziano
// fixed timeout
//
// Revision 1.4  2013-05-09 12:38:53  graziano
// fixed
//
// Revision 1.3  2013-05-09 08:13:15  graziano
// added events
//
// Revision 1.2  2013-05-08 09:16:37  graziano
// added possibility to have strings in formula
//
// Revision 1.1  2012-12-05 13:28:24  graziano
// first commit
//
//
//
//
//
//
//
// copyleft :   Sincrotrone Trieste S.C.p.A.
//              Strada Statale 14 - km 163,5 in AREA Science Park
//              34012 Basovizza, Trieste ITALY
//

#include "readthread.h"
#include "FormulaConf.h"
#include <math.h>

static const char __FILE__rev[] = __FILE__ " $Revision: 1.13 $";

namespace FormulaConf_ns
{

//+------------------------------------------------------------------
//
//	method:			readthread::readthread()
//
//	description:	readthread constructor
//
//-------------------------------------------------------------------
readthread::readthread(Tango::DeviceImpl* devImpl):Tango::LogAdapter(devImpl)
{ 
	DEBUG_STREAM << "readthread::readthread(): constructor... :" << __FILE__rev << endl;
	device = devImpl;
}

//+------------------------------------------------------------------
//
//	method:			readthread::~readthread()
//
//	description:	readthread destructor
//
//-------------------------------------------------------------------
readthread::~readthread()
{
	DEBUG_STREAM << "readthread::~readthread(): destructor... !" << endl;
}

//+------------------------------------------------------------------
//
//	method:			readthread::run()
//
//	description:	Run
//
//-------------------------------------------------------------------
void readthread::run(void *)
{
	INFO_STREAM << "readthread::run(): running... !" << endl;
	/*int pausesec,pausenano,pausesec_dfl,pausenano_dfl,
	    cnt_err = 0,
		max_cnt_err = 3,
		ret;*/

	bool first_poll = true;
	abortflag = false;

	while(!(static_cast<FormulaConf *>(device))->created_attr && !abortflag)
	{
		sleep(1);
		DEBUG_STREAM << "readthread::run(): waiting..." << endl;
	}

	while (!abortflag)
	{
		if((static_cast<FormulaConf *>(device))->polling_period == -1)
		{
			abort_sleep(10);
			continue;
		}
		DEBUG_STREAM << "readthread::run(): polling: period="<< (static_cast<FormulaConf *>(device))->polling_period << endl;
		vector<remote_dev_t>::iterator it;
		for(it=(static_cast<FormulaConf *>(device))->remote_devices.begin(); it!=(static_cast<FormulaConf *>(device))->remote_devices.end(); it++)
		{
			if (abortflag)
				break;
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC,&now);
			double dnow = (double)(now.tv_sec) + ((double)now.tv_nsec)/1e9;
			double dlast_e = (double)(it->last_event_rx.tv_sec) + ((double)it->last_event_rx.tv_nsec)/1e9;
			double dlast_p = (double)(it->last_read.tv_sec) + ((double)it->last_read.tv_nsec)/1e9;

			//DEBUG_STREAM << "readthread::run(): polling: '" << it->remote_name<<"' last_e="<<fixed<<dlast_e<<" last_p="<<dlast_p<<" now="<<dnow;

			//skip if no event error and passed less than read_timeout seconds from last reading
			if(!((dnow - dlast_e > (static_cast<FormulaConf *>(device))->read_timeout) && (dnow - dlast_p > (static_cast<FormulaConf *>(device))->read_timeout)) && !it->event_error)
				continue;

			//skip if event error but passed less than read_timeout seconds from last event
			if((dnow - dlast_e < (static_cast<FormulaConf *>(device))->read_timeout) && it->event_error)
				continue;

			//DEBUG_STREAM << "readthread::run(): polling: '" << it->remote_name<<"'";

			int k = it->stat_index;
			try
			{
				Tango::DeviceAttribute Dout;
				Dout = it->remote_device->read_attribute(it->remote_attr_name);
				it->quality = Dout.quality;
				(static_cast<FormulaConf *>(device))->extract_values(&Dout, it->value, it->value_string);
				clock_gettime(CLOCK_MONOTONIC,&it->last_read);
				double dpoll = (double)(it->last_read.tv_sec) + ((double)it->last_read.tv_nsec)/1e9;

				if(k<3*MAX_ATTR_SIZE)
					sprintf((static_cast<FormulaConf *>(device))->devices_status_str[k], "%s: %13.3f - EVENT %s | POLLING '%s' OK", it->remote_device_name.c_str(), dpoll, it->event_error ? "ERR" : "OK", it->remote_attr_name.c_str());

				//iterate through local attributes, evaluates formulas involved and push events
				for(vector<string>::iterator it_loc_attr=it->loc_attr_name.begin(); it_loc_attr!=it->loc_attr_name.end(); it_loc_attr++)
				//for(map<string,vector<attr_desc_t> >::iterator it_attr = att_data.begin(); it_attr != att_data.end(); it_attr++)
				{
					map<string,vector<attr_desc_t> >::iterator it_attr = (static_cast<FormulaConf *>(device))->att_data.find(*it_loc_attr);
					map<string,attr_val_t >::iterator it_val = (static_cast<FormulaConf *>(device))->m_att_value.find(*it_loc_attr);
					if(it_attr == (static_cast<FormulaConf *>(device))->att_data.end() || it_val == (static_cast<FormulaConf *>(device))->m_att_value.end())
						continue;

					bool formula_err = false;
					for(vector<attr_desc_t>::iterator it_rem_attr = it_attr->second.begin(); it_rem_attr != it_attr->second.end(); it_rem_attr++)
					{
						//cout << __FUNCTION__ << " looping " << it_ev->formula<<endl;

						string attr_values;
						double res = 0.0;	//initialize to false
						try
						{
							res = (static_cast<FormulaConf *>(device))->eval_formula(it_rem_attr->formula_tree, *it_loc_attr, attr_values);
						}
						catch(string &err)
						{
							INFO_STREAM << __func__<<": error evaluating formula="<<it_rem_attr->formula<<" err="<<err<<endl;
							formula_err = true;
							res = NAN;
							break;
						}
						catch(std::out_of_range &err)
						{
							INFO_STREAM << __func__<<": out_of_range exception evaluating formula="<<it_rem_attr->formula<<" err="<<err.what()<<endl;
							formula_err = true;
							res = NAN;
							break;
						}
						catch(...)
						{
							INFO_STREAM << __func__<<": generic exception evaluating formula="<<it_rem_attr->formula<<endl;
							formula_err = true;
							res = NAN;
							break;
						}
						if(it_rem_attr->type == "bool" && it_rem_attr->attr_index<MAX_ATTR_SIZE)
						{
							if(formula_err)
								break;
							it_val->second.val[ it_rem_attr->attr_index ] = (res != 0.0);
						}
						else if(it_rem_attr->type == "double" && it_rem_attr->attr_index<MAX_ATTR_SIZE)
						{
							formula_err = false;
							it_val->second.val_d[ it_rem_attr->attr_index ] = res;
						}
						else if(it_rem_attr->type == "long" && it_rem_attr->attr_index<MAX_ATTR_SIZE)
						{
							if(formula_err)
								break;
							it_val->second.val_l[ it_rem_attr->attr_index ] = res;
						}
						else
						{
							formula_err = true;
							break;
						}
					}
					//something went wrong-> skip this local attribute
					if(formula_err)
						continue;

				    //	Set the attribute value
				    //attr.set_value(&(it_val->second.val[0]), it_attr->second.size());
					bool changed = false;
					for(uint32_t i=0; i<it_attr->second.size() && i<MAX_ATTR_SIZE; i++)
					{
						if(it_attr->second[0].type == "bool")
						{
							if(it_val->second.val[ i ] != it_val->second.old_val[ i ])
								changed = true;
						}
						else if(it_attr->second[0].type == "double")
						{
							if(it_val->second.val_d[ i ] != it_val->second.old_val_d[ i ])
								changed = true;
						}
						else if(it_attr->second[0].type == "long")
						{
							if(it_val->second.val_l[ i ] != it_val->second.old_val_l[ i ])
								changed = true;
						}
					}

					if(changed || it_attr->second[0].first_push)
					{
						//DEBUG_STREAM<<__func__<<": evaluating polled loc attr: "<<*it_loc_attr<<" CHANGED-> pushing changed="<<(int)changed<<" first_push="<<(int)it_attr->second[0].first_push<<endl;
						it_attr->second[0].first_push = false;
						bool push_err=false;
						try{
							if(it_attr->second[0].type == "bool")
								(static_cast<FormulaConf *>(device))->push_change_event(*it_loc_attr,&(it_val->second.val[0]),/*NULL,Tango::ATTR_VALID,*/ it_attr->second.size(), 0, false);
							else if(it_attr->second[0].type == "double")
								(static_cast<FormulaConf *>(device))->push_change_event(*it_loc_attr,&(it_val->second.val_d[0]),/*NULL,Tango::ATTR_VALID,*/ it_attr->second.size(), 0, false);
							else if(it_attr->second[0].type == "long")
								(static_cast<FormulaConf *>(device))->push_change_event(*it_loc_attr,&(it_val->second.val_l[0]),/*NULL,Tango::ATTR_VALID,*/ it_attr->second.size(), 0, false);
						} catch(Tango::DevFailed &e)
						{
							INFO_STREAM <<__func__<< ": error pushing ValveStat="<<e.errors[0].desc<<endl;
							push_err=true;
						}
						if(!push_err)
						{
							for(uint32_t i=0; i<it_attr->second.size(); i++)
							{
								if(it_attr->second[0].type == "bool")
									it_val->second.old_val[ i ] = it_val->second.val[ i ];
								else if(it_attr->second[0].type == "double")
									it_val->second.old_val_d[ i ] = it_val->second.val_d[ i ];
								else if(it_attr->second[0].type == "long")
									it_val->second.old_val_l[ i ] = it_val->second.val_l[ i ];
							}
						}
					}
				}



			}
			catch(Tango::DevFailed &e)
			{
				string err(e.errors[0].desc);
				if(k<3*MAX_ATTR_SIZE)
					sprintf((static_cast<FormulaConf *>(device))->devices_status_str[k], "%s: %13.3f - EVENT %s | POLLING '%s' ERROR: '%s'", it->remote_device_name.c_str(), dnow, it->event_error ? "ERR" : "OK",  it->remote_attr_name.c_str(), err.c_str());

				DEBUG_STREAM << __func__ << ": error reading " << it->remote_name << " err='"<< err <<"'"<< endl;
				abort_sleep(0.1);
			}
			catch(string& err)
			{
				if(k<3*MAX_ATTR_SIZE)
					sprintf((static_cast<FormulaConf *>(device))->devices_status_str[k], "%s: %13.3f - EVENT %s | POLLING '%s' ERROR: '%s'", it->remote_device_name.c_str(), dnow,  it->event_error ? "ERR" : "OK", it->remote_attr_name.c_str(), err.c_str());
				DEBUG_STREAM << __func__ << ": error extracting value from " << it->remote_name << " err='"<< err<<"'"<< endl;
				abort_sleep(0.1);
			}
			usleep(1000);
		}
		if((static_cast<FormulaConf *>(device))->polling_period > 0)
			abort_sleep((static_cast<FormulaConf *>(device))->polling_period);
		first_poll = false;
	}
	INFO_STREAM << "readthread::run(): exit!!!" << endl;
}

//+------------------------------------------------------------------
//
//	method:			readthread::abort_sleep
//
//	description:	Resume from sleep if abort_flag set (sec.)
//
//-------------------------------------------------------------------
void readthread::abort_sleep(double time)
{

	for (int i = 0; i < (time/0.3); i++) {
		if (abortflag)
			break;
		omni_thread::sleep(0,300000000);
	}

}


}


