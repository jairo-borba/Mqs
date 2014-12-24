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
#include "logging/Logger.h"

namespace logging
{
	class LoggersPool
	{
	public:
		LoggersPool(void);
		virtual ~LoggersPool(void);

		const Logger& findLogger( const std::string& a_loggerName ) const;
		const Logger* findLoggerPtr( const std::string& a_loggerName ) const;
		bool addLogger( const Logger& a_logger );
		bool openAll(void);
		void closeAll(void);
		void clear(void);
	private:
		typedef std::map<std::string,Logger> TP_MAP;
		typedef std::pair<std::string,Logger> TP_PAIR;
		TP_MAP m_mapLoggers;
		Logger m_loggerDummy;
	};
}//namespace logging
