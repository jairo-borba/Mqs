/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 jairo-borba jairo.borba.junior@gmail.com
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
#include "mqsProvider/ListPtr.h"
#include <cstring>
#include <appUtil/JJJException.h>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <appUtil/SafeStringDef.h>
#include <base/Identificable.h>
#include "mqsProvider/date_t.h"
#include <appUtil/Shortcuts.h>
namespace mqsProvider
{
	static const unsigned int MAX_NAME_SIZE			= 64;
	static const unsigned int MAX_DESCRIPTION_SIZE	= 256;
	static const char FREELISTHDR_MAGIC_CODE[] = "MQS_@JJJ_FREELIST_HEADER";

	typedef struct tagListHeader
	{
		char magicCode[sizeof FREELISTHDR_MAGIC_CODE];
		unsigned int numClusters;
		unsigned int clusterDataSize;
		char label[MAX_NAME_SIZE+1];
		char description[MAX_DESCRIPTION_SIZE+1];
		date_t creationDate;
		ListPtr mainList;
	} ListHeader;

	inline ListHeader* LIST_HEADER(
			void* a_buffer )
	{
		ListHeader* l_listHeader = reinterpret_cast<ListHeader*>( a_buffer );
		int l_cmpRet =
				memcmp(
						l_listHeader->magicCode,
						FREELISTHDR_MAGIC_CODE,
						sizeof FREELISTHDR_MAGIC_CODE );
		appUtil::assert( l_cmpRet != 0, "Invalid List Header Pointer" );

		return l_listHeader;
	}

	inline void INIT_LIST_HEADER(
		ListHeader* a_listHeader,
		const base::Identificable& a_identification,
		unsigned int a_numClusters = 0, unsigned int a_clusterDataSize = 0 )
	{

		memcpy(
				a_listHeader->magicCode,
				FREELISTHDR_MAGIC_CODE,
				sizeof a_listHeader->magicCode );
		a_listHeader->numClusters		= a_numClusters;
		a_listHeader->clusterDataSize	= a_clusterDataSize;		
		safeSPrintf( 
				a_listHeader->label,
				sizeof a_listHeader->label, "%s" ,
				a_identification.label().c_str() );
		safeSPrintf(
			a_listHeader->description, 
			sizeof a_listHeader->description, "%s",
			a_identification.description().c_str() );

		LOAD_SYSDATE( a_listHeader->creationDate );
		LISTPTR_INIT( &a_listHeader->mainList );
	}
}//namespace mqsProvider
