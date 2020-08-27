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

#include "QUANTAnet_tcp_c.hxx"

#include <string.h>

#ifndef WIN32
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifndef errno
extern int errno;
#endif /* errno */
#endif /* !WIN32 */

#ifdef _WIN32_WCE
#include <errno.h>
#endif

const int QUANTAnet_tcpServer_c::OK = 1;
const int QUANTAnet_tcpServer_c::FAILED = 0;
const int QUANTAnet_tcpServer_c::READ_BUFFER_SIZE = 2;
const int QUANTAnet_tcpServer_c::WRITE_BUFFER_SIZE = 3;

#ifdef sun
long gethostid(void);
#endif

#ifdef WIN32
#define MAXHOSTNAME   128   /* Maximum length of a DNS hostname */
#define DEFAULT_BUFFER_SIZE 32766 /* Default socket bufffer size */
#else
#define DEFAULT_BUFFER_SIZE 1024
#endif /* WIN32 */

#ifdef WIN32
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

QUANTAnet_tcpServer_c::QUANTAnet_tcpServer_c()
{
	/***** Open a TCP socket (an Internet stream socket). */
	/* Notice we want Internet, stream socket and not datagram */
#ifdef WIN32
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET) {
	        printf("QUANTAnet_tcpServer_c::FAILED. Cannot create socket\n");
        }
#else
        if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
                printf("QUANTAnet_tcpServer_c::FAILED. Cannot create socket\n");
        }
#endif

	timeOutPeriod = QUANTAnet_tcpClient_c::DEFAULT_TIME_OUT;
}

int QUANTAnet_tcpServer_c::close()
{
  int closeResult;
#ifdef WIN32
	if (sockfd >= 0) closeResult = ::closesocket(sockfd);
#else
	if (sockfd >= 0) closeResult = ::close(sockfd);
#endif
	sockfd = 0;
  return closeResult;
}

void QUANTAnet_tcpServer_c::setSockOptions(int option, int buffersize)
{
        /*Change the socket buffer size if default is not to be used*/
        int optlen = sizeof(buffersize);

        if (option == QUANTAnet_tcpServer_c::READ_BUFFER_SIZE)
                if (setsockopt(sockfd,SOL_SOCKET, SO_RCVBUF, (char *) &buffersize, optlen) < 0)
                        printf("QUANTAnet_tcpClient_c::setSockOptions: Cannot create a large enough socket buffer for Read.\n");

        if (option == QUANTAnet_tcpServer_c::WRITE_BUFFER_SIZE)
                if (setsockopt(sockfd,SOL_SOCKET, SO_SNDBUF, (char *) &buffersize, optlen) < 0)
                        printf("QUANTAnet_tcpClient_c::setSockOptions: Cannot create a large enough socket buffer for Write.\n");
}

int QUANTAnet_tcpServer_c::init(int port)
{
	int optlen;

	/* Set no delay to stop TCP/IP from buffering */
	int noDelay = 0;// Set to 0 for no buffering. Set to 1 for buffering.
	optlen = sizeof(noDelay);

	if (setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY, (char *) &noDelay, optlen) < 0) 
		printf("QUANTAnet_tcpServer_c::init: Cannot set no delay, you may experience sluggish performance.\n");

	/* Use this to try to eliminate the problem of being unable to rebind to a static port */
	int reuseFlag = 1;
	optlen = sizeof(reuseFlag);
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, (char *) &reuseFlag, optlen) < 0) 
		printf("QUANTAnet_tcpServer_c::init: Cannot reuse port address.\n");
	/***** Bind our local address so that the client can send to us. */

	struct linger lingerData;
	lingerData.l_onoff = 1;
	lingerData.l_linger = 1000;

	optlen = sizeof(lingerData);
	if (setsockopt(sockfd,SOL_SOCKET,SO_LINGER, (char *) &lingerData, optlen) < 0)
		printf("QUANTAnet_tcpServer_c::init: setsockopt: SO_LINGER failed.\n");

	/* Clear out the structure */
	memset((char *) &serverInfo, 0, sizeof(serverInfo));

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	serverInfo.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &serverInfo, sizeof(serverInfo)) < 0) {
		printf("QUANTAnet_tcpServer_c::init: Can't bind local address.\n");
		return QUANTAnet_tcpServer_c::FAILED;
	}

	/* Listen for a connection. Allow queuing for 5 connection requests.
	 */
	listen(sockfd,5);

	/* Non blocking from now on. This is necessary in order for checkForNewConnections() to work.But this seems to be the main thing that is screwing up
	   with threads.
	 */
	//	fcntl(sockfd,F_SETFL,FNDELAY);

	return QUANTAnet_tcpServer_c::OK;
}

