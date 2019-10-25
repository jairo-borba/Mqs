/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 jairo-borba jairo.borba.junior@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include "mqsProvider/ListClient.h"
#include <appUtil/JJJException.h>
#include <appUtil/Shortcuts.h>
namespace mqsProvider
{
	ListClient::ListClient(
			ListPtr* a_listPtr,
			void* a_buffer )
		:m_listPtr(NULL), m_buffer(NULL)
	{
		this->setListPtr( a_listPtr );
		this->setBuffer( a_buffer );
		VALIDATION_STATUS l_val = scanValidation();
		appUtil::assert( l_val == LST_CLI_VAL_OK , "ListClient: Trying to access a corrupted list" );
	}
	ListClient::~ListClient(void)
	{
	}
	void ListClient::setListPtr(
			ListPtr* a_listPtr )
	{
		m_listPtr = a_listPtr;
		appUtil::assertPointer( m_listPtr );
	}
	void ListClient::setBuffer(
			void* a_buffer )
	{
		m_buffer = a_buffer;
		appUtil::assertPointer( m_buffer );
	}
	bool ListClient::remove(
			unsigned int a_leavingPosition )
	{
		VALIDATION_STATUS l_val = scanValidation();
		RETURN_IF( l_val != LST_CLI_VAL_OK, false );
		RETURN_IF( m_listPtr == NULL, false );
		RETURN_IF( m_buffer == NULL, false );
		RETURN_IF(m_listPtr->head == INDEX_NULL, false);
		RETURN_IF(m_listPtr->tail == INDEX_NULL, false);

		unsigned int l_left = CLUSTER_LEFT	(m_buffer,a_leavingPosition);
		unsigned int l_right = CLUSTER_RIGHT(m_buffer,a_leavingPosition);

		if( m_listPtr->head == m_listPtr->tail ){
			m_listPtr->head		= INDEX_NULL			;
			m_listPtr->tail		= INDEX_NULL			;
		} else if( a_leavingPosition == m_listPtr->tail ){
			CLUSTER_LEFT(m_buffer,l_right)	= l_left	;
			m_listPtr->tail = l_right					;
		} else if( a_leavingPosition == m_listPtr->head ){
			CLUSTER_RIGHT(m_buffer,l_left)	= l_right	;
			m_listPtr->head = l_left					;
		} else {
			CLUSTER_RIGHT(m_buffer,l_left)	= l_right	;
			CLUSTER_LEFT(m_buffer,l_right)	= l_left	;
		}

		CLUSTER_LEFT	(m_buffer,a_leavingPosition) = INDEX_NULL;
		CLUSTER_RIGHT	(m_buffer,a_leavingPosition) = INDEX_NULL;
		m_listPtr->countElements--;

		l_val = scanValidation();
		RETURN_IF( l_val != LST_CLI_VAL_OK, false );
		
		return true;
	}
	unsigned int ListClient::dequeue(void)
	{
		VALIDATION_STATUS l_val = scanValidation();
		RETURN_IF( l_val != LST_CLI_VAL_OK, INDEX_NULL );

		RETURN_IF( m_listPtr == NULL, false );
		RETURN_IF( m_buffer == NULL, false );
		RETURN_IF(m_listPtr->head == INDEX_NULL, INDEX_NULL);
		RETURN_IF(m_listPtr->tail == INDEX_NULL, INDEX_NULL);
		
		unsigned int l_dequeued = m_listPtr->head;
		if( m_listPtr->head == m_listPtr->tail ){
			m_listPtr->head		= INDEX_NULL;
			m_listPtr->tail		= INDEX_NULL;
		} else {
			m_listPtr->head = CLUSTER_LEFT(m_buffer,m_listPtr->head);
			CLUSTER_RIGHT(m_buffer,m_listPtr->head)	= INDEX_NULL;
		}
		CLUSTER_LEFT(m_buffer,l_dequeued)	= INDEX_NULL;
		CLUSTER_RIGHT(m_buffer,l_dequeued)	= INDEX_NULL;
		m_listPtr->countElements--;

		l_val = scanValidation();
		RETURN_IF( l_val != LST_CLI_VAL_OK, INDEX_NULL );
		
		return l_dequeued;
	}
	bool ListClient::enqueue(
			unsigned int a_bufferPosition )
	{
		VALIDATION_STATUS l_val = scanValidation();
		RETURN_IF( l_val != LST_CLI_VAL_OK, false );

		RETURN_IF( m_listPtr == NULL, false );
		RETURN_IF( m_buffer == NULL, false );
		
		if( m_listPtr->tail == INDEX_NULL ){//void queue
			m_listPtr->tail		= a_bufferPosition;
			m_listPtr->head		= a_bufferPosition;
			CLUSTER_LEFT(m_buffer,a_bufferPosition)		= INDEX_NULL;
			CLUSTER_RIGHT(m_buffer,a_bufferPosition)	= INDEX_NULL;
		} else {
			unsigned int sl_tail = m_listPtr->tail;
			CLUSTER_RIGHT(m_buffer,a_bufferPosition)	= m_listPtr->tail;
			CLUSTER_LEFT(m_buffer,a_bufferPosition)		= INDEX_NULL;
			CLUSTER_LEFT(m_buffer,sl_tail)				= a_bufferPosition;
			m_listPtr->tail								= a_bufferPosition;
		}

		m_listPtr->countElements++;

		l_val = scanValidation();
		RETURN_IF( l_val != LST_CLI_VAL_OK, false );
		
		return true;
	}

	unsigned int ListClient::head(void)
	{
		RETURN_IF(m_listPtr == NULL, INDEX_NULL);
		return m_listPtr->head;
	}

	unsigned int ListClient::count(void) const
	{
		RETURN_IF(m_listPtr == NULL, 0);
		return m_listPtr->countElements;
	}
	enum ListClient::VALIDATION_STATUS ListClient::scanValidation(void) const
	{
#if !defined(_DEBUG)
		return LST_CLI_VAL_OK;
#endif
		RETURN_IF( m_listPtr == NULL, LST_LIST_PTR_NOT_DEFINED );
		RETURN_IF( m_buffer == NULL, LST_LIST_BUFFER_NOT_DEFINED );
		RETURN_IF(
				m_listPtr->head == INDEX_NULL && m_listPtr->tail == INDEX_NULL && m_listPtr->countElements == 0,
				LST_CLI_VAL_OK );
		RETURN_IF( m_listPtr->head == m_listPtr->tail && m_listPtr->countElements == 1, LST_CLI_VAL_OK );
		RETURN_IF( !IS_CLUSTER_HEAD(m_buffer,m_listPtr->head), LST_CLI_VAL_INVALID_HEADER );
		RETURN_IF( !IS_CLUSTER_TAIL(m_buffer,m_listPtr->tail), LST_CLI_VAL_INVALID_TAIL );

		unsigned int l_current = m_listPtr->head;
		unsigned int l_countElements = 0;
		unsigned int l_previous = INDEX_NULL;
		while(l_current!=INDEX_NULL){
			++l_countElements;
			RETURN_IF( !IS_CLUSTER_VALID( m_buffer, l_current ), LST_CLI_VAL_INVALID_CLUSTER );
			l_previous = l_current;
			l_current = CLUSTER_LEFT(m_buffer,l_current);
		}
		RETURN_IF( l_previous != m_listPtr->tail, LST_CLI_VAL_INVALID_PATH );
		RETURN_IF( l_countElements != m_listPtr->countElements, LST_CLI_VAL_INVALID_COUNT_ELEMENTS );

		return LST_CLI_VAL_OK;
	}
}//namespace mqsProvider
