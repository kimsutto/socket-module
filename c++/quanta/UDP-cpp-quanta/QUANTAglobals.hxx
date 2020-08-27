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

#ifndef _QUANTA_GLOBALS_H
#define _QUANTA_GLOBALS_H

#include <ctype.h>

#include "QUANTAconfig.h"

// add bool, true, false support for irix o32 format
#if (defined(__sgi) && defined(HAVE_BINARY_TYPE_32) && !defined(_BOOL))
typedef int bool;
#define false 0
#define true 1
#endif


// disable ms visual c++ warnings
#ifdef WIN32
// disable ms visual c++ warning - "C++ Exception Specification ignored"
#pragma warning(disable : 4290)

// disable ms visual c++ warning - "deletion of an array expression; conversion to pointer supplied"
// maybe look into this one more
#pragma warning(disable : 4154)
#endif


// add a case sensitive string compare if not available
#ifdef WIN32
#	ifndef HAVE_STRCASECMP 
#		ifdef HAVE_STRICMP 
#			define strcasecmp(x,y) stricmp(x,y) 
#		elif defined(HAVE_STRCMPI) 
#				define strcasecmp(x,y) strcmpi(x,y) 
#		else // no case insensitive function available
#			define QUANTA_STRCASECMP 
extern int strcasecmp(const char *first, const char *second);
#		endif
#	endif
#endif

/** Platform-independent sleep function.
    Different platforms and APIs have different methods for implementing
    ways to suspect program activity. This function serves as a wrapper
    which internally calls usleep() or Sleep() as needed.
    @param microseconds number of microseconds to wait.
*/
void QUANTAsleep(const unsigned int& seconds);

/** Platform-independent usleep function.
*/
void QUANTAusleep(const unsigned int& microseconds);

// add socklen_t to systems that do not provide it (sgi, darwin)
#if !defined(HAVE_SOCKLEN_T)
//typedef int socklen_t;
#endif

#ifndef WIN32
#include <unistd.h>
#include <stdio.h>
#endif

#endif
