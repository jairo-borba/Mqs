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

#include "mqsProvider/WinNamedSharedMemAllocator.h"
#include <base/ExceptionInfo.h>
#include <appCore/Shortcuts.h>
#include <windows.h>
namespace mqsProvider
{
	WinNamedSharedMemAllocator::WinNamedSharedMemAllocator(void)
	{
		m_handle = 0;
	}
	WinNamedSharedMemAllocator::~WinNamedSharedMemAllocator(void)
	{
	}
	void WinNamedSharedMemAllocator::setSharedMemoryName( const std::string& a_sharedMemoryName )
	{
		m_sharedMemoryName = a_sharedMemoryName;
	}
	void* WinNamedSharedMemAllocator::specificAllocate(void)
	{
		m_handle = CreateFileMappingA(
			INVALID_HANDLE_VALUE,		// use paging file
			NULL,						// default security
			PAGE_READWRITE,				// read/write access
			0,							// maximum object size (high-order DWORD)
			this->sharedMemorySize(),	// maximum object size (low-order DWORD)
			m_sharedMemoryName.c_str());// name of mapping object

		RETURN_IF( m_handle == 0, 0 );

		void* l_memoryPtr = MapViewOfFile(
			m_handle,				// handle to map object
			FILE_MAP_ALL_ACCESS,	// read/write permission
			0,
			0,
			this->sharedMemorySize());

		return l_memoryPtr;
	}
	void WinNamedSharedMemAllocator::specificDeallocate(void)
	{
		UnmapViewOfFile(this->sharedMemoryPtr());
 		CloseHandle(m_handle);
	}
}//namespace mqsProvider
