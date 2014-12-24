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

#include "mqsProvider/WinMutexClient.h"
#include <base/ExceptionInfo.h>
#include <windows.h>
#include <appCore/SafeStringDef.h>
#include <cstring>
#include <appCore/Shortcuts.h>
namespace mqsProvider
{
	WinMutexClient::WinMutexClient(void)
	{
	}
	WinMutexClient::~WinMutexClient(void)
	{
	}
	bool WinMutexClient::open(void)
	{
		return true;
	}
	void WinMutexClient::close(void)
	{
		std::map<unsigned short int,void*>::iterator l_it;
		for( l_it = m_handles.begin(); l_it != m_handles.end(); ++l_it ){
			CloseHandle( l_it->second );
		}
		m_handles.clear();
	}
	enum WinMutexClient::ENTER_CRITICAL_AREA_STATUS WinMutexClient::enterCriticalArea( unsigned short int a_semaphoreNumber )
	{
		void* l_handle = 0;
		std::map<unsigned short int,void*>::iterator l_it = m_handles.find( a_semaphoreNumber );
		if( l_it == m_handles.end( ) ){
			char l_eventName[1024];
			safeSPrintf( l_eventName, sizeof l_eventName, "%s%03d", m_eventName.c_str(), a_semaphoreNumber );
			l_handle = OpenEventA( EVENT_ALL_ACCESS, FALSE, l_eventName );
			RETURN_IF( l_handle == 0, ENTER_FAILED );
			m_handles.insert( std::pair<unsigned short int,void*>( a_semaphoreNumber, l_handle ) );
		} else {
			l_handle = l_it->second;
		}
		
		DWORD l_dwWaitResult;
		enum ENTER_CRITICAL_AREA_STATUS l_status;
		DWORD l_miliseconds = INFINITE;

		l_dwWaitResult = WaitForSingleObject( l_handle, l_miliseconds );
		switch( l_dwWaitResult )
		{
		case WAIT_OBJECT_0: l_status = ENTER_SUCCESS;break;
		case WAIT_TIMEOUT:  l_status = ENTER_TIMEOUT;break;
		case WAIT_FAILED:
		case WAIT_ABANDONED:
		default:l_status = ENTER_FAILED;break;
		}

		return l_status;
	}
	bool WinMutexClient::setMutex( unsigned short int a_semaphoreNumber, bool a_bOpen )
	{
		void* l_handle = 0;
		std::map<unsigned short int,void*>::iterator l_it = m_handles.find( a_semaphoreNumber );
		if( l_it == m_handles.end( ) ){
			char l_eventName[1024];
			safeSPrintf( l_eventName, sizeof l_eventName, "%s%03d", m_eventName.c_str(), a_semaphoreNumber );
			l_handle = OpenEventA( EVENT_ALL_ACCESS, FALSE, l_eventName );
			RETURN_IF( l_handle == 0, false );
			m_handles.insert( std::pair<unsigned short int,void*>( a_semaphoreNumber, l_handle ) );
		} else {
			l_handle = l_it->second;
		}
		
		bool l_ret = a_bOpen ? SetEvent( l_handle ) == TRUE : ResetEvent( l_handle ) == TRUE;
		
		return l_ret;
	}
	void WinMutexClient::exitCriticalArea( unsigned short int a_semaphoreNumber )
	{
		void* l_handle = 0;
		std::map<unsigned short int,void*>::iterator l_it = m_handles.find( a_semaphoreNumber );
		if( l_it == m_handles.end( ) ){
			char l_eventName[1024];
			safeSPrintf( l_eventName, sizeof l_eventName, "%s%03d", m_eventName.c_str(), a_semaphoreNumber );
			l_handle = OpenEventA( EVENT_ALL_ACCESS, FALSE, l_eventName );
			m_handles.insert( std::pair<unsigned short int,void*>( a_semaphoreNumber, l_handle ) );
		} else {
			l_handle = l_it->second;
		}

		bool l_ret = SetEvent( l_handle ) != 0;
		appCore::appAssert( l_ret,
			"Error setting Event <%s> errno=<%d>", m_eventName.c_str(),
			GetLastError()
		);
	}
	void WinMutexClient::setEventName( const std::string& a_eventName )
	{
		m_eventName = a_eventName;
	}
}//namespace mqsProvider
