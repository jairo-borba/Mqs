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

#include "mqsProvider/TreeClient.h"
#include <base/ExceptionInfo.h>
#include <appCore/Shortcuts.h>
namespace mqsProvider
{
	TreeClient::TreeClient(void)
	{
		appCore::initPointer( m_treePtr );
		appCore::initPointer( m_buffer );
	}
	TreeClient::TreeClient( TreePtr* a_treePtr, void* a_buffer )
	{
		this->setTreePtr( a_treePtr );
		this->setBuffer( a_buffer );
	}
	TreeClient::~TreeClient(void)
	{
	}
	void TreeClient::setTreePtr( TreePtr* a_treePtr )
	{
		m_treePtr = a_treePtr;
	}
	void TreeClient::setBuffer( void* a_buffer )
	{
		m_buffer = reinterpret_cast<char*>(a_buffer);
	}
	bool TreeClient::isLeaf( unsigned int a_node )
	{
		bool l_isLeaf = NODE_LEFT(m_buffer,a_node) == INDEX_NULL && NODE_RIGHT(m_buffer,a_node) == INDEX_NULL;
		return l_isLeaf;
	}
	bool TreeClient::isLeftChild( unsigned int a_node )
	{
		bool l_isLeftChild = NODE_PARENT_LEFT(m_buffer,a_node) == a_node;
		return l_isLeftChild;
	}
	bool TreeClient::isRightChild( unsigned int a_node )
	{
		bool l_isRightChild = NODE_PARENT_RIGHT(m_buffer,a_node) == a_node;
		return l_isRightChild;
	}
	bool TreeClient::hasBothChilds( unsigned int a_node )
	{
		bool l_hasBothChilds =
			NODE_RIGHT(m_buffer,a_node) != INDEX_NULL && 
			NODE_LEFT(m_buffer,a_node) != INDEX_NULL;
		return l_hasBothChilds;
	}
	bool TreeClient::hasOnlyLeftChild( unsigned int a_node )
	{
		bool l_hasOnlyLeftChild =
			NODE_RIGHT(m_buffer,a_node) == INDEX_NULL && 
			NODE_LEFT(m_buffer,a_node) != INDEX_NULL;
		return l_hasOnlyLeftChild;
	}
	bool TreeClient::hasOnlyRightChild( unsigned int a_node )
	{
		bool l_hasOnlyRightChild =
			NODE_RIGHT(m_buffer,a_node) != INDEX_NULL && 
			NODE_LEFT(m_buffer,a_node) == INDEX_NULL;
		return l_hasOnlyRightChild;
	}
	bool TreeClient::isRoot( unsigned int a_node )
	{
		bool l_isRoot = a_node == m_treePtr->root;
		return l_isRoot;
	}
	void TreeClient::removeRoot( void )
	{
		unsigned int l_leavingRoot = m_treePtr->root;

		if( this->isLeaf( l_leavingRoot ) ){
			m_treePtr->root			= INDEX_NULL;
		} else if( this->hasOnlyLeftChild( l_leavingRoot ) ){
			NODE_LEFT_PARENT(m_buffer,l_leavingRoot) = INDEX_NULL;
			m_treePtr->root = NODE_LEFT(m_buffer,l_leavingRoot);
		} else if( this->hasOnlyRightChild( l_leavingRoot ) ){
			NODE_RIGHT_PARENT(m_buffer,l_leavingRoot) = INDEX_NULL;
			m_treePtr->root = NODE_RIGHT(m_buffer,l_leavingRoot);
		} else if( this->hasBothChilds( l_leavingRoot ) ){
			unsigned int sl_newLeftParent = this->minor( NODE_RIGHT(m_buffer,l_leavingRoot) );
			NODE_LEFT(m_buffer,sl_newLeftParent) = NODE_LEFT(m_buffer,l_leavingRoot);
			NODE_LEFT_PARENT(m_buffer,l_leavingRoot) = sl_newLeftParent;
			m_treePtr->root = NODE_RIGHT(m_buffer,l_leavingRoot);
		}
		NODE_LEFT(m_buffer,l_leavingRoot)		= INDEX_NULL;
		NODE_RIGHT(m_buffer,l_leavingRoot)		= INDEX_NULL;
		NODE_PARENT(m_buffer,l_leavingRoot)		= INDEX_NULL;
	}
	void TreeClient::removeNotRootLeaf( unsigned int a_leavingNode )
	{
		if( this->isLeftChild( a_leavingNode ) ){
			NODE_PARENT_LEFT(m_buffer,a_leavingNode)= INDEX_NULL;
			NODE_LEFT(m_buffer,a_leavingNode)		= INDEX_NULL;
			NODE_RIGHT(m_buffer,a_leavingNode)		= INDEX_NULL;
			NODE_PARENT(m_buffer,a_leavingNode)		= INDEX_NULL;
		} else if( this->isRightChild( a_leavingNode ) ){
			NODE_PARENT_RIGHT(m_buffer,a_leavingNode)	= INDEX_NULL;
			NODE_LEFT(m_buffer,a_leavingNode)			= INDEX_NULL;
			NODE_RIGHT(m_buffer,a_leavingNode)			= INDEX_NULL;
			NODE_PARENT(m_buffer,a_leavingNode)			= INDEX_NULL;
		}
	}
	void TreeClient::removeCommon( unsigned int a_leavingNode )
	{
		if( this->hasBothChilds( a_leavingNode ) ){

			unsigned int sl_newLeftParent = this->minor( NODE_RIGHT(m_buffer,a_leavingNode) );

			if( this->isLeftChild( a_leavingNode ) ){
				NODE_PARENT_LEFT(m_buffer,a_leavingNode) = NODE_RIGHT(m_buffer,a_leavingNode);
				NODE_RIGHT_PARENT(m_buffer,a_leavingNode) = NODE_PARENT(m_buffer,a_leavingNode);
				NODE_LEFT(m_buffer,sl_newLeftParent) = NODE_LEFT(m_buffer,a_leavingNode);
				NODE_LEFT_PARENT(m_buffer,a_leavingNode) = sl_newLeftParent;

			} else if( this->isRightChild( a_leavingNode ) ){
				NODE_PARENT_RIGHT(m_buffer,a_leavingNode) = NODE_RIGHT(m_buffer,a_leavingNode);
				NODE_RIGHT_PARENT(m_buffer,a_leavingNode) = NODE_PARENT(m_buffer,a_leavingNode);
				NODE_LEFT(m_buffer,sl_newLeftParent) = NODE_LEFT(m_buffer,a_leavingNode);
				NODE_LEFT_PARENT(m_buffer,a_leavingNode) = sl_newLeftParent;
			}
		} else if( this->hasOnlyLeftChild( a_leavingNode ) ){
			if( this->isLeftChild( a_leavingNode ) ){
				NODE_LEFT_PARENT(m_buffer,a_leavingNode) = NODE_PARENT(m_buffer,a_leavingNode);
				NODE_PARENT_LEFT(m_buffer,a_leavingNode) = NODE_LEFT(m_buffer,a_leavingNode);
			} else if( this->isRightChild( a_leavingNode ) ){
				NODE_LEFT_PARENT(m_buffer,a_leavingNode) = NODE_PARENT(m_buffer,a_leavingNode);
				NODE_PARENT_RIGHT(m_buffer,a_leavingNode) = NODE_LEFT(m_buffer,a_leavingNode);
			}
		} else if( this->hasOnlyRightChild( a_leavingNode ) ){
			if( this->isLeftChild( a_leavingNode ) ){
				NODE_RIGHT_PARENT(m_buffer,a_leavingNode) = NODE_PARENT(m_buffer,a_leavingNode);
				NODE_PARENT_LEFT(m_buffer,a_leavingNode) = NODE_RIGHT(m_buffer,a_leavingNode);
			} else if( this->isRightChild( a_leavingNode ) ){
				NODE_RIGHT_PARENT(m_buffer,a_leavingNode) = NODE_PARENT(m_buffer,a_leavingNode);
				NODE_PARENT_RIGHT(m_buffer,a_leavingNode) = NODE_RIGHT(m_buffer,a_leavingNode);
			}
		}
		NODE_LEFT(m_buffer,a_leavingNode)	= INDEX_NULL;
		NODE_RIGHT(m_buffer,a_leavingNode)	= INDEX_NULL;
		NODE_PARENT(m_buffer,a_leavingNode)	= INDEX_NULL;
	}

