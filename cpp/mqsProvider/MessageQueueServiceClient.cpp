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
#include "mqsProvider/MessageQueueServiceClient.h"
#include "mqsProvider/TreeClient.h"
#include "mqsProvider/TreeNode.h"
#include "mqsProvider/ListClient.h"
#include "mqsProvider/MessageQueueServiceHeader.h"
#include "mqsProvider/MessageQueueServer.h"
#include <appUtil/JJJException.h>
#include <appUtil/Shortcuts.h>

namespace mqsProvider
{
	MessageQueueServiceClient::MessageQueueServiceClient(void)
	{
		appUtil::initPointer( m_sharedMemoryAccess );
		appUtil::initPointer( m_mqsHeader );
		appUtil::initPointer( m_messageHeaderBuffer );
		appUtil::initPointer( m_messageClusterBuffer );
		appUtil::initPointer( m_messageQueueBuffer );
		appUtil::initPointer( m_treeNodeBuffer );
	}
	MessageQueueServiceClient::~MessageQueueServiceClient(void)
	{
	}
	void* MessageQueueServiceClient::messageHeaderBuffer(void)
	{
		return m_messageHeaderBuffer;
	}
	void* MessageQueueServiceClient::messageClusterBuffer(void)
	{
		return m_messageClusterBuffer;
	}
	void* MessageQueueServiceClient::messageQueueBuffer(void)
	{
		return m_messageQueueBuffer;
	}
	void* MessageQueueServiceClient::treeNodeBuffer(void)
	{
		return m_treeNodeBuffer;
	}
	void  MessageQueueServiceClient::setSharedMemoryAccess(
			SharedMemoryAccess* a_sharedMemoryAccess )
	{
		m_sharedMemoryAccess = a_sharedMemoryAccess;
	}