const int QUANTAnet_tcpClient_c::BLOCKING = 1;
const int QUANTAnet_tcpClient_c::NON_BLOCKING= 2;
const int QUANTAnet_tcpClient_c::NO_TIME_OUT = -1;
const int QUANTAnet_tcpClient_c::DEFAULT_TIME_OUT = NO_TIME_OUT; 
const int QUANTAnet_tcpClient_c::SOCKET_NOT_OPEN = -1;
const int QUANTAnet_tcpClient_c::CONNECTION_TERMINATED = -2;
const int QUANTAnet_tcpClient_c::NON_BLOCKING_HAS_NO_DATA =-3;
const int QUANTAnet_tcpClient_c::TIMED_OUT = -4;
const int QUANTAnet_tcpClient_c::OK = 1;
const int QUANTAnet_tcpClient_c::NON_BLOCKING_NOT_READY_TO_WRITE = -6;
const int QUANTAnet_tcpClient_c::NOT_READY = -7;
const int QUANTAnet_tcpClient_c::READY_TO_READ = -8;
const int QUANTAnet_tcpClient_c::READY_TO_WRITE = -9;
const int QUANTAnet_tcpClient_c::READY_TO_READ_AND_WRITE = -10;
const int QUANTAnet_tcpClient_c::READ_BUFFER_SIZE = -11;  
const int QUANTAnet_tcpClient_c::WRITE_BUFFER_SIZE = -12;

QUANTAnet_tcpClient_c*
QUANTAnet_tcpServer_c::checkForNewConnections(const int& blockingTime)
{
    struct sockaddr_in clientInfo;
    #ifdef HAVE_SOCKLEN_T
        socklen_t clilen;
    #else
        int clilen;
    #endif
    // Perform a non-blocking check to see if socket is ready for
    // an incoming connection.
    fd_set read_set;
    struct timeval timeout;
    timeout.tv_sec = blockingTime;
    timeout.tv_usec = 0;
    int ready_fd;

    FD_ZERO(&read_set);
    FD_SET(sockfd, &read_set);
    ready_fd = select( sockfd+1, &read_set, NULL, NULL, &timeout);

    // If it is not ready then return.
    if ((ready_fd <= 0)  ||  (!FD_ISSET( sockfd, &read_set))) return NULL;

    clilen = sizeof(clientInfo);
    int newsockfd = accept(sockfd,
                   (struct sockaddr *) &clientInfo,
                   &clilen);
    if (newsockfd > 0) {
        struct linger lingerData;
        lingerData.l_onoff = 1;
        lingerData.l_linger = 1000;

        int optlen = sizeof(lingerData);

        if (setsockopt(newsockfd,SOL_SOCKET,SO_LINGER, (char *) &lingerData, optlen) < 0)
            printf("QUANTAnet_tcpServer_c::checkForNewConnections: setsockopt: SO_LINGER failed.\n");

        QUANTAnet_tcpClient_c *clientObj;

        clientObj = new QUANTAnet_tcpClient_c;
        clientObj->timeOutPeriod = timeOutPeriod;
        clientObj->clientInfo = clientInfo;
        clientObj->clientSockFd = newsockfd;
        //      clientObj->makeNonBlocking();
        return clientObj;
    } else return NULL;
}


