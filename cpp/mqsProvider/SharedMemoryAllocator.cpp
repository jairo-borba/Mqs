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
#include "mqsProvider/SharedMemoryAllocator.h"
#include <appUtil/Shortcuts.h>

namespace mqsProvider
{
	SharedMemoryAllocator::SharedMemoryAllocator(void)
	{
		appUtil::initPointer( m_sharedMemoryPtr );
		m_sharedMemorySize = 0;
	}
	SharedMemoryAllocator::~SharedMemoryAllocator(void)
	{
	}
	bool SharedMemoryAllocator::allocate(
			unsigned int a_size )
	{
		bool l_ret = false;
		RETURN_IF( m_sharedMemorySize != 0 || a_size == 0, l_ret );
		m_sharedMemorySize = a_size;

		void* l_allocated = this->specificAllocate();
		if( l_allocated == 0 ){
			m_sharedMemorySize = 0;
		}

		m_sharedMemoryPtr = l_allocated;
		l_ret = true;

		return l_ret;
	}
	void SharedMemoryAllocator::deallocate(void)
	{
		VOID_RETURN_IF_NULL( m_sharedMemoryPtr );
		this->specificDeallocate();
		appUtil::initPointer( m_sharedMemoryPtr );
	}
	unsigned int SharedMemoryAllocator::sharedMemorySize(void) const
	{
		return m_sharedMemorySize;
	}
	void* SharedMemoryAllocator::sharedMemoryPtr(void)
	{
		return m_sharedMemoryPtr;
	}
}//namespace mqsProvider
