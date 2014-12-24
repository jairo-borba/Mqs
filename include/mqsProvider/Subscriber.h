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

namespace mqsProvider
{
	class MessageQueueServiceClient;
	class MessageQueueClient;
	class MutualExclusionClient;

	class Subscriber
	{
	public:
		enum RECEIVE_STATUS
		{
			RECV_FATAL_ERROR		=	-1,
			RECV_SUCCESS			=	0,
			RECV_MSGQUEUE_DESTROYED	=	1,
			RECV_TIMEOUT			=	2,
			RECV_NO_MESSAGE			=	3,
			RECV_SYSTEM_DOWN		=	4,
		};

		Subscriber( MessageQueueServiceClient* a_messageQueueServiceClient,
			MutualExclusionClient* a_mutexClient );
		virtual ~Subscriber(void);

		bool open( const char* a_messageQueueName );
		void close(void);
		enum RECEIVE_STATUS receive(void) const;
		const char* message(void) const;
		unsigned int messageSize(void) const;
		void setTimeout( unsigned int a_milisecondsTimeout );
		const Subscriber& identifyMessage( const char* a_messageIdentification ) const;
		unsigned int elapsedTime(void) const;

	private:
		mutable MessageQueueServiceClient* m_messageQueueServiceClient;
		mutable MessageQueueClient* m_messageQueueClient;
		mutable MutualExclusionClient* m_mutexClient;
		mutable void* m_message;
		mutable void* m_messageIdentification;
		mutable unsigned int m_elapsedTime;

		enum RECEIVE_STATUS tryReceive(void) const;

		unsigned int m_milisecondsTimeout;
		char m_msgqCreationDate[64];
	};
}//namespace mqsProvider
