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
#define CONTINUE_IF(CONDITION)						if(CONDITION) continue;
#define BREAK_IF(CONDITION)							if(CONDITION) break;
#define RETURN_IF(CONDITION,RETURN_VALUE)			if(CONDITION) return RETURN_VALUE;
#define VOID_RETURN_IF(CONDITION)					if(CONDITION) return;
#define RETURN_IF_NULL(POINTER,RETURN_VALUE)		if(POINTER==NULL) return RETURN_VALUE;
#define RETURN_IF_ZERO(VALUE,RETURN_VALUE)			if(VALUE==0) return RETURN_VALUE;
#define VOID_RETURN_IF_ZERO(VALUE)					if(VALUE==0) return;
#define VOID_RETURN_IF_NULL(POINTER)				if(POINTER==NULL) return;

#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <typeinfo>
#include "appCore/SafeStringDef.h"
#include <base/ExceptionInfo.h>

namespace appCore
{
	inline void appAssert( bool a_condition, const char* a_format, ...){
		VOID_RETURN_IF(a_condition);
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_temp[base::ExceptionInfo::MAX_MESSAGE_SIZE];
		safeVSPrintf( l_temp, sizeof l_temp, a_format, l_argList );
		va_end( l_argList );

		base::ExceptionInfo e( l_temp );
		throw e;
	}
	inline void appAssert( bool a_condition, const char* a_function, const char* a_format, ...){
		VOID_RETURN_IF(a_condition);
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_temp[base::ExceptionInfo::MAX_MESSAGE_SIZE];
		safeVSPrintf( l_temp, sizeof l_temp, a_format, l_argList );
		va_end( l_argList );

		base::ExceptionInfo e( a_function, l_temp );
		throw e;
	}
	inline void appAssert( bool a_condition, const char* a_file, int a_line, const char* a_format, ...){
		VOID_RETURN_IF(a_condition);
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_temp[base::ExceptionInfo::MAX_MESSAGE_SIZE];
		safeVSPrintf( l_temp, sizeof l_temp, a_format, l_argList );
		va_end( l_argList );

		base::ExceptionInfo e( a_file, a_line, l_temp );
		throw e;
	}
	inline void appAssert( bool a_condition, const char* a_function, const char* a_file, int a_line, const char* a_format, ...){
		VOID_RETURN_IF(a_condition);
		va_list l_argList;
		va_start( l_argList, a_format );
		char l_temp[base::ExceptionInfo::MAX_MESSAGE_SIZE];
		safeVSPrintf( l_temp, sizeof l_temp, a_format, l_argList );
		va_end( l_argList );

		base::ExceptionInfo e( a_function, a_file, a_line, l_temp );
		throw e;
	}
	template<class TYPE> void appAssertPointer( TYPE* a_pointer, const char* a_file, int a_line ){
		if( a_pointer == NULL ){
			throw base::ExceptionInfo( a_file, a_line, "Invalid pointer of type<%s>", typeid(TYPE).name() );
		}
	}
	template<class TYPE> void appAssertPointer( TYPE* a_pointer, const char* a_function ){
		if( a_pointer == NULL ){
			throw base::ExceptionInfo( a_function, "Invalid pointer of type<%s>", typeid(TYPE).name() );
		}
	}
	template<class TYPE> void appAssertPointer( TYPE* a_pointer ){
		if( a_pointer == NULL ){
			throw base::ExceptionInfo( "Invalid pointer of type<%s>", typeid(TYPE).name() );
		}
	}
	template<class TYPE> void safeNew( TYPE*& a_pointer, const char* a_file, int a_line ){
		a_pointer = new TYPE;
		appAssertPointer( a_pointer, a_file, a_line );
	}
	template<class TYPE> void safeNew( TYPE*& a_pointer, const char* a_function ){
		a_pointer = new TYPE;
		appAssertPointer( a_pointer, a_function );
	}
	template<class TYPE> void safeNew( TYPE*& a_pointer ){
		a_pointer = new TYPE;
		appAssertPointer( a_pointer );
	}
	template<class TYPE> void safeNewArray( TYPE*& a_pointer, size_t a_countElements ){
		a_pointer = new TYPE[a_countElements];
	}
	template<class TYPE> void initPointer( TYPE*& a_pointer ){
		a_pointer = NULL;
	}
	template<class TYPE> void safeDelete( TYPE*& a_pointer ){
		if( a_pointer != NULL ){
			delete a_pointer;
			a_pointer = NULL;
		}
	}
	template<class TYPE> void safeDeleteArray( TYPE*& a_pointer ){
		if( a_pointer != NULL ){
			delete[] a_pointer;
			a_pointer = NULL;
		}
	}
}//namespace appCore
