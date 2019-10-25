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
	class Subscriber;
	class Publisher;
	class SharedMemoryAccess;
	class MutualExclusionClient;
	class MessageQueueServiceClient;
	class Session
	{
	public:
		Session(void);
		virtual ~Session(void);

		bool connect(void);
		void disconnect(void);

		const Subscriber* attachSubscriber(
				const char* a_mqName );
		const Publisher*  attachPublisher(
				const char* a_mqName );
		bool isConnected(void);

	private:
		void* m_subscribers;
		void* m_publishers;
		bool  m_isConnected;

		SharedMemoryAccess*			m_shmAccess;
		MutualExclusionClient*		m_mtxClient;
		MessageQueueServiceClient*	m_mqsClient;
	};
}//namespace mqsProvider