QUANTAnet_tcpClient_c*
QUANTAnet_tcpServer_c::waitForNewConnection()
{
  struct sockaddr_in clientInfo;
#ifdef HAVE_SOCKLEN_T
  socklen_t clilen;
#else
  int clilen;
#endif

  // Perform a blocking check to see if socket is ready for an
  // incoming connection.
  fd_set read_set;
  int ready_fd;

  FD_ZERO(&read_set);
  FD_SET(sockfd, &read_set);
  ready_fd = select( sockfd+1, &read_set, NULL, NULL, NULL);

  // If it is not ready then return.
  if ((ready_fd <= 0)  ||  (!FD_ISSET( sockfd, &read_set))) return NULL;

  clilen = sizeof(clientInfo);
  int newsockfd = accept(sockfd,
			 (struct sockaddr *) &clientInfo,
			 &clilen);
  if (newsockfd > 0) {
    struct linger lingerData;
    lingerData.l_onoff = 1;
    lingerData.l_linger = 1000;

    int optlen = sizeof(lingerData);

    if (setsockopt(newsockfd, SOL_SOCKET, SO_LINGER,
		   reinterpret_cast<char*>(&lingerData), optlen) < 0) {
      printf("QUANTAnet_tcpServer_c::waitForNewConnection: setsockopt: SO_LINGER failed.\n");
    }

    QUANTAnet_tcpClient_c *clientObj = new QUANTAnet_tcpClient_c;
    clientObj->timeOutPeriod = timeOutPeriod;
    clientObj->clientInfo = clientInfo;
    clientObj->clientSockFd = newsockfd;
    return clientObj;
  } else {
    return NULL;
  }
}

/*
 * For the blockingtype parameter:
 * NEVER_TIMEOUT_BLOCKING means the timeout period is bypassed so that it will
 * continue to read forever unless there is a break in the connection.
 *
 * NON_BLOCKING means that if there is no data to be read this call will
 * return immediately.
 *
 * BLOCKING means that it will block waiting for data or until the timeout
 * period expires. 
 * Change the timeout period (in seconds) by changing the timeoutPeriod
 * member variable.
 *
 */

void QUANTAnet_tcpClient_c::setSockOptions(int option, int buffersize)
{
	/*Change the socket buffer size if default is not to be used*/	
	int optlen = sizeof(buffersize);

	if (option == QUANTAnet_tcpClient_c::READ_BUFFER_SIZE)
		if (setsockopt(clientSockFd,SOL_SOCKET, SO_RCVBUF, (char *) &buffersize, optlen) < 0)
			printf("QUANTAnet_tcpClient_c::setSockOptions: Cannot create a large enough socket buffer for Read.\n");

	if (option == QUANTAnet_tcpClient_c::WRITE_BUFFER_SIZE)
		if (setsockopt(clientSockFd,SOL_SOCKET, SO_SNDBUF, (char *) &buffersize, optlen) < 0)
			printf("QUANTAnet_tcpClient_c::setSockOptions: Cannot create a large enough socket buffer for Write.\n");

}

int QUANTAnet_tcpClient_c::isReadyToRead()
{
	struct timeval timeoutDelay;
	fd_set readfds;

	timeoutDelay.tv_sec = 0;
	timeoutDelay.tv_usec = 0;

	FD_ZERO(&readfds);
	FD_SET(clientSockFd, &readfds);
	select(clientSockFd+1,&readfds,NULL,NULL, &timeoutDelay);
	
	if (!(FD_ISSET(clientSockFd, &readfds)))
		return NOT_READY;
	else
		return READY_TO_READ;
}

int QUANTAnet_tcpClient_c::isReadyToWrite()
{
	struct timeval timeoutDelay;
	fd_set writefds;

	timeoutDelay.tv_sec = 0;
	timeoutDelay.tv_usec = 0;

	FD_ZERO(&writefds);
	FD_SET(clientSockFd, &writefds);
	select(clientSockFd+1,NULL,&writefds,NULL, &timeoutDelay);
	
	if (!(FD_ISSET(clientSockFd, &writefds)))
		return NOT_READY;
	else
		return READY_TO_WRITE;
}

