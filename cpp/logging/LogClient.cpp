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

#include "logging/LogClient.h"
#include "logging/LoggersPool.h"
#include "base/ExceptionInfo.h"

namespace logging
{
	LogClient::LogClient(void)
	{
		m_pLoggersPool		= 0;
		m_pDefaultLogger	= 0;
	}
	LogClient::~LogClient(void)
	{
	}
	LogClient::LogClient(const LogClient& a_macroDataManip)
	{
		m_pLoggersPool		= 0;
		m_pDefaultLogger	= 0;
		this->assign( a_macroDataManip );
	}
	LogClient& LogClient::assign(const LogClient& a_macroDataManip)
	{
		m_pLoggersPool		= a_macroDataManip.m_pLoggersPool;
		m_pDefaultLogger	= a_macroDataManip.m_pDefaultLogger;
		m_defaultLoggerName	= a_macroDataManip.m_defaultLoggerName;

		return *this;
	}
	LogClient& LogClient::operator=(const LogClient& a_macroDataManip)
	{
		return this->assign( a_macroDataManip );
	}
	void LogClient::enableLogging( const LogClient& a_logClient )
	{
		m_pLoggersPool = a_logClient.m_pLoggersPool;
	}
	const LoggersPool* LogClient::loggersPool(void) const
	{
		return m_pLoggersPool;
	}
	void LogClient::setLoggersPool( const LoggersPool* a_pLoggersPool )
	{
		m_pLoggersPool = a_pLoggersPool;
	}
	const Logger* LogClient::log( const std::string& a_logName ) const
	{
		std::string l_name = a_logName.length() == 0 ? m_defaultLoggerName : a_logName;

		if( m_pLoggersPool == 0 ){
			throw base::ExceptionInfo( __FILE__, __LINE__, "Undefined LoggerPool" );
		}

		const Logger& l_logger = m_pLoggersPool->findLogger( l_name );

		return &l_logger;
	}
	const Logger* LogClient::log(void) const
	{
		if( m_pDefaultLogger == 0 ){
			if( m_pLoggersPool == 0 ){
				throw base::ExceptionInfo( __FILE__, __LINE__, "Undefined LoggerPool" );
			}
			m_pDefaultLogger = m_pLoggersPool->findLoggerPtr( m_defaultLoggerName );
		}

		return m_pDefaultLogger;
	}
	void LogClient::setDefaultLogName( const std::string& a_defaultLoggerName )
	{
		m_defaultLoggerName = a_defaultLoggerName;
	}

	const std::string& LogClient::defaultLoggerName(void) const
	{
		return m_defaultLoggerName;
	}

}//namespace logging
