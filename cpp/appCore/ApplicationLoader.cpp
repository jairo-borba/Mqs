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
 
#include <cstdio>
#include "appCore/ApplicationLoader.h"
#include "base/ExceptionInfo.h"
#include "appCore/ApplicationUnit.h"

namespace appCore
{
	ApplicationLoader::ApplicationLoader(void)
	{
		this->m_appUnit = 0;
	}
	
	ApplicationLoader::ApplicationLoader( ApplicationUnit* a_pAppUnit )
	{
		this->m_appUnit = a_pAppUnit;
		ApplicationUnit::assert( this->m_appUnit );
	}
	
	ApplicationLoader::~ApplicationLoader(void)
	{
	}

	void ApplicationLoader::setUnit( ApplicationUnit* a_pAppUnit )
	{
		this->m_appUnit = a_pAppUnit;
		ApplicationUnit::assert( this->m_appUnit );
	}
	
	void ApplicationLoader::loadArgs( int argc, char* argv[] )
	{
		ApplicationUnit::assert(this->m_appUnit);
		for( int l_i = 0; l_i < argc; ++l_i ){
			this->m_appUnit->addArg( argv[l_i] );
		}
	}
		
	int ApplicationLoader::run(void)
	{
		int l_startRet;
		int l_runRet;
		int l_mainRet = -1;
		
		ApplicationUnit::assert( this->m_appUnit );
			
		try
		{
			l_startRet = this->m_appUnit->start();
			
			if( l_startRet == 0 ){
				l_runRet = this->m_appUnit->run();
				l_mainRet = this->m_appUnit->end(l_runRet);
			} else {
				l_mainRet = this->m_appUnit->end(l_startRet);
			}
			
		} catch( const base::ExceptionInfo& e ) {			
			try{
				l_mainRet = this->m_appUnit->end(e);
			}
			catch (const base::ExceptionInfo& e) {
				fprintf( stderr, "Exception: %s\n", e.what() );
			}
		}
		
		return l_mainRet;
	}
}//namespace appCore
