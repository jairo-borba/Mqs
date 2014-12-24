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
#include "mqsProvider/SharedMemoryAccess.h"
#include "mqsProvider/MessageQueueClient.h"

namespace mqsProvider
{
	struct tagMessageQueueServiceHeader;
	class MessageQueueServiceClient
	{
	public:
		MessageQueueServiceClient(void);
		virtual ~MessageQueueServiceClient(void);

		void setSharedMemoryAccess( SharedMemoryAccess* a_sharedMemoryAccess );
		bool open(void);
		void close(void);
		bool findMessageQueue(
			const std::string& a_messageQueueName,
			mqsProvider::MessageQueueClient* a_messageQueueClient );

		struct tagMessageQueueServiceHeader* mqsHeader(void);

		void* messageHeaderBuffer(void);
		void* messageClusterBuffer(void);
		void* messageQueueBuffer(void);
		void* treeNodeBuffer(void);

		bool createMessageQueue( const char* a_msgqName, unsigned int a_countLimit, unsigned int a_timeToLive );
		bool destroyMessageQueue( const char* a_msgqName );

	private:
		SharedMemoryAccess* m_sharedMemoryAccess;
		struct tagMessageQueueServiceHeader* m_mqsHeader;

		void* m_messageHeaderBuffer;
		void* m_messageClusterBuffer;
		void* m_messageQueueBuffer;
		void* m_treeNodeBuffer;
	};
}//namespace mqsProvider