	struct tagMessageQueueServiceHeader* MessageQueueServiceClient::mqsHeader(void)
	{
		RETURN_IF( !VALIDATE_MESSAGE_QUEUE_SERVICE_HEADER( m_mqsHeader ), 0 );
		return m_mqsHeader;
	}
	bool MessageQueueServiceClient::open(void)
	{
		appUtil::assert(
				m_sharedMemoryAccess->bind(),
				"Could not attach to shared memory" );
		
		void* l_sharedMemorytr = m_sharedMemoryAccess->sharedMemoryPtr();
		m_mqsHeader					= MSG_QUEUE_SERVICE_HEADER( l_sharedMemorytr );
		
		unsigned int l_offset = m_mqsHeader->buffersStartPositionForEachList[POS_FOR_MESSAGE_HEADERS];
		m_messageHeaderBuffer		= &m_mqsHeader->mqsOperationCluster[l_offset];

		l_offset = m_mqsHeader->buffersStartPositionForEachList[POS_FOR_MESSAGE_BUFFERS];
		m_messageClusterBuffer		= &m_mqsHeader->mqsOperationCluster[l_offset];

		l_offset = m_mqsHeader->buffersStartPositionForEachList[POS_FOR_MESSAGE_QUEUES];
		m_messageQueueBuffer		= &m_mqsHeader->mqsOperationCluster[l_offset];

		l_offset = m_mqsHeader->buffersStartPositionForEachList[POS_FOR_TREE_NODES];
		m_treeNodeBuffer			= &m_mqsHeader->mqsOperationCluster[l_offset];

		return true;
	}
	void MessageQueueServiceClient::close(void)
	{
		m_sharedMemoryAccess->detach();
	}
	bool MessageQueueServiceClient::findMessageQueue(
			const std::string& a_messageQueueName,
			MessageQueueClient* a_messageQueueClient )
	{
		appUtil::assertPointer(a_messageQueueClient,__FILE__,__LINE__);
		appUtil::assertPointer(m_mqsHeader,__FILE__,__LINE__);

		mqsProvider::TreeClient l_treeClient( &m_mqsHeader->busyMessageQueues, m_treeNodeBuffer );
		unsigned int l_msgQueue = l_treeClient.find( a_messageQueueName.c_str() );
		RETURN_IF( l_msgQueue == INDEX_NULL, false );
		mqsProvider::TreeNode* l_pMsgQueue = TREE_NODE(m_treeNodeBuffer, l_msgQueue );
		
		mqsProvider::MessageQueueHeader* l_msgQueueHeader =
			MSG_QUEUE_HEADER( m_messageQueueBuffer, l_pMsgQueue->attach );

		a_messageQueueClient->setMessageClusterDataSize( m_mqsHeader->listMessageClusters.clusterDataSize );
		a_messageQueueClient->setMessageQueueHeader( l_msgQueueHeader );
		a_messageQueueClient->setMessageBuffer( m_messageClusterBuffer );
		a_messageQueueClient->setMessageHeaderBuffer( m_messageHeaderBuffer );
		a_messageQueueClient->setMessageTreeNodeBuffer( m_treeNodeBuffer );

		return true;
	}
	bool MessageQueueServiceClient::destroyMessageQueue(
			const char* a_msgqName )
	{
		appUtil::assertPointer(m_mqsHeader);

		mqsProvider::TreeClient l_treeClient( &m_mqsHeader->busyMessageQueues, m_treeNodeBuffer );
		unsigned int l_msgQueue = l_treeClient.find( a_msgqName );
		RETURN_IF( l_msgQueue == INDEX_NULL, false );

		mqsProvider::TreeNode* l_pMsgQueue = TREE_NODE(m_treeNodeBuffer, l_msgQueue );
		
		///////////////////////////////
		mqsProvider::MessageQueueHeader* l_msgQueueHeader = MSG_QUEUE_HEADER( m_messageQueueBuffer, l_pMsgQueue->attach );

		ListClient l_availClusters		( &l_msgQueueHeader->availableClusters,		m_messageClusterBuffer );
		ListClient l_availHeaders		( &l_msgQueueHeader->availableHeaders,		m_messageHeaderBuffer );
		ListClient l_availTreeNodes		( &l_msgQueueHeader->availableTreeNodes,	m_treeNodeBuffer );
		ListClient l_busyHeaders		( &l_msgQueueHeader->busyHeaders,			m_messageHeaderBuffer );
		TreeClient l_busyHeadersIndex	( &l_msgQueueHeader->busyHeadersIndex,		m_treeNodeBuffer );

		ListClient l_globalHeaders		( &m_mqsHeader->listMessageHeaders.mainList, m_messageHeaderBuffer );
		ListClient l_globalClusters		( &m_mqsHeader->listMessageClusters.mainList, m_messageClusterBuffer );
		ListClient l_globalTreeNodes	( &m_mqsHeader->listTreeNodes.mainList, m_treeNodeBuffer );

		unsigned int l_resource = INDEX_NULL;
		while( (l_resource = l_availHeaders.dequeue()) != INDEX_NULL ){
			l_globalHeaders.enqueue( l_resource );
		}
		while( (l_resource = l_availClusters.dequeue()) != INDEX_NULL ){
			l_globalClusters.enqueue( l_resource );
		}
		while( (l_resource = l_availTreeNodes.dequeue()) != INDEX_NULL ){
			l_globalTreeNodes.enqueue( l_resource );
		}
		while( (l_resource = l_busyHeaders.dequeue()) != INDEX_NULL ){
			l_globalHeaders.enqueue( l_resource );
		}
		while( ( l_resource = l_busyHeadersIndex.root() ) != INDEX_NULL ){
			l_busyHeadersIndex.remove( l_resource );
			l_globalTreeNodes.enqueue( l_resource );
		}
		///////////////////////////////

		unsigned int l_messageQueue = l_pMsgQueue->attach;
		unsigned int l_treeNodeCluster = l_msgQueue;

		ListClient l_msgQueues( &m_mqsHeader->listMessageQueues.mainList, m_messageQueueBuffer );
		INIT_CLOSED_MSG_QUEUE_HEADER( l_msgQueueHeader );
		l_msgQueues.enqueue( l_messageQueue );
		l_treeClient.remove( l_treeNodeCluster );
		ListClient l_buffersTreeNodes( &m_mqsHeader->listTreeNodes.mainList, m_treeNodeBuffer );
		l_buffersTreeNodes.enqueue( l_treeNodeCluster );

		return true;
	}
	bool MessageQueueServiceClient::createMessageQueue(
			const char* a_msgqName,
			unsigned int a_countLimit,
			unsigned int a_timeToLive )
	{
		appUtil::assertPointer(m_mqsHeader);
		
		mqsProvider::ListClient l_msgQueues( &m_mqsHeader->listMessageQueues.mainList, m_messageQueueBuffer );
		unsigned short int l_semaphoreNumber = static_cast<unsigned short int>(m_mqsHeader->listMessageQueues.mainList.countElements);

		unsigned int l_messageQueue = l_msgQueues.head();
		RETURN_IF( l_messageQueue == INDEX_NULL, false );
		
		mqsProvider::MessageQueueHeader* l_msgQueueHeader = MSG_QUEUE_HEADER(m_messageQueueBuffer,l_messageQueue);

		MessageQueueServer l_msgQueueServer;
		l_msgQueueServer.setMessageQueueName( a_msgqName );
		l_msgQueueServer.setMessageQueueHeader( l_msgQueueHeader );
		l_msgQueueServer.setSemaphoreNumber( l_semaphoreNumber );
		l_msgQueueServer.setCountLimit( a_countLimit );
		l_msgQueueServer.setTimeToLive( a_timeToLive );
		
		bool l_bMqsCreate = l_msgQueueServer.create();
		RETURN_IF( !l_bMqsCreate, false );
				
		mqsProvider::ListClient l_buffersTreeNodes( &m_mqsHeader->listTreeNodes.mainList, m_treeNodeBuffer );

		unsigned int l_treeNodeCluster = l_buffersTreeNodes.head();
		RETURN_IF( l_treeNodeCluster == INDEX_NULL, false );
		
		trn_key_t l_key;
		LOAD_TREE_NODE_KEY( l_key, a_msgqName );
		unsigned int l_attach = l_messageQueue;
		mqsProvider::TreeClient l_treeClient(&m_mqsHeader->busyMessageQueues,m_treeNodeBuffer);
		
		bool l_insertRet = l_treeClient.insert( l_treeNodeCluster, l_key, l_attach );
		appUtil::assert( l_treeClient.scanValidation(), "Fatal error, TreeClient Object corrupted" );

		l_msgQueues.dequeue();
		l_buffersTreeNodes.dequeue();

		/////////////////////////////
		//INICIALIZA OS RECURSOS
		ListClient l_availClusters		( &l_msgQueueHeader->availableClusters,		m_messageClusterBuffer );
		ListClient l_availHeaders		( &l_msgQueueHeader->availableHeaders,		m_messageHeaderBuffer );
		ListClient l_availTreeNodes		( &l_msgQueueHeader->availableTreeNodes,	m_treeNodeBuffer );

		ListClient l_globalHeaders		( &m_mqsHeader->listMessageHeaders.mainList, m_messageHeaderBuffer );
		ListClient l_globalClusters		( &m_mqsHeader->listMessageClusters.mainList, m_messageClusterBuffer );
		ListClient l_globalTreeNodes	( &m_mqsHeader->listTreeNodes.mainList, m_treeNodeBuffer );

		unsigned int sl_moreResources = 0;
		unsigned int sl_resource = l_globalHeaders.dequeue();
		while( sl_resource != INDEX_NULL ){
			l_availHeaders.enqueue( sl_resource );
			BREAK_IF( ++sl_moreResources >= m_mqsHeader->parameters.countStartResources );
			sl_resource = l_globalHeaders.dequeue();
		}

		sl_moreResources = 0;
		sl_resource = l_globalTreeNodes.dequeue();
		while( sl_resource != INDEX_NULL ){
			l_availTreeNodes.enqueue( sl_resource );
			BREAK_IF( ++sl_moreResources >= m_mqsHeader->parameters.countStartResources );
			sl_resource = l_globalTreeNodes.dequeue();
		}

		sl_moreResources = 0;
		sl_resource = l_globalClusters.dequeue();
		while( sl_resource != INDEX_NULL ){
			l_globalClusters.enqueue( sl_resource );
			BREAK_IF( ++sl_moreResources >= m_mqsHeader->parameters.countStartResources );
			sl_resource = l_globalClusters.dequeue();
		}
		/////////////////////////////

		l_msgQueueHeader->open = true;
		
		return l_insertRet;
	}
}//namespace mqsProvider
