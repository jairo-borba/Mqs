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
#include <cstring>//for size_t type

namespace logging
{
	class Level
	{
	public:

		enum LEVEL
		{
			OFF			= 0,
			SEVERE		= 1,
			WARNING		= 2,
			INFO		= 3,
			CONFIG		= 4,
			FINE		= 5,
			FINER		= 6,
			FINEST		= 7,
			ALL			= 8
		};

		Level(void);
		Level(enum LEVEL a_eLevel);
		virtual ~Level(void);

		Level& setLevel( enum LEVEL a_eLevel );
		enum LEVEL level(void) const;
		bool levelOn( const Level& a_level ) const;
		void setLevelOn( enum LEVEL a_eLevel );
		void setLevelOff( enum LEVEL a_eLevel );

	private:

		static const size_t m_numLevels = ALL + 1;
		enum LEVEL m_eLevel;
		bool m_levelsOn[m_numLevels];
	};
}//namespace logging
