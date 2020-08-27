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

/*
 * File name: QUANTAnet_iovec.hxx
 *
 * Description: This class is a container class for the iovec structure 
 *
 * Usasge: 1) readv(), writev() Methods in QUANTAnet_tcp_c class
 *         2) recvmsg(), sendmsg() Methods in QUANRTAnet_udp_c class
 *
 * Comments: QUANTAnet_iovec_c is a dynamic array of Iovec structures
 *			 Each Iovec strucure contains two values
 *           1) base pointer 2) its length
 *           You can add diffrenet type of objects to the QUANTAnet_iovec_c
 *  
 * How to use -> See: /QUANTA/demos/network/tcp/ioservertcp.cxx
 *											    ioclienttcp.cxx
 *
 * Change Log:
 */


#ifndef _QUANTANET_IOVEC
#define _QUANTANET_IOVEC

#ifndef __HAVE_STDIO_H
#include <stdio.h>
#define __HAVE_STDIO_H
#endif
#include <stdlib.h>


#include "QUANTAglobals.hxx"

#ifdef WIN32
#ifndef __HAVE_WINSOCK2_H
#include <winsock2.h>
#define __HAVE_WINSOCK2_H
#endif /* __HAVE_WINSOCK2_H */
#else
#include <sys/uio.h>
#endif

class QUANTAnet_iovec_c
{
	private:
		
		//keeps the number of elements in the iovec array
		int num_of_elements;

		//keeps the number of elements dynamically allocated
		int nalloc;
		
		//base pointer of the iovec dynamic array
#ifdef WIN32
		WSABUF* bufList;
#else
		struct iovec* bufList;
#endif
							
	public:		

		//constructor
		//By default bufList is allocated 512 entries because 
		//512*sizeof(iovec) = 4K(Page Size) to avoid page fault
		QUANTAnet_iovec_c(int size = 512);
		
		//destructor
		~QUANTAnet_iovec_c();
		
		/**
		@desc add the pointer and its length to the iovec array
		@param buff void*
    	@param length int
	    @return true or false
	    **/
		bool add(void* buff, int length);
			
		/**
		@desc remove the specific iovec entry			
    	@param index int
	    @return true or false
	    **/
		bool removeAt(int index);				
				
		/**
		@desc clear all entry in iovec array
	    @return void
	    **/
		void removeAll();
				
		/**
		@desc returns size of the iovec array    	
	    @return size int
	    **/
		int size();

		/**
		@desc used in readv(), writev() methods (internal use only)    	
	    @return base pointer of the dynamic array of iovec structures    			
	    **/
		
#ifdef WIN32
		WSABUF* getBuffer() {return bufList;}
#else
		const iovec* getBuffer() {return bufList;}
#endif

};

#endif

