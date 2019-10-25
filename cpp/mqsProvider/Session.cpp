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
#include "mqsProvider/Session.h"
#include <string>
#include <appUtil/JJJException.h>
#include <appUtil/Shortcuts.h>
#include "mqsProvider/Subscriber.h"
#include "mqsProvider/Publisher.h"
#include "mqsProvider/MessageQueueServiceClient.h"
#include "mqsProvider/MutualExclusionClient.h"
#include "mqsFactory/SharedMemoryAccessFactory.h"
#include "mqsFactory/MutualExclusionClientFactory.h"
#include <map>

namespace mqsProvider
{
	typedef std::map<std::string,Publisher*>	PUBLISHERS;
	typedef std::map<std::string,Subscriber*>	SUBSCRIBERS;
	typedef std::pair<std::string,Publisher*>	PAIR_PUB;
	typedef std::pair<std::string,Subscriber*>	PAIR_SUB;
	typedef std::pair<PUBLISHERS::iterator,bool>	INSERT_RET_PUB;
	typedef std::pair<SUBSCRIBERS::iterator,bool>	INSERT_RET_SUB;

	Session::Session(void)
	{
		m_subscribers	= NULL;
		m_publishers	= NULL;
		m_isConnected	= false;
		appUtil::initPointer(m_shmAccess);
		appUtil::initPointer(m_mtxClient);
		appUtil::initPointer(m_mqsClient);
	}
	Session::~Session(void)
	{
		this->disconnect();
	}
	bool Session::connect(void)
	{
		RETURN_IF( m_isConnected, false );
		
		mqsFactory::SharedMemoryAccessFactory l_shmFactory;
		m_shmAccess		= l_shmFactory.create();
		if( !m_shmAccess->bind() ){
			appUtil::safeDelete(m_shmAccess);
			return false;
		}
		mqsFactory::MutualExclusionClientFactory l_mtxFactory;
		m_mtxClient		= l_mtxFactory.create();
		if( !m_mtxClient->open() ){
			appUtil::safeDelete(m_shmAccess);
			appUtil::safeDelete(m_mtxClient);
			return false;
		}
		appUtil::safeNew(m_mqsClient);
		m_mqsClient->setSharedMemoryAccess( m_shmAccess );
		if( !m_mqsClient->open() ){
			appUtil::safeDelete(m_shmAccess);
			appUtil::safeDelete(m_mtxClient);
			appUtil::safeDelete(m_mqsClient);
			return false;
		}
		m_subscribers	= new SUBSCRIBERS;
		m_publishers	= new PUBLISHERS;

		m_isConnected = true;
		
		return m_isConnected;
	}
	void Session::disconnect(void)
	{
		VOID_RETURN_IF( !m_isConnected );
				
		if( m_subscribers != NULL ){
			SUBSCRIBERS* l_pmap = reinterpret_cast<SUBSCRIBERS*>( m_subscribers );
			SUBSCRIBERS::iterator sl_it;
			for( sl_it = l_pmap->begin(); sl_it != l_pmap->end(); ++sl_it ){
				sl_it->second->close();
				delete sl_it->second;
			}
			reinterpret_cast<SUBSCRIBERS*>(m_subscribers)->clear();
			delete reinterpret_cast<SUBSCRIBERS*>(m_subscribers);
		}
		if( m_publishers != NULL ){
			PUBLISHERS* l_pmap = reinterpret_cast<PUBLISHERS*>( m_publishers );
			PUBLISHERS::iterator sl_it;
			for( sl_it = l_pmap->begin(); sl_it != l_pmap->end(); ++sl_it ){
				sl_it->second->close();
				delete sl_it->second;
			}
			reinterpret_cast<PUBLISHERS*>(m_publishers)->clear();
			delete reinterpret_cast<PUBLISHERS*>(m_publishers);
		}
		
		m_mqsClient->close();
		m_shmAccess->detach();
		m_mtxClient->close();
		appUtil::safeDelete(m_shmAccess);
		appUtil::safeDelete(m_mtxClient);
		appUtil::safeDelete(m_mqsClient);

		m_isConnected = false;
	}
	
	const Subscriber* Session::attachSubscriber(
			const char* a_mqName )
	{
		appUtil::assert( isConnected(), "Session not connected" );

		SUBSCRIBERS* l_pmap = reinterpret_cast<SUBSCRIBERS*>( m_subscribers );
		Subscriber* l_sub = NULL;
		SUBSCRIBERS::iterator l_it = l_pmap->find( a_mqName );
		RETURN_IF( l_it != l_pmap->end(), l_it->second );

		l_sub = new Subscriber( m_mqsClient, m_mtxClient );
		
		bool l_open = l_sub->open( a_mqName );
		if( l_open ){
			INSERT_RET_SUB sl_ret = l_pmap->insert( PAIR_SUB(a_mqName,l_sub) );
			if( !sl_ret.second ){
				l_sub->close();
				delete l_sub;
				l_sub = NULL;
			}
		} else {
			delete l_sub;
			l_sub = NULL;
		}
		return l_sub;
	}
	const Publisher* Session::attachPublisher(
			const char* a_mqName )
	{
		appUtil::assert( isConnected(), "Session not connected" );

		PUBLISHERS* l_pmap = reinterpret_cast<PUBLISHERS*>( m_publishers );
		Publisher* l_pub = NULL;
		PUBLISHERS::iterator l_it = l_pmap->find( a_mqName );
		RETURN_IF( l_it!= l_pmap->end(), l_it->second );
		
		l_pub = new Publisher( m_mqsClient, m_mtxClient );
		
		bool l_open = l_pub->open( a_mqName );
		if( l_open ){
			INSERT_RET_PUB sl_ret = l_pmap->insert( PAIR_PUB(a_mqName,l_pub) );
			if( !sl_ret.second ){
				l_pub->close();
				delete l_pub;
				l_pub = NULL;
			}
		} else {
			delete l_pub;
			l_pub = NULL;
		}
		
		return l_pub;
	}
	bool Session::isConnected(void)
	{
		return m_isConnected;
	}
}//namespace mqsProvider
