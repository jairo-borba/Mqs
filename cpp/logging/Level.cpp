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

#include "logging/Level.h"

namespace logging
{
	Level::Level(void)
	{
		this->setLevel( OFF );
	}
	Level::Level(enum LEVEL a_eLevel)
	{
		this->setLevel( a_eLevel );
	}
	Level::~Level(void)
	{
	}
	Level& Level::setLevel( enum LEVEL a_eLevel )
	{
		m_eLevel = a_eLevel;
		
		m_levelsOn[OFF		] = OFF		 <=	m_eLevel;
		m_levelsOn[SEVERE	] = SEVERE	 <=	m_eLevel;
		m_levelsOn[WARNING	] = WARNING	 <=	m_eLevel;
		m_levelsOn[INFO		] = INFO	 <=	m_eLevel;
		m_levelsOn[CONFIG	] = CONFIG	 <=	m_eLevel;
		m_levelsOn[FINE		] = FINE	 <=	m_eLevel;
		m_levelsOn[FINER	] = FINER	 <=	m_eLevel;
		m_levelsOn[FINEST	] = FINEST	 <=	m_eLevel;
		m_levelsOn[ALL		] = ALL		 <=	m_eLevel;

		return *this;
	}
	bool Level::levelOn( const Level& a_level ) const
	{
		return m_levelsOn[a_level.level()];
	}
	enum Level::LEVEL Level::level(void) const
	{
		return m_eLevel;
	}

	void Level::setLevelOn( enum LEVEL a_eLevel )
	{
		m_levelsOn[a_eLevel] = true;
	}
	
	void Level::setLevelOff( enum LEVEL a_eLevel )
	{
		m_levelsOn[a_eLevel] = false;
	}
}//namespace logging
