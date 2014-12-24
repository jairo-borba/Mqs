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
namespace mqsProvider
{
	class MessageQueueServiceClient;
	class MessageQueueClient;
	class MutualExclusionClient;

	class Controller
	{
	public:
		Controller( MessageQueueServiceClient* a_messageQueueServiceClient, MutualExclusionClient* a_mutexClient );
		virtual ~Controller(void);

		bool open(void);
		void close(void);

		bool createMessageQueue(  const char* a_msgqName, unsigned int a_countLimit, unsigned int a_timeToLive ) const;
		bool destroyMessageQueue( const char* a_msgqName ) const;
		
	private:
		mutable MessageQueueServiceClient* m_messageQueueServiceClient;
		mutable MutualExclusionClient* m_mutexClient;
		bool enableMessageQueueInput(MessageQueueClient* a_mqClient, bool a_enable) const;
		bool enableMessageQueueOutput(MessageQueueClient* a_mqClient, bool a_enable) const;

		void emptyMessageQueue(MessageQueueClient* a_mqClient) const;

		unsigned int countMessages(MessageQueueClient* a_mqClient) const;
	};
}//namespace mqsProvider
