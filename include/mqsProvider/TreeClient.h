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
#include <cstdio>
#include "mqsProvider/TreeNode.h"
#include "mqsProvider/TreePtr.h"
#include "mqsProvider/Cluster.h"

namespace mqsProvider
{
	class TreeClient
	{
	public:
		TreeClient(void);
		TreeClient(
				TreePtr* a_treePtr,
				void* a_buffer );
		virtual ~TreeClient(void);

		void setTreePtr(
				TreePtr* a_treePtr );
		void setBuffer(
				void* a_buffer );

		bool remove(
				unsigned int a_leavingNode );
		bool insert(
				unsigned int a_newNode,
				const trn_key_t& a_key,
				unsigned int a_attach );
		unsigned int find( const char* a_key );

		unsigned int count(void) const;
		bool scanValidation(void) const;
		unsigned int root(void) const;
	private:
		TreePtr* m_treePtr;
		char* m_buffer;

		void removeRoot( void );
		void removeNotRootLeaf(
				unsigned int a_leavingNode );
		void removeCommon(
				unsigned int a_leavingNode );
		bool isLeaf(
				unsigned int a_node );
		bool isRoot(
				unsigned int a_node );
		bool isLeftChild(
				unsigned  int a_node );
		bool isRightChild(
				unsigned int a_node );
		bool hasBothChilds(
				unsigned int a_node );
		bool hasOnlyLeftChild(
				unsigned int a_node );
		bool hasOnlyRightChild(
				unsigned int a_node );
		unsigned int minor(
				unsigned int a_startNode );
		unsigned int scanAndValidate(
				unsigned int a_parent,
				unsigned int a_node ) const;

	};
}//namespace mqsProvider
