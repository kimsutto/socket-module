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

/* E.g. of client using TCP protocol.
 * This will connect to a server and send text to it.
 * Then on every 10 intervals, it will send the text word "UPDATE"
 * to the server and the server will return by sending text from
 * all its buffers.
 */

#include <QUANTA/QUANTAinit.hxx>
#include <QUANTA/QUANTAnet_tcp_c.hxx>

#include <stdlib.h>
#ifdef WIN32
#include <string.h>
#else
#include <strings.h>
#endif

/* This is the size of all messages sent from clients to servers. */
#define TRANSMITTED_DATA_SIZE 512

/* This is the size of all messages sent from servers to clients. */
#define BROADCAST_DATA_SIZE 1024

/* This is the socket buffer size. */
#define SOCK_BUF_SIZE ((1024*1024*32))

char sendBuffer[TRANSMITTED_DATA_SIZE];
char receiveBuffer[BROADCAST_DATA_SIZE];


int main (int argc, char *argv[])
{
	int tic = 0;
	int status, dataSize;

	QUANTAinit();

	QUANTAnet_tcpClient_c *client = new QUANTAnet_tcpClient_c;

	// Set socket buffer sizes
	client->setSockOptions(QUANTAnet_tcpClient_c::READ_BUFFER_SIZE, SOCK_BUF_SIZE);
	client->setSockOptions(QUANTAnet_tcpClient_c::WRITE_BUFFER_SIZE, SOCK_BUF_SIZE);
	
	if (client->connectToServer(argv[1], 6544) < 0){
		exit(1);
	}

    /*to create a logfile for storing performance monitoring results 
      (logfile = "Machine_IP + Port")
     */
    char* fileName = new char[256];
    char IP[256];
    int port;

    client->getSelfIP(IP);
    port = client->getSelfPort();
 
    sprintf(fileName, "%s_%d.log", IP, port);    
    FILE* filePtr;
    filePtr = fopen(fileName, "a");


	while(1) {

		char name[256];
		client->getSelfIP(name);
		printf(">>> %s,%u\n", name, client->getSelfPort());

		tic++;
		if (tic % 10)
		sprintf(sendBuffer,"TICK(%d)",tic);
		else
		sprintf(sendBuffer,"UPDATE");

		dataSize = TRANSMITTED_DATA_SIZE;
		status = client->write(sendBuffer, &dataSize,
				       QUANTAnet_tcpClient_c::BLOCKING);

if (status != QUANTAnet_tcpClient_c::OK) {
			client->showStatus(status, dataSize);
		}

		dataSize = BROADCAST_DATA_SIZE;

		status =  client->read(receiveBuffer, &dataSize,
				 QUANTAnet_tcpClient_c::BLOCKING);

		if (status == QUANTAnet_tcpClient_c::OK) {
			printf("Data received: %s\n", receiveBuffer);
            printf("\nPerformance Monitoring Statistics:\n");
            client->showStats("tcpClient_perf_monitoring", "Some_local_data");
          
            //uncomment for storing performance stats in a logfile
            //if (fileOpenOK)
            //if (client->logStats("Some_tcpClient_data", "New_local_data", filePtr)!= QUANTAnet_perfMonitor_c::OK)
            //printf("\nlogStats failed!\n");
   
		} else if (status != QUANTAnet_tcpClient_c::NON_BLOCKING_HAS_NO_DATA)
			client->showStatus(status, dataSize);

		QUANTAsleep(1);
	}
    if (filePtr)
        fclose(filePtr);
    return 1;
}