int QUANTAnet_tcpClient_c::isReady()
{
	struct timeval timeoutDelay;
	fd_set writefds, readfds;

	timeoutDelay.tv_sec = 0;
	timeoutDelay.tv_usec = 0;

	FD_ZERO(&writefds);
	FD_ZERO(&readfds);
	FD_SET(clientSockFd, &writefds);
	FD_SET(clientSockFd, &readfds);
	select(clientSockFd+1,&readfds,&writefds,NULL, &timeoutDelay);
	
	if (FD_ISSET(clientSockFd, &writefds)
	    &&
	    FD_ISSET(clientSockFd, &readfds)) return READY_TO_READ_AND_WRITE;
	if (FD_ISSET(clientSockFd, &writefds)) return READY_TO_WRITE;
	if (FD_ISSET(clientSockFd, &readfds)) return READY_TO_READ;

	return NOT_READY;
}

int QUANTAnet_tcpClient_c::read(char *ptr, int *nbytes, int blockingType)
{

    //used for bandwidth calculations
 //   int totalDataRead =0;    

	struct timeval timeoutDelay;
	fd_set readfds;

	if (clientSockFd < 0) {
		printf("QUANTAnet_tcpClient_c::Read: This connection is not open.\n");
		return SOCKET_NOT_OPEN;
	}

	int nleft, nread;
	nleft = *nbytes;
	timeOutStatus = 0;

	while (nleft > 0) {

        /*The totalDataRead is incremented  each time something is read - in case of an error 
		this is passed on to the performance monitoring instance using the incrementDataRead() function 
		for bandwidth calculations - to calculate the bandwidth for the data read till the break in loop*/

        FD_ZERO(&readfds);
		FD_SET(clientSockFd, &readfds);

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
				error = select(clientSockFd+1,&readfds,NULL,NULL, NULL);
		else
			error = select(clientSockFd+1,&readfds,NULL,NULL, &timeoutDelay);
		if (error == SOCKET_ERROR) {
			errno = WSAGetLastError();
			return errno;
		}
#else
		// This one may not work as it should be..
		// Unix socket's man page doesn't say select will alter the
		// value of errno...
		if ((blockingType == BLOCKING) &&
			(timeOutPeriod == NO_TIME_OUT))
				select(clientSockFd+1,&readfds,NULL,NULL, NULL);
		else
			select(clientSockFd+1,&readfds,NULL,NULL, &timeoutDelay);
		if (errno) {
			return errno;
		}
#endif


		// If fd is not set
		if (!(FD_ISSET(clientSockFd, &readfds))) {
			// If non blocking then it simply means there is no data.
			if (blockingType == NON_BLOCKING) 
				return NON_BLOCKING_HAS_NO_DATA;

			// If blocking then timed out waiting for data.
			if (blockingType == BLOCKING) {
				timeOutStatus = 1;
				return TIMED_OUT;
			}

		}

		// If it passes this stage then things are set as blocking.
		blockingType = BLOCKING;

#ifdef WIN32
		nread = ::recv(clientSockFd, ptr, nleft, 0);

		if (nread == SOCKET_ERROR) {
			*nbytes = nread;
			errno = WSAGetLastError();
			if (errno == WSAECONNRESET) 
				return CONNECTION_TERMINATED;
			return errno;
		}
#else
		errno = 0;
		nread = ::read(clientSockFd,ptr,nleft);

		if (errno) {
			*nbytes = nread;
			return errno;
		}
#endif

		if (nread == 0) {
			*nbytes = (*nbytes - nleft);
			return CONNECTION_TERMINATED;
		}

		if (nread > 0) {
			nleft -= nread;
			ptr += nread;
#if 0
            pmonitor.incrementDataRead(nread);
#endif
		}

	}
	*nbytes = *nbytes - nleft;
    return OK;
}

