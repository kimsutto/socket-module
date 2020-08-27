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

//#include <QUANTA/QUANTAinit.hxx>
#include "QUANTAnet_socketbase_c.hxx"

#ifdef QUANTA_USE_PTHREADS
#include <pthread.h>
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <netdb.h>
#include <sys/time.h>
#include <sys/socket.h>
#endif /* !WIN32 */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#include "gettimeofday.hxx"
#endif

const int QUANTAnet_socketbase_c::FAILED = 0;
const int QUANTAnet_socketbase_c::OK = 1;
const int QUANTAnet_socketbase_c::TIMED_OUT = 0;

QUANTAnet_socketbase_c :: QUANTAnet_socketbase_c() {
    initialReadTime = initialSentTime = getTimeInSecs();
	previousSentTime = initialSentTime;
	previousReadTime = initialReadTime;

	totalDataRead = totalDataSent = prevTotalDataRead = prevTotalDataSent = 0;
	latCount = readCount = sentCount =0;
	instLat = latTotal = maxLat = minLat = avgLat = 0;
	instDelay = maxDelay = minDelay = avgDelay = totalDelay = 0;
   instReadBandwidth= instSendBandwidth= avgReadBandwidth= avgSendBandwidth =0;
    
    jitter = burstiness = STALatency =STABandwidth =0;
    for (int i=0; i<LATENCY_BUF_SIZE; i++, latencyBuffer[i] = bandwidthBuffer[i] =0);
    STlatencySum = STbandwidthSum =0;    
    tempLatCounter = tempBWCounter = 0;  
}

double QUANTAnet_socketbase_c :: getTimeInSecs()
{
/*#ifdef WIN32 //using either high performance counter or _ftime in Windows 
	if (UseFTime) { // Use _ftime. Precision is limited to milliseconds level.
	    const double oneMilliSec = 0.001;	
		struct _timeb timeSec;

		_ftime(&timeSec);

		return (((double) timeSec.time) + (((double) timeSec.millitm) * oneMilliSec));
	}
	else { // Use high performance counter. Precision is in microseconds level.
		LARGE_INTEGER endCounter;	
		double tempL;

		QueryPerformanceCounter(&endCounter);
		return StartTime + (double) ((endCounter.QuadPart - StartCounter.QuadPart) / (double) Frequency.QuadPart);
	}
#else //using gettimeofday() otherwise - gives time with microsecond level precision*/
    const double oneMicroSec = 0.000001;
	struct timeval timeSec;

	quanta_gettimeofday(&timeSec, NULL);

	return (((double) timeSec.tv_sec) +
		(((double) timeSec.tv_usec) * oneMicroSec));
//#endif
}

void QUANTAnet_socketbase_c::ipNumToString(unsigned int ipnum, char *ipstring)
{
#if 0
	/* Old implementation disabled. */
	char *name = (char*) &ipnum;

	int a,b,c,d;
	a = (unsigned char) name[0];
	b = (unsigned char) name[1];
	c = (unsigned char) name[2];
	d = (unsigned char)  name[3];
	sprintf(ipstring,"%d.%d.%d.%d",a,b,c,d);
#endif
	if (ipnum && ipstring) {
	  struct in_addr addr;
	  memcpy(&addr, &ipnum, sizeof(ipnum));

	  char *ipAddr = inet_ntoa(addr);
	  if (ipAddr)
	    strcpy(ipstring, ipAddr);
	  else
	    *ipstring = '\0';
	} else if (ipstring)
	  *ipstring = '\0';
}

void QUANTAnet_socketbase_c :: expandHostName(const char *src, char* newname, int len)
{
	struct hostent *	hp_ptr;

        hp_ptr = gethostbyname(src);
        if (hp_ptr != NULL) {
		struct in_addr      addr;

		memcpy(&(addr.s_addr), hp_ptr->h_addr, hp_ptr->h_length);
		hp_ptr = gethostbyaddr((char *)&addr,sizeof(addr),AF_INET);
		if (hp_ptr != NULL && strcmp(hp_ptr->h_name, src) != 0){
			strncpy(newname, hp_ptr->h_name,len);
		}
		else {
			hp_ptr = NULL;
		}
	}
	if (hp_ptr == NULL) {
		strncpy(newname,src,len);
	}

}

