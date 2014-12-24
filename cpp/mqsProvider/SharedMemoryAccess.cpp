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

#include "mqsProvider/SharedMemoryAccess.h"
#include <appCore/Shortcuts.h>

namespace mqsProvider
{
	SharedMemoryAccess::SharedMemoryAccess(void)
	{
		appCore::initPointer( m_sharedMemoryPtr );
	}
	SharedMemoryAccess::~SharedMemoryAccess(void)
	{
	}
	bool SharedMemoryAccess::bind(void)
	{
		m_sharedMemoryPtr = this->specificBind();
		bool l_ret = m_sharedMemoryPtr != 0;

		return l_ret;
	}
	void SharedMemoryAccess::detach(void)
	{
		VOID_RETURN_IF_NULL( m_sharedMemoryPtr );
		this->specificDetach();
		appCore::initPointer( m_sharedMemoryPtr );
	}
	void* SharedMemoryAccess::sharedMemoryPtr(void)
	{
		return m_sharedMemoryPtr;
	}

}//namespace mqsProvider
