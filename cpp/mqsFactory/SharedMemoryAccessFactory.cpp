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

#include "mqsFactory/SharedMemoryAccessFactory.h"

#if _MSC_VER > 1600
#include <appCore/Shortcuts.h>
#include <appCore/SafeStringDef.h>
#include <appCore/OSEnvironment.h>
#include "mqsProvider/WinNamedSharedMemAccess.h"
#include "mqsFactory\DefEnvVarsNames.h"

namespace mqsFactory
{
	SharedMemoryAccessFactory::SharedMemoryAccessFactory(void)
	{
	}
	SharedMemoryAccessFactory::~SharedMemoryAccessFactory(void)
	{
	}
	mqsProvider::SharedMemoryAccess* SharedMemoryAccessFactory::create(void)
	{
		mqsProvider::WinNamedSharedMemAccess* l_pshm;
		appCore::initPointer(l_pshm);
		appCore::safeNew(l_pshm);
		char l_shmName[1024];
		appCore::OSEnvironment l_env;
		safeSPrintf(l_shmName, sizeof l_shmName, "Global\\%s", l_env.getRequiredEnv(ENVIRONMENT_VARIABLE_NAME_FOR_KEY_WORD).c_str());
		l_pshm->setSharedMemoryName( l_shmName );

		return l_pshm;
	}
}//namespace mqsFactory
#else
#	if defined(GCC)

#include <appCore/Shortcuts.h>
#include <appCore/SafeStringDef.h>
#include <appCore/OSEnvironment.h>
#include "mqsProvider/PosixSharedMemoryAccess.h"

namespace mqsFactory
{
	SharedMemoryAccessFactory::SharedMemoryAccessFactory(void)
	{
	}
	SharedMemoryAccessFactory::~SharedMemoryAccessFactory(void)
	{
	}
	mqsProvider::SharedMemoryAccess* SharedMemoryAccessFactory::create(void)
	{
		mqsProvider::PosixSharedMemoryAccess* l_pshm;
		appCore::initPointer(l_pshm);
		appCore::safeNew(l_pshm);
		char l_shmName[1024];
		appCore::OSEnvironment l_env;
		safeSPrintf( l_shmName, sizeof l_shmName, "%s", l_env.getEnv( ENVIRONMENT_VARIABLE_NAME_FOR_KEY_WORD ).c_str() );
		l_pshm->setFileToKey( fileUtil::File(l_shmName) );

		return l_pshm;
	}
}//namespace mqsFactory
#	endif
#endif
