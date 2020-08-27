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

/*
This is a C++ class to support TCP/IP sockets.
The 2 main classes are QUANTAnet_tcpClient_c and QUANTAnet_tcpServer_c.

If you are writing a server you need to instantiate a tcpServer object.
Likewise if you are writing a client you need to instantiate a tcpClient
object.

*/

#ifndef _QUANTAPLUS_TCP_C
#define _QUANTAPLUS_TCP_C
#include "QUANTAglobals.hxx"
#include "QUANTAnet_socketbase_c.hxx"
#if 0
#include "QUANTAnet_perfMonitor_c.hxx"
#endif
#include "QUANTAnet_iovec_c.hxx"
//#include <QUANTAnet_perfDaemon_c.hxx>

//class QUANTAnet_perfMonitor_c;
class QUANTAnet_tcpServer_c;
//class QUANTAnet_perfDaemon_c;

/** TCP Class for creating client connections. This class bypasses QUANTA's standard method for establishing connections and provides the user with direct control of TCP connections.

*/
class QUANTAnet_tcpClient_c : public QUANTAnet_socketbase_c {

	// Allow QUANTAnet_tcpServer_c objects to call MakeNonBlocking.
	friend class QUANTAnet_tcpServer_c;

protected:
	/* Not to be called by user */
	void makeNonBlocking();

	struct sockaddr_in clientInfo;
	int clientSockFd;
	int clientStatus;
	int timeOutStatus;
	int timeOutPeriod;
#if 0
    QUANTAnet_perfMonitor_c pmonitor;
#endif
public:

	/// Blocking parameter values for Read/Write method.
	//@{
	/// Make the connection blocking but time out eventually.
	static const int BLOCKING/* = 1*/;

	/// Make the connection non blocking.
	static const int NON_BLOCKING/* = 2*/;

	// Make the conncection blocking and never time out.
	//static const int NEVER_TIME_OUT_BLOCKING/* = 3*/;

	//@}

	/// Time out values.
	//@{
	/// Used to set time out period to never time out.
	static const int NO_TIME_OUT/* = -1*/;

	/// Default time out is no time out.
	static const int DEFAULT_TIME_OUT/* = NO_TIME_OUT*/; 
	//@}

	/// Return values from Read/Write methods.
	//@{
	/// Socket is not open.
	static const int SOCKET_NOT_OPEN/* = -1*/;

	/// Connection you attempted to read/write is terminated.
	static const int CONNECTION_TERMINATED/* = -2*/;

	/// You attempted to read with non blocking and there was not data.
	static const int NON_BLOCKING_HAS_NO_DATA/* = -3*/;

	/// The read/write timed out.
	static const int TIMED_OUT/* = -4*/;

	/// Read/Write succeeded.
	static const int OK/* = 1*/;

	/// You attempted to write with non blocking and socket was not ready.
	static const int NON_BLOCKING_NOT_READY_TO_WRITE/* = -6*/;

	/// Socket was not ready.
	static const int NOT_READY/* = -7*/;

	/// Socket ready to read.
	static const int READY_TO_READ/* = -8*/;

	/// Socket ready to write.
	static const int READY_TO_WRITE/* = -9*/;

	/// Socket ready to read and write.
	static const int READY_TO_READ_AND_WRITE/* = -10*/;

    /// Change the socket buffer size before Read().
    static const int READ_BUFFER_SIZE/* = -11*/;

    /// Change the socket buffer size before Write().
    static const int WRITE_BUFFER_SIZE/* = -12*/;
	//@}

	
	/** Set timeout period when performing read() or write() operations.
	    @param t Time out period in seconds. Set to QUANTAnet_tcpClient_c::NO_TIME_OUT to set no time out period.
	*/
	void setTimeOut(int t) {timeOutPeriod = t;}

	/// Get timeout period
	int getTimeOut() {return timeOutPeriod;}

	/** Set client info. For internal use.
	    @param info client information
	*/
	void setClientInfo(struct sockaddr_in* info) { clientInfo = *info; }

	/** Set client socket. For internal use.
	    @param sockfd client socket
	*/
	void setClientSockFd(int sockfd) { clientSockFd = sockfd; }

	QUANTAnet_tcpClient_c();
	virtual ~QUANTAnet_tcpClient_c() { if (clientSockFd) close(); }

	/// Accessor functions
	//@{
	/** Get the IP address of remote connection.
	If you are a client this returns the ip of the destination server.
	If you are a server this returns the ip of the destination client.
	*/
	unsigned int getRemoteIP();

	/** Get the IP address of remote connection.
	If you are a client this returns the ip of the destination server.
	If you are a server this returns the ip of the destination client.
	*/
	void getRemoteIP(char* name);

	/// Get port of self.
	int  getSelfPort();

