/******************************************************************************
 * QUANTA - A toolkit for High Performance Data Sharing
 * Copyright (C) 2003 Electronic Visualization Laboratory,  
 * University of Illinois at Chicago
 *
 * This library is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either Version 2.1 of the License, or 
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser Public License along
 * with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Direct questions, comments etc about Quanta to cavern@evl.uic.edu
 *****************************************************************************/
   
#include "QUANTAnet_udp_c.hxx"

#include <string.h>
#ifndef WIN32
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifndef errno
extern int errno;
#endif /* errno */
#endif /* WIN32 */

#ifdef _WIN32_WCE
#include <errno.h>
#endif

const int QUANTAnet_udp_c::OK = 1;
const int QUANTAnet_udp_c::FAILED = 0;
const int QUANTAnet_udp_c::BLOCKING = 1;
const int QUANTAnet_udp_c::NON_BLOCKING = 2;
const int QUANTAnet_udp_c::NULL_VALUE = 3;
const int QUANTAnet_udp_c::NO_TIME_OUT = -1;
const int QUANTAnet_udp_c::DEFAULT_TIME_OUT = NO_TIME_OUT; 
const int QUANTAnet_udp_c::SOCKET_NOT_OPEN = -1;
const int QUANTAnet_udp_c::TIMED_OUT = -4;
const int QUANTAnet_udp_c::NON_BLOCKING_NOT_READY_TO_WRITE = -5;
const int QUANTAnet_udp_c::NON_BLOCKING_HAS_NO_DATA = -6;

#ifdef sun
long gethostid(void);
#endif

#ifdef WIN32
#define MAXHOSTNAME   128   /* Maximum length of a DNS hostname */

// This function is excerpted from "Windows Sockets Network Programming"
// by Bob Quinn and Dave Shute published from Addison-Wesley.
static long gethostid(void)
{
	char szLclHost [MAXHOSTNAME];
	LPHOSTENT lpstHostent;
	SOCKADDR_IN stLclAddr;
	SOCKADDR_IN stRmtAddr;
	int nAddrSize = sizeof(SOCKADDR);
	SOCKET hSock;
	int nRet;

	/* Init local address (to zero) */
	stLclAddr.sin_addr.s_addr = INADDR_ANY;

	/* Get the local hostname */
	nRet = gethostname(szLclHost, MAXHOSTNAME); 
	if (nRet != SOCKET_ERROR) {
		/* Resolve hostname for local address */
		lpstHostent = gethostbyname((LPSTR)szLclHost);
		if (lpstHostent)
			stLclAddr.sin_addr.s_addr = *((u_long FAR*) (lpstHostent->h_addr));
	} 

	/* If still not resolved, then try second strategy */
	if (stLclAddr.sin_addr.s_addr == INADDR_ANY) {
		/* Get a UDP socket */
		hSock = socket(AF_INET, SOCK_DGRAM, 0);
		if (hSock != INVALID_SOCKET)  {
			/* Connect to arbitrary port and address (NOT loopback) */
			stRmtAddr.sin_family = AF_INET;
			stRmtAddr.sin_port   = htons(IPPORT_ECHO);
			stRmtAddr.sin_addr.s_addr = inet_addr("128.127.50.1");
			nRet = connect(hSock, (LPSOCKADDR)&stRmtAddr,
						   sizeof(SOCKADDR));
			if (nRet != SOCKET_ERROR) {
				/* Get local address */
				getsockname(hSock, (LPSOCKADDR)&stLclAddr, 
							(int FAR*)&nAddrSize);
			}
			closesocket(hSock);   /* we're done with the socket */
		}
	}
	return (stLclAddr.sin_addr.s_addr);
}
#endif

QUANTAnet_udp_c :: QUANTAnet_udp_c ()
{
	typeOfBlocking = QUANTAnet_udp_c::BLOCKING;
	timeOutPeriod = QUANTAnet_udp_c::DEFAULT_TIME_OUT;
#if 0
	enableInstr = 0;
#endif
}

void QUANTAnet_udp_c :: makeNonBlocking()
{
#ifdef __sgi
	fcntl(sockfd, F_SETFL, FNDELAY);
#endif
#ifdef linux
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
#endif
#ifdef sun
	fcntl(sockfd, F_SETFL, O_NDELAY);
#endif
#ifdef WIN32
    u_long nonBlockingMode = 1L;

    ioctlsocket(sockfd, FIONBIO, &nonBlockingMode);
#endif

typeOfBlocking = QUANTAnet_udp_c::NON_BLOCKING;
}

