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
#include <base/Identificable.h>
#include <logging/LogClient.h>
#include "mqsProvider/ListServer.h"
#include "mqsProvider/MessageQueueHeader.h"
#include "mqsProvider/MessageHeader.h"
#include "mqsProvider/MessageQueueServiceHeader.h"
#include "mqsProvider/TreeNode.h"
#include "mqsProvider/MutualExclusionClient.h"
#include "mqsProvider/MutualExclusionServer.h"

namespace mqsProvider
{
	class SharedMemoryAllocator;
	class MessageQueueServiceServer : public base::Identificable, public logging::LogClient
	{
	public:
		MessageQueueServiceServer(void);
		MessageQueueServiceServer(struct tagMessageQueueServiceHeader* a_mqsHeader);
		virtual ~MessageQueueServiceServer(void);

		void setSharedMemoryAllocator(mqsProvider::SharedMemoryAllocator* a_shmAllocator);
		void setMutualExclusionServer(mqsProvider::MutualExclusionServer* a_mutexServer);
		void setMutualExclusionClient(mqsProvider::MutualExclusionClient* a_mutexClient);

		void setGlobalMaxMessages( int a_globalMaxMessages );
		void setGlobalMessageClustersSize( unsigned int a_globalMessageClustersSize );
		void setGlobalMessageClustersQuantity( int a_globalMessageClustersQuantity );
		void setLimitMessagesPerQueue( int a_limitMessagesPerQueue );

		bool create(void);

		struct tagMessageQueueServiceHeader* header(void);

	private:
		struct tagMessageQueueServiceHeader*	m_mqsHeader;
		mqsProvider::SharedMemoryAllocator*		m_shmAllocator;
		mqsProvider::MutualExclusionClient*		m_mutexClient;
		mqsProvider::MutualExclusionServer*		m_mutexServer;

		mqsProvider::ListServer m_listServerMessageHeaders;
		mqsProvider::ListServer m_listServerMessageClusters;
		mqsProvider::ListServer m_listServerMessageQueues;
		mqsProvider::ListServer m_listServerTreeNodes;

		int m_maxMessageHeaders					;
		unsigned int m_messageHeaderSize		;
		int m_maxMessageClusters				;
		unsigned int m_messageClusterSize		;
		int m_maxMessageQueues					;
		unsigned int m_messageQueueSize			;
		int m_maxTreeNodes						;
		unsigned int m_treeNodeSize				;
		
		unsigned int m_messageQueueServiceHeaderOffset	;
		unsigned int m_messageHeadersOffset				;
		unsigned int m_messageClustersOffset			;
		unsigned int m_messageQueuesOffset				;
		unsigned int m_treeNodesOffset					;

		static const unsigned int DEFAULT_MAX_MESSAGE_HEADERS		= 1024;
		static const unsigned int DEFAULT_MAX_MESSAGE_BUFFERS		= 4096;
		static const unsigned int DEFAULT_MESSAGE_BUFFER_SIZE		= 256;
		static const unsigned int DEFAULT_MAX_MESSAGE_QUEUES		= 8;
		static const unsigned int MESSAGE_QUEUE_SIZE			= sizeof( mqsProvider::MessageQueueHeader );
		static const unsigned int MESSAGE_HEADER_SIZE			= sizeof( mqsProvider::MessageHeader );
		static const unsigned int TREE_NODE_SIZE				= sizeof( mqsProvider::TreeNode );
		static const unsigned int MESSAGE_QUEUE_SERVICE_SIZE	= sizeof( mqsProvider::MessageQueueServiceHeader ) - sizeof(char);

		bool m_openSuccess;

		void setupLists(void);
		unsigned int calculateTotalSharedMemorySizeAndOffset(void);
		void setupMemoryOffset(char* a_baseAddr);
		void initialiseHeader(void);
		void setupListsHeaders(void);
		bool createLists(void);
	};
}//namespace mqsProvider
