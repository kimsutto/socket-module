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

#include "QUANTAmisc_debug.hxx"
#include "QUANTAconfig.h"
#include "QUANTAinit.hxx"

#include <stdio.h>
#include <stdarg.h>

int _cvrnDebugLevel= 0;
void cvrnSetDebugFilter(int lvl) {_cvrnDebugLevel = lvl;}

void cvrnPrintf(char *fmt, ...)
{
	#ifdef WIN32
		EnterCriticalSection(&Win32StdioLock);
	#else
		pthread_mutex_lock(&pthread_stdio_lock); 
	#endif
	
	va_list args;
	va_start(args,fmt);
	vprintf(fmt,args);
	fflush(stdout);
	
	#ifdef WIN32
		LeaveCriticalSection(&Win32StdioLock);
	#else
		pthread_mutex_unlock(&pthread_stdio_lock);
	#endif
}

void cvrnDebug(char* prefix, int lvl, char *fmt, ...)
{
	#ifdef WIN32
		EnterCriticalSection(&Win32StdioLock);
	#else
		pthread_mutex_lock(&pthread_stdio_lock);
	#endif
		
		va_list args;
		va_start(args,fmt);
		if (prefix)
		printf("    quanta(%d) %s: ", lvl, prefix);
		else
		printf("    quanta(%d) ", lvl);
		vprintf(fmt,args);
		printf("\n");

		fflush(stdout);
	
	#ifdef WIN32
		LeaveCriticalSection(&Win32StdioLock);
	#else
		pthread_mutex_unlock(&pthread_stdio_lock);
	#endif
}

void cvrnMesg(char* prefix, char *fmt, ...)
{
	#ifdef WIN32
		EnterCriticalSection(&Win32StdioLock);
	#else
		pthread_mutex_lock(&pthread_stdio_lock);
	#endif
	
	va_list args;
	va_start(args,fmt);
	if (prefix)
		printf("QUANTA: %s: ", prefix);
	else
		printf("QUANTA: ");
	vprintf(fmt,args);
	printf("\n");

	fflush(stdout);
	
	#ifdef WIN32
		LeaveCriticalSection(&Win32StdioLock);
	#else
		pthread_mutex_unlock(&pthread_stdio_lock);
	#endif
}

