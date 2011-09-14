////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 22nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pool set
// pool set allows you to store elements, access them in logarithmic time, add elements in amortized constant time
// and iterate them and delete them in constant time. It can be seen as a two dimensional pool where you can iterate on
// elements "vertically" as if it was a map or "horizontally" as if it was a vector
#ifndef _REV_RTL_POOLSET_H_
#define _REV_RTL_POOLSET_H_

#if defined(_linux) || defined(ANDROID)
// Note: GCC requires the inclusion of <new> in order to use placement new operator.
#include <new>
#endif

#include "baseTypes.h"

namespace rev	{	namespace rtl
{
	template< typename _T> // _T type must define default ctor, copy ctor, dtor and operator <
	class poolset
	{
	private:
		//--------------------------------------------------------------------------------------------------------------
		// classes for internal use
		//--------------------------------------------------------------------------------------------------------------
		// -- Node -----------------------------------------------------------------------------------------------------
		// Each node of the tree stores a value plus the indices for its lower and higher nodes in the tree.
		class node
		{
		public:
			// -- Constructor
			node(const _T& _element);
			node(const node& _node);
			~node();
			// -- Accessors
			const _T&	element		()	const;
			void		setHigher	(node* _higher);
			node*		higher		()	const;
			void		setLower	(node* _lower);
			node*		lower		()	const;
			void		setParent	(node* _parent);
			node*		parent		()	const;
		private:
			node*	mHigher;
			node*	mLower;
			node*	mParent;
			_T		mElement;
		};

	public:
		//--------------------------------------------------------------------------------------------------------------
		// Iterators
		class const_iterator								///< Iterator that can modify pool content.
		{
		public:
					void	operator++	();
			const	_T&		operator*	();
					bool	operator!=	(const const_iterator& i);
			//
		private:
			const node * mElement;
		friend class poolset<_T>;
		};

		class iterator: public const_iterator {};	///< Iterator that can not modify pool content.

	public:
		// -- Constructor and destructor --
		poolset		();
		~poolset	();

		// -- Elements management --
		void	push	(const _T& _element);		///< Pushes given element into the pool.
		void	erase	(const _T& _element);		///< removes given element from the pool.
		void	clear	();							///< Erases all content from the pool.

		// -- Horizontally traverse the pool --
		iterator		begin	();
		const_iterator	begin	()	const;

		iterator		end		();
		const_iterator	end		()	const;

		// -- Size management --
		sizeT			size	()	const;			///< Returns current size of the pool.
		sizeT			capacity()	const;			///< Returns current allocated capacity of the pool.
		void			reserve	(const sizeT _n);	///< Allocates space for at least _n elements inside the pool.

