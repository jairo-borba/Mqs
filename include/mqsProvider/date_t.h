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
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <appCore/SafeStringDef.h>
namespace mqsProvider
{
	typedef char date_t[14+1];

	inline date_t& DATECOPY( date_t& a_dest, const date_t& a_orig ){
		memcpy( a_dest, a_orig, sizeof( date_t ) );
		return a_dest;
	}
	inline void LOAD_SYSDATE( date_t& a_date ){
		time_t l_rawtime;
		struct tm * l_timeinfo;
		time ( &l_rawtime );
		l_timeinfo = localtime( &l_rawtime );
		safeSPrintf( a_date, sizeof a_date,
			"%04d%02d%02d%02d%02d%02d" ,
			l_timeinfo->tm_year + 1900, l_timeinfo->tm_mon + 1, l_timeinfo->tm_mday,
			l_timeinfo->tm_hour, l_timeinfo->tm_min, l_timeinfo->tm_sec );
	}
	inline unsigned int GETDAY( const date_t& a_date ){
		char l_tmp[3];
		memset( l_tmp, 0, sizeof l_tmp );
		strncpy( l_tmp, reinterpret_cast<const char*>(&a_date) + 6, 2 );
		unsigned int l_day = atoi( l_tmp );
		return l_day;
	}
	inline unsigned int GETMONTH( const date_t& a_date ){
		char l_tmp[3];
		memset( l_tmp, 0, sizeof l_tmp );
		strncpy( l_tmp, reinterpret_cast<const char*>(&a_date) + 4, 2 );
		unsigned int l_month = atoi( l_tmp );
		return l_month;
	}
	inline unsigned int GETYEAR( const date_t& a_date ){
		char l_tmp[5];
		memset( l_tmp, 0, sizeof l_tmp );
		strncpy( l_tmp, reinterpret_cast<const char*>(&a_date), 4 );
		unsigned int l_year = atoi( l_tmp );
		return l_year;
	}
	inline unsigned int GETHOUR( const date_t& a_date ){
		char l_tmp[3];
		memset( l_tmp, 0, sizeof l_tmp );
		strncpy( l_tmp, reinterpret_cast<const char*>(&a_date) + 8, 2 );
		unsigned int l_hour = atoi( l_tmp );
		return l_hour;
	}
	inline unsigned int GETMINUTE( const date_t& a_date ){
		char l_tmp[3];
		memset( l_tmp, 0, sizeof l_tmp );
		strncpy( l_tmp, reinterpret_cast<const char*>(&a_date) + 10, 2 );
		unsigned int l_minute = atoi( l_tmp );
		return l_minute;
	}
	inline unsigned int GETSECOND( const date_t& a_date ){
		char l_tmp[3];
		memset( l_tmp, 0, sizeof l_tmp );
		strncpy( l_tmp, reinterpret_cast<const char*>(&a_date) + 12, 2 );
		unsigned int l_second = atoi( l_tmp );
		return l_second;
	}
	inline int ELAPSEDTIME( const date_t& a_date ){
		
		time_t l_endTime;
		time ( &l_endTime );
		
		struct tm* l_timeInfo = localtime( &l_endTime );
		l_timeInfo->tm_year	= GETYEAR	(a_date) - 1900;
		l_timeInfo->tm_mon	= GETMONTH	(a_date) - 1;
		l_timeInfo->tm_mday	= GETDAY	(a_date);
		l_timeInfo->tm_hour	= GETHOUR	(a_date);
		l_timeInfo->tm_min	= GETMINUTE	(a_date);
		l_timeInfo->tm_sec	= GETSECOND	(a_date);

		time_t l_startTime = mktime( l_timeInfo );

		double l_diffTime = difftime( l_endTime, l_startTime );

		return (int)l_diffTime;
	}
}//namespace mqsProvider
