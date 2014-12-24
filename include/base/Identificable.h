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

#include <string>

namespace base
{
	class Identificable
	{
	private:
		int m_id;
		int m_parentId;
		size_t m_sequence;
		std::string m_type;
		std::string m_name;
		std::string m_description;
	public:
		Identificable(void){
			m_id = 0;
			m_parentId = 0;
			m_sequence = 0;
			m_type = "notype";
			m_name = "noname";
			m_description = "Not Initialised";
		}

		Identificable(int a_id, const std::string& a_name, const std::string& a_description){
			m_id = a_id;
			m_parentId = 0;
			m_sequence = 0;
			m_type = "notype";
			m_name = a_name;
			m_description = a_description;
		}

		virtual ~Identificable(void){
		}

		Identificable(const Identificable& a_identificable){
			this->assign( a_identificable );
		}

		Identificable& operator=( const Identificable& a_identificable ){
			return this->assign( a_identificable );
		}

		Identificable& assign( const Identificable& a_identificable ){
			m_id = a_identificable.m_id;
			m_parentId = a_identificable.m_parentId;
			m_sequence = a_identificable.m_sequence;
			m_name = a_identificable.m_name;
			m_description = a_identificable.m_description;
			m_type = a_identificable.m_type;

			return *this;
		}
		Identificable& setId( int a_id ){
			m_id=a_id;
			return *this;
		}
		Identificable& setParentId( int a_parentId ){
			m_parentId=a_parentId;
			return *this;
		}
		Identificable& setSequence( size_t a_sequence ){
			m_sequence=a_sequence;
			return *this;
		}
		Identificable& setName( const std::string& a_name ){
			m_name=a_name;
			return *this;
		}
		Identificable& setDescription( const std::string& a_description ){
			m_description=a_description;
			return *this;
		}
		Identificable& setType( const std::string& a_type ){
			m_type=a_type;
			return *this;
		}
		
		int id(void) const{
			return m_id;
		}
		int parentId(void) const{
			return m_parentId;
		}
		size_t sequence(void) const{
			return m_sequence;
		}
		const std::string& name(void) const{
			return m_name;
		}
		const std::string& type(void) const{
			return m_type;
		}
		const std::string& description(void) const{
			return m_description;
		}
	};
}//namespace base