int QUANTAnet_udp_c :: init(unsigned short port)
{
	/* Open datagram socket */
#ifdef WIN32
	if ((sockfd = socket(AF_INET, SOCK_DGRAM,0)) == INVALID_SOCKET) {
		printf("QUANTAnet_udp_c::init: cannot open datagram socket.\n");
		return FAILED;
	}
#else
	if ((sockfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
		printf("QUANTAnet_udp_c::init: cannot open datagram socket.\n");
		return FAILED;
	}
#endif

	memset((char *) &selfAddress,0, sizeof(selfAddress));
	selfAddress.sin_family = AF_INET;
	selfAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	selfAddress.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &selfAddress, sizeof(selfAddress)) < 0) {
		printf("QUANTAnet_udp_c::init: Cannot bind local address.\n");
		return FAILED;
	}

	return OK;
}


int QUANTAnet_udp_c::send(char* message, int length, int blockingType)
{
	char* newmsg = message;
	int newlength = length;

	struct timeval timeoutDelay;
	fd_set writefds;

	if (sockfd < 0) {
		printf("QUANTAnet_udp_c::send: This connection is not open.\n");
		return SOCKET_NOT_OPEN;
	}

	//Check the blocking type and set the timeout values

	if (blockingType == NULL_VALUE)
		blockingType = typeOfBlocking;
 
	timeOutStatus = 0;

        FD_ZERO(&writefds);
	FD_SET(sockfd, &writefds);

	if (blockingType == NON_BLOCKING) {
		timeoutDelay.tv_sec = 0;
		timeoutDelay.tv_usec = 0;
	}
	if (blockingType == BLOCKING) {
		timeoutDelay.tv_sec = timeOutPeriod;
		timeoutDelay.tv_usec = 0;
	}

	errno = 0;

	// Wait for some file descriptor to be ready
#ifdef WIN32
	int error;

	if ((blockingType == BLOCKING) &&
		(timeOutPeriod == NO_TIME_OUT))
		error = select(sockfd+1,NULL,&writefds,NULL, NULL);
	else
		error = select(sockfd+1,NULL,&writefds,NULL, &timeoutDelay);
	if (error == SOCKET_ERROR) {
		errno = WSAGetLastError();
		return errno;
	}
#else
	if ((blockingType == BLOCKING) &&
		(timeOutPeriod == NO_TIME_OUT))
			select(sockfd+1,NULL,&writefds,NULL, NULL);
	else
			select(sockfd+1,NULL,&writefds,NULL, &timeoutDelay);
	if (errno) {
		return errno;
	}
#endif

	// If fd is not set
	if (!(FD_ISSET(sockfd, &writefds))) {

		// If blocking then timed out waiting for data.
		if (blockingType == BLOCKING) {
			timeOutStatus = 1;
			return TIMED_OUT;
		}

		// If nonblocking then there is nothing to send.
		if (blockingType == NON_BLOCKING)
			return NON_BLOCKING_NOT_READY_TO_WRITE;
	}

#if 0
	if (enableInstr) {
		newmsg = _quanta_allocMemForPerformanceMonitoredMesg(length, &newlength);
		if (newmsg == NULL) return 0;

		_quanta_prependPerformanceMonitoringHeader(newmsg, message, length);
	}
#endif

	int n = sendto(sockfd, newmsg, newlength, 0, (sockaddr *)&sendAddress,
		       sizeof(sendAddress));
#ifdef WIN32
	if (n == SOCKET_ERROR) errno = WSAGetLastError();
#endif
#if 0
	if (n > 0) 
        pmonitor.incrementDataSent(n);
	if (enableInstr) {
		delete[] newmsg;
		int realsize = n-_quanta_getSizeOfPerformanceMonitoringHeader();
		if (realsize > 0)
			return realsize;
		else
			return 0;
	}
	else
#endif
		return n;
}

int QUANTAnet_udp_c::receive(char* message, int length, int blockingType)
{
	char *newmsg = message;
	int newlength = length;

	struct timeval timeoutDelay;
	fd_set readfds;

	if (sockfd < 0) {
		printf("QUANTAnet_udp_c::receive: This connection is not open.\n");
		return SOCKET_NOT_OPEN;
	}

	//Check the blocking type and set the timeout values
	
	if (blockingType == NULL_VALUE)
		blockingType = typeOfBlocking;
	
    timeOutStatus = 0;

        FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);

	if (blockingType == NON_BLOCKING) {
		timeoutDelay.tv_sec = 0;
		timeoutDelay.tv_usec = 0;
	}
	if (blockingType == BLOCKING) {
		timeoutDelay.tv_sec = timeOutPeriod;
		timeoutDelay.tv_usec = 0;
	}

	errno = 0;

	// Wait for some file descriptor to be ready
	// If no timeout then wait indefinitely until data arrives
