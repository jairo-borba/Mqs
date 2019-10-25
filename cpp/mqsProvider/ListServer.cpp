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
#include "mqsProvider/ListServer.h"
#include "mqsProvider/ListHeader.h"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <appUtil/SafeStringDef.h>
#include <appUtil/JJJException.h>
#include "mqsProvider/Cluster.h"
#include "mqsProvider/ListClient.h"
#include <appUtil/Shortcuts.h>
namespace mqsProvider
{
	ListServer::ListServer(void)
	{
		m_clusterDataSize	= 0;
		m_numOfClusters		= 0;
		m_bufferSize		= 0;
		appUtil::initPointer( m_listHeader );
		appUtil::initPointer( m_buffer );
	}
	ListServer::~ListServer(void)
	{
	}
	unsigned int ListServer::bufferRequiredSize(void) const
	{
		appUtil::assert( m_clusterDataSize != 0 && m_numOfClusters != 0,
			"ListServer::memoryRequiredSize called, buffer size <%d> and num of buffers <%d> must to be defined",
			m_clusterDataSize, m_numOfClusters
		);
	
		unsigned int l_requiredSize = ( m_clusterDataSize + sizeof( ClusterHeader ) ) * m_numOfClusters;

		return l_requiredSize;
	}
	void ListServer::setListHeader(
			ListHeader* a_listHeader )
	{
		m_listHeader = a_listHeader;
	}
	void ListServer::setBuffer(
			void* a_bufferPtr,
			unsigned int a_bufferSize )
	{
		appUtil::assert( m_clusterDataSize != 0 && m_numOfClusters != 0,
			"ListServer::setBuffer called, cluster data size <%d> and num of clusters <%d> must to be defined, "
			"then call memoryRequiredSize to get buffer requidred size. After call setBuffer with the, "
			"buffer properly allocated (with required size)",
			m_clusterDataSize, m_numOfClusters
		);

		appUtil::assert( a_bufferSize == this->bufferRequiredSize(),
			"The buffer size must to be exactly as defined by memoryRequiredSize() method" );

		appUtil::assertPointer( a_bufferPtr );
		m_buffer = a_bufferPtr;
	}
	void* ListServer::bufferPtr(void)
	{
		return m_buffer;
	}
	void ListServer::setClusterDataSize(
			unsigned int a_clusterDataSize)
	{
		m_clusterDataSize = a_clusterDataSize;
	}
	void ListServer::setNumOfClusters(
			unsigned int a_numOfClusters)
	{
		m_numOfClusters = a_numOfClusters;
	}
	bool ListServer::create(void)
	{
		appUtil::assertPointer( m_listHeader );
		appUtil::assertPointer( m_buffer );
		
		INIT_LIST_HEADER(m_listHeader, *this, m_numOfClusters, m_clusterDataSize );

		m_listHeader->mainList.countElements	= m_numOfClusters;		
		unsigned int l_previousPosition			= INDEX_NULL;
		
		for( unsigned int sl_clusterNum = 0; sl_clusterNum < m_numOfClusters; ++sl_clusterNum ){
			
			unsigned int l_currentPosition = sl_clusterNum * (sizeof(ClusterHeader)+m_clusterDataSize);
			INIT_CLUSTER_HEADER(m_buffer,l_currentPosition);
			INIT_CLUSTER_DATA(m_buffer,l_currentPosition,m_clusterDataSize);
			CLUSTER_LEFT(m_buffer,l_currentPosition)	= l_previousPosition;
			if( sl_clusterNum == 0 ){
				m_listHeader->mainList.tail = l_currentPosition;
				unsigned int sl_rightCluster = (sl_clusterNum+1) * (sizeof(ClusterHeader)+m_clusterDataSize);
				CLUSTER_RIGHT(m_buffer,l_currentPosition) = sl_rightCluster;
			} else if( sl_clusterNum == m_numOfClusters - 1 ){
				CLUSTER_RIGHT(m_buffer,l_currentPosition) = INDEX_NULL;
				m_listHeader->mainList.head = l_currentPosition;
			} else {
				unsigned int sl_rightCluster = (sl_clusterNum+1) * (sizeof(ClusterHeader)+m_clusterDataSize);
				CLUSTER_RIGHT(m_buffer,l_currentPosition) = sl_rightCluster;
			}
			
			l_previousPosition = l_currentPosition;
		}
		mqsProvider::ListClient l_cli( &m_listHeader->mainList, m_buffer );
		return true;
	}
}//namespace mqsProvider
