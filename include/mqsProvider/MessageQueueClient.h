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
#include <string>
#include "mqsProvider/MessageQueueHeader.h"
#include <vector>

namespace mqsProvider
{
	struct tagCluster;
	class MessageQueueClient : public base::Identificable
	{
	public:
		enum SEND_STATUS
		{
			SUCCESS,
			MISSING_HEADER,
			MISSING_TREE_NODE,
			MISSING_BUFFER,
			INPUT_OFF,
			COUNT_LIMIT
		};
				
		MessageQueueClient(void);
		virtual ~MessageQueueClient(void);

		void setMessageQueueHeader( MessageQueueHeader* a_msgQueueHeader );
		void setMessageClusterDataSize( unsigned int a_clusterDataSize );
		void setMessageHeaderBuffer( void* a_messageHeaderBuffer );
		void setMessageTreeNodeBuffer( void* a_messageTreeNodeBuffer );
		void setMessageBuffer( void* a_messageBuffer );

		enum SEND_STATUS sendMessage( const std::string& a_messageId, const std::string& a_message );
				
		bool receiveMessage( const std::string& a_messageId, std::string& a_message, date_t& a_messageDate );
		bool dropExpiredMessage( std::string& a_message );
		bool addMessageHeaderCluster( unsigned int a_messageHeaderCluster );
		bool addMessageTreeNodeCluster( unsigned int a_messageTreeNodeCluster );
		bool addMessageCluster( unsigned int a_messageCluster );
		unsigned int releaseHeaderCluster(void);
		unsigned int releaseTreeNodeCluster(void);
		unsigned int releaseCluster(void);
		unsigned int countMessages(void) const;

		ListPtr& availableHeaders(void);
		ListPtr& availableClusters(void);
		ListPtr& availableTreeNodes(void);
		ListPtr& busyHeaders(void);
		TreePtr& busyHeadersIndex(void);

		unsigned short int semaphoreNumber(void) const;
		bool inputOn(void) const;
		bool outputOn(void) const;
		void enableInput( bool a_enable );
		void enableOutput( bool a_enable );

		std::string creationDate(void) const;
		
	private:
		MessageQueueHeader* m_msgQueueHeader;
		void* m_messageHeaderBuffer;
		void* m_messageTreeNodeBuffer;
		void* m_messageBuffer;
		unsigned int m_clusterDataSize;
	};
}//namespace mqsProvider
