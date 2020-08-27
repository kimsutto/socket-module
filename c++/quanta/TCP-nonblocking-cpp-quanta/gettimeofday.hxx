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

#ifdef WIN32
#ifndef __GETTIMEOFDAY_H_
#define __GETTIMEOFDAY_H_

#ifndef __HAVE_WINSOCK2_H
#include <winsock2.h>
#define __HAVE_WINSOCK2_H
#endif /* __HAVE_WINSOCK2_H */

#ifdef __cplusplus
extern "C"
{
#endif

/** This function is provided only for Windows version of QUANTA library
    to be used in performance monitoring routines. It is not a perfect 
    solution to this problem but is working for QUANTA library.
    Note that QUANTAinit function must be called before using this function.
    If this function is used in 486 or older machines that do not provide
    the high performance counter, time will be measured in milliseconds level
    only. Thus, Pentium or higher machines must be used to measure the time 
    in microsecond level accuracy.

    There is a known bug in high performance counter that only happens on
    NEC PC9800 machine running Japanese Windows 95. This is excerpt from
    Microsoft's knowledgebase #Q152145:

    "QueryPerformanceFrequency called from an application running on an 
     NEC PC9800 Series computer with Japanese Win95 (PC9800 version), returns
     a static value (1193180 Ticks/sec) regardless of the actual frequency of 
     the actual frequency of the high-resolution performance counter. On NEC
     PC 9800 Series compuers, the static value can be 2457600 ticks/sec."

    @param struct timeval* tp Returns local time through this variable
       tp->tv_sec: Time in seconds since midnight (00:00:00), January 1, 1970, 
                   coordinated universal time (UTC).
       tp->tv_usec: Fraction of a second in microseconds.
    @param tzp Not supported time zone information. Must be NULL
    @return returns always 0.
 */
int quanta_gettimeofday(struct timeval *tp, void *tzp = NULL);

#ifdef __cplusplus
}
#endif

#endif
#endif
