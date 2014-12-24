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

#include "mqsProvider/WinNamedSharedMemAccess.h"
#include <windows.h>

namespace mqsProvider
{
	WinNamedSharedMemAccess::WinNamedSharedMemAccess(void)
	{
		m_handle = NULL;
	}
	WinNamedSharedMemAccess::~WinNamedSharedMemAccess(void)
	{
	}
	void WinNamedSharedMemAccess::setSharedMemoryName( const std::string& a_sharedMemoryName )
	{
		m_sharedMemoryName = a_sharedMemoryName;
	}
	void* WinNamedSharedMemAccess::specificBind(void)
	{
		m_handle = OpenFileMappingA(
			FILE_MAP_ALL_ACCESS,			// read/write access
			FALSE,							// do not inherit the name
			m_sharedMemoryName.c_str() );	// name of mapping object

		void* l_memoryPtr = (LPTSTR) MapViewOfFile(
			m_handle,				// handle to map object
			FILE_MAP_ALL_ACCESS,	// read/write permission
			0,
			0,
			0	);

		return l_memoryPtr;
	}
	void WinNamedSharedMemAccess::specificDetach(void)
	{
		UnmapViewOfFile(this->sharedMemoryPtr());
 		CloseHandle(m_handle);
	}
}//namespace mqsProvider
