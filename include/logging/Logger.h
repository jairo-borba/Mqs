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

#include "logging/LogHandler.h"
#include "logging/Level.h"
#include "logging/LogMessagePreambule.h"
#include "base/Identificable.h"
#include <map>
#include <cstdarg>

namespace logging
{
	class Logger : public base::Identificable
	{
	public:
		Logger(void);
		Logger(const Logger& a_logger);
		virtual ~Logger(void);

		Logger& assign(const Logger& a_logger);
		Logger& operator=(const Logger& a_logger);

		void severe		( const char* a_format, ... ) const;
		void warning    ( const char* a_format, ... ) const;
		void info       ( const char* a_format, ... ) const;
		void config     ( const char* a_format, ... ) const;
		void fine       ( const char* a_format, ... ) const;
		void finer      ( const char* a_format, ... ) const;
		void finest     ( const char* a_format, ... ) const;

		void setLevel( const logging::Level& a_level );
		void setLogHandler( const LogHandler& a_logHandler );
		void setLogMessagePreambule( const LogMessagePreambule& a_logMessagePreambule );

		bool open(void);
		void close(void);

	private:
		mutable LogHandler* m_logHandler;
		LogMessagePreambule* m_preambule;
		logging::Level m_level;
	};
}//namespace logging
