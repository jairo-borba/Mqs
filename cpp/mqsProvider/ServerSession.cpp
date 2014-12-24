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

#include "mqsProvider/ServerSession.h"
#include <string>
#include <base/ExceptionInfo.h>
#include <appCore/Shortcuts.h>
#include "mqsProvider/Controller.h"
#include "mqsProvider/MessageQueueServiceClient.h"
#include "mqsProvider/MutualExclusionClient.h"
#include "mqsFactory/SharedMemoryAccessFactory.h"
#include "mqsFactory/MutualExclusionClientFactory.h"
#include <map>

namespace mqsProvider
{
	ServerSession::ServerSession(void)
	{
		m_isConnected	= false;
		appCore::initPointer(m_controller);
		appCore::initPointer(m_shmAccess);
		appCore::initPointer(m_mtxClient);
		appCore::initPointer(m_mqsClient);
	}
	ServerSession::~ServerSession(void)
	{
		this->disconnect();
	}
	bool ServerSession::connect(void)
	{
		RETURN_IF( m_isConnected, false );

		mqsFactory::SharedMemoryAccessFactory l_shmFactory;
		m_shmAccess		= l_shmFactory.create();
		if( !m_shmAccess->bind() ){
			appCore::safeDelete(m_shmAccess);
			return false;
		}
		mqsFactory::MutualExclusionClientFactory l_mtxFactory;
		m_mtxClient		= l_mtxFactory.create();
		if( !m_mtxClient->open() ){
			appCore::safeDelete(m_shmAccess);
			appCore::safeDelete(m_mtxClient);
			return false;
		}
		appCore::safeNew(m_mqsClient);
		m_mqsClient->setSharedMemoryAccess( m_shmAccess );
		if( !m_mqsClient->open() ){
			appCore::safeDelete(m_shmAccess);
			appCore::safeDelete(m_mtxClient);
			appCore::safeDelete(m_mqsClient);
			return false;
		}

		m_isConnected = true;

		return m_isConnected;
	}
	void ServerSession::disconnect(void)
	{
		VOID_RETURN_IF( !m_isConnected );

		if( m_controller != NULL ){
			m_controller->close();
			appCore::safeDelete(m_controller);
		}

		m_mqsClient->close();
		m_shmAccess->detach();
		m_mtxClient->close();
		appCore::safeDelete(m_shmAccess);
		appCore::safeDelete(m_mtxClient);
		appCore::safeDelete(m_mqsClient);

		m_isConnected = false;
	}
	const Controller* ServerSession::attachController(void)
	{
		appCore::appAssert( isConnected(), "ServerSession not connected" );
		RETURN_IF( m_controller != NULL, m_controller );

		m_controller = new Controller( m_mqsClient, m_mtxClient );

		bool l_open = m_controller->open();
		if( !l_open ){
			appCore::safeDelete(m_controller);
		}

		return m_controller;
	}
	bool ServerSession::isConnected(void)
	{
		return m_isConnected;
	}
}//namespace mqsProvider