#ifdef WIN32
	int error;

	if ((blockingType == BLOCKING) &&
		(timeOutPeriod == NO_TIME_OUT))
			error = select(sockfd+1,&readfds,NULL,NULL, NULL);
		else
			error = select(sockfd+1,&readfds,NULL,NULL, &timeoutDelay);
		if (error == SOCKET_ERROR) {
			errno = WSAGetLastError();
			return errno;
		}
#else
		if ((blockingType == BLOCKING) &&
			(timeOutPeriod == NO_TIME_OUT))
				select(sockfd+1,&readfds,NULL,NULL, NULL);
		else
			select(sockfd+1,&readfds,NULL,NULL, &timeoutDelay);
		if (errno) {
			return errno;
		}
#endif

	// If fd is not set
	if (!(FD_ISSET(sockfd, &readfds))) {

    		// If blocking then timed out waiting for data.
	    	if (blockingType == BLOCKING) {
		       	timeOutStatus = 1;
	    		return TIMED_OUT;
		}
		if (blockingType == NON_BLOCKING) 
			return NON_BLOCKING_HAS_NO_DATA;
	}


#if 0
	// if instrumentation enabled
	if (enableInstr) {
		newmsg = _quanta_allocMemForPerformanceMonitoredMesg(length, &newlength);
		if (newmsg == NULL) return 0;
	}
#endif

    #ifdef HAVE_SOCKLEN_T
        socklen_t clilen;
    #else
        int clilen;
    #endif

	clilen = sizeof(receiveAddress);
	int n;

	n = recvfrom(sockfd, newmsg, newlength, 0, (struct sockaddr *) &receiveAddress,& clilen);
#ifdef WIN32 // latest addition 2/2006 Yongjoo Cho
	if (n > 0 || (n == SOCKET_ERROR && WSAGetLastError() == WSAEMSGSIZE)) {
		if (n == SOCKET_ERROR) n = newlength;
#else
	if (n > 0) {
#endif
#if 0
        pmonitor.incrementDataRead(n);

		// if instrumentation enabled
		if (enableInstr) {
			double remTimeStamp, latency;
			int realSize;
			_quanta_unprependPerformanceMonitoringHeader(&remTimeStamp, &latency, newmsg, n, message, &realSize);

			delete[] newmsg;

			pmonitor.setInstantLatency(latency);
			return realSize;
		}
	} else {
#endif
#ifdef WIN32
		if (n == SOCKET_ERROR) errno = WSAGetLastError();
#endif
#if 0
		if (enableInstr) delete newmsg;
#endif
	}

	return n;
}


