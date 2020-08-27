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

#ifndef _QUANTA_INIT
#define _QUANTA_INIT

#include "QUANTAconfig.h"

#ifdef WIN32
#ifndef __HAVE_WINSOCK2_H
#include <winsock2.h>
#define __HAVE_WINSOCK2_H
#endif /* __HAVE_WINSOCK2_H */

extern CRITICAL_SECTION Win32StdioLock;
extern LARGE_INTEGER Frequency;
extern LARGE_INTEGER StartCounter;
extern struct timeval StartTimeVal;
extern int UseFTimeOnly;
#endif

#ifndef WIN32
#define quanta_gettimeofday	gettimeofday
#endif

#if defined(QUANTA_USE_PTHREADS)
#ifndef __HAVE_PTHREAD_H
#include <pthread.h>
#define __HAVE_PTHREAD_H
#endif

extern pthread_mutex_t pthread_stdio_lock;
#endif

/**
This is the first thing you do to start QUANTA. This initializes
stuffs that must be initialized before calling any other QUANTA 
calls. Do not use any other QUANTA calls until you do this first.
*/
void QUANTAinit();

/**
Exit QUANTA when your application shuts down. This shuts down Globus.
*/
void QUANTAexit();

#endif
