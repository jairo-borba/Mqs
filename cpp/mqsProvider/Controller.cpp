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
#include "mqsProvider/Controller.h"

#include <appUtil/JJJException.h>
#include <appUtil/Shortcuts.h>
#include <string>

#if defined(_MSC_VER)
#	include <windows.h>
#	define CONTROLLER_SLEEP(DELAY)	Sleep(DELAY)
#else
#	if defined(GCC) || defined(XLC) || defined(__GNUC__)
#		include<unistd.h>
#		define CONTROLLER_SLEEP(DELAY)	usleep(DELAY*1000)
#	endif
#endif

namespace mqsProvider
{
	Controller::Controller(
			MessageQueueServiceClient* a_messageQueueServiceClient,
			MutualExclusionClient* a_mutexClient )
	{
		m_messageQueueServiceClient	= a_messageQueueServiceClient;
		m_mutexClient				= a_mutexClient;
		appUtil::assertPointer( m_messageQueueServiceClient );
		appUtil::assertPointer( m_mutexClient );
	}
	Controller::~Controller(void)
	{
		VOID_RETURN_IF_NULL( m_mutexClient );
		VOID_RETURN_IF_NULL( m_messageQueueServiceClient );
		m_mutexClient->close();
		m_messageQueueServiceClient->close();
	}
	bool Controller::open(void)
	{
		appUtil::assertPointer( m_messageQueueServiceClient );
		appUtil::assertPointer( m_mutexClient);
		appUtil::assert( m_mutexClient->open(),"Controller::open: Error opening Mutex" );

		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				m_mutexClient->enterCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		appUtil::assert(
				l_enterStatus == MutualExclusionClient::ENTER_SUCCESS,
				"Controller::open: Could not enter in critical area" );

		bool l_mqscliRet =
				m_messageQueueServiceClient->open();

		m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		appUtil::assert(
				l_mqscliRet,
				"Controller::open: Error opening MessageQueueServiceClient" );

		return true;
	}
	void Controller::close(void)
	{
		m_mutexClient->close();
		m_messageQueueServiceClient->close();
	}

	bool Controller::createMessageQueue(
			const char* a_msgqName,
			unsigned int a_countLimit,
			unsigned int a_timeToLive ) const
	{
		appUtil::assertPointer( m_messageQueueServiceClient );
		appUtil::assertPointer( m_mutexClient);
		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				m_mutexClient->enterCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		appUtil::assert(
				l_enterStatus == MutualExclusionClient::ENTER_SUCCESS,
				"Controller::open: Could not enter in critical area" );

		bool l_created =
				m_messageQueueServiceClient->createMessageQueue(
						a_msgqName,
						a_countLimit,
						a_timeToLive );
		MessageQueueClient l_mqCli;
		bool l_found =
				m_messageQueueServiceClient->findMessageQueue(
						a_msgqName,
						&l_mqCli );
		if( l_created && l_found ){
			unsigned int l_semNum =
					l_mqCli.semaphoreNumber();
			m_mutexClient->setMutex( l_semNum, true );
		}
				
		m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);

		return l_created;
	}
	bool Controller::destroyMessageQueue( const char* a_msgqName ) const
	{

		appUtil::assertPointer( m_messageQueueServiceClient );
		appUtil::assertPointer( m_mutexClient );

		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				MutualExclusionClient::ENTER_SUCCESS;
		l_enterStatus = m_mutexClient->enterCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		RETURN_IF( l_enterStatus != MutualExclusionClient::ENTER_SUCCESS, false );

		MessageQueueClient l_mqCli;
		bool l_found = m_messageQueueServiceClient->findMessageQueue(
				a_msgqName, &l_mqCli );
		m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		RETURN_IF( !l_found, false );

		//FECHA A PORTA DE ENTRADA DA MESSAGE QUEUE
		bool l_ret = enableMessageQueueInput(
				&l_mqCli,
				false );
		//CERTIFICA QUE A PORTA DE SAIDA ESTA ABERTA
		l_ret = enableMessageQueueOutput(
				&l_mqCli,
				true );

		//ESPERA A MESSAGE QUEUE SER CONSUMIDA
		unsigned int l_timeout		=
				m_messageQueueServiceClient->mqsHeader()->parameters.destroyMessageQueueWaitSeconds;
		unsigned int l_msecsTimeout = l_timeout * 1000;
		const unsigned int l_poll	= 12;
		unsigned int l_countMsg = 0;
		for(unsigned int l_countTries = 0;l_countTries < l_poll; ++l_countTries)
		{
			l_countMsg = countMessages( &l_mqCli );
			BREAK_IF( l_countMsg == 0 );
			CONTROLLER_SLEEP( l_msecsTimeout / l_poll );
		}

		//DEVOLVE OS RECURSOS DA MESSAGEQUEUE
		l_enterStatus = MutualExclusionClient::ENTER_SUCCESS;

		unsigned short l_mqSemNum = l_mqCli.semaphoreNumber();

		l_enterStatus = m_mutexClient->enterCriticalArea(l_mqSemNum);
		RETURN_IF( l_enterStatus != MutualExclusionClient::ENTER_SUCCESS, false );
		
		if( l_countMsg > 0 ){
			this->emptyMessageQueue( &l_mqCli );
		}
		
		l_ret = m_messageQueueServiceClient->destroyMessageQueue(
				a_msgqName );

		m_mutexClient->exitCriticalArea(l_mqSemNum);

		return l_ret;
	}
	void Controller::emptyMessageQueue(
			MessageQueueClient* a_mqClient) const
	{
		std::string l_dropped;
		date_t l_messageDate;

		//const char* l_path = m_messageQueueServiceClient->mqsHeader()->parameters.droppedMessagesFilePath;
		while( a_mqClient->receiveMessage(
				"",
				l_dropped,
				l_messageDate ) ){
		}
	}
	
	bool Controller::enableMessageQueueInput(
			MessageQueueClient* a_mqClient,
			bool a_enable) const
	{
		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				MutualExclusionClient::ENTER_SUCCESS;

		unsigned short l_mqSemNum = a_mqClient->semaphoreNumber();

		l_enterStatus = m_mutexClient->enterCriticalArea(l_mqSemNum);
		RETURN_IF( l_enterStatus != MutualExclusionClient::ENTER_SUCCESS, false );

		a_mqClient->enableInput( a_enable );

		m_mutexClient->exitCriticalArea(l_mqSemNum);

		return true;
	}

	bool Controller::enableMessageQueueOutput(
			MessageQueueClient* a_mqClient,
			bool a_enable) const
	{
		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				MutualExclusionClient::ENTER_SUCCESS;

		unsigned short l_mqSemNum = a_mqClient->semaphoreNumber();

		l_enterStatus = m_mutexClient->enterCriticalArea(l_mqSemNum);
		RETURN_IF( l_enterStatus != MutualExclusionClient::ENTER_SUCCESS, false );

		a_mqClient->enableOutput( a_enable );

		m_mutexClient->exitCriticalArea(l_mqSemNum);

		return true;
	}

	unsigned int Controller::countMessages(
			MessageQueueClient* a_mqClient) const
	{
		unsigned int l_pendingMessages = a_mqClient->countMessages();
		
		return l_pendingMessages;
	}
}//namespace mqsProvider