	bool TreeClient::remove( unsigned int a_leavingNode )
	{
		appCore::appAssertPointer( m_treePtr );
		appCore::appAssertPointer( m_buffer );
		RETURN_IF( a_leavingNode == INDEX_NULL, false );

		if( this->isRoot( a_leavingNode ) ){		//Removing ROOT Node
			this->removeRoot();
		} else if( this->isLeaf( a_leavingNode ) ){	//Removing a Leaf (except if it is a root node)
			this->removeNotRootLeaf( a_leavingNode );
		} else {
			this->removeCommon( a_leavingNode );
		}
		m_treePtr->countElements--;

		return true;
	}
	bool TreeClient::insert( unsigned int a_newNode, const trn_key_t& a_key, unsigned int a_attach )
	{
		appCore::appAssertPointer( m_treePtr );
		appCore::appAssertPointer( m_buffer );
		RETURN_IF( a_newNode == INDEX_NULL, false );
		
		INIT_TREE_NODE(m_buffer,a_newNode);
		NODE_ATTACH(m_buffer,a_newNode)	= a_attach;
		memcpy( NODE_KEY(m_buffer,a_newNode), a_key, sizeof a_key );

		if( m_treePtr->root == INDEX_NULL ){
			NODE_PARENT(m_buffer,a_newNode)	= INDEX_NULL;
			m_treePtr->root					= a_newNode;
		} else {
			unsigned int l_current	= m_treePtr->root;
			unsigned int l_parent	= INDEX_NULL;
			while(true){
				l_parent = l_current;
				if( strcmp( NODE_KEY(m_buffer,a_newNode), NODE_KEY(m_buffer,l_current) ) < 0 ){
					l_current = NODE_LEFT(m_buffer,l_current);
					if( l_current == INDEX_NULL ){
						NODE_LEFT(m_buffer,l_parent)	= a_newNode;
						NODE_PARENT(m_buffer,a_newNode)	= l_parent;
						break;
					}
				} else {
					l_current = NODE_RIGHT(m_buffer,l_current);
					if( l_current == INDEX_NULL ){
						NODE_RIGHT(m_buffer,l_parent)	= a_newNode;
						NODE_PARENT(m_buffer,a_newNode)	= l_parent;
						break;
					}
				}
			}
		}

		m_treePtr->countElements++;
		return true;
	}
	unsigned int TreeClient::minor( unsigned int a_startNode )
	{
		unsigned int l_current	= a_startNode;
		unsigned int l_minor = INDEX_NULL;

		while( l_current != INDEX_NULL ){
			l_minor		= l_current;
			l_current	= NODE_LEFT(m_buffer,l_current);
		}

		return l_minor;
	}
	unsigned int TreeClient::find( const char* a_key )
	{
		appCore::appAssertPointer( m_treePtr );
		appCore::appAssertPointer( m_buffer );

		unsigned int l_current	= m_treePtr->root;
		RETURN_IF( l_current == INDEX_NULL, INDEX_NULL );

		const char* l_currentNodeKey = NODE_KEY(m_buffer,l_current);
		int l_comparing = strcmp( a_key, l_currentNodeKey );
		while( l_comparing != 0 ){
			if( l_comparing < 0 ){
				l_current = NODE_LEFT(m_buffer,l_current);
			} else {
				l_current = NODE_RIGHT(m_buffer,l_current);
			}
			if( l_current == INDEX_NULL ){
				break;
			}
			l_currentNodeKey = NODE_KEY(m_buffer,l_current);
			l_comparing = strcmp( a_key, l_currentNodeKey );
		}

		return l_current;
	}
	unsigned int TreeClient::root(void) const
	{
		appCore::appAssertPointer( m_treePtr );
		return m_treePtr->root;
	}
	unsigned int TreeClient::count(void) const
	{
		RETURN_IF( m_treePtr == 0, 0 );

		return m_treePtr->countElements;
	}