int QUANTAnet_tcpClient_c::write(const char *ptr, int  *nbytes, int blockingType)
{
    //used for bandwidth calculations
//    int totalDataSent =0;

	// Place here to prevent crashes when write occurs to a socket
	// closed by an external source.
#ifndef WIN32
	signal(SIGPIPE,SIG_IGN);
#endif

	struct timeval timeoutDelay;
	fd_set writefds;

	if (clientSockFd < 0) {
		printf("QUANTAnet_tcpClient_c::Write: This connection is not open.\n");
		return SOCKET_NOT_OPEN;
	}

	int nleft, nwrite;
	nleft = *nbytes;
	timeOutStatus = 0;

	while (nleft > 0) {
		
  /*The totalDataSent is incremented  each time something is written - in case of an error this is passed on to the performance monitoring instance using the incrementDataSent() function for bandwidth calculations - to calculate the bandwidth for the data written till the break in the loop*/

        FD_ZERO(&writefds);
		FD_SET(clientSockFd, &writefds);

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
			error = select(clientSockFd+1,NULL,&writefds,NULL, NULL);
		else
			error = select(clientSockFd+1,NULL,&writefds,NULL, &timeoutDelay);
		if (error == SOCKET_ERROR) {
			errno = WSAGetLastError();
			return errno;
		}
#else
		// This one may not work as it should be..
		// Unix socket's man page doesn't say select will alter the
		// value of errno...
		if ((blockingType == BLOCKING) &&
			(timeOutPeriod == NO_TIME_OUT))
			select(clientSockFd+1,NULL,&writefds,NULL, NULL);
		else
			select(clientSockFd+1,NULL,&writefds,NULL, &timeoutDelay);
		if (errno) {
			return errno;
		}
#endif

		// If fd is not set
		if (!(FD_ISSET(clientSockFd, &writefds))) {

			// If non blocking then it simply means there is no data.
			if (blockingType == NON_BLOCKING) 
				return NON_BLOCKING_NOT_READY_TO_WRITE;

			// If blocking then timed out waiting for data.
			if (blockingType == BLOCKING) {
				timeOutStatus = 1;
				return TIMED_OUT;
			}
		}


		// If it passes this stage then things are set as blocking.
		blockingType = BLOCKING;

#ifdef WIN32
		nwrite = ::send(clientSockFd, ptr, nleft, 0);
        if (nwrite == SOCKET_ERROR) {
		    *nbytes = nwrite;
            errno = WSAGetLastError(); 
		    return errno;
		}
#else
		errno = 0;
		nwrite = ::write(clientSockFd,ptr,nleft);

		if (errno) {
			*nbytes = nwrite;
			return errno;
		}
#endif

		if (nwrite == 0) {
			*nbytes = (*nbytes - nleft);
			return CONNECTION_TERMINATED;
		}

		if (nwrite > 0) {
			nleft -= nwrite;
			ptr += nwrite;
#if 0
            pmonitor.incrementDataSent(nwrite);
#endif
		}

	}
	*nbytes = *nbytes - nleft;
	return OK;

}

int  QUANTAnet_tcpClient_c::connectToServer(const char* ip, int port)
{

	char serverAddr[128];

	int status = hostnameToIP(ip, serverAddr);
	

	if (status == 0) {
		printf("QUANTAnet_tcpClient_c::Connect: Host IP address %s is invalid.\n", ip);
		return -1;
	}

	/* Fill in the structure "clientInfo" with the address of the
	 * server that we want to connect with.
	 */



	memset((char *) &clientInfo,0, sizeof(clientInfo));

	clientInfo.sin_family = AF_INET;
	clientInfo.sin_addr.s_addr = inet_addr(serverAddr);
	clientInfo.sin_port = htons(port);

	int optlen;

	/* Set no delay to stop TCP/IP from buffering */
	int noDelay = 1;
	optlen = sizeof(noDelay);

	if (setsockopt(clientSockFd,IPPROTO_TCP,TCP_NODELAY, (char *) &noDelay, optlen) < 0) 
		printf("QUANTAnet_tcpClient_c::Open: Cannot set no delay, you may experience sluggish performance.\n");

	/* Use this to try to eliminate the problem of being unable to rebind to a static port */
	int reuseFlag = 1;
	optlen = sizeof(reuseFlag);
	if (setsockopt(clientSockFd,SOL_SOCKET,SO_REUSEADDR, (char *) &reuseFlag, optlen) < 0) 
		printf("QUANTAnet_tcpClient_c::Open: Cannot reuse port address.\n");
	/***** Bind our local address so that the client can send to us. */

	struct linger lingerData;
	lingerData.l_onoff = 1;
	lingerData.l_linger = 1000;

	optlen = sizeof(lingerData);
	if (setsockopt(clientSockFd,SOL_SOCKET,SO_LINGER, (char *) &lingerData, optlen) < 0)
		printf("QUANTAnet_tcpClient_c::Open: setsockopt: SO_LINGER failed.\n");

	/* Connect to server. */
    int connectResult;
	if ((connectResult = connect(clientSockFd, (struct sockaddr *) &clientInfo, sizeof(clientInfo))) < 0) {
		printf("QUANTAnet_tcpClient_c::Connect: Can't connect to server.\n");
		return connectResult;
	}
	return clientSockFd;
}