int QUANTAnet_udp_c::peek(char* message, int length)
{
    #ifdef HAVE_SOCKLEN_T
        socklen_t clilen;
    #else
        int clilen;
    #endif
	clilen = sizeof(receiveAddress);
	int n;

	char *newmsg = message;
	int newlength = length;

#if 0
	// if instrumentation enabled
	if (enableInstr) {
		newmsg = _quanta_allocMemForPerformanceMonitoredMesg(length, &newlength);
		if (newmsg == NULL) return 0;
	}
#endif

	n = recvfrom(sockfd, newmsg, newlength, MSG_PEEK, (struct sockaddr *) &receiveAddress, & clilen);

	// if instrumentation enabled
#ifdef WIN32 // latest addition 2/2006 Yongjoo Cho
	if (n > 0 || (n == SOCKET_ERROR && WSAGetLastError() == WSAEMSGSIZE)) {
		if (n == SOCKET_ERROR) n = newlength;
#else
	if (n > 0) {
#endif
#if 0
        pmonitor.incrementDataRead(n);
		if (enableInstr) {
			double remoteTimeStamp, latency;
			int realSize;

			_quanta_unprependPerformanceMonitoringHeader(&remoteTimeStamp, &latency, newmsg, n, message, &realSize);
			delete[] newmsg;
            pmonitor.setInstantLatency(latency);
			return realSize;
		}
#endif
	} 
#if 0
	else {
		if (enableInstr) delete newmsg;
	}
#endif
	return n;
}

int QUANTAnet_udp_c :: setSendAddress(const char* host, unsigned short port)
{

	char destHostIP[32];
	int status;

	status = hostnameToIP(host, destHostIP);

	if (status == 0) return 0;

	memset((char *) &sendAddress,0, sizeof(sendAddress));

	sendAddress.sin_family = AF_INET;
	sendAddress.sin_addr.s_addr = inet_addr(destHostIP);
	sendAddress.sin_port = htons(port);

	return 1;
}

void QUANTAnet_udp_c::copyReceiveAddressToSendAddress()
{
	// Must copy entire structure not just ip and port.
	sendAddress = receiveAddress;
}

#if 0
void QUANTAnet_udp_c::enableInstrumentation()
{
	enableInstr = 1;
}

void QUANTAnet_udp_c::disableInstrumentation()
{
	enableInstr = 0;
}
#endif 

void QUANTAnet_udp_c::printInfo() {
	char ip[256];
	QUANTAnet_socketbase_c::getSelfIP(ip);
	printf("QUANTAnet_udp_c: Self IP accepting connections on: %s Port: %u\n", ip, getSelfPort());
	getSendIP(ip);
	printf("QUANTAnet_udp_c: Send IP: %s Port: %u\n", ip, getSendPort());
	getReceiveIP(ip);
	printf("QUANTAnet_udp_c: Receive IP: %s Port: %u\n", ip, getReceivePort());
}

int QUANTAnet_udp_c::close()
{
#ifdef WIN32
  return ::closesocket(sockfd);
#else
  return ::close(sockfd);
#endif
}

QUANTAnet_udp_c * QUANTAnet_udp_c :: clone()
{
  // css: Make sure that the clone operation uses a deep copy! Otherwise,
  // there will be memory corruption because the default C++ copy
  // constructor will only perform a shallow copy. This leads to single
  // pointers shared by multiple objects, not to mention a few headaches.
	QUANTAnet_udp_c *duplicate = new QUANTAnet_udp_c(*this);
	return duplicate;
}

unsigned short  QUANTAnet_udp_c :: getSelfPort() {
	struct sockaddr_in name;
    #ifdef HAVE_SOCKLEN_T
        socklen_t size;
    #else
        int size;
    #endif
	size = sizeof(name);
	getsockname (sockfd, (sockaddr *)&name, &size);
	return ntohs(name.sin_port);
}


unsigned long QUANTAnet_udp_c :: getReceiveIP() {
    return receiveAddress.sin_addr.s_addr;
}

void  QUANTAnet_udp_c :: getReceiveIP(char* ip) {
	ipNumToString(receiveAddress.sin_addr.s_addr, ip);
}

unsigned short  QUANTAnet_udp_c :: getReceivePort() {return ntohs(receiveAddress.sin_port);}

unsigned long  QUANTAnet_udp_c :: getSendIP() {return sendAddress.sin_addr.s_addr;}

void  QUANTAnet_udp_c :: getSendIP(char* ip) {
	ipNumToString(sendAddress.sin_addr.s_addr, ip);
}

unsigned short  QUANTAnet_udp_c :: getSendPort()
{
    return ntohs(sendAddress.sin_port);
}

#if 0
/*For performance monitoring*/

void QUANTAnet_udp_c ::showStats(char* streamInfo, char* comment)        
{
  char *selfIp = new char[256];
  char *remoteIp = new char[256];
  unsigned short remotePort;

  QUANTAnet_socketbase_c::getSelfIP(selfIp);
  if (this->getSendIP())
    getSendIP(remoteIp);
  else
    this->getReceiveIP(remoteIp);
  pmonitor.setIPs(selfIp, remoteIp);
  remotePort = (this->getSendPort()) ? this->getSendPort() : this->getReceivePort();
  pmonitor.setPorts(this->getSelfPort(), remotePort);
  pmonitor.showStats(streamInfo, comment);
  delete[] selfIp;
  delete[] remoteIp;
}

int QUANTAnet_udp_c ::logStats(char* streamInfo, char* comment, FILE* filePtr)
{
    char *selfIp = new char[256];
    char *remoteIp = new char[256];   
	unsigned short remotePort;

    QUANTAnet_socketbase_c::getSelfIP(selfIp);
	if (getSendIP()) getSendIP(remoteIp);
	else getReceiveIP(remoteIp);
    pmonitor.setIPs(selfIp, remoteIp);
	remotePort = (getSendPort()) ? getSendPort() : getReceivePort();
    pmonitor.setPorts(getSelfPort(), remotePort);
    int result = pmonitor.logStats(streamInfo, comment, filePtr);
    delete[] selfIp;
    delete[] remoteIp;
    return result;
}

int QUANTAnet_udp_c::sendStats(char* streamInfo, char* comment)
{
    char *selfIp = new char[256];
    char *remoteIp = new char[256];   
	unsigned short remotePort;

    QUANTAnet_socketbase_c::getSelfIP(selfIp);
	if (getSendIP()) getSendIP(remoteIp);
	else getReceiveIP(remoteIp);
    pmonitor.setIPs(selfIp, remoteIp);
	remotePort = (getSendPort()) ? getSendPort() : getReceivePort();
    pmonitor.setPorts(getSelfPort(), remotePort);
    int result = pmonitor.sendStats(streamInfo, comment);
    delete[] selfIp;
    delete[] remoteIp;
    return result;
}

int QUANTAnet_udp_c::initSendStats(char* monitorClientIP, int port)
{
    return pmonitor.initSendStats(monitorClientIP, port);
}

void QUANTAnet_udp_c::exitSendStats()
{
    pmonitor.exitSendStats();
}
#endif

int QUANTAnet_udp_c::getSocketId()
{
        return sockfd;
}

int QUANTAnet_udp_c::readv(QUANTAnet_iovec_c recv_iovec, int blockingType)
{
	struct timeval timeoutDelay;
	fd_set readfds;

	if (sockfd < 0) {
		printf("QUANTAnet_udp_c::readv: This connection is not open.\n");
		return SOCKET_NOT_OPEN;
	}

	//Check the blocking type and set the timeout values
	if (blockingType == NULL_VALUE)
		blockingType = typeOfBlocking;

	timeOutStatus = 0;

	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);

	if (blockingType == NON_BLOCKING) {
		timeoutDelay.tv_sec = 0;
		timeoutDelay.tv_usec = 0;
	}

	if (blockingType == BLOCKING) {
		timeoutDelay.tv_sec = timeOutPeriod;
		timeoutDelay.tv_usec = 0;
	}

	errno = 0;

	//Wait for some file descriptor to be ready
	//If no timeout then wait indefinitely until the data arrives

	#ifdef WIN32
	int error;

	if ((blockingType == BLOCKING) &&
	   (timeOutPeriod == NO_TIME_OUT))
		error = select(sockfd+1,&readfds,NULL,NULL,NULL);
	else
		error = select(sockfd+1,&readfds,NULL,NULL,&timeoutDelay);

	if (error == SOCKET_ERROR) {
		errno = WSAGetLastError();
		return errno;
	}

	#else
	if ((blockingType == BLOCKING) &&
	    (timeOutPeriod == NO_TIME_OUT))
		select(sockfd+1,&readfds,NULL,NULL,NULL);
	else
		select(sockfd+1,&readfds,NULL,NULL,&timeoutDelay);

	if (errno) {
		return errno;
	}

	#endif

	//If fd is not set
	if (!(FD_ISSET(sockfd, &readfds))) {
		//If blocking then timed out waiting for data.
		if (blockingType == BLOCKING) {
			timeOutStatus = 1;
			return TIMED_OUT;
		}

		if (blockingType == NON_BLOCKING)
			return NON_BLOCKING_HAS_NO_DATA;
	}

	/** The UDP has to be connected to use readv method
	* setSendAddress() is called in demo program to establish the connnection 
	*/

	//readv returns the number of bytes read
	int nread = 0;

#ifdef WIN32

	DWORD noOfBytesRecvd;
	DWORD WSAflags = 0;

	int cliLen = sizeof(receiveAddress);
	nread = ::WSARecvFrom(sockfd,recv_iovec.getBuffer(),recv_iovec.size(), &noOfBytesRecvd,
				&WSAflags,(struct sockaddr *)&receiveAddress, &cliLen, NULL, NULL);

	if (nread == SOCKET_ERROR) {
		errno = WSAGetLastError();
		if (errno == WSAEMSGSIZE) 
		    nread = noOfBytesRecvd;
		else 
    		return errno;
	}
	else
	{
		nread = noOfBytesRecvd;
	}

#else
#ifdef HAVE_SOCKLEN_T
	socklen_t cliLen;
#else
	int cliLen;
#endif

	cliLen = sizeof(receiveAddress);

	struct msghdr packetMsgHdr;
	memset((void*)&packetMsgHdr,0, sizeof(packetMsgHdr));
	packetMsgHdr.msg_name = (caddr_t)&receiveAddress;
	packetMsgHdr.msg_namelen = cliLen;
	packetMsgHdr.msg_iov = (struct iovec *)recv_iovec.getBuffer();
	packetMsgHdr.msg_iovlen = recv_iovec.size();

	errno = 0;

	nread = ::recvmsg(sockfd, &packetMsgHdr, 0);

	if (errno != 0)
	{
		printf("QUANTAnet_udp_c::readv - could not read.\n");
		return errno;
	}
#endif

	if (nread == 0)
	{
		return FAILED;
	}

#if 0
	if (nread > 0)
	{
		pmonitor.incrementDataRead(nread);
	}
#endif

	return OK;
}

