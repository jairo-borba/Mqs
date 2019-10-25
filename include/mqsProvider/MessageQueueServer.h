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
#include <string>

namespace mqsProvider
{
	struct tagMessageQueueHeader;
	class MessageQueueServer
	{
	public:
		MessageQueueServer(void);
		MessageQueueServer(
				struct tagMessageQueueHeader* a_msgQueueHeader);
		virtual ~MessageQueueServer(void);
		void setMessageQueueHeader(
				struct tagMessageQueueHeader* a_msgQueueHeader );
		void setSemaphoreNumber(
				unsigned short int a_semaphoreNumber );
		void setMessageQueueName(
				const char* a_msgqName );
		void setCountLimit(
				unsigned int a_countLimit );
		void setTimeToLive(
				unsigned int a_timeToLive );

		bool create(void);
	private:
		struct tagMessageQueueHeader* m_msgQueueHeader;
		unsigned short int m_semaphoreNumber;
		std::string m_msgqName;
		unsigned int m_countLimit;
		unsigned int m_timeToLive;
	};
}//namespace mqsProvider