	/// Get port of client.
	int  getRemotePort();

	/// Get socket id
	int getSocketId();
	//@}

	/**@name Read and Write calls.*/
	//@{
	/**
	  @param nbytes

	  Fill nbytes with num bytes you want to read.
	  nbytes will return with number of bytes successfully read.

	  @param blockingtype

	  BLOCKING means that it will block waiting for data or until
	  the timeout period expires.  Change the timeout period by
	  calling setTimeOut().

	  NON_BLOCKING means that if there is no data to be read this call will
	  return immediately. If it does get some data it will keep reading
	  until the specified number of bytes is received, or if it timesout, or if
	  the connection is broken. In the latter two cases the number of bytes
	  it was able to read is returned.

	  In either the BLOCKING or NON_BLOCKING case you can set the
	  time out period to never time out by setting the timeout
	  time to: NO_TIME_OUT.  

	  @return SOCKET_NOT_OPEN, NON_BLOCKING_HAS_NO_DATA, TIMED_OUT, CONNECTION_TERMINATED, OK, errno
	  */
	int read(char *ptr, int *nbytes, int blockingType);

	/** Allows you to change the parameters to the setsockopt() options.Currently this member allows you to change socket buffer size.

	    @param option 
	    Specify QUANTAnet_tcpClient_c::READ_BUFFER_SIZE if you want to change the size of the receive buffer.
	    Specify QUANTAnet_tcpClient_c::WRITE_BUFFER_SIZE if you want to change the size of the send buffer			

	    @param buffersize
	    Specify the size.
	*/	
	void setSockOptions(int option, int buffersize);	


	/** Determines if a socket has data available to read.
	    @return Either: QUANTAnet_tcpClient_c::NOT_READY
	    or QUANTAnet_tcpClient_c::READY_TO_READ
	*/
	int isReadyToRead();

	/** Determines if a socket is ready to write.
	    @return Either: QUANTAnet_tcpClient_c::NOT_READY
	    or QUANTAnet_tcpClient_c::READY_TO_WRITE
	*/
	int isReadyToWrite();

	/** Determines if a socket is ready to write or read or both.
	    @return Either: QUANTAnet_tcpClient_c::NOT_READY
	    or QUANTAnet_tcpClient_c::READY_TO_WRITE
	    or QUANTAnet_tcpClient_c::READY_TO_READ
	    or QUANTAnet_tcpClient_c::READY_TO_READ_AND_WRITE
	*/
	int isReady();

	/** Write data to socket.
	@return SOCKET_NOT_OPEN, NON_BLOCKING_NOT_READY_TO_WRITE, TIMED_OUT, CONNECTION_TERMINATED, OK
	*/
	int write(const char *ptr, int  *nbytes, int blockingType);

	/// Show status of connection in English.
	void showStatus(int status, int nbytes);

	/// Close the current client connection.
	int close();

	/** Connect to a server.
	    @return A negative number(that is returned by a socket sonnect() call if failed. 
		Else returns client's socket file descriptor. (note: the return value in the previous 
		versions was a zero in case of an error - this has been changed to a negative value from version 1.2)
	*/
	int connectToServer(const char *ip, int port);

#if 0
    //Functions added for performance monitoring interface
    /**
       Displays the resultant statistics instantaneously in the netlogger format - 
	   this should be typically done after a read/write is done over a network.
       
       Also, it should be noted that a showStats call should be made at the end of atleast one send 
	   and receive for two-way information (the same applies for logStats and sendStats)
       
       @param streamInfo
       A label describing the stream that is being monitored.
       
       @param comment
       A comment on the event that marks the time at which the stream is being monitored
    */

    void showStats(char* streamInfo, char* comment);
    
    /**
       This logs performance statistics in a file. The user opens a file and passes the file pointer 
	   with this function and results of monitoring are written into the logfile.
      
       @param streamInfo
        A label describing the stream that is being monitored.

        @param comment
        A comment on the event that marks the time at which the stream is being monitored
        
        @param filePtr
        File pointer to the file in which the results of monitoring are to be stored

        @return
        Either QUANTAnet_perfMonitor_c::OK or QUANTAnet_perfMonitor_c::FAILED
     */


    int logStats(char* streamInfo, char* comment, FILE* filePtr);

    /**
       Sends the performance statistics to a remote perfdaemon -for further analysis of the monitored data - 
	   the initSendStats API should be called first, before calling a sendStats (In order to connect to the perfdaemon initially) 

       @param streamInfo
       A label describing the stream that is being monitored.
       
       @param comment
       A comment on the event that marks the time at which the stream is being monitored
       
       @return
       Either QUANTAnet_perfMonitor_c::OK or QUANTAnet_perfMonitor_c::FAILED
       
     */

