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

#include "mqsProvider/MessageQueueClient.h"
#include "mqsProvider/ListClient.h"
#include <base/ExceptionInfo.h>
#include "mqsProvider/ListClient.h"
#include "mqsProvider/TreeClient.h"
#include "mqsProvider/MessageHeader.h"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <appCore/SafeStringDef.h>
#include <appCore/Shortcuts.h>
#include "mqsProvider/Cluster.h"

namespace mqsProvider
{
	MessageQueueClient::MessageQueueClient(void)
	{
		m_clusterDataSize = 0;
		appCore::initPointer( m_msgQueueHeader );
		appCore::initPointer( m_messageHeaderBuffer );
		appCore::initPointer( m_messageTreeNodeBuffer );
		appCore::initPointer( m_messageBuffer );
	}
	MessageQueueClient::~MessageQueueClient(void)
	{
	}
	void MessageQueueClient::setMessageQueueHeader( MessageQueueHeader* a_msgQueueHeader )
	{
		m_msgQueueHeader = a_msgQueueHeader;
		
		appCore::appAssertPointer( m_msgQueueHeader );
		
		int l_cmpRet = memcmp( m_msgQueueHeader->magicCode, MSGQHDR_MAGIC_CODE,  sizeof MSGQHDR_MAGIC_CODE );

		appCore::appAssert( l_cmpRet == 0, "Invalid Message Queue Header Pointer" );
	}
	unsigned short int MessageQueueClient::semaphoreNumber(void) const
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->semaphoreNumber;
	}
	void MessageQueueClient::setMessageClusterDataSize( unsigned int a_clusterDataSize )
	{
		m_clusterDataSize = a_clusterDataSize;
	}
	void MessageQueueClient::setMessageHeaderBuffer( void* a_messageHeaderBuffer )
	{
		m_messageHeaderBuffer = a_messageHeaderBuffer;
	}
	void MessageQueueClient::setMessageTreeNodeBuffer( void* a_messageTreeNodeBuffer )
	{
		m_messageTreeNodeBuffer = a_messageTreeNodeBuffer;
	}
	void MessageQueueClient::setMessageBuffer( void* a_messageBuffer )
	{
		m_messageBuffer = a_messageBuffer;
	}
	bool MessageQueueClient::addMessageHeaderCluster( unsigned int a_messageHeaderCluster )
	{
		ListClient l_listCli( &m_msgQueueHeader->availableHeaders, m_messageHeaderBuffer );
		bool l_ret = l_listCli.enqueue( a_messageHeaderCluster );

		return l_ret;
	}
	bool MessageQueueClient::addMessageTreeNodeCluster( unsigned int a_messageTreeNodeCluster )
	{
		ListClient l_listCli(&m_msgQueueHeader->availableTreeNodes,m_messageTreeNodeBuffer);
		bool l_ret = l_listCli.enqueue( a_messageTreeNodeCluster );

		return l_ret;
	}
	bool MessageQueueClient::addMessageCluster( unsigned int a_messageCluster )
	{
		ListClient l_listCli( &m_msgQueueHeader->availableClusters, m_messageBuffer );
		bool l_ret = l_listCli.enqueue( a_messageCluster );

		return l_ret;
	}
	std::string MessageQueueClient::creationDate(void) const
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->creationDate;
	}
	unsigned int MessageQueueClient::releaseHeaderCluster(void)
	{
		ListClient l_listCli( &m_msgQueueHeader->availableHeaders, m_messageHeaderBuffer );
		unsigned int l_released = l_listCli.dequeue();
		return l_released;
	}
	unsigned int MessageQueueClient::releaseTreeNodeCluster(void)
	{
		ListClient l_listCli(&m_msgQueueHeader->availableTreeNodes,m_messageTreeNodeBuffer);
		unsigned int l_released = l_listCli.dequeue();
		return l_released;
	}
	unsigned int MessageQueueClient::releaseCluster(void)
	{
		ListClient l_listCli( &m_msgQueueHeader->availableClusters, m_messageBuffer );
		unsigned int l_released = l_listCli.dequeue();
		return l_released;
	}

	enum MessageQueueClient::SEND_STATUS MessageQueueClient::sendMessage( const std::string& a_messageId, const std::string& a_message )
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		RETURN_IF( !m_msgQueueHeader->inputOn, INPUT_OFF );

		ListClient l_cliAvailHeaders	( &m_msgQueueHeader->availableHeaders,		m_messageHeaderBuffer	);
		ListClient l_cliAvailTreeNodes	( &m_msgQueueHeader->availableTreeNodes,	m_messageTreeNodeBuffer	);
		TreeClient l_cliIndexBusyHeaders( &m_msgQueueHeader->busyHeadersIndex	, m_messageTreeNodeBuffer	);
		ListClient l_cliBusyHeaders		( &m_msgQueueHeader->busyHeaders		, m_messageHeaderBuffer		);

		unsigned int l_countLimit = m_msgQueueHeader->countLimit;
		RETURN_IF( l_countLimit > 0 && l_countLimit <= l_cliBusyHeaders.count(), COUNT_LIMIT );
		
		unsigned int l_header = l_cliAvailHeaders.head();
		RETURN_IF( l_header == INDEX_NULL, MISSING_HEADER );
		unsigned int l_treeNodeCluster = l_cliAvailTreeNodes.head();
		RETURN_IF( l_treeNodeCluster == INDEX_NULL, MISSING_TREE_NODE );
				
		INIT_MSG_HEADER( m_messageHeaderBuffer, l_header );
		MessageHeader* l_pMessage = MSG_HEADER(m_messageHeaderBuffer,l_header);
		l_pMessage->index = l_treeNodeCluster;
		l_pMessage->messageSize = static_cast<unsigned int>( a_message.size() );
		ListClient l_msgClusterFreeList( &m_msgQueueHeader->availableClusters,	m_messageBuffer	);
		ListClient l_msgClusterBusyList( &l_pMessage->messageClusters,			m_messageBuffer	);

		unsigned int l_remaingData		= l_pMessage->messageSize;
		unsigned int l_startPos			= 0;
		unsigned int l_bufferMessage	= INDEX_NULL;
		unsigned int l_countClusters	= 0;

		while( l_remaingData > 0 ){
			
			l_bufferMessage = l_msgClusterFreeList.dequeue();
			if( l_bufferMessage == INDEX_NULL ){
				unsigned int sl_rollback = l_msgClusterBusyList.dequeue();
				while( sl_rollback != INDEX_NULL ){
					l_msgClusterFreeList.enqueue( sl_rollback );
					sl_rollback = l_msgClusterBusyList.dequeue();
				}
				return MISSING_BUFFER;
			}
			++l_countClusters;
			l_msgClusterBusyList.enqueue( l_bufferMessage );
			
			if( m_clusterDataSize < l_remaingData ){
				memcpy( CLUSTER_DATA(m_messageBuffer, l_bufferMessage), &a_message.data()[l_startPos], m_clusterDataSize );
				l_remaingData	-= m_clusterDataSize;
				l_startPos		+= m_clusterDataSize;
			} else {
				memcpy( CLUSTER_DATA(m_messageBuffer, l_bufferMessage), &a_message.data()[l_startPos], l_remaingData );
				memset( &CLUSTER_DATA(m_messageBuffer, l_bufferMessage)[l_remaingData], 0, m_clusterDataSize - l_remaingData );
				l_remaingData = 0;
			}		
		}
		
		trn_key_t l_key;
		LOAD_TREE_NODE_KEY( l_key, a_messageId.c_str() );
		unsigned int l_attach = l_header;
		l_cliIndexBusyHeaders.insert( l_treeNodeCluster, l_key, l_attach );
		l_cliBusyHeaders.enqueue( l_cliAvailHeaders.dequeue() );
		l_cliAvailTreeNodes.dequeue();

		return SUCCESS;
	}
	ListPtr& MessageQueueClient::availableHeaders(void)
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->availableHeaders;
	}
	ListPtr& MessageQueueClient::availableClusters(void)
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->availableClusters;
	}
	ListPtr& MessageQueueClient::availableTreeNodes(void)
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->availableTreeNodes;
	}
	ListPtr& MessageQueueClient::busyHeaders(void){
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->busyHeaders;
	}
	TreePtr& MessageQueueClient::busyHeadersIndex(void){
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->busyHeadersIndex;
	}
	bool MessageQueueClient::inputOn(void) const
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->inputOn;
	}
	bool MessageQueueClient::outputOn(void) const
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		return m_msgQueueHeader->outputOn;
	}
	void MessageQueueClient::enableInput( bool a_enable )
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		m_msgQueueHeader->inputOn = a_enable;
	}
	void MessageQueueClient::enableOutput( bool a_enable )
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		m_msgQueueHeader->outputOn = a_enable;
	}
	unsigned int MessageQueueClient::countMessages(void) const
	{
		ListClient l_cliBusyHeaders( &m_msgQueueHeader->busyHeaders, m_messageHeaderBuffer );
		unsigned int l_countMessages = l_cliBusyHeaders.count();
		return l_countMessages;
	}
	bool MessageQueueClient::receiveMessage( const std::string& a_messageId, std::string& a_message, date_t& a_messageDate )
	{
		appCore::appAssertPointer( m_msgQueueHeader );

		a_message.clear();

		RETURN_IF( !m_msgQueueHeader->outputOn, false );

		ListClient l_cliAvailHeaders	( &m_msgQueueHeader->availableHeaders	,m_messageHeaderBuffer		);
		ListClient l_cliAvailTreeNodes	( &m_msgQueueHeader->availableTreeNodes	,m_messageTreeNodeBuffer	);
		ListClient l_cliAvailClusters	( &m_msgQueueHeader->availableClusters	,m_messageBuffer			);
		ListClient l_cliBusyHeaders		( &m_msgQueueHeader->busyHeaders		,m_messageHeaderBuffer		);
		TreeClient l_cliIndexBusyHeaders( &m_msgQueueHeader->busyHeadersIndex	,m_messageTreeNodeBuffer	);

		unsigned int l_treeNodeHeader = INDEX_NULL;
		if( a_messageId.length() == 0 ){
			unsigned int sl_head = l_cliBusyHeaders.head();
			RETURN_IF( sl_head == INDEX_NULL, false );
			l_treeNodeHeader = MSG_HEADER( m_messageHeaderBuffer, sl_head )->index;
		} else {
			l_treeNodeHeader = l_cliIndexBusyHeaders.find( a_messageId.c_str() );
			RETURN_IF( l_treeNodeHeader == INDEX_NULL, false );
		}

		TreeNode* l_pMsgQueue = TREE_NODE( m_messageTreeNodeBuffer, l_treeNodeHeader );
		unsigned int l_header = l_pMsgQueue->attach;

		MessageHeader* l_messageHeader = MSG_HEADER( m_messageHeaderBuffer, l_header );
		ListClient l_cliBusyClusters( &l_messageHeader->messageClusters, m_messageBuffer );

		DATECOPY( a_messageDate, l_messageHeader->creationDate );
		
		unsigned int l_remaining = l_messageHeader->messageSize;
		unsigned int l_bufferMessage = l_cliBusyClusters.dequeue();
		while( l_bufferMessage != INDEX_NULL ){
			a_message.append( CLUSTER_DATA(m_messageBuffer, l_bufferMessage), l_remaining < m_clusterDataSize ? l_remaining : m_clusterDataSize );
			memset( CLUSTER_DATA(m_messageBuffer, l_bufferMessage), 0, m_clusterDataSize );
			l_cliAvailClusters.enqueue( l_bufferMessage );
			l_bufferMessage = l_cliBusyClusters.dequeue();
			l_remaining = l_remaining < m_clusterDataSize ? 0 : l_remaining - m_clusterDataSize;
		}

		l_cliBusyHeaders.remove		( l_header );
		l_cliIndexBusyHeaders.remove( l_treeNodeHeader );
		l_cliAvailTreeNodes.enqueue	( l_treeNodeHeader );
		l_cliAvailHeaders.enqueue	( l_header );
		return true;
	}
	/////////
	bool MessageQueueClient::dropExpiredMessage( std::string& a_message )
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		a_message.clear();

		RETURN_IF( !m_msgQueueHeader->outputOn, false );

		ListClient l_cliAvailHeaders	( &m_msgQueueHeader->availableHeaders	,m_messageHeaderBuffer		);
		ListClient l_cliAvailTreeNodes	( &m_msgQueueHeader->availableTreeNodes	,m_messageTreeNodeBuffer	);
		ListClient l_cliAvailClusters	( &m_msgQueueHeader->availableClusters	,m_messageBuffer			);
		ListClient l_cliBusyHeaders		( &m_msgQueueHeader->busyHeaders		,m_messageHeaderBuffer		);
		TreeClient l_cliIndexBusyHeaders( &m_msgQueueHeader->busyHeadersIndex	,m_messageTreeNodeBuffer	);

		unsigned int sl_head = l_cliBusyHeaders.head();
		RETURN_IF( sl_head == INDEX_NULL, false );
		unsigned int l_treeNodeHeader = MSG_HEADER( m_messageHeaderBuffer, sl_head )->index;
	
		TreeNode* l_pMsgQueue = TREE_NODE( m_messageTreeNodeBuffer, l_treeNodeHeader );
		unsigned int l_header = l_pMsgQueue->attach;

		MessageHeader* l_messageHeader = MSG_HEADER( m_messageHeaderBuffer, l_header );
		
		unsigned int l_timeToLive	= m_msgQueueHeader->timeToLive;
		int l_elapsedTime	= ELAPSEDTIME( l_messageHeader->creationDate );
		RETURN_IF( l_elapsedTime > 0 && (unsigned int)l_elapsedTime < l_timeToLive, false );
		
		ListClient l_cliBusyClusters( &l_messageHeader->messageClusters, m_messageBuffer );

		unsigned int l_remaining = l_messageHeader->messageSize;
		unsigned int l_bufferMessage = l_cliBusyClusters.dequeue();
		while( l_bufferMessage != INDEX_NULL ){
			a_message.append( CLUSTER_DATA(m_messageBuffer, l_bufferMessage), l_remaining < m_clusterDataSize ? l_remaining : m_clusterDataSize );
			memset( CLUSTER_DATA(m_messageBuffer, l_bufferMessage), 0, m_clusterDataSize );
			l_cliAvailClusters.enqueue( l_bufferMessage );
			l_bufferMessage = l_cliBusyClusters.dequeue();
			l_remaining = l_remaining < m_clusterDataSize ? 0 : l_remaining - m_clusterDataSize;
		}

		l_cliBusyHeaders.remove		( l_header );
		l_cliIndexBusyHeaders.remove( l_treeNodeHeader );
		l_cliAvailTreeNodes.enqueue	( l_treeNodeHeader );
		l_cliAvailHeaders.enqueue	( l_header );
		return true;
	}
}//namespace mqsProvider
