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
#include "mqsProvider/PosixSharedMemoryAccess.h"
#include <appUtil/JJJException.h>
#include <appUtil/Shortcuts.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>

namespace mqsProvider
{
	PosixSharedMemoryAccess::PosixSharedMemoryAccess(void)
	{
		m_sharedMemoryID = -1;
	}
	PosixSharedMemoryAccess::~PosixSharedMemoryAccess(void)
	{
	}
	void PosixSharedMemoryAccess::setFileToKey(
			const fileUtil::File& a_fileToKey )
	{
		m_fileToKey = a_fileToKey;
	}
	void* PosixSharedMemoryAccess::specificBind(void)
	{
		key_t l_sharedMemoryKey = ftok( m_fileToKey.fullName().c_str(), 0 );
		m_sharedMemoryID = shmget( l_sharedMemoryKey , 0, 0 );
		RETURN_IF( m_sharedMemoryID == -1, 0 );

		void* l_memoryPtr = shmat( m_sharedMemoryID, 0, 0 );
		RETURN_IF( l_memoryPtr == (void*)-1, 0 );

		return l_memoryPtr;
	}
	void PosixSharedMemoryAccess::specificDetach(void)
	{
		shmdt( this->sharedMemoryPtr() );
	}
}//namespace mqsProvider

