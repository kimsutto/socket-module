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

#include "QUANTAinit.hxx"
#include "QUANTAmisc_debug.hxx"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32_WCE
#include <time.h> // for InitTime function
#endif

#if defined(WIN32) && !defined(_WIN32_WCE)
#include <fcntl.h>
#endif

#define QUANTA_VERSION VERSION

#ifdef QUANTA_USE_WINDOWSTHREADS
#define QUANTA_THREAD_VERSION "MICROSOFT WINDOWS THREAD VERSION"
#else
#define QUANTA_THREAD_VERSION "PTHREAD VERSION"
#endif

#define QUANTA_VERSION_DATE "JUNE 06 2006"
#define QUANTA_BUILD_DATE __DATE__
#define QUANTA_BUILD_TIME __TIME__

#define _CVRN_MAX_SPROCS 64
#define _CVRN_MAX_SPROC_MUTEX_MEM (1024*1024*4)  // Four Megs default

#ifdef WIN32
#include <sys\timeb.h>

// This will be used in QUANTAmisc_debug.cxx file for internal use only.
CRITICAL_SECTION Win32StdioLock;
LARGE_INTEGER Frequency;
LARGE_INTEGER StartCounter;
struct timeval StartTimeVal;
int UseFTimeOnly = 0;
#endif

#ifdef _WIN32_WCE  
int errno;
#endif

#if defined(QUANTA_USE_PTHREADS)
pthread_mutex_t pthread_stdio_lock;
#endif

#ifdef __cplusplus
extern "C" {
   extern int findquanta(char *c) { return 1; }
}
#endif

// Show the version # of QUANTA.
static void version()
{
	//	char date[256], dummy1[256], dummy2[256];
	//	char versionNum[256];
	//	sscanf(rcsid,"%s %s %s %s", dummy1, dummy2, versionNum, date);
	printf("QUANTA %s (%s) [%s] - [COMPILED %s %s]\n\n",  QUANTA_VERSION,
	       QUANTA_VERSION_DATE, QUANTA_THREAD_VERSION,
	       QUANTA_BUILD_DATE, QUANTA_BUILD_TIME);

}

// Check the QUANTA_DEBUG environment variable for debug level to use.
#ifndef _WIN32_WCE
void checkDebugLevel()
{
	// Figure out how verbose QUANTA will be. 0 = user level messages.
	char* debugLevel;
	debugLevel = getenv("QUANTA_DEBUG");

	if (debugLevel)
		cvrnSetDebugFilter(atoi(debugLevel));
	else
		cvrnSetDebugFilter(0);
}
#endif


#ifdef WIN32
static int WinSockInitialized = 0;

// This function is going to be registered using atexit function so that
// it is going to be called whenever the program exits.
#ifdef _WIN32_WCE
void __cdecl ExitApplication()
#else
static void ExitApplication()
#endif
{
    if (WinSockInitialized) WSACleanup();
	DeleteCriticalSection(&Win32StdioLock);
}

static void InitWinsock()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
 
    wVersionRequested = MAKEWORD(1, 1); // It requires Winsock version 2.
 
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
		printf("Cannot load the valid winsock 2 dll. Program will be terminated.\n");
        exit(1);
    }
    WinSockInitialized = 1; // Winsock dll is loaded.

	// This will register ExitApplication function to exit chain. By 
	// registering this function, it will be called when the program 
	// exits normally, i.e. not crashing in the middle of execution.
    atexit(ExitApplication);
}
#endif

// Initialize QUANTA.
void QUANTAinit()
{
#ifdef WIN32
    InitializeCriticalSection(&Win32StdioLock);
    InitWinsock();

    #ifndef _WIN32_WCE
     _fmode = _O_BINARY; // set reading file mode to binary by default.
    #endif
    #ifdef _WIN32_WCE
        InitTime(); // initialization for all time related functions such as gmtime and _ftime
    #endif

    if (!QueryPerformanceFrequency(&Frequency)) {
        printf("High performance counter is not available. ftime function will be used instead to calculate performance.\n");
        UseFTimeOnly = 1;
    }
    else {
        struct _timeb timeSec;

        QueryPerformanceCounter(&StartCounter);
        _ftime(&timeSec);
        StartTimeVal.tv_sec = timeSec.time;
        StartTimeVal.tv_usec = (long) timeSec.millitm;
        StartTimeVal.tv_usec *= 1000;
    }        
#endif

	version();
	#ifndef _WIN32_WCE
        	checkDebugLevel();
	#endif


#if defined(QUANTA_USE_PTHREADS)
    pthread_mutex_init(&pthread_stdio_lock, NULL);
#endif

	return;
}

void QUANTAexit()
{
#if defined(QUANTA_USE_PTHREADS)
    pthread_mutex_destroy(&pthread_stdio_lock);
#endif
}