void QUANTAnet_socketbase_c :: getHostName(char *name, int len)
{
	static char hostname[256];

	if (gethostname(hostname,256) < 0) {
		name[0] = '\0';
		return;
	}

	expandHostName(hostname, name, len);
}

long QUANTAnet_socketbase_c::getSelfIP()
{
  const int maxhostnamelen = 250;
  char hostname[maxhostnamelen];

  // First get the name of the current host.
  if (gethostname(hostname, maxhostnamelen) == -1) {
    printf("Error calling gethostname.\n");
    return 0;
  }
  // Look up the host entry for the current host.
  struct hostent* he = gethostbyname(hostname);
  if (he == NULL) {
    printf("Error calling gethostbyname.\n");
    return 0;
  }
  // Process the list of addresses.
  long ipnum;
  for (int i = 0; he->h_addr_list[i] != 0; ++i) {
    if (he->h_addrtype == AF_INET) {
      memcpy(&ipnum, he->h_addr_list[i], sizeof(ipnum));
      return ipnum;
    }
  }
  return 0;
}

void QUANTAnet_socketbase_c::getSelfIP(char* name)
{
  ipNumToString(getSelfIP(), name);
}

int QUANTAnet_socketbase_c::hostnameToIP(const char* hostname, char* hostIP)
{
#ifndef WIN32
	extern int h_errno;
#endif
	struct hostent* host;

	host = gethostbyname(hostname);

	if (!host) {
		// check if hostname is a dotted ip address
                unsigned a, b, c, d;
                int result = sscanf(hostname, "%u.%u.%u.%u", &a, &b, &c,&d);	
		printf("Unable to resolve hostname: %s\n",hostname);
		if (result == 0 || result == EOF) {
        	         printf("Unable to resolve hostname:%s\n",hostname);

                        /* This is needed becoz on the Sun
                         * gethostbyname returns nothing if
                         * the explicit IP address is specified,
                         * whereas on the SGI the same IP is
                         * returned.
                         */

                        sprintf(hostIP,"?.?.?.?");

                        return FAILED;
                }
                else {
                        if (a > 255 || b > 255 || c > 255 || d > 255) {
                                printf("Unable to resolve host name: %s \n", hostname); 
                                sprintf(hostIP, "?.?.?.?");
                                return FAILED;
                        }
                        else {
                                unsigned addr, addr2;

                                addr = (a << 24) + ((b << 16) &
0x00FF0000) + ((c << 8) & 0x0000FF00) + (d & 0x000000FF);
                                addr2 = htonl(addr);
                                ipNumToString(addr2, hostIP);
                                return OK;
                        }
                }
        }
        unsigned int* addr;
        addr = (unsigned int*) host->h_addr_list[0];
        ipNumToString(*addr, hostIP);
        return OK;
}

unsigned int QUANTAnet_socketbase_c::getTotalDataSent()
{
	return totalDataSent;
}

unsigned int QUANTAnet_socketbase_c::getTotalDataRead()
{
	return totalDataRead;
}

double QUANTAnet_socketbase_c::getInstantSendBandwidth(){
	return instSendBandwidth;
}

double QUANTAnet_socketbase_c::getInstantReceiveBandwidth(){
	return instReadBandwidth;
}

double QUANTAnet_socketbase_c::getAverageSendBandwidth()
{
	return avgSendBandwidth;
}


double QUANTAnet_socketbase_c::getAverageReceiveBandwidth()
{
	return avgReadBandwidth;
}

void QUANTAnet_socketbase_c::computeInstantSendBandwidth()
{
	/* Figure out what time it is */

	currentTime = getTimeInSecs();
    if (!sentCount)
    {
        initialSentTime = currentTime;
    }
    else
    {
         double sentDifference = currentTime - previousSentTime ;
        if (sentDifference > 0)
            instSendBandwidth = double(totalDataSent - prevTotalDataSent) / (currentTime-previousSentTime);
        //else instSendBandwidth is the previous instantaneous SBW value
    }
    prevTotalDataSent = totalDataSent;		
	previousSentTime = currentTime;
}

