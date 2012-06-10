////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on September 17th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// multimap

#ifndef _REV_RTL_MULTIMAP_H_
#define _REV_RTL_MULTIMAP_H_

#include <vector.h>

namespace rtl
{
	//------------------------------------------------------------------------------------------------------------------
	// map class declaration
	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT = pair<_keyT, _valueT> >
	class multimap
	{
	private:
		class node
		{
		public:
			node(const valueT& _value);
			node*	higher() const;
			node*	lower() const;
			void	addChild(node * _higher);

			node * mHigher;
			node * mLower;
			node * mParent;
			valueT mValue;
		};

	public:
		class iterator
		{
		public:
			bool		operator==	(const iterator& _i) const	{ return mNode == _i.mNode;	}
			bool		operator!=	(const iterator& _i) const	{ return mNode != _i.mNode;	}
			void		operator++	();
			valueT&		operator*	() const;
			iterator	(): mNode(0)	{}
		private:
			iterator(node * _node);
			node * mNode;
		friend class multimap<_keyT, _valueT, valueT>;
		};
		
		class reverse_iterator
		{
		public:
			bool		operator==	(const reverse_iterator& _i) const	{ return mNode == _i.mNode;	}
			bool		operator!=	(const reverse_iterator& _i) const	{ return mNode != _i.mNode;	}
			void		operator--	();
			valueT&		operator*	() const;
			reverse_iterator(): mNode(0)	{}
		private:
			reverse_iterator(node * _node);
			node * mNode;
		friend class multimap<_keyT, _valueT, valueT>;
		};

	public:
		multimap();
		~multimap();
		// Operator =

		iterator			begin	();
		iterator			end		();
		reverse_iterator	rbegin	();
		reverse_iterator	rend	();

		bool				empty	()	{	return 0 == mSize;	}
		unsigned			size	()	{	return mSize;		}

		iterator			insert	(const valueT& _value);
		void				erase	(const iterator _position);
		void				clear	();

		iterator			find	(const _keyT& key);

	private:

		node *		mLowestNode;
		node *		mHighestNode;
		node *		mRootNode;
		unsigned	mSize;
	};