    int sendStats(char* streamInfo, char* comment);

    /**
       Initialize sendStats - provide the IP of the perfDaemon and an optional port number to connect to. 
	   This should be done initially  before using the sendStats API.
       
       @param monitorClientIP
       IP address of the perfDameon to connect to
       
       @param port
       Port number at which the perfDaemon is running -this is optional. The default port number for perfDaemon is 9500 
	   -so a different port number has to be specified if the perfDaemon is running ona different port.
       
       @return
       Either QUANTAnet_perfMonitor_c::OK or QUANTAnet_perfMonitor_c::FAILED
    */

    int initSendStats(const char* monitorClientIP, int port = PERF_DAEMON_DEFAULT_PORT);
    
    /**
       Properly delete the perFDaemon Client after sendStats is done
     */

    void exitSendStats();

    //Set the instantaneous latency 
    void setInstantaneousLatency(double latency);
#endif

	/**
	  @param q_iovec QUANTAnet_iovec_c 
	  @param blockingtype QUANTAnet_tcpClient_c::BLOCKING or, NON_BLOCKING
	  @return 1 if succed, or errno if fails 
	  comment:readv gives bad address error when iovec pointer is not initialized.
	*/
	int readv(QUANTAnet_iovec_c q_iovec, int blockingType);


   /** 
	  @param q_iovec QUANTAnet_iovec_c	  
	  @param blockingtype QUANTAnet_tcpClient_c::BLOCKING or, NON_BLOCKING	   
	  @return 1 if succed, or errno if fails 	
	  comment:writev gives invalid argument error when iovec length is not initialized.	
	*/
	int writev(QUANTAnet_iovec_c q_iovec, int blockingType);	
};

/** TCP Class for creating servers. This class bypasses QUANTA's standard method for 
establishing connections and provides the user with direct control of TCP connections.
*/

class QUANTAnet_tcpServer_c : public QUANTAnet_socketbase_c {


protected:
	int serverPort;

	int timeOutPeriod;
	int sockfd;
	struct sockaddr_in  serverInfo;

public:

	//@{
	/// Status ok.
	static const int OK/* = 1*/;

	/// Status failed.
	static const int FAILED/* = 0*/;

	/// Change the socket buffer size before read().
	static const int READ_BUFFER_SIZE/* = 2*/;

	/// Change the socket buffer size before write().
	static const int WRITE_BUFFER_SIZE/* = 3*/;

	//@}

	//	static int QUANTAnet_tcpServer_c::NO_TIME_OUT;

	/** Set timeout period when performing read() or write() operations.
	    @param t Time out period in seconds. Set to QUANTAnet_tcpClient_c::NO_TIME_OUT to set no time out period.
	*/
	void setTimeOut(int t) {timeOutPeriod = t;}

	/// Get timeout period.
	int getTimeOut() {return timeOutPeriod;}

	QUANTAnet_tcpServer_c();
	virtual ~QUANTAnet_tcpServer_c() { if (sockfd) close(); }

	/// Get server's port.
	int getSelfPort() {
		return ntohs(serverInfo.sin_port);
	}

	/** Open the server on a port.
	 Typically after this call you sit in a loop and call
	 checkForNewConnections to wait for incoming connections.
	 @return FAILED if failed, OK if success
	 */
	int init(int port);

	/** Allows you to change the parameters to the setsockopt() options.Currently this member allows you to change socket buffer size.
	 
	 @param option
	 Specify QUANTAnet_tcpServer_c::READ_BUFFER_SIZE if you want to change the size of the receive buffer.
	 Specify QUANTAnet_tcpServer_c::WRITE_BUFFER_SIZE if you want to change the size of the send buffer
	
	 @param buffersize
	 Specify the size.
       */
        void setSockOptions(int option, int buffersize);

	/// Close the server port.
	int close();

	/** Check to see if there is a request from clients for connection.
	    This function checks for new connections. If there is a
	    new connection, then a new QUANTAnet_tcpClient_c object is
	    returned, and that object can be used to talk to the new
	    client.

	    @param blockingTime time to block while checking
	    @return a new QUANTAnet_tcpClient_c object
	    @see QUANTAnet_tcpClient_c class.
    	*/
	QUANTAnet_tcpClient_c* checkForNewConnections(const int& blockingTime=0);

  /** Blocks while waiting for a new connection.
     While waiting for new connections to arrive, the server blocks
     until a new client has connected. This blocking wait results in
     better performance than the checkForNewConnections
     implementation. The server will block until a new client has
     connected, and when a client connects, a pointer to the new
     client object is returned.

     @return a new QUANTAnte_tcpClient_c object
     @see QUANTAnet_tcpClient_c class.
  */
  QUANTAnet_tcpClient_c* waitForNewConnection();

};

#endif
