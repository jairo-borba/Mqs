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
#include <string>
#include "mqsProvider/MutualExclusionClient.h"
#include <unixSystem/semaphore/BinarySemaphoreClient.h>
namespace mqsProvider
{
	class PosixBinSemaphoreClient : public MutualExclusionClient
	{
	public:
		PosixBinSemaphoreClient(void);
		virtual ~PosixBinSemaphoreClient(void);

		bool open(void);
		void close(void);
		enum ENTER_CRITICAL_AREA_STATUS enterCriticalArea( unsigned short int a_semaphoreNumber );
		void exitCriticalArea( unsigned short int a_semaphoreNumber );
		void setFileToKey( const fileUtil::File& a_fileToKey );
		bool setMutex( unsigned short int a_semaphoreNumber, bool a_bOpen = true );

	private:
		unixSystem::semaphore::BinarySemaphoreClient m_binSemCli;
	};
}//namespace mqsProvider
