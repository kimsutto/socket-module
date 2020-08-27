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
 * File name: QUANTAnet_iovec.cxx
 *
 * Description: Wrapper class for the iovec structure
 *
 * Comments:
 *
 * Change Log:
 */

#include "QUANTAnet_iovec_c.hxx"

//constructor
QUANTAnet_iovec_c :: QUANTAnet_iovec_c(int size)
{ 	
	num_of_elements = 0; 						
	nalloc = size; 
	bufList = NULL;			
}

//destructor
QUANTAnet_iovec_c :: ~QUANTAnet_iovec_c()
{	
	
	if (bufList) {
		free(bufList);
		bufList = NULL;
	}
	 
}

//add pointer and its length to the array
bool QUANTAnet_iovec_c :: add(void* buff, int length)
{
	if(bufList == NULL)
	{
#ifdef WIN32
		bufList = (WSABUF *) malloc (sizeof (WSABUF) * nalloc);
#else
		bufList = (struct iovec*) malloc (sizeof (iovec) * nalloc);
#endif

		//if malloc can not allocate memory
		if(bufList == NULL)
		{
			printf("QUANTAnet_iovec_c :: add() :: malloc - out of memory");
			return false;
		}		
	}	
	
	else if(num_of_elements >= nalloc)		
	{
		nalloc = nalloc*2; //nalloc is doubled to reallocate twice number of elements									
#ifdef WIN32
		bufList = (WSABUF *)realloc((WSABUF *)bufList, nalloc * sizeof(WSABUF));	
#else
		bufList = (struct iovec*)realloc((iovec *)bufList, nalloc * sizeof(iovec));
#endif

		//if realloc can not allocate memory
		if(bufList == NULL)
		{
			printf("QUANTAnet_iovec_c :: add() :: realloc - out of memory");			
			return false;
		}				
	}
		
#ifdef WIN32
	bufList[num_of_elements].buf = (char*)buff;
	bufList[num_of_elements].len = length;	
#else
	bufList[num_of_elements].iov_base = buff;
	bufList[num_of_elements].iov_len = length;
#endif

	++num_of_elements;				
	return true;
}

//remove the entry at index
bool QUANTAnet_iovec_c :: removeAt(int index)
{		
	--num_of_elements;
	
	if(index > num_of_elements)
		return false;
	else
	{
		for(int i = index; i < num_of_elements; i++)
			bufList[i] = bufList[i + 1];	
		return true;	
	}
}

//clear the array
void QUANTAnet_iovec_c :: removeAll()
{
	num_of_elements = 0;
	bufList = NULL;
	
}
//returns the size fo teh array
int QUANTAnet_iovec_c :: size()
{
	return num_of_elements;
}




