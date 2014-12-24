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
 
 #include "appCore/ApplicationUnit.h"

namespace appCore
{
	
	ApplicationUnit::APPLICATION_SET_TYPE ApplicationUnit::m_setAppUnits;
	ApplicationUnit::APPLICATION_MAP_TYPE ApplicationUnit::m_mapApps;
	
	ApplicationUnit::ApplicationUnit( const std::string& a_name, const std::string& a_version )
	: m_name(a_name), m_version(a_version)
	{
		ApplicationUnit::m_setAppUnits.insert( this );
		ApplicationUnit::m_mapApps.insert( APPLICATION_MAP_PAIR(a_name,this) );
	}
	
	ApplicationUnit::~ApplicationUnit(void)
	{
	}
	
	const std::string& ApplicationUnit::name(void) const
	{
		return this->m_name;
	}
	
	const std::string& ApplicationUnit::version(void) const
	{
		return this->m_version;
	}
	
	ApplicationUnit* ApplicationUnit::findUnit( const std::string& a_name )
	{
		APPLICATION_MAP_TYPE::iterator l_it;
		
		l_it = ApplicationUnit::m_mapApps.find( a_name );
		if( l_it == ApplicationUnit::m_mapApps.end( ) ){
			return 0;
		}
		
		return l_it->second;
	}
	
	void ApplicationUnit::assert( const ApplicationUnit* a_pAppUnit )
	{
		APPLICATION_SET_TYPE::const_iterator it = ApplicationUnit::m_setAppUnits.find( a_pAppUnit );
		
		if( it == ApplicationUnit::m_setAppUnits.end( ) ){
			throw base::ExceptionInfo("Application unit not found!");
		}
	}
	
	void ApplicationUnit::addArg( const std::string& a_arg )
	{
		this->m_vectorArgs.push_back( a_arg );
	}
	
	int ApplicationUnit::start(void)
	{
		return 0;
	}
	
	int ApplicationUnit::end( int a_runReturnCode )
	{
		return a_runReturnCode;
	}
	
	int ApplicationUnit::end(const base::ExceptionInfo& a_e)
	{
		throw a_e;
	}
	
	const ApplicationUnit::TP_VECTOR_ARGS& ApplicationUnit::args(void) const
	{
		return this->m_vectorArgs;
	}
}//namespace appCore