void QUANTAnet_socketbase_c::computeInstantReceiveBandwidth()
{
	/* Figure out what time it is */
	currentTime = getTimeInSecs();
    if (!readCount)
    {
        initialReadTime = currentTime;
    }
    else
    {
        double readDifference = currentTime - previousReadTime ;
        if (readDifference > 0)
            instReadBandwidth = double(totalDataRead - prevTotalDataRead) / (currentTime-previousReadTime);
        //else instReadBandwidth is the previous inst.RBW
    }
    prevTotalDataRead = totalDataRead;
    previousReadTime = currentTime;
}

void QUANTAnet_socketbase_c::computeAverageReceiveBandwidth()
{
    //Average bandwidth calculated from the second packet received
    if (readCount)
    {        
        currentTime = getTimeInSecs();
        avgReadBandwidth = double(totalDataRead)/(currentTime-initialReadTime);
    }
    //calculate the burstiness with the inst read bandwidth
    calculateBurstiness(instReadBandwidth);
}

void QUANTAnet_socketbase_c::computeAverageSendBandwidth()
{
    //Average bandwidth calculated from the second packet sent
	if (sentCount)
    {
        currentTime = getTimeInSecs();
        avgSendBandwidth = double(totalDataSent)/(currentTime-initialSentTime);
    } 
}

double QUANTAnet_socketbase_c :: getInstantLatency()
{
	return instLat;
}

double QUANTAnet_socketbase_c :: getAverageLatency()
{
    return avgLat;
}

void QUANTAnet_socketbase_c::setInstantLatency(double lat)
{
    double curTime = getTimeInSecs();
	if (latCount == 0){
		minLat = lat;
	}
	instLat = lat;
	if (lat > maxLat) maxLat = lat;
	if (lat < minLat) minLat = lat;

    //Calculate the jitter with the inst.latency -also increment latency count and total latency calculated
    calculateJitter(lat);
}

double QUANTAnet_socketbase_c::getMinLatency()
{
	return minLat;
}

double QUANTAnet_socketbase_c::getMaxLatency()
{
	return maxLat;
}

void QUANTAnet_socketbase_c::incrementDataSent(unsigned long size)
{
    totalDataSent+=size;
    computeInstantSendBandwidth();
    computeAverageSendBandwidth();
    sentCount++;
}

void QUANTAnet_socketbase_c::incrementDataRead(unsigned long size)
{
    double curTime = getTimeInSecs();
    totalDataRead+=size;
	if (!readCount)
    {
        prevLocalTime = curTime;
    }
    if (readCount)
    { 
        instDelay = curTime - prevLocalTime; 
        if (readCount == 1) minDelay = instDelay;
        totalDelay+=instDelay;
        prevLocalTime = curTime; 
        if (instDelay>maxDelay) maxDelay = instDelay;
        if (instDelay<minDelay) minDelay = instDelay; 
    }

    computeInstantReceiveBandwidth();
    computeAverageReceiveBandwidth();
}

double QUANTAnet_socketbase_c::getInstantInterMesgDelay()
{
	return instDelay;
}

double QUANTAnet_socketbase_c::getMaxInterMesgDelay()
{
	return maxDelay;
}

double QUANTAnet_socketbase_c::getMinInterMesgDelay()
{
	return minDelay;
}

double QUANTAnet_socketbase_c::getAverageInterMesgDelay()
{
    return avgDelay;
}

unsigned long QUANTAnet_socketbase_c :: getPacketsRead()
{
    return readCount;
}

unsigned long QUANTAnet_socketbase_c :: getPacketsSent()
{
    return sentCount;
}

double QUANTAnet_socketbase_c::getSTALatency()
{
	return STALatency;
}

double QUANTAnet_socketbase_c::getJitter()
{
	return jitter;
}

