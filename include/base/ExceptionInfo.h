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

namespace base
{
	class ExceptionInfo
	{
	public:
		ExceptionInfo(const char* a_function, const char* a_file, int a_line, const char* a_format, ...);
		ExceptionInfo(const char* a_function, const char* a_format, ...);
		ExceptionInfo(const char* a_file, int a_line, const char* a_format, ...);
		ExceptionInfo(const char* a_format, ...);
		virtual ~ExceptionInfo(void);
		const char* what(void) const;
		const char* file(void) const;
		int line(void) const;
		static const int MAX_MESSAGE_SIZE = 4096; //4K
	private:
		char m_function[512];
		char m_file[512];
		int m_line;
		char m_message[MAX_MESSAGE_SIZE];
	};
}//namespace base
