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

#include <cstdarg>
#include <cstring>
#include <cstdio>
#include "base/ExceptionInfo.h"
#include "appCore/SafeStringDef.h"
#include <cstdio>
namespace base
{
	ExceptionInfo::ExceptionInfo(const char* a_format, ...)
	{
		memset( m_function, 0, sizeof m_function );
		memset( m_file, 0, sizeof m_file );
		m_line = 0;
		va_list l_argList;
		va_start( l_argList, a_format );
		safeVSPrintf( m_message, sizeof m_message, a_format, l_argList );
		va_end( l_argList );
	}

	ExceptionInfo::ExceptionInfo(const char* a_function, const char* a_format, ...)
	{
		safeSPrintf( m_function, sizeof m_function, "%s", a_function );
		memset( m_file, 0, sizeof m_file );
		m_line = 0;
		va_list l_argList;
		va_start( l_argList, a_format );
		safeVSPrintf( m_message, sizeof m_message, a_format, l_argList );
		va_end( l_argList );
	}

	ExceptionInfo::ExceptionInfo(const char* a_function, const char* a_file, int a_line, const char* a_format, ...)
	{
		safeSPrintf( m_function, sizeof m_function, "%s", a_function );
		safeSPrintf( m_file, sizeof m_file, "%s", a_file );
		m_line = a_line;
		va_list l_argList;
		va_start( l_argList, a_format );
		safeVSPrintf( m_message, sizeof m_message, a_format, l_argList );
		va_end( l_argList );
	}

	ExceptionInfo::ExceptionInfo(const char* a_file, int a_line, const char* a_format, ...)
	{
		memset( m_function, 0, sizeof m_function );
		safeSPrintf( m_file, sizeof m_file, "%s", a_file );
		m_line = a_line;
		va_list l_argList;
		va_start( l_argList, a_format );
		safeVSPrintf( m_message, sizeof m_message, a_format, l_argList );
		va_end( l_argList );
	}
	ExceptionInfo::~ExceptionInfo(void)
	{
	}
	const char* ExceptionInfo::what(void) const
	{
		return this->m_message;
	}
	const char* ExceptionInfo::file(void) const
	{
		return this->m_file;
	}
	int ExceptionInfo::line(void) const
	{
		return this->m_line;
	}
}//namespace base

