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
// * DSH.LMC is distributed in the hope that it will be useful, but 			    *
// * WITHOUT ANY WARRANTY; without even the implied warranty of             * 
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
// * See the GNU General Public License for more details. You should        * 
// * have received a copy of the GNU General Public License along with      * 
// * DSH.LMC. If not, see http://www.gnu.org/licenses/.                      *
// **************************************************************************

/**
* @file EventCallBack.h
* @class EventCallBack
* @brief EventCallBack
*
* EventCallBack
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef _EVENT_CALLBACK_H
#define _EVENT_CALLBACK_H

#ifndef _TANGO_H
	#include <tango.h>
#endif


namespace LMCDevice_ns {
  
class LMCDevice;

class EventCallBack : public Tango::CallBack { 
		
	private:
		static LMCDevice* m_dev;	
		
	public:
		EventCallBack(LMCDevice* dev);
		~EventCallBack();
		   
	public:
		void push_event(Tango::EventData*);
			
	protected:
		virtual int process_event(Tango::EventData*);

};

}//close namespace

#endif
