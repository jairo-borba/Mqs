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
#include "mqsProvider/IndexDef.h"
#include "mqsProvider/Cluster.h"

namespace mqsProvider
{
	static const char TRND_MCODE[] = "V.la%";
	static const unsigned int TRND_KEY_SIZE	= 50;
	typedef char trn_key_t[TRND_KEY_SIZE+1];
	typedef struct tagTreeNode
	{
		char magicCode[sizeof TRND_MCODE];
		unsigned int _this;
		unsigned int parent;
		unsigned int left;
		unsigned int right;
		trn_key_t key;
		unsigned int attach;
	} TreeNode;

	inline int COMPARE_TREE_NODE_KEY( const trn_key_t& a_key1, const trn_key_t& a_key2 ){
		int l_ret = strncmp( reinterpret_cast<const char*>(a_key1), reinterpret_cast<const char*>(a_key2), TRND_KEY_SIZE );
		return l_ret;
	}
	inline void LOAD_TREE_NODE_KEY( trn_key_t& a_key, const char* a_value ){
		memset( &a_key, 0, TRND_KEY_SIZE );
		strncpy( reinterpret_cast<char*>(&a_key), a_value, TRND_KEY_SIZE );
	}
	inline TreeNode* TREE_NODE(void*a_buffer,unsigned int a_position){
		TreeNode* l_treeNode = reinterpret_cast<TreeNode*>(CLUSTER_DATA(a_buffer,a_position));
		return l_treeNode;
	}
	inline unsigned int& NODE_THIS(void*a_buffer,unsigned int a_position){
		return TREE_NODE(a_buffer,a_position)->_this;
	}
	inline char* NODE_MAGIC_CODE(void*a_buffer,unsigned int a_position){
		return TREE_NODE(a_buffer,a_position)->magicCode;
	}
	inline unsigned int& NODE_PARENT(void*a_buffer,unsigned int a_position){
		return TREE_NODE(a_buffer,a_position)->parent;
	}
	inline unsigned int& NODE_LEFT(void*a_buffer,unsigned int a_position){
		return TREE_NODE(a_buffer,a_position)->left;
	}
	inline unsigned int& NODE_RIGHT(void*a_buffer,unsigned int a_position){
		return TREE_NODE(a_buffer,a_position)->right;
	}
	inline unsigned int& NODE_ATTACH(void*a_buffer,unsigned int a_position){
		return TREE_NODE(a_buffer,a_position)->attach;
	}
	inline char* NODE_KEY(void*a_buffer,unsigned int a_position){
		return TREE_NODE(a_buffer,a_position)->key;
	}
	inline unsigned int& NODE_PARENT_LEFT(void*a_buffer,unsigned int a_position){
		return NODE_LEFT(a_buffer,NODE_PARENT(a_buffer,a_position));
	}
	inline unsigned int& NODE_PARENT_RIGHT(void*a_buffer,unsigned int a_position){
		return NODE_RIGHT(a_buffer,NODE_PARENT(a_buffer,a_position));
	}
	inline unsigned int& NODE_LEFT_PARENT(void*a_buffer,unsigned int a_position){
		return NODE_PARENT(a_buffer,NODE_LEFT(a_buffer,a_position));
	}
	inline unsigned int& NODE_RIGHT_PARENT(void*a_buffer,unsigned int a_position){
		return NODE_PARENT(a_buffer,NODE_RIGHT(a_buffer,a_position));
	}
	
	inline void INIT_TREE_NODE(void*a_buffer,unsigned int a_position){
		memcpy( NODE_MAGIC_CODE(a_buffer,a_position), TRND_MCODE, sizeof TRND_MCODE );
		memset( NODE_KEY(a_buffer,a_position), 0, TRND_KEY_SIZE + 1 );
		NODE_THIS(a_buffer,a_position)		= a_position;
		NODE_PARENT(a_buffer,a_position)	= INDEX_NULL;
		NODE_LEFT(a_buffer,a_position)		= INDEX_NULL;
		NODE_RIGHT(a_buffer,a_position)		= INDEX_NULL;
		NODE_ATTACH(a_buffer,a_position)	= INDEX_NULL;
	}

	inline bool IS_TREE_NODE_VALID(void*a_buffer,unsigned int a_position){
		bool l_ret = NODE_THIS(a_buffer,a_position) == a_position;
		if( !l_ret ) return false;

		l_ret = memcmp( NODE_MAGIC_CODE(a_buffer,a_position), TRND_MCODE, sizeof TRND_MCODE ) == 0;
		return l_ret;
	}
}//namespace mqsProvider
