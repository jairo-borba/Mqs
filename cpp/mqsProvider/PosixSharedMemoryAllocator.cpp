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
#include "mqsProvider/PosixSharedMemoryAllocator.h"
#include <appUtil/JJJException.h>
#include <appUtil/Shortcuts.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>

namespace mqsProvider
{
	PosixSharedMemoryAllocator::PosixSharedMemoryAllocator(void)
	{
		m_sharedMemoryID = 0;
		m_sharedMemoryID = -1;
	}
	PosixSharedMemoryAllocator::~PosixSharedMemoryAllocator(void)
	{
	}
	void PosixSharedMemoryAllocator::setFileToKey(
			const fileUtil::File& a_fileToKey )
	{
		m_fileToKey = a_fileToKey;
	}
	void* PosixSharedMemoryAllocator::specificAllocate(void)
	{
		key_t l_sharedMemoryKey = ftok( m_fileToKey.fullName().c_str(), 0 );
		m_sharedMemoryID =
				shmget(
						l_sharedMemoryKey ,
						this->sharedMemorySize(),
						IPC_CREAT | 0600 );
		RETURN_IF( m_sharedMemoryID == -1, 0 );

		//ANEXA SHARED MEMORY AO PROCESSO
		void* l_memoryPtr = shmat( m_sharedMemoryID, 0, 0 );
		if( l_memoryPtr == (void*)-1 ){
			shmctl( m_sharedMemoryID, IPC_RMID, 0 );
			l_memoryPtr = NULL;
		}

		return l_memoryPtr;
	}
	void PosixSharedMemoryAllocator::specificDeallocate(void)
	{
		shmdt( this->sharedMemoryPtr() );
		shmctl( m_sharedMemoryID, IPC_RMID, 0 );
	}
}//namespace mqsProvider

