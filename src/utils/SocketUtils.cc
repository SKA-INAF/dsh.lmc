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
* @file SocketUtils.cc
* @class SocketUtils
* @brief SocketUtils
*
* SocketUtils
* @author S. Riggi
* @date 15/01/2017
*/

#include <SocketUtils.h>
#include <Logger.h>

#include <tango.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h> 

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/un.h>

#include <iostream>
using namespace std;

namespace Utils_ns {

SocketClient::SocketClient(Tango::DeviceImpl* dev,std::string path,bool isBlocking)
	: m_dev(dev), m_sockpath(path), m_isBlocking(isBlocking)
{
	
	m_sockfd= 0;

}//close constructor


SocketClient::~SocketClient(){

	//Closing socket
	Close();

}//close destructor

int SocketClient::Close(){

	//## Close socket if open
	if (m_sockfd > 0){
    close(m_sockfd);
  } 
	return 0;

}//close Close()

int SocketClient::Reset(){

	Close();
	return Init();

}//close Reset()


int SocketClient::Init(){

	/*
	//## Check is socket is already open
	if(m_sockfd>0){
		cout << "SocketClient::Init(): INFO: Socket already init and connected..." << endl;
 		return 0;
	}
	*/

	__INFO_LOG(m_dev,"Initializing socket ...");

	//## Define socket vars
  struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0; 	

  struct sockaddr_un remote_socket_name;
 	int len;
  long arg; 
  int retval;
  fd_set fdset;

	//## Open socket
	m_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if(m_sockfd<0) {
  	__ERROR_LOG(m_dev,"Failed to open socket "<<m_sockpath<<"!");
    return -1;
  }

  //## Set non-blocking socket 
	memset(&remote_socket_name , 0 , sizeof ( struct sockaddr_un ) ) ;
	remote_socket_name.sun_family = AF_UNIX;
  strncpy(remote_socket_name.sun_path, m_sockpath.c_str(),sizeof(remote_socket_name.sun_path)-1);
  len= sizeof(struct sockaddr_un);

	if(!m_isBlocking){
  	arg = fcntl(m_sockfd, F_GETFL, NULL);
  	arg |= O_NONBLOCK; 
  	fcntl(m_sockfd, F_SETFL, arg); 
  }

	//## Connect socket
	retval = connect( m_sockfd, (struct sockaddr*)&remote_socket_name,len );

  if( errno == EINPROGRESS){
		__WARN_LOG(m_dev,"Connection to socket "<< m_sockpath<<" is in EINPROGRESS status...");
  	
		FD_ZERO(&fdset);      
		FD_SET(m_sockfd, &fdset);

		if( select( m_sockfd+1, 0, &fdset, 0, &timeout) ){

			int result;
			socklen_t result_len = sizeof(result);
			if (getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) {
    		__ERROR_LOG(m_dev,"Socket connection failed!");
				return -1;
			}

			if (result != 0) { // connection failed; error code is in 'result'
    		__ERROR_LOG(m_dev,"Connection to socket "<<m_sockpath<<" failed (err="<<result<<")!");
				return -1;
			}
				
			__INFO_LOG(m_dev,"Connection to socket "<<m_sockpath<<" performed with success.");

		}//close if select
		else{
			__ERROR_LOG(m_dev,"Connection to socket "<<m_sockpath<<" failed!");
			return -1;
		}

  }//close if errno == EINPROGRESS 

  else if (retval < 0) {
		__ERROR_LOG(m_dev,"Connection to socket "<<m_sockpath<<" failed (err=" <<strerror(errno)<<")!");
		return -1;
  }//close else if

  else {
		__INFO_LOG(m_dev,"Connection to socket "<<m_sockpath<<" performed with success.");
  }//close else

 	return 0;

}//close Init()

ssize_t SocketClient::Write(const char* buffer, size_t bufsize) {
	if(bufsize<=0) return 0;
  return write(m_sockfd, buffer, bufsize);
}//close Write()

ssize_t SocketClient::Read(char* buffer, size_t bufsize, int timeout) {
	
	//Indefinite wait read
  if (timeout <= 0) {
    ssize_t n= read(m_sockfd, buffer, bufsize);
    return n;
  }
    
	//Read with timeout
  if (WaitForSocketReadEvent(timeout) == true) {
    return read(m_sockfd, buffer, bufsize);
  }
  return eConnectionTimedOut;

}//close Read()


bool SocketClient::WaitForSocketReadEvent(int timeout) {
    
  fd_set sdset;
  struct timeval tv;

  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  FD_ZERO(&sdset);
  FD_SET(m_sockfd, &sdset);
  if (select(m_sockfd+1, &sdset, NULL, NULL, &tv) > 0) {
    return true;
  }
  return false;
}

}//close namespace

