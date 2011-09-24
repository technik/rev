////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 17th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// multimap

#ifndef _REV_RTL_MULTIMAP_H_
#define _REV_RTL_MULTIMAP_H_

#include "pair.h"
#include "vector.h"

namespace rev { namespace rtl
{
	//------------------------------------------------------------------------------------------------------------------
	// map class declaration
	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _vauleT>
	class map<_keyT, _valueT>
	{
	public:
		class valueT: public pair<_keyT, _valueT>
		{};

	private:
		class node
		{
		public:
			node(const valueT& _value);
			node*	higher();
			node*	lower();
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
			bool		operator==	(const iterator& i) const;
			bool		operator!=	(const iterator& i) const;
			void		operator++	();
			_valueT&	operator*	() const;
		private:
			iterator(node * _node);
			node * mNode;
		friend class map<_keyT, _valueT>;
		};
		
		class reverse_iterator
		{
		public:
			bool		operator==	(const iterator& i) const;
			bool		operator!=	(const iterator& i) const;
			void		operator--	();
			_valueT&	operator*	() const;
		private:
			iterator(node * _node);
			node * mNode;
		friend class map<_keyT, _valueT>;
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
	template<typename _keyT, typename _valueT>
	inline multimap<_keyT, _valueT>::node::node(const multimap<_keyT, _valueT>::valueT& _value):
		mHigher(0),
		mLower(0),
		mValue(_value)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	typename multimap<_keyT, _valueT>::node * multimap<_keyT, _valueT>::node::higher() const
	{
		if(mHigher)
		{
			return mHigher;
		}
		else
		{	// Find the nearest parent whose value is higher than mine
			node * parent = mParent;
			while(mParent)
			{
				if(mValue.first < parent->mValue.first)
					return parent;
				else
					parent = parent->mParent;
			}
			return 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	typename multimap<_keyT, _valueT>::node * multimap<_keyT, _valueT>::node::lower() const
	{
		if(mLower)
		{
			return mLower;
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
	template<typename _keyT, typename _valueT>
	void multimap<_keyT, _valueT>::node::addChild(typename multimap<_keyT, _valueT>::node * _child)
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
	template<typename _keyT, typename _valueT>
	bool multimap<_keyT, _valueT>::iterator::operator==(const typename multimap<_keyT, _valueT>::iterator& i)
	{
		return mNode == i.mNode;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	bool multimap<_keyT, _valueT>::iterator::operator!=(const typename multimap<_keyT, _valueT>::iterator& i)
	{
		return mNode != i.mNode;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	void multimap<_keyT, _valueT>::iterator::operator++()
	{
		if(mNode)
			mNode = mNode->higher();
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	_vaueT& multimap<_keyT, _valueT>::iterator::operator*() const
	{
		if(mNode)
			return mNode->mValue;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	multimap<_keyT, _valueT>::iterator::iterator( typename multimap<_keyT, _valueT>::node * _node):
		mNode(_node);
	{
		// Intentionally blank
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	bool multimap<_keyT, _valueT>::reverse_iterator::operator==(const typename multimap<_keyT, _valueT>::reverse_iterator& i)
	{
		return mNode == i.mNode;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	bool multimap<_keyT, _valueT>::reverse_iterator::operator!=(const typename multimap<_keyT, _valueT>::reverse_iterator& i)
	{
		return mNode != i.mNode;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	void multimap<_keyT, _valueT>::reverse_iterator::operator--()
	{
		if(mNode)
			mNode = mNode->lower();
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	_vaueT& multimap<_keyT, _valueT>::reverse_iterator::operator*() const
	{
		if(mNode)
			return mNode->mValue;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	multimap<_keyT, _valueT>::reverse_iterator::reverse_iterator( typename multimap<_keyT, _valueT>::node * _node):
		mNode(_node);
	{
		// Intentionally blank
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	multimap<_keyT, _valueT>::multimap():
		mLowestNode(0),
		mHighestNode(0),
		mRootNode(0),
		mSize(0)
	{
		// Intentionally blank
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	multimap<_keyT, _valueT>::~multimap()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	typename multimap<_keyT, _valueT>::iterator multimap<_keyT, _valueT>::begin()
	{
		return iterator(mLowestNode);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	typename multimap<_keyT, _valueT>::iterator multimap<_keyT, _valueT>::end()
	{
		return iterator(0);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, _valueT>
	typename multimap<_keyT, _valueT>::reverse_iterator multimap<_keyT, _valueT>::rbegin()
	{
		return reverse_iterator(mHighestNode);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	typename multimap<_keyT, _valueT>::iterator multimap<_keyT, _valueT>::rend()
	{
		return reverse_iterator(0);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _valueT>
	typename multimap<_keyT, _valueT>::iterator multimap<_keyT, _valueT>::insert(typename const multimap<_keyT, _valueT>::valueT& _value)
	{
		// Create a node from value
		node * newNode = new node(_value);
		if(mHighestNode->mValue.first < _value.first)
			mHighestNode = newNode;
		if(!(mLowestNode->mValue.first < _value.first))
			mLowestNode = newNode;
		mRootNode->addChild(newNode);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT,typename _valueT>
	void multimap<_keyT, _valueT>::erase(typename const multimap<_keyT, _valueT>::iterator& _i)
	{
		node * n = _i.mNode;
		if(mHighestNode == n)
			mHighestNode = n->lower();
		else if (mLowestNode == n)
			mLowestNode = n->higher();

		if(mRootNode == n)
		{
			if(n->//
	}

}	// namespace rtl
}	// namespace rev

#endif // _REV_RTL_MULTIMAP_H_
