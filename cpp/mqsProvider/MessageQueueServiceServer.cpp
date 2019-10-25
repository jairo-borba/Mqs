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
#include "mqsProvider/SharedMemoryAllocator.h"
#include "mqsProvider/MessageQueueServiceHeader.h"
#include "mqsProvider/MessageQueueServiceServer.h"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <appUtil/SafeStringDef.h>
#include "mqsProvider/ListClient.h"
#include "mqsProvider/MessageQueueServer.h"
#include "mqsProvider/MessageQueueClient.h"
#include "mqsProvider/TreeClient.h"
#include <appUtil/JJJException.h>
#include <appUtil/Shortcuts.h>

namespace mqsProvider
{
	MessageQueueServiceServer::MessageQueueServiceServer(void)
	{
		appUtil::initPointer( m_mqsHeader );
		appUtil::initPointer( m_shmAllocator );
		appUtil::initPointer( m_mutexClient );
		appUtil::initPointer( m_mutexServer );
		m_maxMessageHeaders					= DEFAULT_MAX_MESSAGE_HEADERS;
		m_messageHeaderSize					= sizeof( mqsProvider::MessageHeader );
		m_maxMessageClusters				= DEFAULT_MAX_MESSAGE_BUFFERS;
		m_messageClusterSize				= DEFAULT_MESSAGE_BUFFER_SIZE;
		m_maxMessageQueues					= DEFAULT_MAX_MESSAGE_QUEUES;
		m_messageQueueSize					= sizeof( mqsProvider::MessageQueueHeader );
		m_maxTreeNodes						= m_maxMessageHeaders + m_maxMessageQueues;
		m_treeNodeSize						= sizeof( mqsProvider::TreeNode );
		m_messageQueueServiceHeaderOffset	= 0;
		m_messageHeadersOffset				= 0;
		m_messageClustersOffset				= 0;
		m_messageQueuesOffset				= 0;
		m_treeNodesOffset					= 0;
		m_openSuccess						= false;
	}
	MessageQueueServiceServer::~MessageQueueServiceServer(void)
	{
	}
	struct tagMessageQueueServiceHeader* MessageQueueServiceServer::header(void)
	{
		return m_mqsHeader;
	}
	void MessageQueueServiceServer::setSharedMemoryAllocator(
			mqsProvider::SharedMemoryAllocator* a_shmAllocator)
	{
		m_shmAllocator = a_shmAllocator;
	}
	void MessageQueueServiceServer::setMutualExclusionClient(
			mqsProvider::MutualExclusionClient* a_mutexClient)
	{
		m_mutexClient = a_mutexClient;
	}
	void MessageQueueServiceServer::setMutualExclusionServer(
			mqsProvider::MutualExclusionServer* a_mutexServer)
	{
		m_mutexServer = a_mutexServer;
	}
	void MessageQueueServiceServer::setGlobalMaxMessages(
			int a_globalMaxMessages )
	{
		m_maxMessageHeaders = a_globalMaxMessages;
	}
	void MessageQueueServiceServer::setGlobalMessageClustersSize(
			unsigned int a_globalMessageClustersSize )
	{
		m_messageClusterSize = a_globalMessageClustersSize;
	}
	void MessageQueueServiceServer::setGlobalMessageClustersQuantity(
			int a_globalMessageClustersQuantity )
	{
		m_maxMessageClusters = a_globalMessageClustersQuantity;
	}
	
	bool MessageQueueServiceServer::create(void)
	{
		appUtil::assertPointer( m_mutexServer );
		appUtil::assertPointer( m_mutexClient );
		appUtil::assertPointer( m_shmAllocator );
		
		this->setupLists();
		//Um semaforo para casa message queue + um semaforo para acessar as listas globais.
		m_mutexServer->setNumOfSemaphores( m_maxMessageQueues + 1 );
		appUtil::assert( m_mutexServer->create(), "Could not create semaphore system" );
		appUtil::assert( m_mutexClient->open(), "MessageQueueServiceServer::create: Error opening Mutex" );
		unsigned int l_shmRequiredSize = this->calculateTotalSharedMemorySizeAndOffset();

		bool l_bMutexRet = m_mutexClient->setMutex( GLOBAL_LISTS_SEMAPHORE_NUMBER, false );
		appUtil::assert( l_bMutexRet, "MessageQueueServiceServer::create: Could not set mutex" );

		bool l_shmCreate = this->m_shmAllocator->allocate( l_shmRequiredSize );
		if( !l_shmCreate ){
			m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
			throw appUtil::JJJException( "Could not allocate shared memory space" );
		}

		char* l_shmPtr = (char*)this->m_shmAllocator->sharedMemoryPtr();

		this->setupMemoryOffset( l_shmPtr );
		this->initialiseHeader();
		this->setupListsHeaders();

		bool l_createListsRet	= this->createLists();
		m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		m_openSuccess			= l_createListsRet;

		m_mqsHeader->status = l_createListsRet ? ACTIVE : ERROR;

		return l_createListsRet;
	}

