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

#include "QUANTAts_thread_c.hxx"
#include <stdio.h>

#if defined(WIN32) && defined(QUANTA_USE_WINDOWSTHREADS)
#include <errno.h>
#ifndef _WIN32_WCE
#include <process.h> // for _beginthreadex function
#else
#include <thread.h>
#endif
#endif
#if defined(WIN32) && defined(QUANTA_USE_WINDOWSTHREADS)
	typedef unsigned (__stdcall *THREAD_FUNC) (void *);
#endif


extern "C" void *entryPoint(void *arg)
{
	((QUANTAts_thread_c*)arg)->run();
	delete (QUANTAts_thread_c*)arg;
	return NULL;
}


QUANTAts_thread_c::QUANTAts_thread_c(void)
{
	_running = false;
	_initted = false;
}


QUANTAts_thread_c::~QUANTAts_thread_c(void)
{
}


void QUANTAts_thread_c::create(void)
{
	if(_initted)
		return;

	// place variable _running first so isRunning() works
	_running = true;
	
	// detect create() errors, throw exception
	if(create(entryPoint, this))
	{
		fprintf(stderr, "QUANTAts_thread_c::create(void) : cannot create thread\n");
		_initted = false;
		_running = false;
		
		// throw exception
		
		return;
	}
	
	_initted = true;
}


int QUANTAts_thread_c::create(void *(*threaded_func)(void *), void *arg)
{
	int status;
#ifdef QUANTA_USE_PTHREADS
//	printf("\nCreating a thread");
//	status = pthread_create(&itsThread, (pthread_attr_t *)NULL, threaded_func, arg);
	status = pthread_create(&itsThread, NULL, threaded_func, arg);
//	pthread_detach(itsThread);
	return status;
#endif
#if defined(WIN32) && defined(QUANTA_USE_WINDOWSTHREADS)
	unsigned result = _beginthreadex(NULL, 0, (THREAD_FUNC) threaded_func, arg,	0, &itsThread);
	return (result == 0) ? errno : 0;
#endif
}


QUANTA_THREAD_T* QUANTAts_thread_c :: getThread()
{
	return &itsThread;
}


bool QUANTAts_thread_c::detach(void)
{
	int ret_val = 0;

#ifdef QUANTA_USE_PTHREADS
	if((ret_val = pthread_detach(itsThread)) != 0)
		return false;

	return true;

/*
	{
    switch(ret_val)
    {
       case EINVAL:
          fprintf(stderr, "QUANTAts_thread_c::detach() - this thread isn't a joinable thread!\n");
          break;
       case ESRCH:
          fprintf(stderr, "QUANTAts_thread_c::detach() - no thread found that refers to this thread!\n");
          break;
       default:
          fprintf(stderr, "QUANTAts_thread_c::detach() - default error!\n");
          break;
    }
	}
*/
#endif


#ifdef QUANTA_USE_WINDOWSTHREADS
	return true; //Temporary fix to remove error 'Function must return a value'
#endif //JAVID 03/05/2003.Equivalent of pthread_detach has to be done for Windowsthreads

//	return ret_val;
}


bool QUANTAts_thread_c::join(void)
{
	int ret_val = 0;
	void *status;

#ifdef QUANTA_USE_PTHREADS
	if((ret_val = pthread_join(itsThread, &status)) != 0)
		return false;
	
	return true;

// if((ret_val = pthread_join(itsThread, (void**)NULL)) != 0)
// {
/*
    switch(ret_val)
    {
       case EINVAL:
          fprintf(stderr, "QUANTAts_thread_c::join() - this thread isn't a joinable thread!\n");
          break;
       case ESRCH:
          fprintf(stderr, "QUANTAts_thread_c::join() - no thread found that refers to this thread!\n");
          break;
       case EDEADLK:
          fprintf(stderr, "QUANTAts_thread_c::join() - deadlock or thread specifies calling thread!\n");
          break;
       default:
          fprintf(stderr, "QUANTAts_thread_c::join() - default error!\n");
          break;
    }
*/
// }
#endif


#ifdef QUANTA_USE_WINDOWSTHREADS
	return true; //Temporary fix to remove error 'Function must return a value'
#endif //JAVID 03/05/2003. Equivalent of pthread_join has to be done for Windowsthreads

// return ret_val;
}
