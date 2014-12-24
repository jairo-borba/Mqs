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

#include "logging/Logger.h"
#include "base/ExceptionInfo.h"
#include "appCore/SafeStringDef.h"
#include <cstdio>

namespace logging
{
	Logger::Logger(void)
	{
		m_logHandler = 0;
		m_preambule = 0;
	}
	Logger::Logger(const Logger& a_logger)
	{
		m_logHandler = 0;
		m_preambule = 0;
		this->assign( a_logger );
	}
	Logger::~Logger(void)
	{
		if( m_logHandler != 0 ){
			delete m_logHandler;
		}

		if( m_preambule != 0 ){
			delete m_preambule;
		}
	}
	Logger& Logger::assign(const Logger& a_logger)
	{
		if( a_logger.m_logHandler != 0 ){
			this->setLogHandler( *(a_logger.m_logHandler) );
		} else {
			m_logHandler = 0;
		}

		if( a_logger.m_preambule != 0 ){
			this->setLogMessagePreambule( *(a_logger.m_preambule) );
		} else {
			m_preambule = 0;
		}

		this->setLevel( a_logger.m_level );

		return *this;
	}
	Logger& Logger::operator=(const Logger& a_logger)
	{
		return this->assign( a_logger );
	}
	void Logger::setLevel( const logging::Level& a_level )
	{
		m_level = a_level;
	}
	void Logger::setLogHandler( const LogHandler& a_logHandler )
	{
		if( m_logHandler != 0 ){
			delete m_logHandler;
		}
		m_logHandler = a_logHandler.clone( );
	}
	bool Logger::open(void)
	{
		if( m_logHandler == 0 ){
			return false;
		}
		bool l_success = m_logHandler->open();
		return l_success;
	}
	void Logger::close(void)
	{
		if( m_logHandler == 0 ){
			return;
		}
		m_logHandler->close();
	}
	void Logger::setLogMessagePreambule( const LogMessagePreambule& a_preambule )
	{
		if( m_preambule != 0 ){
			delete m_preambule;
		}
		m_preambule = a_preambule.clone( );
	}
	void Logger::severe( const char* a_format, ... ) const
	{
		bool l_on = m_level.levelOn( Level::SEVERE );
		if( !l_on ){
			return;
		}
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_msgLog[16384];
		int l_size = safeVSPrintf( l_msgLog, sizeof l_msgLog, a_format, l_argList );
		va_end( l_argList );

		if( m_logHandler != 0 ){
			if( m_preambule != 0 ){
				std::string l_pre = m_preambule->make(Level::SEVERE);
				m_logHandler->write( l_pre.c_str(), l_pre.length() );
			}
			m_logHandler->write( l_msgLog, l_size );
			m_logHandler->write( "\n", 1 );
		}
	}
	void Logger::warning( const char* a_format, ... ) const
	{
		bool l_on = m_level.levelOn( Level::WARNING );
		if( !l_on ){
			return;
		}
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_msgLog[16384];
		int l_size = safeVSPrintf( l_msgLog, sizeof l_msgLog, a_format, l_argList );
		va_end( l_argList );

		if( m_logHandler != 0 ){
			if( m_preambule != 0 ){
				std::string l_pre = m_preambule->make(Level::WARNING);
				m_logHandler->write( l_pre.c_str(), l_pre.length() );
			}
			m_logHandler->write( l_msgLog, l_size );
			m_logHandler->write( "\n", 1 );
		}
	}
	void Logger::info( const char* a_format, ... ) const
	{
		bool l_on = m_level.levelOn( Level::INFO );
		if( !l_on ){
			return;
		}
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_msgLog[16384];
		int l_size = safeVSPrintf( l_msgLog, sizeof l_msgLog, a_format, l_argList );
		va_end( l_argList );

		if( m_logHandler != 0 ){
			if( m_preambule != 0 ){
				std::string l_pre = m_preambule->make(Level::INFO);
				m_logHandler->write( l_pre.c_str(), l_pre.length() );
			}
			m_logHandler->write( l_msgLog, l_size );
			m_logHandler->write( "\n", 1 );
		}
	}
	void Logger::config( const char* a_format, ... ) const
	{
		bool l_on = m_level.levelOn( Level::CONFIG );
		if( !l_on ){
			return;
		}
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_msgLog[16384];
		int l_size = safeVSPrintf( l_msgLog, sizeof l_msgLog, a_format, l_argList );
		va_end( l_argList );

		if( m_logHandler != 0 ){
			if( m_preambule != 0 ){
				std::string l_pre = m_preambule->make(Level::CONFIG);
				m_logHandler->write( l_pre.c_str(), l_pre.length() );
			}
			m_logHandler->write( l_msgLog, l_size );
			m_logHandler->write( "\n", 1 );
		}
	}
	void Logger::fine( const char* a_format, ... ) const
	{
		bool l_on = m_level.levelOn( Level::FINE );
		if( !l_on ){
			return;
		}
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_msgLog[16384];
		int l_size = safeVSPrintf( l_msgLog, sizeof l_msgLog, a_format, l_argList );
		va_end( l_argList );

		if( m_logHandler != 0 ){
			if( m_preambule != 0 ){
				std::string l_pre = m_preambule->make(Level::FINE);
				m_logHandler->write( l_pre.c_str(), l_pre.length() );
			}
			m_logHandler->write( l_msgLog, l_size );
			m_logHandler->write( "\n", 1 );
		}
	}
	void Logger::finer( const char* a_format, ... ) const
	{
		bool l_on = m_level.levelOn( Level::FINER );
		if( !l_on ){
			return;
		}
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_msgLog[16384];
		int l_size = safeVSPrintf( l_msgLog, sizeof l_msgLog, a_format, l_argList );
		va_end( l_argList );

		if( m_logHandler != 0 ){
			if( m_preambule != 0 ){
				std::string l_pre = m_preambule->make(Level::FINER);
				m_logHandler->write( l_pre.c_str(), l_pre.length() );
			}
			m_logHandler->write( l_msgLog, l_size );
			m_logHandler->write( "\n", 1 );
		}
	}
	void Logger::finest( const char* a_format, ... ) const
	{
		bool l_on = m_level.levelOn( Level::FINEST );
		if( !l_on ){
			return;
		}
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_msgLog[16384];
		int l_size = safeVSPrintf( l_msgLog, sizeof l_msgLog, a_format, l_argList );
		va_end( l_argList );

		if( m_logHandler != 0 ){
			if( m_preambule != 0 ){
				std::string l_pre = m_preambule->make(Level::FINEST);
				m_logHandler->write( l_pre.c_str(), l_pre.length() );
			}
			m_logHandler->write( l_msgLog, l_size );
			m_logHandler->write( "\n", 1 );
		}
	}
}//namespace logging