	void MessageQueueServiceServer::initialiseHeader(void)
	{
		INIT_MSG_QUEUE_SERVICE_HEADER(m_mqsHeader, *this);

		m_mqsHeader->status = OPENING;
		
		m_mqsHeader->buffersStartPositionForEachList[POS_FOR_MESSAGE_HEADERS]	= m_messageHeadersOffset;
		m_mqsHeader->buffersStartPositionForEachList[POS_FOR_MESSAGE_BUFFERS]	= m_messageClustersOffset;
		m_mqsHeader->buffersStartPositionForEachList[POS_FOR_MESSAGE_QUEUES]	= m_messageQueuesOffset;
		m_mqsHeader->buffersStartPositionForEachList[POS_FOR_TREE_NODES]		= m_treeNodesOffset;
	}

	bool MessageQueueServiceServer::createLists(void)
	{
		RETURN_IF( !m_listServerMessageHeaders.create(),	false );
		RETURN_IF( !m_listServerMessageClusters.create(),	false );
		RETURN_IF( !m_listServerMessageQueues.create(),		false );
		RETURN_IF( !m_listServerTreeNodes.create(),			false );

		return true;
	}

	void MessageQueueServiceServer::setupListsHeaders(void)
	{
		m_listServerMessageHeaders	.setListHeader(&m_mqsHeader->listMessageHeaders);
		m_listServerMessageClusters	.setListHeader(&m_mqsHeader->listMessageClusters);
		m_listServerMessageQueues	.setListHeader(&m_mqsHeader->listMessageQueues);
		m_listServerTreeNodes		.setListHeader(&m_mqsHeader->listTreeNodes);
	}

	unsigned int MessageQueueServiceServer::calculateTotalSharedMemorySizeAndOffset(void)
	{
		unsigned int l_totalSize = 0;

		m_messageHeadersOffset = l_totalSize;
		l_totalSize += m_listServerMessageHeaders.bufferRequiredSize();
		m_messageClustersOffset = l_totalSize;
		l_totalSize += m_listServerMessageClusters.bufferRequiredSize();
		m_messageQueuesOffset = l_totalSize;
		l_totalSize += m_listServerMessageQueues.bufferRequiredSize();
		m_treeNodesOffset = l_totalSize;
		l_totalSize += m_listServerTreeNodes.bufferRequiredSize();	

		l_totalSize += MESSAGE_QUEUE_SERVICE_SIZE;

		return l_totalSize;
	}

	void MessageQueueServiceServer::setupMemoryOffset(
			char* a_baseAddr)
	{
		m_mqsHeader = reinterpret_cast<MessageQueueServiceHeader*>( a_baseAddr );
		
		m_listServerMessageHeaders.
			setBuffer(&m_mqsHeader->mqsOperationCluster[m_messageHeadersOffset]	, m_listServerMessageHeaders.bufferRequiredSize());
		m_listServerMessageClusters.
			setBuffer(&m_mqsHeader->mqsOperationCluster[m_messageClustersOffset]	, m_listServerMessageClusters.bufferRequiredSize());
		m_listServerMessageQueues.
			setBuffer(&m_mqsHeader->mqsOperationCluster[m_messageQueuesOffset]	, m_listServerMessageQueues.bufferRequiredSize());
		m_listServerTreeNodes.
			setBuffer(&m_mqsHeader->mqsOperationCluster[m_treeNodesOffset]	, m_listServerTreeNodes.bufferRequiredSize());
	}

	void MessageQueueServiceServer::setupLists(void)
	{
		m_maxTreeNodes = m_maxMessageHeaders + m_maxMessageQueues;

		m_listServerMessageHeaders.setClusterDataSize(m_messageHeaderSize);
		m_listServerMessageClusters.setClusterDataSize(m_messageClusterSize);
		m_listServerMessageQueues.setClusterDataSize(m_messageQueueSize);
		m_listServerTreeNodes.setClusterDataSize(m_treeNodeSize);

		m_listServerMessageHeaders.setNumOfClusters(m_maxMessageHeaders);
		m_listServerMessageClusters.setNumOfClusters(m_maxMessageClusters);
		m_listServerMessageQueues.setNumOfClusters(m_maxMessageQueues);
		m_listServerTreeNodes.setNumOfClusters(m_maxTreeNodes);
	}
}//namespace mqsProvider
