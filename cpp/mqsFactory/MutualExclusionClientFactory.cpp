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
#include "mqsFactory/MutualExclusionClientFactory.h"

#if defined(_MSCVER)
#include "mqsProvider/WinMutexClient.h"
#include <appUtil/Shortcuts.h>
#include <appUtil/OSEnvironment.h>
namespace mqsFactory
{
	MutualExclusionClientFactory::MutualExclusionClientFactory(void)
	{
	}
	MutualExclusionClientFactory::~MutualExclusionClientFactory(void)
	{
	}
	mqsProvider::MutualExclusionClient* MutualExclusionClientFactory::create(void)
	{
		mqsProvider::WinMutexClient* l_pmtx;
		appUtil::initPointer(l_pmtx);
		appUtil::safeNew(l_pmtx);
		appUtil::OSEnvironment l_env;
		l_pmtx->setEventName( l_env.getEnv( "MQS-Provider-Instance" ).c_str() );

		return l_pmtx;
	}
}//namespace mqsFactory
#else
#	if defined(__GNUC__)
#include "mqsProvider/PosixBinSemaphoreClient.h"
#include <appUtil/Shortcuts.h>
#include <appUtil/OSEnvironment.h>
namespace mqsFactory
{
	MutualExclusionClientFactory::MutualExclusionClientFactory(void)
	{
	}
	MutualExclusionClientFactory::~MutualExclusionClientFactory(void)
	{
	}
	mqsProvider::MutualExclusionClient* MutualExclusionClientFactory::create(void)
	{
		mqsProvider::PosixBinSemaphoreClient* l_pmtx;
		appUtil::initPointer(l_pmtx);
		appUtil::safeNew(l_pmtx);
		appUtil::OSEnvironment l_env;
		l_pmtx->setFileToKey( fileUtil::File(l_env.getEnv( "MQS_PROVIDER_ID" )) );

		return l_pmtx;
	}
}//namespace mqsFactory
#	endif
#endif