void QUANTAnet_tcpClient_c::makeNonBlocking()
{
#ifdef __sgi
	fcntl(clientSockFd, F_SETFL, FNDELAY);
#endif
#ifdef linux
	fcntl(clientSockFd, F_SETFL, O_NONBLOCK);
#endif
#ifdef sun
	fcntl(clientSockFd, F_SETFL, O_NDELAY);
#endif
#ifdef WIN32
    u_long nonBlockingMode = 1L;

    ioctlsocket(clientSockFd, FIONBIO, &nonBlockingMode);
#endif
}

int QUANTAnet_tcpClient_c::close()
{
  int closeResult;
#ifdef WIN32
	if (clientSockFd > 0) closeResult = ::closesocket(clientSockFd);
#else
	if (clientSockFd > 0) closeResult = ::close(clientSockFd);
#endif
	clientSockFd = 0;
  return closeResult;
}

QUANTAnet_tcpClient_c::QUANTAnet_tcpClient_c() {
	timeOutPeriod = QUANTAnet_tcpClient_c::DEFAULT_TIME_OUT;
	clientSockFd = 0;
	#ifdef WIN32_WCE
		clientSockFd = -1;
	#endif

	/* Open a TCP socket (an Internet stream socket). */
#ifdef WIN32
        if ((clientSockFd = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET) {
                printf("QUANTAnet_tcpClient_c: Can't open stream socket.\n");
        }
#else
        if ((clientSockFd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
                printf("QUANTAnet_tcpClient_c: Can't open stream socket.\n");
        }
#endif

}

void QUANTAnet_tcpClient_c::showStatus(int status, int nbytes)
{
	switch (status) {
	case SOCKET_NOT_OPEN:
		printf("QUANTAnet_tcpClient_c::showStatus: Socket not open.\n");
		break;
	case CONNECTION_TERMINATED:
		printf("QUANTAnet_tcpClient_c::showStatus: Connection terminated with %d bytes transacted.\n", nbytes);
		break;
	case NON_BLOCKING_HAS_NO_DATA:
		printf("QUANTAnet_tcpClient_c::showStatus: Non-blocking read found no data on stream.\n");
		break;
	case NON_BLOCKING_NOT_READY_TO_WRITE:
		printf("QUANTAnet_tcpClient_c::showStatus: Non-blocking connection is not ready to write.\n");
		break;
	case TIMED_OUT:
		printf("QUANTAnet_tcpClient_c::showStatus: Transaction timed out after %d seconds.\n", timeOutPeriod);
		break;
		//	case NON_BLOCKING_WROTE_NO_DATA:
		//		printf("Non-blocking write wrote no data.\n");
		//		break;
	case OK:
		printf("QUANTAnet_tcpClient_c::showStatus: Transaction ok.\n");
		break;
	default:
		printf("QUANTAnet_tcpClient_c::showStatus: UNIX Socket error no: %d\n ",errno);
		//perror("");
		break;
	}
	fflush(stdout);
}


unsigned int QUANTAnet_tcpClient_c :: getRemoteIP() {
	return clientInfo.sin_addr.s_addr;
}

void QUANTAnet_tcpClient_c :: getRemoteIP(char* name)
{
	ipNumToString(getRemoteIP(),name);

}

int QUANTAnet_tcpClient_c :: getSelfPort() {
	struct sockaddr_in name;
    #ifdef HAVE_SOCKLEN_T
        socklen_t size;
    #else
        int size;
    #endif
	size = sizeof(name);
	getsockname (clientSockFd, (sockaddr *)&name, &size);
	return ntohs(name.sin_port);
}

/* Get the port # of the client connection */
int QUANTAnet_tcpClient_c :: getRemotePort() {
	return ntohs(clientInfo.sin_port);
}

#if 0
//Functions added for performance monitoring interface

void QUANTAnet_tcpClient_c ::showStats(char* streamInfo, char* comment)
{
    char *selfIp = new char[256];
    char *remoteIp = new char[256];   
    QUANTAnet_socketbase_c::getSelfIP(selfIp);
    getRemoteIP(remoteIp);
    pmonitor.setIPs(selfIp, remoteIp);
    pmonitor.setPorts(getSelfPort(), getRemotePort());
    pmonitor.showStats(streamInfo, comment);
    delete[] selfIp;
    delete[] remoteIp;
}

int QUANTAnet_tcpClient_c ::logStats(char* streamInfo, char* comment, FILE* filePtr)
{
    char *selfIp = new char[256];
    char *remoteIp = new char[256];   
    QUANTAnet_socketbase_c::getSelfIP(selfIp);
    getRemoteIP(remoteIp);
    pmonitor.setIPs(selfIp, remoteIp);
    pmonitor.setPorts(getSelfPort(), getRemotePort());
    int result = pmonitor.logStats(streamInfo, comment, filePtr);
    delete[] selfIp;
    delete[] remoteIp;
    return result;
}


int QUANTAnet_tcpClient_c::sendStats(char* streamInfo, char* comment)
{
    char *selfIp = new char[256];
    char *remoteIp = new char[256];   
    QUANTAnet_socketbase_c::getSelfIP(selfIp);
    getRemoteIP(remoteIp);
    pmonitor.setIPs(selfIp, remoteIp);
    pmonitor.setPorts(getSelfPort(), getRemotePort());
    int result = pmonitor.sendStats(streamInfo, comment);
    delete[] selfIp;
    delete[] remoteIp;
    return result;
}

int QUANTAnet_tcpClient_c::initSendStats(const char* monitorClientIP, int port)
{
    return pmonitor.initSendStats(monitorClientIP, port);
}

void QUANTAnet_tcpClient_c::exitSendStats()
{
    pmonitor.exitSendStats();
}

void QUANTAnet_tcpClient_c::setInstantaneousLatency(double latency)
{
    pmonitor.setInstantLatency(latency);    
}
#endif

int QUANTAnet_tcpClient_c::getSocketId()
{
    return clientSockFd;
}


//Readv() method to read iovec

int QUANTAnet_tcpClient_c::readv(QUANTAnet_iovec_c recv_iovec, int blockingType)
{
	struct timeval timeoutDelay;
	fd_set readfds;

	if (clientSockFd < 0) {
		printf("QUANTAnet_tcpClient_c::Read: This connection is not open.\n");
		return SOCKET_NOT_OPEN;
	}

	int nread;	
	timeOutStatus = 0;


    FD_ZERO(&readfds);
	FD_SET(clientSockFd, &readfds);

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
			error = select(clientSockFd+1,&readfds,NULL,NULL, NULL);
	else
		error = select(clientSockFd+1,&readfds,NULL,NULL, &timeoutDelay);
	if (error == SOCKET_ERROR) {
		errno = WSAGetLastError();
		return errno;
	}
#else
	// This one may not work as it should be..
	// Unix socket's man page doesn't say select will alter the
	// value of errno...
	if ((blockingType == BLOCKING) &&
		(timeOutPeriod == NO_TIME_OUT))
			select(clientSockFd+1,&readfds,NULL,NULL, NULL);
	else
		select(clientSockFd+1,&readfds,NULL,NULL, &timeoutDelay);
	if (errno) {
		return errno;
	}
#endif


	// If fd is not set
	if (!(FD_ISSET(clientSockFd, &readfds))) {
		// If non blocking then it simply means there is no data.
		if (blockingType == NON_BLOCKING) 
			return NON_BLOCKING_HAS_NO_DATA;

		// If blocking then timed out waiting for data.
		if (blockingType == BLOCKING) {
			timeOutStatus = 1;
			return TIMED_OUT;
		}

	}

	// If it passes this stage then things are set as blocking.
	blockingType = BLOCKING;

#ifdef WIN32
	DWORD noOfBytesRecvd;
	DWORD WSAFlags = 0;
	nread = ::WSARecv(clientSockFd, recv_iovec.getBuffer(),recv_iovec.size(),&noOfBytesRecvd,&WSAFlags,NULL,NULL);

	if (nread == SOCKET_ERROR) {
		//*nbytes = nread;
		errno = WSAGetLastError();
		if (errno == WSAECONNRESET) 
			return CONNECTION_TERMINATED;
		return errno;
	}
	else
	{
		nread = noOfBytesRecvd;
	}
#else
	errno = 0;	

	nread = ::readv(clientSockFd, recv_iovec.getBuffer(), recv_iovec.size());
			
	if(errno != 0)		
	{
		perror("QUANTAnet_tcp_c :: readv - could not read");			
		return errno;
	}

#endif

	if (nread == 0) 
	{
		//*nbytes = (*nbytes - nleft);
		return CONNECTION_TERMINATED;
	}	

#if 0
	if (nread > 0) 
	{
      pmonitor.incrementDataRead(nread);
	}
#endif
	return OK;
}

//writev() method to write iovec

int QUANTAnet_tcpClient_c::writev(QUANTAnet_iovec_c send_iovec, int blockingType)
{

#ifndef WIN32
	signal(SIGPIPE,SIG_IGN);
#endif

	struct timeval timeoutDelay;
	fd_set writefds;

	if (clientSockFd < 0) {
		printf("QUANTAnet_tcpClient_c::Writev: This connection is not open.\n");
		return SOCKET_NOT_OPEN;
	}
	
	int	nwrite;	
	timeOutStatus = 0;	
		
  /*The totalDataSent is incremented  each time something is written - in case of an error this is passed on to the 
	performance monitoring instance using the incrementDataSent() function for bandwidth calculations - 
	to calculate the bandwidth for the data written till the break in the loop*/

    FD_ZERO(&writefds);
	FD_SET(clientSockFd, &writefds);

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
		error = select(clientSockFd+1,NULL,&writefds,NULL, NULL);
	else
		error = select(clientSockFd+1,NULL,&writefds,NULL, &timeoutDelay);
	if (error == SOCKET_ERROR) {
		errno = WSAGetLastError();
		return errno;
	}
#else
	// This one may not work as it should be..
	// Unix socket's man page doesn't say select will alter the
	// value of errno...
	if ((blockingType == BLOCKING) &&
		(timeOutPeriod == NO_TIME_OUT))
		select(clientSockFd+1,NULL,&writefds,NULL, NULL);
	else
		select(clientSockFd+1,NULL,&writefds,NULL, &timeoutDelay);
	if (errno) {
		return errno;
	}
#endif

	// If fd is not set
	if (!(FD_ISSET(clientSockFd, &writefds))) {

		// If non blocking then it simply means there is no data.
		if (blockingType == NON_BLOCKING) 
			return NON_BLOCKING_NOT_READY_TO_WRITE;

		// If blocking then timed out waiting for data.
		if (blockingType == BLOCKING) {
			timeOutStatus = 1;
			return TIMED_OUT;
		}
	}


	// If it passes this stage then things are set as blocking.
	blockingType = BLOCKING;

#ifdef WIN32
	DWORD noOfBytesSent;
	DWORD WSAFlags = 0;

	nwrite = ::WSASend(clientSockFd,send_iovec.getBuffer(),send_iovec.size(),&noOfBytesSent,WSAFlags,NULL,NULL);
    if (nwrite == SOCKET_ERROR) {
		//*nbytes = noOfBytesSent;
        errno = WSAGetLastError(); 
		return errno;
	}
	else {
		nwrite = noOfBytesSent;
	}
#else
	errno = 0;
	nwrite = ::writev(clientSockFd, send_iovec.getBuffer(), send_iovec.size());

	if (errno != 0) {
		//*nbytes = nwrite;
		return errno;
	}
#endif

	if (nwrite == 0) 
	{		
		return CONNECTION_TERMINATED;
	}		

#if 0
	if (nwrite > 0) 
	{
      pmonitor.incrementDataSent(nwrite);
	}
#endif
	return OK;
}