int QUANTAnet_udp_c::writev(QUANTAnet_iovec_c send_iovec, int blockingType)
{
        struct timeval timeOutDelay;
        fd_set writefds;

        if (sockfd < 0) {
                printf("QUANTAnet_udp_c::writev. This connection is not open.\n");
                return SOCKET_NOT_OPEN;
        }

        //Check the blocking type and set the timeout values

        if (blockingType == NULL_VALUE)
                blockingType = typeOfBlocking;

        timeOutStatus = 0;

        FD_ZERO(&writefds);
        FD_SET(sockfd, &writefds);

        if (blockingType == NON_BLOCKING) {
                timeOutDelay.tv_sec = 0;
                timeOutDelay.tv_usec = 0;
        }
        if (blockingType == BLOCKING) {
                timeOutDelay.tv_sec = timeOutPeriod;
                timeOutDelay.tv_usec = 0;
        }

        errno = 0;

        //Wait for some file descriptor to be ready

#ifdef WIN32
        int error;

        if ((blockingType == BLOCKING) &&
            (timeOutPeriod == NO_TIME_OUT))
                error = select(sockfd+1,NULL,&writefds, NULL, NULL);
        else
                error = select(sockfd+1,NULL,&writefds,NULL,&timeOutDelay);
	if (error == SOCKET_ERROR) {
		errno = WSAGetLastError();
		return errno;
	}
#else
	if ((blockingType == BLOCKING) &&
	    (timeOutPeriod == NO_TIME_OUT))
		select(sockfd+1,NULL,&writefds,NULL,NULL);
	else
		select(sockfd+1,NULL,&writefds,NULL,&timeOutDelay);

	if (errno) {
		return errno;
	}
#endif

	// If fd is not set
	if (!(FD_ISSET(sockfd, &writefds))) {
		//If blocking then timed out waiting for data.
		if (blockingType == BLOCKING) {
			timeOutStatus = 1;
			return TIMED_OUT;
		}

		// IF nonblocking then there is nothing to send
		if (blockingType == NON_BLOCKING)
			return NON_BLOCKING_NOT_READY_TO_WRITE;
	}

	int nwrite = 0;

#ifdef WIN32

	DWORD noOfBytesSent;
	DWORD WSAflags = 0;
	nwrite = ::WSASendTo(sockfd,send_iovec.getBuffer(),send_iovec.size(),&noOfBytesSent,
				WSAflags,(const struct sockaddr *)&sendAddress,sizeof(sendAddress),NULL,NULL);

	if (nwrite == SOCKET_ERROR) {
		errno = WSAGetLastError();
		return errno;
	}
	else
	{
		nwrite = noOfBytesSent;
	}
#else
#ifdef HAVE_SOCKLEN_T
	socklen_t cliLen;
#else
	int cliLen;
#endif

	cliLen = sizeof(sendAddress);

	struct msghdr packetMsgHdr;
	memset((void*)&packetMsgHdr,0, sizeof(packetMsgHdr));
	packetMsgHdr.msg_name = (caddr_t)&sendAddress;
	packetMsgHdr.msg_namelen = cliLen;
	packetMsgHdr.msg_iov = (struct iovec *)send_iovec.getBuffer();
	packetMsgHdr.msg_iovlen = send_iovec.size();

	errno = 0;

	nwrite = ::sendmsg(sockfd, &packetMsgHdr, 0);

	if (errno != 0)
	{
		printf("QUANTAnet_udp_c::writev - could not write\n");
		return errno;
	}		
#endif

	if (nwrite == 0)
	{
		return FAILED;
	}
#if 0
	if (nwrite > 0)
	{
		pmonitor.incrementDataSent(nwrite);
	}
#endif

	return OK;
}
