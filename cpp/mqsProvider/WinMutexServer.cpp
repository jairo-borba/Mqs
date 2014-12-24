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

#include "mqsProvider/WinMutexServer.h"
#include <windows.h>
#include <base/ExceptionInfo.h>
#include <appCore/SafeStringDef.h>
#include <cstring>
namespace mqsProvider
{
	WinMutexServer::WinMutexServer(void)
	{
	}
	WinMutexServer::~WinMutexServer(void)
	{
	}
	bool WinMutexServer::create(void)
	{
		char l_eventName[1024];
		for( size_t i = 0; i < this->numOfSemaphores(); ++i ){
			safeSPrintf( l_eventName, sizeof l_eventName, "%s%03d", m_eventName.c_str(), i );
			void* sl_handle = CreateEventA( NULL, FALSE, TRUE, l_eventName );
			m_handles.push_back( sl_handle );
			if( sl_handle == 0 ){
				this->destroy();
				return false;
			}
		}
		
		return true;
	}
	void WinMutexServer::destroy(void)
	{
		for( size_t i = 0; i < this->numOfSemaphores(); ++i ){
			CloseHandle( m_handles[i] );
		}
		m_handles.clear();
	}
	void WinMutexServer::setEventName( const std::string& a_eventName )
	{
		m_eventName = a_eventName;
	}
}//namespace mqsProvider
