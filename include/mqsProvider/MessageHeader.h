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
#include "mqsProvider/Cluster.h"
#include "mqsProvider/ListPtr.h"
#include "mqsProvider/date_t.h"

namespace mqsProvider
{
	typedef struct tagMessageHeader
	{
		unsigned int index;
		date_t creationDate;
		unsigned int messageSize;
		ListPtr messageClusters;
	} MessageHeader;
	inline MessageHeader* MSG_HEADER(
			void*a_buffer,
			unsigned int a_position)
	{
		return reinterpret_cast<MessageHeader*>( CLUSTER_DATA(a_buffer,a_position) );
	}
	inline void INIT_MSG_HEADER(
			void*a_buffer,
			unsigned int a_position)
	{
		MSG_HEADER(a_buffer,a_position)->index = INDEX_NULL;
		MSG_HEADER(a_buffer,a_position)->messageSize = 0;
		LOAD_SYSDATE( MSG_HEADER(a_buffer,a_position)->creationDate );
		LISTPTR_INIT( &MSG_HEADER(a_buffer,a_position)->messageClusters );
	}
	inline void INIT_MSG_HEADER(
			MessageHeader*a_msghdr)
	{
		a_msghdr->index = INDEX_NULL;
		a_msghdr->messageSize = 0;
		LOAD_SYSDATE( a_msghdr->creationDate );
		LISTPTR_INIT( &a_msghdr->messageClusters );
	}
}//namespace mqsProvider
