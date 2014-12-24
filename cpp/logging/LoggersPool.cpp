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

#include "logging/LoggersPool.h"

namespace logging
{
	LoggersPool::LoggersPool(void)
	{
	}
	LoggersPool::~LoggersPool(void)
	{
	}
	const Logger& LoggersPool::findLogger( const std::string& a_loggerName ) const
	{
		TP_MAP::const_iterator l_it = m_mapLoggers.find( a_loggerName );

		if( m_mapLoggers.end() != l_it ){
			return l_it->second;
		}
		return m_loggerDummy;
	}
	const Logger* LoggersPool::findLoggerPtr( const std::string& a_loggerName ) const
	{
		TP_MAP::const_iterator l_it = m_mapLoggers.find( a_loggerName );

		if( m_mapLoggers.end() != l_it ){
			return &(l_it->second);
		}
		return &m_loggerDummy;
	}

	bool LoggersPool::addLogger( const Logger& a_logger )
	{
		std::pair<TP_MAP::iterator,bool> l_insertRet;
		l_insertRet = m_mapLoggers.insert( TP_PAIR( a_logger.name(), a_logger ) );
		
		return l_insertRet.second;
	}

	void LoggersPool::clear(void)
	{
		m_mapLoggers.clear();
	}

	bool LoggersPool::openAll(void)
	{
		TP_MAP::iterator l_it;

		for( l_it = m_mapLoggers.begin(); l_it != m_mapLoggers.end(); ++l_it ){
			if( !l_it->second.open() ){
				return false;
			}
		}

		return true;
	}
	
	void LoggersPool::closeAll(void)
	{
		TP_MAP::iterator l_it;

		for( l_it = m_mapLoggers.begin(); l_it != m_mapLoggers.end(); ++l_it ){
			l_it->second.close();
		}
	}
}//namespace genius
