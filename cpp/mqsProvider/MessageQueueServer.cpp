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

#include "mqsProvider/MessageQueueServer.h"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <appCore/SafeStringDef.h>
#include <base/ExceptionInfo.h>
#include <appCore/Shortcuts.h>
#include "mqsProvider/MessageQueueHeader.h"
#include "mqsProvider/Cluster.h"

namespace mqsProvider
{
	MessageQueueServer::MessageQueueServer(void)
	{
		appCore::initPointer( m_msgQueueHeader );
		m_semaphoreNumber = 0;
		m_countLimit = 0;
		m_timeToLive = 0;
	}
	MessageQueueServer::MessageQueueServer(struct tagMessageQueueHeader* a_msgQueueHeader)
	{
		this->setMessageQueueHeader( a_msgQueueHeader );
	}
	MessageQueueServer::~MessageQueueServer(void)
	{
	}
	void MessageQueueServer::setCountLimit( unsigned int a_countLimit )
	{
		m_countLimit = a_countLimit;
	}
	void MessageQueueServer::setTimeToLive( unsigned int a_timeToLive )
	{
		m_timeToLive = a_timeToLive;
	}
	void MessageQueueServer::setMessageQueueHeader( struct tagMessageQueueHeader* a_msgQueueHeader )
	{
		m_msgQueueHeader = a_msgQueueHeader;
	}
	void MessageQueueServer::setSemaphoreNumber( unsigned short int a_semaphoreNumber )
	{
		m_semaphoreNumber = a_semaphoreNumber;
	}
	void MessageQueueServer::setMessageQueueName( const char* a_msgqName )
	{
		m_msgqName = a_msgqName;
	}
	bool MessageQueueServer::create(void)
	{
		appCore::appAssertPointer( m_msgQueueHeader );
		INIT_MSG_QUEUE_HEADER( m_msgQueueHeader, m_msgqName.c_str(), m_semaphoreNumber, m_countLimit, m_timeToLive );
		
		return true;
	}
}//namespace mqsProvider
