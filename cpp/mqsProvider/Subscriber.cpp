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
#include "mqsProvider/ListClient.h"
#include "mqsProvider/MessageQueueServiceClient.h"
#include "mqsProvider/MessageQueueClient.h"
#include "mqsProvider/MessageQueueServiceHeader.h"
#include "mqsProvider/MutualExclusionClient.h"
#include "mqsProvider/Subscriber.h"
#include <appUtil/JJJException.h>
#include <appUtil/Shortcuts.h>
#include <string>

#if defined(_MSCVER)
#	include <windows.h>
#	define SUBSCRIBER_SLEEP(DELAY)	Sleep(DELAY)
#else
#	if defined(GCC) || defined(XLC) || defined(__GNUC__)
#		include<unistd.h>
#		define SUBSCRIBER_SLEEP(DELAY)	usleep(DELAY*1000)
#	endif
#endif

namespace mqsProvider
{
	Subscriber::Subscriber(
			MessageQueueServiceClient* a_messageQueueServiceClient,
			MutualExclusionClient* a_mutexClient )
	{
		m_messageQueueServiceClient = a_messageQueueServiceClient;
		m_mutexClient = a_mutexClient;

		appUtil::assertPointer( m_messageQueueServiceClient );
		appUtil::assertPointer( m_mutexClient );
		appUtil::initPointer( m_messageQueueClient );
		appUtil::initPointer( m_message );
		appUtil::initPointer( m_messageIdentification );

		m_milisecondsTimeout	= 0;
		m_elapsedTime			= 0;

		memset( m_msgqCreationDate, 0, sizeof m_msgqCreationDate );
	}
	Subscriber::~Subscriber(void)
	{
		std::string* l_msg = reinterpret_cast<std::string*>(m_message);
		std::string* l_id = reinterpret_cast<std::string*>(m_messageIdentification);
		appUtil::safeDelete( m_messageQueueClient );
		delete l_msg;
		m_message = NULL;
		delete l_id;
		m_messageIdentification = 0;
		VOID_RETURN_IF_NULL( m_mutexClient );
		VOID_RETURN_IF_NULL( m_messageQueueServiceClient );
		m_mutexClient->close();
		m_messageQueueServiceClient->close();
	}
	bool Subscriber::open(
			const char* a_messageQueueName )
	{
		appUtil::assertPointer( m_messageQueueServiceClient );
		appUtil::assertPointer( m_mutexClient );
		
		appUtil::safeNew( m_messageQueueClient );
		m_message				= new std::string;
		m_messageIdentification	= new std::string;
		appUtil::assertPointer( m_message );
		appUtil::assertPointer( m_messageIdentification );

		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				m_mutexClient->enterCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		appUtil::assert(
				l_enterStatus == MutualExclusionClient::ENTER_SUCCESS,
				__FILE__,
				__LINE__,
				"Subscriber::open: Could not enter in critical area" );
				
		bool l_ret =
				m_messageQueueServiceClient->findMessageQueue(
						a_messageQueueName, m_messageQueueClient );
		if( l_ret ){
			safeSPrintf(
					m_msgqCreationDate,
					sizeof m_msgqCreationDate,
					"%s",
					m_messageQueueClient->creationDate().c_str() );
		}

		m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		
		return l_ret;
	}
	void Subscriber::close(void)
	{
		std::string* l_msg = reinterpret_cast<std::string*>(m_message);
		std::string* l_id = reinterpret_cast<std::string*>(m_messageIdentification);
		appUtil::safeDelete( m_messageQueueClient );
		delete l_msg;
		m_message = NULL;
		delete l_id;
		m_messageIdentification = 0;

		m_mutexClient->close();
		m_messageQueueServiceClient->close();
	}

	void Subscriber::setTimeout(
			unsigned int a_milisecondsTimeout )
	{
		m_milisecondsTimeout = a_milisecondsTimeout;
	}

	const Subscriber& Subscriber::identifyMessage(
			const char* a_messageIdentification ) const
	{
		std::string& l_refMsgId =
				*reinterpret_cast<std::string*>(m_messageIdentification);
		l_refMsgId.assign( a_messageIdentification );

		return *this;
	}

	enum Subscriber::RECEIVE_STATUS Subscriber::receive(void) const
	{
		unsigned int l_delay = 100;
		unsigned int l_maxTries = m_milisecondsTimeout / l_delay;
		unsigned int l_tryCount = 0;
		enum RECEIVE_STATUS l_recvRet = RECV_NO_MESSAGE;
		while( (l_recvRet = tryReceive()) == RECV_NO_MESSAGE ){
			SUBSCRIBER_SLEEP( l_delay );
			CONTINUE_IF( m_milisecondsTimeout == 0 );
			++l_tryCount;
			RETURN_IF( l_tryCount >= l_maxTries, RECV_TIMEOUT );
		}

		return l_recvRet;
	}
	enum Subscriber::RECEIVE_STATUS Subscriber::tryReceive(void) const
	{
		RETURN_IF_NULL( m_messageQueueClient, RECV_FATAL_ERROR );
		RETURN_IF_NULL( m_message, RECV_FATAL_ERROR );

		const mqsProvider::MessageQueueServiceHeader* l_mqsHeader =
				m_messageQueueServiceClient->mqsHeader();
		RETURN_IF_NULL( l_mqsHeader, RECV_FATAL_ERROR );

		std::string l_originalCreationDate = m_msgqCreationDate;
		std::string& l_msg = *(reinterpret_cast<std::string*>(m_message));
		unsigned short int l_msgqSemNum = m_messageQueueClient->semaphoreNumber();

		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				m_mutexClient->enterCriticalArea(l_msgqSemNum);
		appUtil::assert(
			l_enterStatus == MutualExclusionClient::ENTER_SUCCESS,
			__FILE__, __LINE__, "Subscriber::open: Could not enter in critical area" );

		if( l_mqsHeader->status != ACTIVE ){
			m_mutexClient->exitCriticalArea(l_msgqSemNum);
			return RECV_SYSTEM_DOWN;
		}
		int l_compareDate = l_originalCreationDate.compare(
				m_messageQueueClient->creationDate() );
		if( l_compareDate != 0 ){
			m_mutexClient->exitCriticalArea(l_msgqSemNum);
			return RECV_MSGQUEUE_DESTROYED;
		}

		std::string& l_refMsgId =
				*reinterpret_cast<std::string*>(m_messageIdentification);
		date_t l_messageDate;
		bool l_ret = m_messageQueueClient->receiveMessage(
				l_refMsgId,
				l_msg,
				l_messageDate );
		m_mutexClient->exitCriticalArea(l_msgqSemNum);
		m_elapsedTime = ELAPSEDTIME( l_messageDate );

		enum RECEIVE_STATUS l_recvRet = l_ret ? RECV_SUCCESS : RECV_NO_MESSAGE;
		return l_recvRet;
	}

	unsigned int Subscriber::elapsedTime(void) const
	{
		return m_elapsedTime;
	}

	const char* Subscriber::message(void) const
	{
		RETURN_IF( m_message == NULL, "" );
		std::string& l_msg = *(reinterpret_cast<std::string*>(m_message));
		return l_msg.data();
	}
	unsigned int Subscriber::messageSize(void) const
	{
		RETURN_IF( m_message == NULL, 0 );
		std::string& l_msg = *(reinterpret_cast<std::string*>(m_message));
		return static_cast<unsigned int>( l_msg.size() );
	}

}//namespace mqsProvider