double QUANTAnet_socketbase_c::getSTABandwidth()
{
	return STABandwidth;
}

double QUANTAnet_socketbase_c::getBurstiness()
{
	return burstiness;
}

void QUANTAnet_socketbase_c :: calculateJitter (double lat)
{
    int lastnumber;
    int i;
    double devn;    
    //find the absolute latency
    lat=fabs(lat);
    
    if (latCount <  LATENCY_BUF_SIZE)
         lastnumber = tempLatCounter+1;
    else 
        lastnumber = LATENCY_BUF_SIZE;
    
    
    //calculate the cumulative sum of the latencies
    latTotal+= lat;        
    latencyBuffer[tempLatCounter] = lat;                     
    
    STlatencySum =0;
    for (i=0; i<lastnumber; i++)
    {
        STlatencySum+= latencyBuffer[i];        
    }
    
    STALatency = STlatencySum / lastnumber;
    
    //calculating jitter for the latencies in the buffer
    
    jitter =0;    
    for (i=0; i<lastnumber; i++)
    {
        devn = fabs(latencyBuffer[i] - STALatency);
        jitter+= devn;
    }
    jitter = jitter/lastnumber;        
    
    latCount++;
    avgLat = (double)latTotal/(double)latCount;

    if (tempLatCounter ==  LATENCY_BUF_SIZE -1)    
        tempLatCounter =0;        
    else
        tempLatCounter++;
}


void QUANTAnet_socketbase_c :: calculateBurstiness (double bw)
{
    int i;
    int lastnumber; 
    double devn;    
    //find the absolute bandwidth
    bw = fabs(bw);
    if (readCount)
    {        
        if (readCount <=  BANDWIDTH_BUF_SIZE)
            lastnumber = tempBWCounter+1;
        else 
            lastnumber = BANDWIDTH_BUF_SIZE;
        
        bandwidthBuffer[tempBWCounter] = bw;    
        
        STbandwidthSum =0;
        for (i=0; i<lastnumber; i++)
        {
            STbandwidthSum+= bandwidthBuffer[i];        
        }
    
        STABandwidth = STbandwidthSum / lastnumber;
        
        //calculating burstiness for the bandwitdh values in the buffer
        
        burstiness =0;    
        for (i=0; i<lastnumber; i++)
        {
            devn = fabs(bandwidthBuffer[i] - STABandwidth);
            burstiness+= devn;
        }
        burstiness = burstiness/lastnumber;
    }
    
    readCount++;

    //Delay is calculated from the second packet onwards
    if (readCount >1)
    {
        avgDelay = (double )(totalDelay /(readCount -1));
        if (tempBWCounter == BANDWIDTH_BUF_SIZE -1)    
            tempBWCounter = 0;        
        else
            tempBWCounter++;
    }
    else
        avgDelay = 0;
}

int QUANTAnet_socketbase_c::getSocketId()
{
        printf("In Socketbase\n");
        return 0;
}

int QUANTAnet_socketbase_c::selectSock(void * ptr, int *status, int numSocks, 
 struct timeval * timeout)
{
	fd_set rset;
	int maxfd=0;
	int i, retval;

	QUANTAnet_socketbase_c **arrayOfSocks = (QUANTAnet_socketbase_c**) ptr;
	FD_ZERO(&rset);

	for(i = 0; i < numSocks; i++)
	{
		if (arrayOfSocks[i]!= NULL)
		{
			FD_SET(arrayOfSocks[i]->getSocketId(), &rset);
			if (arrayOfSocks[i]->getSocketId() > maxfd) 
				maxfd = arrayOfSocks[i]->getSocketId();
		}
	}

	retval = select(maxfd+1, &rset, NULL, NULL, timeout);

	if (retval < 0)
		return FAILED;

	if (retval == 0)
		return TIMED_OUT;

	for(i = 0; i < numSocks; i++)
	{
		if (arrayOfSocks[i]!= NULL)
		{
			if(FD_ISSET(arrayOfSocks[i]->getSocketId(), &rset))
				status[i] = 1;
			else
				status[i] = 0;
		}
	}
	return OK;
}
