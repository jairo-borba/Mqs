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

#include "mqsProvider/PosixBinSemaphoreClient.h"
#include <base/ExceptionInfo.h>
#include <appCore/Shortcuts.h>

namespace mqsProvider
{
	PosixBinSemaphoreClient::PosixBinSemaphoreClient(void)
	{
	}
	PosixBinSemaphoreClient::~PosixBinSemaphoreClient(void)
	{
	}

	void PosixBinSemaphoreClient::setFileToKey( const fileUtil::File& a_fileToKey )
	{
		m_binSemCli.setFileToKey( a_fileToKey );
	}
	bool PosixBinSemaphoreClient::open(void)
	{
		bool l_ret = m_binSemCli.open();

		return l_ret;
	}
	void PosixBinSemaphoreClient::close(void)
	{
		m_binSemCli.close();
	}
	enum PosixBinSemaphoreClient::ENTER_CRITICAL_AREA_STATUS PosixBinSemaphoreClient::enterCriticalArea( unsigned short int a_semaphoreNumber )
	{
		enum ENTER_CRITICAL_AREA_STATUS l_status = ENTER_FAILED;

		if( m_binSemCli.lock( a_semaphoreNumber ) ){
			l_status = ENTER_SUCCESS;
		}

		return l_status;
	}
	void PosixBinSemaphoreClient::exitCriticalArea( unsigned short int a_semaphoreNumber )
	{
		m_binSemCli.unlock( a_semaphoreNumber );
	}
	bool PosixBinSemaphoreClient::setMutex( unsigned short int a_semaphoreNumber, bool a_bOpen )
	{
		bool l_ret = m_binSemCli.setVal( a_semaphoreNumber, a_bOpen ? 1 : 0 );
		return l_ret;
	}

}//namespace mqsProvider

