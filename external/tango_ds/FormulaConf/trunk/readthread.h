//
// file :         readthread.h
//
// description :  reading thread
//
// project :      TANGO Device Server
//
// $Author: graziano $ 
//
// $Revision: 1.3 $
//
// $Log: readthread.h,v $
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
// copyleft :   Sincrotrone Trieste S.C.p.A.
//              Strada Statale 14 - km 163,5 in AREA Science Park
//              34012 Basovizza, Trieste ITALY
//


#ifndef READTHREAD_H
#define READTHREAD_H

#include <omnithread.h>
#include <tango.h>
#include <inttypes.h>

namespace FormulaConf_ns
{

class readthread : public omni_thread, public Tango::LogAdapter
{	
	private:
		Tango::DeviceImpl* device;
		void abort_sleep(double time); 

				
	public:
		
		readthread(Tango::DeviceImpl* devImpl);
		~readthread();

		bool abortflag;
					
	protected:
		void run(void *);
		
};	/* end class readthread() */

}

#endif

/* EOF */
