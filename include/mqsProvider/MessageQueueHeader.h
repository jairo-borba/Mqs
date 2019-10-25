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
#pragma once
#include "mqsProvider/ListHeader.h"
#include "mqsProvider/TreePtr.h"
#include "mqsProvider/ListPtr.h"
#include <cstring>
#include "mqsProvider/Cluster.h"
#include "mqsProvider/date_t.h"

namespace mqsProvider
{
	static const unsigned int MSGQHDR_MAX_NAME_SIZE			= 64;
	static const unsigned int MSGQHDR_MAX_DESCRIPTION_SIZE	= 256;
	static const char MSGQHDR_MAGIC_CODE[] = "MQS@JJJMSGQHDR";

	typedef struct tagMessageQueueHeader
	{
		char magicCode[sizeof MSGQHDR_MAGIC_CODE];
		char name[MSGQHDR_MAX_NAME_SIZE+1];
		date_t creationDate;
		bool inputOn;
		bool outputOn;
		bool open;
		unsigned int countLimit;
		unsigned int timeToLive;

		unsigned short int semaphoreNumber;

		ListPtr availableHeaders	;
		ListPtr availableClusters	;
		ListPtr availableTreeNodes	;
		ListPtr busyHeaders			;
		TreePtr busyHeadersIndex	;

	} MessageQueueHeader;

	inline MessageQueueHeader* MSG_QUEUE_HEADER(
			void*buffer,
			unsigned int position)
	{
		return reinterpret_cast<MessageQueueHeader*>(CLUSTER_DATA(buffer,position));
	}

	inline void INIT_CLOSED_MSG_QUEUE_HEADER(
			MessageQueueHeader* a_msgQueueHeader)
	{
		
		memset( a_msgQueueHeader, 0, sizeof( MessageQueueHeader ) );
		memcpy(
				a_msgQueueHeader->magicCode,
				MSGQHDR_MAGIC_CODE,
				sizeof a_msgQueueHeader->magicCode );
		a_msgQueueHeader->inputOn		= false;
		a_msgQueueHeader->outputOn		= false;
		a_msgQueueHeader->open			= false;
		a_msgQueueHeader->countLimit	= 0;
		a_msgQueueHeader->timeToLive	= 0;
				
		TREEPTR_INIT(&a_msgQueueHeader->busyHeadersIndex);
		LISTPTR_INIT(&a_msgQueueHeader->busyHeaders);
		LISTPTR_INIT(&a_msgQueueHeader->availableHeaders);
		LISTPTR_INIT(&a_msgQueueHeader->availableClusters);
		LISTPTR_INIT(&a_msgQueueHeader->availableTreeNodes);
	}

	inline void INIT_MSG_QUEUE_HEADER(
			MessageQueueHeader* a_msgQueueHeader,
			const char* a_name,
			unsigned short int a_semaphoreNumber,
			unsigned int a_countLimit,
			unsigned int a_timeToLive )
	{
		
		memset( a_msgQueueHeader, 0, sizeof( MessageQueueHeader ) );
		
		memcpy( a_msgQueueHeader->magicCode, MSGQHDR_MAGIC_CODE, sizeof a_msgQueueHeader->magicCode );
		safeSPrintf( a_msgQueueHeader->name, sizeof a_msgQueueHeader->name, "%s" , a_name );

		a_msgQueueHeader->inputOn		= true;
		a_msgQueueHeader->outputOn		= true;
		a_msgQueueHeader->open			= false;
		a_msgQueueHeader->countLimit	= a_countLimit;
		a_msgQueueHeader->timeToLive	= a_timeToLive;
		a_msgQueueHeader->semaphoreNumber = a_semaphoreNumber;
		LOAD_SYSDATE( a_msgQueueHeader->creationDate );
		TREEPTR_INIT(&a_msgQueueHeader->busyHeadersIndex);
		LISTPTR_INIT(&a_msgQueueHeader->busyHeaders);
		LISTPTR_INIT(&a_msgQueueHeader->availableHeaders);
		LISTPTR_INIT(&a_msgQueueHeader->availableClusters);
		LISTPTR_INIT(&a_msgQueueHeader->availableTreeNodes);
	}

	inline void INIT_MSG_QUEUE_HEADER(
			void* buffer,
			unsigned int position)
	{
		memset( MSG_QUEUE_HEADER(buffer,position), 0, sizeof( MessageQueueHeader ) );
		MSG_QUEUE_HEADER(buffer,position)->inputOn		= true;
		MSG_QUEUE_HEADER(buffer,position)->outputOn		= true;
		MSG_QUEUE_HEADER(buffer,position)->open			= false;
		MSG_QUEUE_HEADER(buffer,position)->countLimit	= 0;
		MSG_QUEUE_HEADER(buffer,position)->timeToLive	= 0;
		LOAD_SYSDATE( MSG_QUEUE_HEADER(buffer,position)->creationDate );
		TREEPTR_INIT(&MSG_QUEUE_HEADER(buffer,position)->busyHeadersIndex);
		LISTPTR_INIT(&MSG_QUEUE_HEADER(buffer,position)->busyHeaders);
		LISTPTR_INIT(&MSG_QUEUE_HEADER(buffer,position)->availableHeaders);
		LISTPTR_INIT(&MSG_QUEUE_HEADER(buffer,position)->availableClusters);
		LISTPTR_INIT(&MSG_QUEUE_HEADER(buffer,position)->availableTreeNodes);
	}
}//namespace mqsProvider

	


