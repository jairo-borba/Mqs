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


#include "appCore/OSEnvironment.h"
#include "appCore/Shortcuts.h"
#include "base/ExceptionInfo.h"
#include <cstdlib>

namespace appCore
{
	OSEnvironment::OSEnvironment(void)
	{
		
	}

	OSEnvironment::~OSEnvironment(void)
	{
	}

	std::string OSEnvironment::getEnv( const std::string& envName ) const
	{
		char* pEnv = NULL;
		size_t l_envVarSize = 0;

		_dupenv_s(&pEnv, &l_envVarSize, envName.c_str());
		
		if (l_envVarSize != 0){
			return std::string(pEnv, l_envVarSize );
		}

		return std::string();
	}

	std::string OSEnvironment::getRequiredEnv( const std::string& envName ) const
	{
		std::string l_ret = this->getEnv(envName);

		appCore::appAssert(l_ret.length() != 0, __FILE__, __LINE__,
			"Required environment variable <%s> has not been defined", envName.c_str());

		return l_ret;
	}
}//namespace appCore
