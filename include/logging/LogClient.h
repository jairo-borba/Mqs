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
#include "logging/Logger.h"
namespace logging
{
	class LoggersPool;

	class LogClient
	{
	public:
		LogClient(void);
		LogClient(const LogClient& a_macroDataManip);
		virtual ~LogClient(void);

		void setLoggersPool( const LoggersPool* a_pLoggersPool );
		const Logger* log( const std::string& a_logName ) const;
		const Logger* log(void) const;
		void setDefaultLogName( const std::string& a_defaultLoggerName );
		void enableLogging( const LogClient& a_logClient );
		const std::string& defaultLoggerName(void) const;
		const LoggersPool* loggersPool(void) const;

		LogClient& assign(const LogClient& a_macroDataManip);
		LogClient& operator=(const LogClient& a_macroDataManip);

	private:
		const LoggersPool* m_pLoggersPool;
		std::string m_defaultLoggerName;
		mutable const Logger* m_pDefaultLogger;
	};
}//namespace logging
