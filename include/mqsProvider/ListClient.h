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

#pragma once
#include <cstdio>
#include "mqsProvider/ListPtr.h"
#include "mqsProvider/Cluster.h"

namespace mqsProvider
{
	class ListClient
	{
	public:
		enum VALIDATION_STATUS
		{
			LST_CLI_VAL_OK						= 0,
			LST_LIST_PTR_NOT_DEFINED			= 1,
			LST_LIST_BUFFER_NOT_DEFINED			= 2,
			LST_CLI_VAL_INVALID_HEADER			= 3,
			LST_CLI_VAL_INVALID_TAIL			= 4,
			LST_CLI_VAL_INVALID_CLUSTER			= 5,
			LST_CLI_VAL_INVALID_PATH			= 6,
			LST_CLI_VAL_INVALID_COUNT_ELEMENTS	= 7
		};
		ListClient( ListPtr* a_listPtr, void* a_buffer );
		virtual ~ListClient(void);

		void setListPtr( ListPtr* a_listPtr );
		void setBuffer( void* a_buffer );

		unsigned int dequeue(void);
		bool enqueue( unsigned int a_bufferPosition );
		bool remove( unsigned int a_leavingPosition );

		unsigned int head(void);
		unsigned int count(void) const;

		enum VALIDATION_STATUS scanValidation(void) const;

	private:
		ListPtr* m_listPtr;
		void* m_buffer;
	};
}//namespace mqsProvider
