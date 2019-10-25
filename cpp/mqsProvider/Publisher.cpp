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
#include "mqsProvider/Publisher.h"
#include "appUtil/SafeStringDef.h"
#include <appUtil/JJJException.h>
#include <cstdio>
#include <appUtil/Shortcuts.h>

#if defined(_MSC_VER)
#	include <windows.h>
#	define PUBLISHER_SLEEP	Sleep
#else
#	if defined(GCC) || defined(XLC) || defined(__GNUC__)
#		include<unistd.h>
#		define PUBLISHER_SLEEP	usleep
#	endif
#endif

namespace mqsProvider
{
	static const unsigned short int DEFAULT_MORE_ALLOC_RESOURCES_QTY	= 32	;	//32 HEADERS AND 2*32 CLUSTERS MORE WHEN MISSING
	static const unsigned int DEFAULT_RETRY_DELAY						= 100	;

	Publisher::Publisher(
			MessageQueueServiceClient* a_messageQueueServiceClient,
			MutualExclusionClient* a_mutexClient)
	{
		m_messageQueueServiceClient = a_messageQueueServiceClient;
		m_mutexClient = a_mutexClient;
		appUtil::assertPointer( m_messageQueueServiceClient );
		appUtil::assertPointer( m_mutexClient );
		appUtil::initPointer( m_messageQueueClient );
		appUtil::initPointer( m_messageIdentification );
		m_numMaxTriesToSend		= 1;
		m_retryDelay 			= 0;
		m_moreAllocResourcesQty = 0;
		this->setRetryDelay				( DEFAULT_RETRY_DELAY );
		this->setMoreAllocResourcesQty	( DEFAULT_MORE_ALLOC_RESOURCES_QTY );
		memset( m_msgqCreationDate, 0, sizeof m_msgqCreationDate );
	}
	Publisher::~Publisher(void)
	{
		appUtil::safeDelete( m_messageQueueClient );
		std::string* l_id = reinterpret_cast<std::string*>(m_messageIdentification);
		delete l_id;
		m_messageIdentification = 0;
	}
	void Publisher::setNumMaxRetriesToSend(
			unsigned int a_numMaxTriesToSend )
	{
		m_numMaxTriesToSend = a_numMaxTriesToSend;
	}
	void Publisher::setMoreAllocResourcesQty(
			unsigned short int a_moreAllocResourcesQty )
	{
		m_moreAllocResourcesQty = a_moreAllocResourcesQty;
	}
	void Publisher::setRetryDelay(
			unsigned int a_milisecondsRetryDelay )
	{
#if defined(_MSC_VER)
		m_retryDelay = a_milisecondsRetryDelay;
#else
#	if defined(GCC) || defined(XLC) || defined(__GNUC__)
		m_retryDelay = a_milisecondsRetryDelay * 1000;
#	endif
#endif
	}
	bool Publisher::open(
			const char* a_messageQueueName )
	{
		appUtil::assertPointer( m_messageQueueServiceClient );
		appUtil::assertPointer( m_mutexClient );

		m_messageQueueClient	= new MessageQueueClient;
		m_messageIdentification	= new std::string;
		appUtil::assertPointer( m_messageQueueClient );
		appUtil::assertPointer( m_messageIdentification );

		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				m_mutexClient->enterCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
		appUtil::assert(
				l_enterStatus == MutualExclusionClient::ENTER_SUCCESS,
				"Publisher::open: Could not enter in critical area" );

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
	void Publisher::close(void)
	{
		appUtil::safeDelete( m_messageQueueClient );

		std::string* l_id =
				reinterpret_cast<std::string*>(m_messageIdentification);
		delete l_id;
		m_messageIdentification = 0;
	}
	const Publisher& Publisher::identifyMessage(
			const char* a_messageIdentification ) const
	{
		std::string& l_refMsgId =
				*reinterpret_cast<std::string*>(m_messageIdentification);
		l_refMsgId.assign( a_messageIdentification );

		return *this;
	}
	enum Publisher::SEND_STATUS Publisher::sendf(
			const char* a_format,
			... ) const
	{
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_msg[16384];
		int l_size = safeVSPrintf( l_msg, sizeof l_msg, a_format, l_argList );
		va_end( l_argList );

		enum SEND_STATUS l_ret = send( l_msg, l_size );

		return l_ret;
	}

	enum Publisher::SEND_STATUS Publisher::send(
			const char* a_message,
			int a_messageSize ) const
	{
		unsigned int l_numTries = 0;
		bool l_bContinue = true;

		for(;l_bContinue;++l_numTries)
		{
			enum TRY_SEND_STATUS l_trySendStatus =
					this->trySend( a_message, a_messageSize );

			switch(l_trySendStatus)
			{
			case TS_FATAL_ERROR:		return SND_FATAL_ERROR;
			case TS_SUCCESS:			l_bContinue=false;break;
			case TS_MUTEX_TIMEOUT:		return SND_FATAL_ERROR;
			case TS_MISSING_HEADER:
				RETURN_IF( l_numTries >= m_numMaxTriesToSend, SND_MISSING_HEADER );
				PUBLISHER_SLEEP(m_retryDelay);
				break;
			case TS_MISSING_TREE_NODE:
				RETURN_IF( l_numTries >= m_numMaxTriesToSend, SND_MISSING_TREE_NODE );
				PUBLISHER_SLEEP(m_retryDelay);
				break;
			case TS_MISSING_CLUSTER:
				RETURN_IF( l_numTries >= m_numMaxTriesToSend, SND_MISSING_CLUSTER );
				PUBLISHER_SLEEP(m_retryDelay);
				break;
			case TS_INPUT_OFF:
				RETURN_IF( l_numTries >= m_numMaxTriesToSend, SND_INPUT_OFF );
				PUBLISHER_SLEEP(m_retryDelay);
				break;
			case TS_MSGQUEUE_DESTROYED:
				return SND_MSGQUEUE_DESTROYED;
			case TS_SYSTEM_DOWN:
				return SND_SYSTEM_DOWN;
			case TS_COUNT_LIMIT:
				RETURN_IF( l_numTries >= m_numMaxTriesToSend, SND_COUNT_LIMIT );
				PUBLISHER_SLEEP(m_retryDelay);
				break;
			}
		}

		enum SEND_STATUS l_ret = l_numTries > 1 ? SND_ON_THROTLING_SUCCESS : SND_EASY_SUCCESS;

		return l_ret;
	}

	enum Publisher::TRY_SEND_STATUS Publisher::trySend(
			const char* a_message,
			int a_messageSize ) const
	{
		//DANGEROUS OPERATIONS WHICH DO NOT NEED TO BE IN CRITICAL AREA (to avoid deadlock) [START]
		RETURN_IF_NULL( m_messageQueueServiceClient,	TS_FATAL_ERROR );
		RETURN_IF_NULL( m_mutexClient,					TS_FATAL_ERROR );
		RETURN_IF_NULL( m_messageQueueClient,			TS_FATAL_ERROR );
		RETURN_IF_NULL( a_message,						TS_FATAL_ERROR );
		RETURN_IF_ZERO( a_messageSize,					TS_FATAL_ERROR );

		const mqsProvider::MessageQueueServiceHeader* l_mqsHeader =
				m_messageQueueServiceClient->mqsHeader();
		RETURN_IF_NULL( l_mqsHeader, TS_FATAL_ERROR );
		std::string l_message( a_message, a_messageSize );
		std::string& l_refMsgId = *reinterpret_cast<std::string*>(m_messageIdentification);
		std::string l_originalCreationDate = m_msgqCreationDate;
		//DANGEROUS OPERATIONS WHICH DO NOT NEED TO BE IN CRITICAL AREA[END]

		unsigned short int l_msgqSemNum = m_messageQueueClient->semaphoreNumber();
		MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS l_enterStatus =
				m_mutexClient->enterCriticalArea(l_msgqSemNum);
	///////////// CRITICAL CRITICAL CRITICAL CRITICAL CRITICAL CRITICAL CRITICAL CRITICAL /////////////[START]
		try
		{
			switch( l_enterStatus )
			{
			case MutualExclusionClient::ENTER_SUCCESS:break;
			case MutualExclusionClient::ENTER_TIMEOUT:return TS_MUTEX_TIMEOUT;
			case MutualExclusionClient::ENTER_FAILED :return TS_FATAL_ERROR;
			}

			if( l_mqsHeader->status != ACTIVE ){
				m_mutexClient->exitCriticalArea(l_msgqSemNum);
				return TS_SYSTEM_DOWN;
			}

			int l_compareDate = l_originalCreationDate.compare( m_messageQueueClient->creationDate() );
			if( l_compareDate != 0 ){
				m_mutexClient->exitCriticalArea(l_msgqSemNum);
				return TS_MSGQUEUE_DESTROYED;
			}

			mqsProvider::MessageQueueClient::SEND_STATUS l_sendStatus = m_messageQueueClient->sendMessage( l_refMsgId, l_message );
			if( l_sendStatus == MessageQueueClient::INPUT_OFF ){
				m_mutexClient->exitCriticalArea(l_msgqSemNum);
				return TS_INPUT_OFF;
			} else if( l_sendStatus == MessageQueueClient::COUNT_LIMIT ) {
				m_mutexClient->exitCriticalArea(l_msgqSemNum);
				return TS_COUNT_LIMIT;
			} else if( l_sendStatus != MessageQueueClient::SUCCESS ){
				try
				{
					MutualExclusionClient::ENTER_CRITICAL_AREA_STATUS sl_globalEnterStatus = m_mutexClient->enterCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
					///////////// CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 /////////////[START]
					switch( sl_globalEnterStatus ){
					case MutualExclusionClient::ENTER_SUCCESS:break;
					case MutualExclusionClient::ENTER_TIMEOUT:
						m_mutexClient->exitCriticalArea(l_msgqSemNum);
						return TS_MUTEX_TIMEOUT;
					case MutualExclusionClient::ENTER_FAILED :
						m_mutexClient->exitCriticalArea(l_msgqSemNum);
						return TS_FATAL_ERROR;
					}

					enum MORE_RESOURCES_STATUS sl_moreResourcesStatus = MRS_NO_MORE;

					do {
						switch( l_sendStatus ){
							case mqsProvider::MessageQueueClient::SUCCESS:break;

							case mqsProvider::MessageQueueClient::MISSING_HEADER:
								sl_moreResourcesStatus = this->moreHeaders();
								switch( sl_moreResourcesStatus ){
								case MRS_SUCCESS:break;
								case MRS_NO_MORE:
									m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
									m_mutexClient->exitCriticalArea(l_msgqSemNum);
									return TS_MISSING_HEADER;
								case MRS_FATAL_ERROR:
									m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
									m_mutexClient->exitCriticalArea(l_msgqSemNum);
									return TS_FATAL_ERROR;
								}
								break;
							case mqsProvider::MessageQueueClient::MISSING_TREE_NODE:
								sl_moreResourcesStatus = this->moreTreeNodes();
								switch( sl_moreResourcesStatus ){
								case MRS_SUCCESS:break;
								case MRS_NO_MORE:
									m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
									m_mutexClient->exitCriticalArea(l_msgqSemNum);
									return TS_MISSING_TREE_NODE;
								case MRS_FATAL_ERROR:
									m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
									m_mutexClient->exitCriticalArea(l_msgqSemNum);
									return TS_FATAL_ERROR;
								}
								break;
							case mqsProvider::MessageQueueClient::MISSING_BUFFER:
								sl_moreResourcesStatus = this->moreBuffers();
								switch( sl_moreResourcesStatus ){
								case MRS_SUCCESS:break;
								case MRS_NO_MORE:
									m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
									m_mutexClient->exitCriticalArea(l_msgqSemNum);
									return TS_MISSING_CLUSTER;
								case MRS_FATAL_ERROR:
									m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
									m_mutexClient->exitCriticalArea(l_msgqSemNum);
									return TS_FATAL_ERROR;
								}
								break;
							case mqsProvider::MessageQueueClient::INPUT_OFF:
								m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
								m_mutexClient->exitCriticalArea(l_msgqSemNum);
								return TS_INPUT_OFF;
							case mqsProvider::MessageQueueClient::COUNT_LIMIT:
								m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
								m_mutexClient->exitCriticalArea(l_msgqSemNum);
								return TS_COUNT_LIMIT;
						}
						l_sendStatus = m_messageQueueClient->sendMessage( l_refMsgId, l_message );
					} while( l_sendStatus != mqsProvider::MessageQueueClient::SUCCESS );

					m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
					///////////// CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 CRITICAL2 /////////////[END]
				} catch( const appUtil::JJJException& e ) {
					m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
					m_mutexClient->exitCriticalArea(l_msgqSemNum);
					throw e;
				}  catch( ... ) {
					m_mutexClient->exitCriticalArea(GLOBAL_LISTS_SEMAPHORE_NUMBER);
					m_mutexClient->exitCriticalArea(l_msgqSemNum);
					throw appUtil::JJJException( __FILE__, __LINE__, "Unknown exception" );
				}
			}
			m_mutexClient->exitCriticalArea(l_msgqSemNum);
		} catch( const appUtil::JJJException& e ) {
			m_mutexClient->exitCriticalArea(l_msgqSemNum);
			throw e;
		} catch( ... ) {
			m_mutexClient->exitCriticalArea(l_msgqSemNum);
			throw appUtil::JJJException( __FILE__, __LINE__, "Unknown exception" );
		}
///////////// CRITICAL CRITICAL CRITICAL CRITICAL CRITICAL CRITICAL CRITICAL CRITICAL /////////////[END]

		return TS_SUCCESS;
	}

	enum Publisher::MORE_RESOURCES_STATUS Publisher::moreHeaders(void) const
	{
		mqsProvider::MessageQueueServiceHeader* l_mqsHeader = m_messageQueueServiceClient->mqsHeader();
		mqsProvider::ListClient a_listCliMessageHeaders(
				&l_mqsHeader->listMessageHeaders.mainList,
				m_messageQueueServiceClient->messageHeaderBuffer() );
		unsigned int sl_moreResources = 0;
		unsigned int sl_header = a_listCliMessageHeaders.dequeue();
		while( sl_header != INDEX_NULL ){
			RETURN_IF( !m_messageQueueClient->addMessageHeaderCluster( sl_header ), MRS_FATAL_ERROR );
			BREAK_IF( ++sl_moreResources >= DEFAULT_MORE_ALLOC_RESOURCES_QTY );
			sl_header = a_listCliMessageHeaders.dequeue();
		}
		RETURN_IF( sl_moreResources == 0, MRS_NO_MORE );
		return MRS_SUCCESS;
	}
	enum Publisher::MORE_RESOURCES_STATUS Publisher::moreTreeNodes(void) const
	{
		mqsProvider::MessageQueueServiceHeader* l_mqsHeader = m_messageQueueServiceClient->mqsHeader();
		mqsProvider::ListClient a_listCliMessageTreeNodes(
				&l_mqsHeader->listTreeNodes.mainList,
				m_messageQueueServiceClient->treeNodeBuffer() );
		unsigned int sl_moreResources = 0;
		unsigned int sl_treeNodes = a_listCliMessageTreeNodes.dequeue();
		while( sl_treeNodes != INDEX_NULL ){
			RETURN_IF( !m_messageQueueClient->addMessageTreeNodeCluster( sl_treeNodes ), MRS_FATAL_ERROR );
			BREAK_IF( ++sl_moreResources >= DEFAULT_MORE_ALLOC_RESOURCES_QTY );
			sl_treeNodes = a_listCliMessageTreeNodes.dequeue();
		}

		RETURN_IF( sl_moreResources == 0, MRS_NO_MORE );
		return MRS_SUCCESS;
	}
	enum Publisher::MORE_RESOURCES_STATUS Publisher::moreBuffers(void) const
	{
		mqsProvider::MessageQueueServiceHeader* l_mqsHeader =
				m_messageQueueServiceClient->mqsHeader();
		mqsProvider::ListClient a_listCliMessageClusters(
				&l_mqsHeader->listMessageClusters.mainList,
				m_messageQueueServiceClient->messageClusterBuffer() );
		unsigned int sl_moreResources = 0;
		unsigned int sl_messageCluster = a_listCliMessageClusters.dequeue();
		while( sl_messageCluster != INDEX_NULL ){
			RETURN_IF( !m_messageQueueClient->addMessageCluster( sl_messageCluster ), MRS_FATAL_ERROR );
			BREAK_IF( ++sl_moreResources >= 2 * DEFAULT_MORE_ALLOC_RESOURCES_QTY );
			sl_messageCluster = a_listCliMessageClusters.dequeue();
		}

		RETURN_IF( sl_moreResources == 0, MRS_NO_MORE );
		return MRS_SUCCESS;
	}
}//namespace mqsProvider