	//------------------------------------------------------------------------------------------------------------------
	// map implementation
	//------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline multimap<_keyT, _valueT, valueT>::node::node(const valueT& _value):
		mHigher(0),
		mLower(0),
		mParent(0),
		mValue(_value)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline typename multimap<_keyT, _valueT, valueT>::node * multimap<_keyT, _valueT, valueT>::node::higher() const
	{
		if(mHigher)
		{
			node * higher = mHigher;
			while(0 != higher->mLower)
				higher = higher->mLower;
			return higher;
		}
		else
		{	// Find the nearest parent whose value is higher than mine
			node * parent = mParent;
			while(mParent)
			{
				if(!(parent->mValue.first < mValue.first))
					return parent;
				else
					parent = parent->mParent;
			}
			return 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline typename multimap<_keyT, _valueT, valueT>::node * multimap<_keyT, _valueT, valueT>::node::lower() const
	{
		if(mLower)
		{
			node * lower = mLower;
			while(0 != lower->mHigher)
				lower = lower->mHigher;
			return lower;
		}
		else
		{
			node * parent = mParent;
			while(mParent)
			{
				if(!(mValue.first < parent->mValue.first))
					return parent;
				else
					parent = parent->mParent;
			}
			return 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline
	void multimap<_keyT, _valueT, valueT>::node::addChild(typename multimap<_keyT, _valueT, valueT>::node * _child)
	{
		if(mValue.first < _child->mValue.first)
		{
			if(mHigher)
			{
				mHigher->addChild(_child);
			}
			else
			{
				mHigher = _child;
				_child->mParent = this;
			}
		}
		else
		{
			if(mLower)
			{
				mLower->addChild(_child);
			}
			else
			{
				mLower = _child;
				_child->mParent = this;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline
	void multimap<_keyT, _valueT, valueT>::iterator::operator++()
	{
		if(mNode)
			mNode = mNode->higher();
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline
	valueT& multimap<_keyT, _valueT, valueT>::iterator::operator*() const
	{
		return mNode->mValue;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline
	multimap<_keyT, _valueT, valueT>::iterator::iterator( typename multimap<_keyT, _valueT, valueT>::node * _node):
		mNode(_node)
	{
		// Intentionally blank
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	void multimap<_keyT, _valueT, valueT>::reverse_iterator::operator--()
	{
		if(mNode)
			mNode = mNode->lower();
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	valueT& multimap<_keyT, _valueT, valueT>::reverse_iterator::operator*() const
	{
		return mNode->mValue;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	multimap<_keyT, _valueT, valueT>::reverse_iterator::reverse_iterator(
		typename multimap<_keyT, _valueT, valueT>::node * _node):
		mNode(_node)
	{
		// Intentionally blank
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	multimap<_keyT, _valueT, valueT>::multimap():
		mLowestNode(0),
		mHighestNode(0),
		mRootNode(0),
		mSize(0)
	{
		// Intentionally blank
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	multimap<_keyT, _valueT, valueT>::~multimap()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	typename multimap<_keyT, _valueT, valueT>::iterator multimap<_keyT, _valueT, valueT>::begin()
	{
		return iterator(mLowestNode);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	typename multimap<_keyT, _valueT, valueT>::iterator multimap<_keyT, _valueT, valueT>::end()
	{
		return iterator(0);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	typename multimap<_keyT, _valueT, valueT>::reverse_iterator multimap<_keyT, _valueT, valueT>::rbegin()
	{
		return reverse_iterator(mHighestNode);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	typename multimap<_keyT, _valueT, valueT>::reverse_iterator multimap<_keyT, _valueT, valueT>::rend()
	{
		return reverse_iterator(0);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline
	typename multimap<_keyT, _valueT, valueT>::iterator multimap<_keyT, _valueT, valueT>::insert(const valueT& _value)
	{
		// Create a node from value
		node * newNode = new node(_value);
		if((0 == mHighestNode) || (mHighestNode->mValue.first < _value.first))
			mHighestNode = newNode;
		if((0 == mLowestNode) || !(mLowestNode->mValue.first < _value.first))
			mLowestNode = newNode;
		if(0 != mRootNode)
			mRootNode->addChild(newNode);
		else mRootNode = newNode;
		return iterator(newNode);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline
	void multimap<_keyT, _valueT, valueT>::erase(const typename multimap<_keyT, _valueT, valueT>::iterator _i)
	{
		node * n = _i.mNode;
		// Update tree boundaries
		if(mHighestNode == n)
			mHighestNode = n->lower();
		else if (mLowestNode == n)
			mLowestNode = n->higher();
		// 
		if(mRootNode == n)
		{
			if(n->mHigher)
			{
				n->mHigher->mParent = 0;
				mRootNode = n->mHigher;
				if(n->mLower)
					mRootNode->addChild(n->mLower);
			}
			else if(n->mLower)
			{
				n->mLower->mParent = 0;
				mRootNode = n->mLower;
			}
			else
			{
				mRootNode = 0;
			}
		}
		else
		{
			if(n->mHigher)
				n->mParent->addChild(n->mHigher);
			if(n->mLower)
				n->mParent->addChild(n->mLower);
		}

		// Actual deletion
		delete n;
		mSize--;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline
	void multimap<_keyT, _valueT, valueT>::clear()
	{
		node * n = mLowestNode;
		node * aux;
		while(n)
		{
			aux = n;
			n = n->higher();
			delete aux;
		}
		mLowestNode = 0;
		mHighestNode = 0;
		mRootNode = 0;
		mSize = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT, typename valueT>
	inline
	typename multimap<_keyT, _valueT, valueT>::iterator multimap<_keyT, _valueT, valueT>::find(const _keyT& _key)
	{
		node * n = mRootNode;
		iterator i;
		while(0 != n)
		{
			if(_key < n->mValue.first)
				n = n->mHigher;
			else if ( n->mValue.first < _key)
				n = n->mLower;
			else return iterator(n);
		}
		return i;
	}

}	// namespace rtl

#endif // _REV_RTL_MULTIMAP_H_
