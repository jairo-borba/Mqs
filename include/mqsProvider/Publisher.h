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

namespace mqsProvider
{
	class MessageQueueServiceClient;
	class MessageQueueClient;
	class MutualExclusionClient;

	class Publisher
	{
	public:
		enum SEND_STATUS
		{
			SND_FATAL_ERROR				=	-1,
			SND_EASY_SUCCESS			=	0,
			SND_ON_THROTLING_SUCCESS	=	1,
			SND_MISSING_HEADER			=	2,
			SND_MISSING_TREE_NODE		=	3,
			SND_MISSING_CLUSTER			=	4,
			SND_INPUT_OFF				=	5,
			SND_MSGQUEUE_DESTROYED		=	6,
			SND_SYSTEM_DOWN				=	7,
			SND_COUNT_LIMIT				=	8
		};

		Publisher(
			MessageQueueServiceClient* a_messageQueueServiceClient,
			MutualExclusionClient* a_mutexClient);
		virtual ~Publisher(void);

		bool open(
				const char* a_messageQueueName );
		void close(void);

		const Publisher& identifyMessage(
				const char* a_messageIdentification ) const;
		enum SEND_STATUS sendf(
				const char* a_format,
				... ) const;
		enum SEND_STATUS send(
				const char* a_message,
				int a_messageSize ) const;

		void setNumMaxRetriesToSend(
				unsigned int a_numMaxTriesToSend );
		void setRetryDelay(
				unsigned int a_milisecondsRetryDelay );
		void setMoreAllocResourcesQty(
				unsigned short int a_moreAllocResourcesQty );

	private:
		mutable MessageQueueServiceClient* m_messageQueueServiceClient;
		mutable MessageQueueClient* m_messageQueueClient;
		mutable MutualExclusionClient* m_mutexClient;
		unsigned int m_numMaxTriesToSend;
		unsigned int m_retryDelay;
		unsigned short int m_moreAllocResourcesQty;
		mutable void* m_messageIdentification;

		enum TRY_SEND_STATUS
		{
			TS_FATAL_ERROR			= -1,
			TS_SUCCESS				=  0,
			TS_MUTEX_TIMEOUT		=  1,
			TS_MISSING_HEADER		=  2,
			TS_MISSING_TREE_NODE	=  4,
			TS_MISSING_CLUSTER		=  5,
			TS_INPUT_OFF			=  6,
			TS_MSGQUEUE_DESTROYED	=  7,
			TS_SYSTEM_DOWN			=  8,
			TS_COUNT_LIMIT			=  9
		};
		enum MORE_RESOURCES_STATUS
		{
			MRS_SUCCESS,
			MRS_NO_MORE,
			MRS_FATAL_ERROR
		};
		enum TRY_SEND_STATUS trySend( const char* a_message, int a_messageSize ) const;
		enum MORE_RESOURCES_STATUS moreHeaders(void) const;
		enum MORE_RESOURCES_STATUS moreTreeNodes(void) const;
		enum MORE_RESOURCES_STATUS moreBuffers(void) const;
		char m_msgqCreationDate[64];
	};
}//namespace mqsProvider