	bool TreeClient::scanValidation(void) const
	{
		appCore::appAssertPointer( m_treePtr );
		appCore::appAssertPointer( m_buffer );

		unsigned int l_root	= m_treePtr->root;
		unsigned int l_countElements = this->scanAndValidate( INDEX_NULL, l_root );
		RETURN_IF( l_countElements == INDEX_NULL || l_countElements != m_treePtr->countElements, false );

		return true;
	}
	unsigned int  TreeClient::scanAndValidate( unsigned int a_parent, unsigned int a_node ) const
	{
		unsigned int l_countElements = 0;
		RETURN_IF( a_node == INDEX_NULL, 0 );
		RETURN_IF( !IS_TREE_NODE_VALID(m_buffer,a_node), INDEX_NULL );
		RETURN_IF( NODE_PARENT(m_buffer,a_node) != a_parent, INDEX_NULL );

		unsigned int l_ret;

		l_ret = scanAndValidate( a_node, NODE_LEFT(m_buffer,a_node) );
		RETURN_IF( l_ret == INDEX_NULL, INDEX_NULL );
		l_countElements += l_ret;
		l_ret = scanAndValidate( a_node, NODE_RIGHT(m_buffer,a_node) );
		RETURN_IF( l_ret == INDEX_NULL, INDEX_NULL );
		l_countElements += l_ret;
		++l_countElements;

		return l_countElements;
	}
}//namespace mqsProvider