	private:
		void			ensureCapacity	(const sizeT _n);	///< Ensure there is enough capacity to store _n elements.
															///< Grow if necessary.
	private:
		//--------------------------------------------------------------------------------------------------------------
		// Internal data
		//--------------------------------------------------------------------------------------------------------------
		sizeT		mSize;
		sizeT		mCapacity;
		node*		mRootNode;
		node*		mElements;
	};

//----------------------------------------------------------------------------------------------------------------------
// Implementation : Node
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
template < typename _T>
poolset<_T>::node::node(const _T& _element):
	mHigher(0),
	mLower(0),
	mParent(0),
	mElement(_element)
{
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
poolset<_T>::node::node(const poolset<_T>::node& _node):
	mElement(_node.mElement)
{
	// Note: Never should a node be constructed as a copy of root node, so mParent is granted to be different from 0
	if(_node.mParent->higher() == &_node)
	{
		_node.mParent->setHigher(this);
	}
	else
	{
		_node.mParent->setLower(this);
	}
	setHigher(_node.mHigher);
	setLower(_node.mLower);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
poolset<_T>::node::~node()
{
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
inline const _T& poolset<_T>::node::element() const
{
	return mElement;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
inline void poolset<_T>::node::setHigher(node * _higher)
{
	mHigher = _higher;
	_higher->setParent(this);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
inline typename poolset<_T>::node * poolset<_T>::node::higher() const
{
	return mHigher;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
inline void poolset<_T>::node::setLower(node * _lower)
{
	mLower = _lower;
	_lower->setParent(this);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
inline typename poolset<_T>::node * poolset<_T>::node::lower() const
{
	return mLower;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
inline void poolset<_T>::node::setParent(node *_parent)
{
	mParent = _parent;
}

//----------------------------------------------------------------------------------------------------------------------
template< typename _T>
inline typename poolset<_T>::node * poolset<_T>::node::parent() const
{
	return mParent;
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation : Iterators
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
template< typename _T>
inline void poolset<_T>::const_iterator::operator ++()
{
	mElement = &mElement[1]; // Advance 1 element
}

//----------------------------------------------------------------------------------------------------------------------
template < typename _T>
inline const _T& poolset<_T>::const_iterator::operator *()
{
	return mElement->element();
}

//----------------------------------------------------------------------------------------------------------------------
template< typename _T>
inline bool poolset<_T>::const_iterator::operator !=(const const_iterator& i)
{
	return mElement != i.mElement;
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation : Poolset
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
poolset<_T>::poolset():
	mSize(0),
	mCapacity(0),
	mRootNode(0),
	mElements(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
poolset<_T>::~poolset()
{
	// Erase all content
	clear();
	// Free all memory
	if(mCapacity)
	{
		delete[] reinterpret_cast<unsigned char*>(mElements); // Free memory without calling destructors
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T> void poolset<_T>::push(const _T& _element)
{
	// First step: find lower node.
	if(0 != mRootNode)
	{
		node* nodeA = mRootNode;
		bool bInserted = false;
		while(!bInserted)
		{
			// Test against A node
			if(nodeA->element() < _element)
			{
				if(0 != nodeA->higher()) // Exists a higher node of A
				{
					nodeA = nodeA->higher();
				}
				else // given element must be set as A's higher
				{
					// Ensure there is capacity in the pool to store one more element
					ensureCapacity(mSize+1);
					// Initialize last node to given element
					new(&mElements[mSize]) node(_element);
					// Set new node as A's higher
					nodeA->setHigher(&mElements[mSize]);
					// Grow 1 element
					++mSize;
					// End the loop
					bInserted = true;
				}
			}
			else if (_element < nodeA->element())
			{
				if(0 != nodeA->lower()) // Exists a lower node of A
				{
					nodeA = nodeA->lower();
				}
				else // given element must be set as A's higher
				{
					// Ensure there is capacity in the pool to store one more element
					ensureCapacity(mSize+1);
					// Initialize last node to given element
					new(&mElements[mSize]) node(_element);
					// Set new node as A's higher
					nodeA->setLower(&mElements[mSize]);
					// Grow 1 element
					++mSize;
					// End the loop
					bInserted = true;
				}
			}
			else
			{
				// If !(a<element) && !(element<a) then element equals a, so element is alredy inserted
				bInserted = true;
			}
		}
	}
	else // No elements stored yet
	{
		ensureCapacity(1);
		new(&mElements[0]) node(_element);
		mRootNode = &mElements[0];
		mSize = 1;
	}
}

//----------------------------------------------------------------------------------------------------------------------
template< typename _T>
void poolset<_T>::erase(const _T& _element)
{
	// Find requested node
	node * nodeA = mRootNode;
	while(0 != nodeA)
	{
		if(nodeA->element() < _element)
			nodeA = nodeA->higher();
		else if (_element < nodeA->element())
			nodeA = nodeA->lower();
		else break; // Node found
	}
	if(0 != nodeA) // If we found the requested node
	{
		// Remove requested node from the tree
		if(mRootNode != nodeA)
		{
			if(nodeA->higher())
			{
				nodeA->parent()->setHigher(nodeA->higher());
				if(nodeA->lower())
				{
					if(nodeA->parent()->lower())
					{
						node * nodeB = nodeA->parent()->lower();
						while( 0 != nodeB->higher())
						{
							nodeB = nodeB->higher();
						}
						nodeB->setHigher(nodeA->lower());
					}
					else nodeA->parent()->setLower(nodeA->lower());
				}
			}
			else if (nodeA->lower())
			{
				nodeA->parent()->setLower(nodeA->lower());
			}
		}
		else // nodeA == rootNode
		{
			if(nodeA->higher())
			{
				mRootNode = nodeA->higher();
				if(nodeA->lower())
				{
					if(mRootNode->lower())
					{
						node * nodeB = mRootNode->lower();
						while( 0 != nodeB->higher())
						{
							nodeB = nodeB->higher();
						}
						nodeB->setHigher(nodeA->lower());
					}
					else mRootNode->setLower(nodeA->lower());
				}
			}
			else if (nodeA->lower())
			{
				mRootNode = nodeA->lower();
			}
			mRootNode->setParent(0);
		}
		// Copy last node into removed node
		new(nodeA) node(mElements[mSize-1]);
		// Delete last node
		mElements[mSize-1].~node();
		--mSize;
	}
}

//----------------------------------------------------------------------------------------------------------------------
template< typename _T>
void poolset<_T>::clear()
{
	// Destroy all stored elements
	for(indexT i = 0; i < mSize; ++i)
	{
		mElements[i].~node();
	}
	mSize = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template< typename _T>
typename poolset<_T>::iterator poolset<_T>::begin()
{
	iterator i;
	i.mElement = &(mElements[0]);
	return i;
}

//----------------------------------------------------------------------------------------------------------------------
template< typename _T>
typename poolset<_T>::const_iterator poolset<_T>::begin() const
{
	const_iterator i;
	i.mElement = &(mElements[0]);
	return i;
}

//----------------------------------------------------------------------------------------------------------------------
template< typename _T>
typename poolset<_T>::iterator poolset<_T>::end()
{
	iterator i;
	i.mElement = &(mElements[mSize]);
	return i;
}

//----------------------------------------------------------------------------------------------------------------------
template < typename _T>
typename poolset<_T>::const_iterator poolset<_T>::end() const
{
	const_iterator i;
	i.mElement = &(mElements[mSize]);
	return i;
}

//----------------------------------------------------------------------------------------------------------------------
template < typename _T>
inline sizeT poolset<_T>::size() const
{
	return mSize;
}

//----------------------------------------------------------------------------------------------------------------------
template < typename _T>
inline sizeT poolset<_T>::capacity() const
{
	return mCapacity;
}

//----------------------------------------------------------------------------------------------------------------------
template < typename _T>
void poolset<_T>::reserve(const sizeT _n)
{
	// If requested capacity is bigger than current capacity, get some more storage space
	if(mCapacity < _n)
	{
		// Allocate new memory
		node * newArray = reinterpret_cast<node*> (new unsigned char[_n * sizeof(node)]);
		// Copy stored elements and destroy old copies of them
		for(indexT i = 0; i < mSize; ++i)
		{
			new(&newArray[i]) node(mElements[i]); // Copy
			mElements[i].~node(); // destroy
		}
		// Release old array, if it existed
		if(0 != mElements)
		{
			delete[] reinterpret_cast<unsigned char*>(mElements);
		}
		// Set the new array
		mElements = newArray;
		mCapacity = _n;
	}
}

//----------------------------------------------------------------------------------------------------------------------
template < typename _T>
void poolset<_T>::ensureCapacity(const sizeT _n)
{
	sizeT newSize = mCapacity;
	while(newSize < _n)
	{
		// Compute new size
		newSize = (0 == newSize) ? 1 : (2 * newSize);
	}
	reserve(newSize);
}

}	// namespace rtl
}	// namespace rtl

#endif // _REV_RTL_POOLSET_H_
