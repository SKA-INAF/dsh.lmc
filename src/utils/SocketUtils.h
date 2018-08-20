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
* @file SocketUtils.h
* @class SocketUtils
* @brief SocketUtils
*
* SocketUtils
* @author S. Riggi
* @date 15/01/2017
*/

#ifndef _SOCKET_UTILS_h__
#define _SOCKET_UTILS_h__

#include <tango.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

namespace Utils_ns {

struct SBuffer {
	std::string data;
	size_t size;

	SBuffer(std::string buf,size_t bufsize)
		: data(buf), size(bufsize)
	{}
};

class SocketClient {

  public:
    SocketClient(Tango::DeviceImpl* dev,std::string path,bool isBlocking=false);
    ~SocketClient();

		enum SocketStatus {
    	eConnectionClosed = 0,
      eConnectionReset = -1,
      eConnectionTimedOut = -2
    };

	public:
    int Init();
		int Close();
		int Reset();
    ssize_t Write(const char* buffer, size_t bufsize);
		ssize_t Read(char* buffer, size_t bufsize, int timeout);

	private:
		bool WaitForSocketReadEvent(int timeout);

	private:
		Tango::DeviceImpl* m_dev;
		int m_sockfd;
		std::string m_sockpath;
		bool m_isBlocking;
  
};//close SocketClient()

}//close namespace

#endif

