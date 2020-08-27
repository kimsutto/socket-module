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

// Turn this on to do in function tracing.
//#define CVRN_TRACE



#ifndef _CVRN_DEBUG
#define _CVRN_DEBUG


#define DBG_MEM 32
#define DBG_ERR 2
#define DBG_MSG 4
#define DBG_FAIL 8
#define DBG_REF 16
#define DBG_PUB 1
#define DBG_LOCK 64
#define DBG_THREAD 128

#define DBG_ALL (!0)
#ifdef CVRN_TRACE
	#ifdef WIN32
	#define cvrnTrace(prefix,mesg) 	EnterCriticalSection(&Win32StdioLock); printf("  quanta: "); if (prefix) {printf(prefix); printf(":");}  printf(mesg); printf("\n"); fflush(stdout); LeaveCriticalSection(&Win32StdioLock);
	#else
	#define cvrnTrace(prefix,mesg) 	nexus_stdio_lock(); printf("  quanta: "); if (prefix) {printf(prefix); printf(":");}  printf(mesg); printf("\n"); fflush(stdout); nexus_stdio_unlock();
	#endif
#else
#define cvrnTrace(prefix,mesg)
#endif


extern int _cvrnDebugLevel;
void cvrnSetDebugFilter(int);
void cvrnDebug(char* prefix, int lvl, char *fmt, ...);
void cvrnMesg(char *prefix, char *fmt, ...);

/**
A thread-safe printf. Use this rather than the standard printf to print
text to the TTY.

example 1:

cvrnPrintf("Hello %d %f", foo,bar);

prints something like:

Hello 123 32.4

@param fmt format string that you normally would have specified for
printf.
@param arguments for the format string.

*/
void cvrnPrintf(char *fmt, ...);




#endif
