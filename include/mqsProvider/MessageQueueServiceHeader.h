/*
 * 
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 jairo-borba
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
#include "mqsProvider/ListPtr.h"
#include "mqsProvider/TreePtr.h"
#include <base/ExceptionInfo.h>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <appCore/SafeStringDef.h>
#include <base/Identificable.h>
#include "mqsProvider/date_t.h"
#include <appCore/Shortcuts.h>

namespace mqsProvider
{
	static const unsigned int MSGQSHDR_MAX_NAME_SIZE						= 64;
	static const unsigned int MSGQSHDR_MAX_DESCRIPTION_SIZE					= 256;
	static const char MSGQSHDR_MAGIC_CODE[]									= "AVL@GNSMSGQSHDR";
	static const unsigned short GLOBAL_LISTS_SEMAPHORE_NUMBER				= 0;
	static const unsigned int DEFAULT_COUNT_START_RESOURCES					= 32;
	static const unsigned int DEFAULT_DESTROY_MESSAGE_QUEUE_WAIT_SECONDS	= 60;
	static const unsigned int PATH_LENGTH									= 256;

	enum START_BUFFER_POSITION_FOR_LISTS
	{
		POS_FOR_MESSAGE_HEADERS,
		POS_FOR_MESSAGE_BUFFERS,
		POS_FOR_MESSAGE_QUEUES,
		POS_FOR_TREE_NODES,
	};
	enum MQS_STATUS
	{
		ERROR		= -1,
		OPENING		=  0,
		ACTIVE		=  1,
		CLOSING		=  2,
		CLOSED		=  3,
	};
	typedef struct tagParameters{
		unsigned int countStartResources;
		unsigned int destroyMessageQueueWaitSeconds;
		char droppedMessagesFilePath[PATH_LENGTH];
	} Parameters;
	typedef struct tagMessageQueueServiceHeader
	{
		char magicCode[sizeof MSGQSHDR_MAGIC_CODE];
		int internalId;
		char name[MSGQSHDR_MAX_NAME_SIZE+1];
		char description[MSGQSHDR_MAX_DESCRIPTION_SIZE+1];
		date_t creationDate;

		TreePtr busyMessageQueues		;
		ListHeader listMessageQueues	;

		ListHeader listMessageHeaders	;
		ListHeader listMessageClusters	;
		ListHeader listTreeNodes		;

		Parameters parameters;

		enum MQS_STATUS status;

		unsigned int buffersStartPositionForEachList[4];

		char mqsOperationCluster[1];
	} MessageQueueServiceHeader;

	inline bool VALIDATE_MESSAGE_QUEUE_SERVICE_HEADER( MessageQueueServiceHeader* l_header ){
		int l_cmpRet = memcmp( l_header->magicCode, MSGQSHDR_MAGIC_CODE, sizeof MSGQSHDR_MAGIC_CODE );
		return l_cmpRet == 0;
	}
	inline MessageQueueServiceHeader* MSG_QUEUE_SERVICE_HEADER(void*a_ptrheader){
		MessageQueueServiceHeader* l_header = reinterpret_cast<MessageQueueServiceHeader*>(a_ptrheader);

		RETURN_IF( !VALIDATE_MESSAGE_QUEUE_SERVICE_HEADER( l_header ), 0 );

		return l_header;
	}
	
	inline void INIT_MSG_QUEUE_SERVICE_HEADER( 
		MessageQueueServiceHeader* a_msqHeader,
		const base::Identificable& a_identification ){

		memcpy( a_msqHeader->magicCode, MSGQSHDR_MAGIC_CODE, sizeof a_msqHeader->magicCode );
		
		a_msqHeader->internalId = a_identification.id();
		safeSPrintf( a_msqHeader->name, sizeof a_msqHeader->name, "%s" , a_identification.name().c_str() );
		safeSPrintf( 
			a_msqHeader->description, 
			sizeof a_msqHeader->description, "%s",
			a_identification.description().c_str() );
		
		LOAD_SYSDATE( a_msqHeader->creationDate );
		TREEPTR_INIT(&a_msqHeader->busyMessageQueues);
		INIT_LIST_HEADER( &a_msqHeader->listMessageHeaders, a_identification );
		INIT_LIST_HEADER( &a_msqHeader->listMessageClusters, a_identification );
		INIT_LIST_HEADER( &a_msqHeader->listMessageQueues, a_identification );
		INIT_LIST_HEADER( &a_msqHeader->listTreeNodes, a_identification );
		a_msqHeader->parameters.countStartResources = DEFAULT_COUNT_START_RESOURCES;
		a_msqHeader->parameters.destroyMessageQueueWaitSeconds = DEFAULT_DESTROY_MESSAGE_QUEUE_WAIT_SECONDS;
		a_msqHeader->status = OPENING;
		memset(
			a_msqHeader->buffersStartPositionForEachList, 
			0, 
			sizeof a_msqHeader->buffersStartPositionForEachList 
		);
	}
}//namespace mqsProvider
