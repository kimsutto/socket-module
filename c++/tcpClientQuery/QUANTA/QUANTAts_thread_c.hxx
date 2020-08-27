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

#ifndef _QUANTAPLUS_THREAD_C
#define _QUANTAPLUS_THREAD_C

#include "QUANTAinit.hxx"
#include "QUANTAglobals.hxx"

typedef void* (*QUANTA_THREADED_FUNC)(void*);

#ifdef QUANTA_USE_PTHREADS
	typedef pthread_t QUANTA_THREAD_T;
#elif defined(WIN32) && defined(QUANTA_USE_WINDOWSTHREADS)
    typedef unsigned QUANTA_THREAD_T;
//    typedef unsigned (__stdcall *QUANTA_THREADED_FUNC)(void *);
//	typedef unsigned (__stdcall *THREAD_START) (void *);
#else
	#error One of QUANTA_USE_PTHREADS or QUANTA_USE_WINDOWSTHREADS must be defined.
#endif


extern "C" void *entryPoint(void *arg);


/**
Thread class. Note: the QUANTAts_thread-related classes are intended to be
simplified versions of the pthread interfaces so that beginners
can quickly create threaded programs. For greater control over threads
use the globus thread API which is very similar to the pthreads API.
*/
class QUANTAts_thread_c {
public:
	QUANTAts_thread_c(void);
	virtual ~QUANTAts_thread_c(void);


	// run() is not a pure virtual for
	// backwards compatability with original thread class
	virtual void run(void) {}

	/** Create a thread.
	    @param threaded_func is your function that will be called in
	    a separate thread. The function needs to be of the form:

	    void * threaded_func(void *arg);

	    @param arg is the argument to pass to the threaded function.
	    @return 0 if thread successful, else non-zero.
	    */
	int create(void * (*threaded_func)(void *), void *arg);
	void create(void);
	

	/// Return thread handle.
	QUANTA_THREAD_T *getThread();
    
	inline void start(void) { _running = 1; }
	inline void stop(void) { _running = 0; }

	/**
		* return 1 if running is true; 0 otherwise
		**/
	inline bool isRunning(void) const { return _running; }


	/**
		* mark this thread as detached to reclaim its resources when it terminates
		* @return 0 if successful, else non-zero.
		**/
	bool detach(void);


	/**
		* wait for this thread to terminate
		*	@return 0 if successful, else non-zero.
		**/
	bool join(void);


private:
	QUANTA_THREAD_T itsThread;
	bool _initted;
	bool _running;
};


#endif
