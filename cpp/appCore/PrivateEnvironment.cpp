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

#include "appCore/PrivateEnvironment.h"
#include "base/ExceptionInfo.h"
#include "appCore/SafeStringDef.h"
#include <cstring>
#include <cstdio>

namespace appCore
{
	PrivateEnvironment::PrivateEnvironment(void)
	{
	}
	PrivateEnvironment::~PrivateEnvironment(void)
	{
	}
	const std::string& PrivateEnvironment::value( const std::string& a_varName ) const
	{
		MAP_VARS::const_iterator l_it = m_gnsVariables.find( a_varName );

		if( l_it == m_gnsVariables.end() ){
			throw base::ExceptionInfo( __FILE__, __LINE__,
				"Genius Environment Variable <%s> not defined", a_varName.c_str() );
		}

		return l_it->second;
	}

	void PrivateEnvironment::addVariable( const std::string& a_varName, const std::string& a_value )
	{
		m_gnsVariables.insert( PAIR_VARS( a_varName, a_value ) );
	}

}//namespace appCore
