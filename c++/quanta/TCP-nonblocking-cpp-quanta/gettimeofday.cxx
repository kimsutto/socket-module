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
#include <sys\timeb.h>
#include <math.h>

#include "gettimeofday.hxx"
#include "QUANTAinit.hxx"

// Note that tzp is going to be ignored in this function and only milliseconds will
// be reported instead of microseconds.
int quanta_gettimeofday(struct timeval *tp, void *tzp)
{
	if (UseFTimeOnly) { // Use _ftime. Precision is limited to milliseconds level.
	    struct _timeb t;

	    _ftime(&t);
		tp->tv_sec = t.time;
		tp->tv_usec = t.millitm * 1000;
	}
	else { // Use high performance counter. Precision is in microseconds level.
		LARGE_INTEGER endCounter;
		double extraSeconds;
		
		QueryPerformanceCounter(&endCounter);
		extraSeconds = (double) (endCounter.QuadPart - StartCounter.QuadPart) / (double) Frequency.QuadPart;
		tp->tv_sec = StartTimeVal.tv_sec + (long) floor(extraSeconds);
		tp->tv_usec = StartTimeVal.tv_usec + (long) floor((extraSeconds - floor(extraSeconds)) * 1000000);
	}

    return 0;
}
#endif
