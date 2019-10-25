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
#include "mqsProvider/IndexDef.h"
#include <cstring>

namespace mqsProvider
{
	static const char CLTR_MCODE[] = "#Mqs*";
	typedef struct tagClusterHeader
	{
		char magicCode[sizeof CLTR_MCODE];
		unsigned int _this;
		unsigned int left;
		unsigned int right;
	} ClusterHeader;

	typedef struct tagCluster
	{
		ClusterHeader header;
		char data[1];
	} Cluster;

	inline unsigned int& CLUSTER_THIS(
			void*a_buffer,
			unsigned int a_position)
	{
		Cluster* l_cluster =
				reinterpret_cast<Cluster*>(&reinterpret_cast<char*>(a_buffer)[a_position]);
		return l_cluster->header._this;
	}
	inline char* CLUSTER_MAGIC_CODE(
			void*a_buffer,
			unsigned int a_position)
	{
		Cluster* l_cluster =
				reinterpret_cast<Cluster*>(&reinterpret_cast<char*>(a_buffer)[a_position]);
		return l_cluster->header.magicCode;
	}
	inline unsigned int& CLUSTER_LEFT(
			void*a_buffer,
			unsigned int a_position)
	{
		Cluster* l_cluster =
				reinterpret_cast<Cluster*>(&reinterpret_cast<char*>(a_buffer)[a_position]);
		return l_cluster->header.left;
	}
	inline unsigned int& CLUSTER_RIGHT(
			void*a_buffer,
			unsigned int a_position)
	{
		Cluster* l_cluster =
				reinterpret_cast<Cluster*>(&reinterpret_cast<char*>(a_buffer)[a_position]);
		return l_cluster->header.right;
	}
	inline char* CLUSTER_DATA(
			void*a_buffer,
			unsigned int a_position)
	{
		Cluster* l_cluster =
				reinterpret_cast<Cluster*>(&reinterpret_cast<char*>(a_buffer)[a_position]);
		return l_cluster->data;
	}
	inline void INIT_CLUSTER_HEADER(
			void*a_buffer,
			unsigned int a_position)
	{
		memcpy( CLUSTER_MAGIC_CODE(a_buffer,a_position), CLTR_MCODE, sizeof CLTR_MCODE );
		CLUSTER_THIS(a_buffer,a_position)	= a_position;
		CLUSTER_LEFT(a_buffer,a_position)	= INDEX_NULL;
		CLUSTER_RIGHT(a_buffer,a_position)	= INDEX_NULL;
	}
	inline void INIT_CLUSTER_DATA(
			void*a_buffer,
			unsigned int a_position,
			unsigned int a_clusterDataSize)
	{
		memset( CLUSTER_DATA(a_buffer,a_position), 0, a_clusterDataSize );
	}
	inline bool IS_CLUSTER_HEAD(
			void*a_buffer,
			unsigned int a_position)
	{
		bool l_ret = CLUSTER_RIGHT(a_buffer,a_position) == INDEX_NULL && CLUSTER_LEFT(a_buffer,a_position) != INDEX_NULL;
		return l_ret;
	}
	inline bool IS_CLUSTER_TAIL(
			void*a_buffer,
			unsigned int a_position)
	{
		bool l_ret = CLUSTER_RIGHT(a_buffer,a_position) != INDEX_NULL && CLUSTER_LEFT(a_buffer,a_position) == INDEX_NULL;
		return l_ret;
	}
	inline bool IS_CLUSTER_VALID(
			void*a_buffer,
			unsigned int a_position)
	{
		bool l_ret = CLUSTER_THIS(a_buffer,a_position) == a_position;
		if( !l_ret ) return false;

		l_ret = memcmp( CLUSTER_MAGIC_CODE(a_buffer,a_position), CLTR_MCODE, sizeof CLTR_MCODE ) == 0;
		return l_ret;
	}
	inline unsigned int CLUSTER_HEADER_SIZE(void)
	{
		return sizeof(ClusterHeader);
	}
}//namespace mqsProvider
