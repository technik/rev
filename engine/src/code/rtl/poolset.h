////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pool set
// pool set allows you to store elements, access them in logarithmic time, add elements in amortized constant time
// and iterate them and delete them in constant time. It can be seen as a two dimensional pool where you can iterate on
// elements "vertically" as if it was a map or "horizontally" as if it was a vector
#ifndef _RTL_POOLSET_H_
#define _RTL_POOLSET_H_

#if defined(_WIN32) || defined(_linux) || defined (ANDROID)
#include <new>
#endif

#include "baseTypes.h"
#include <utility.h>

namespace rtl
{
	template<typename _T>
	class poolset
	{
	public:
		// Iterator classes
		class const_iterator
		{
		public:
			// Construction and copy
			const_iterator();
			const_iterator(const const_iterator& _i);
			const_iterator& operator=(const const_iterator& _i);
			
			// Increment and decrement
			const_iterator& operator++();
			const_iterator	operator++(int);
			const_iterator& operator--();
			const_iterator	operator--(int);

			// Equality, inequality
			bool	operator==(const const_iterator& _i) const;
			bool	operator!=(const const_iterator& _i) const;
			// Dereferencing
			_T&		operator*();
			_T*		operator->();
		protected:
			poolset<_T>*	mPool;
			size_type		mIdx;

			friend class poolset<_T>;
		};
		class iterator: public const_iterator
		{
		public:
			// Construction and copy
			iterator();
			iterator(const iterator& _i);
			iterator& operator=(const iterator& _i);
			
			// Increment and decrement
			iterator&	operator++();
			iterator	operator++(int);
			iterator&	operator--();
			iterator	operator--(int);

			friend class poolset<_T>;
		};
	public:
		// Construction, copy and destruction
		poolset();
		poolset(const poolset<_T>& _other);
		~poolset();
		poolset<_T>& operator = (const poolset<_T>& _other);

		// Iterators
		// Since iteration in poolsets doesn't commit to any specific order of iteration,
		// it makes no sense to distinguish between forward iteration or backward iteration
		iterator		begin	();
		const_iterator	begin	() const;
		iterator		end		();
		const_iterator	end		() const;

		// Capacity
		void			reserve	(unsigned _size);
		bool			empty	() const;
		size_type		size	() const;
		//size_type		max_size() const;

		// Modifiers
		pair<iterator, bool>	insert	(const _T& _x);
		//iterator				insert	(iterator _position, const _T& _x);
		//template<class _inputIteratorT>
		//iterator				insert(_inputIteratorT _first, _inputIteratorT _last);
		void					erase	(iterator _position);
		size_type				erase	(const _T& _x);
		//void					erase	(iterator _first, iterator _last);
		void					clear	();

		// Operations
		iterator				find	(const _T& _x);
		const_iterator			find	(const _T& _x) const;
		//size_type				count	(const _T& _x) const;
		//iterator				lower_bound (const _T& _x) const;
		//iterator				upper_bound (const _T& _x) const;
		//pair<iterator, iterator> equal_range(const _T& _x) const;

	private:
		struct elementT
		{
			elementT() {}
			elementT(const elementT& _x)
				:mElement(_x.mElement)
				,mParentIdx(_x.mParentIdx)
				,mHighChildIdx(_x.mHighChildIdx)
				,mLowChildIdx(_x.mLowChildIdx)
			{}
			~elementT(){}

			_T			mElement;
			size_type	mParentIdx;
			size_type	mHighChildIdx;
			size_type	mLowChildIdx;
			
			void		insertChild(size_type _myIdx, elementT * _array, size_type _childIdx)
			{
				if(0xffFFffFF == _childIdx)
					return;
				else
				{
					if(_array[_childIdx].mElement < mElement)
					{
						if(0xffFFffFF != mLowChildIdx)
							_array[mLowChildIdx].insertChild(mLowChildIdx, _array, _childIdx);
						else
						{
							mLowChildIdx = _childIdx;
							_array[_childIdx].mParentIdx = _myIdx;
						}
					}else
					{
						if(0xffFFffFF != mHighChildIdx)
							_array[mHighChildIdx].insertChild(mHighChildIdx, _array, _childIdx);
						else
						{
							mHighChildIdx = _childIdx;
							_array[_childIdx].mParentIdx = _myIdx;
						}
					}
				}
			}
		};
	private:
		void enforceMinSize(size_type _size);

	private:
		elementT *	mArray;
		size_type	mRootIdx;
		size_type	mSize;
		size_type	mCapacity;

		friend class const_iterator;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline poolset<_T>::const_iterator::const_iterator():mPool(0), mIdx(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline poolset<_T>::const_iterator::const_iterator(const typename poolset<_T>::const_iterator& _i):
		mPool(_i.mPool), mIdx(_i.mIdx)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	typename poolset<_T>::const_iterator&
		poolset<_T>::const_iterator::operator=(const typename poolset<_T>::const_iterator& _i)
	{
		mPool = _i.mPool;
		mIdx = _i.mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline typename poolset<_T>::const_iterator& poolset<_T>::const_iterator::operator++()
	{
		++mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline typename poolset<_T>::const_iterator poolset<_T>::const_iterator::operator++(int)
	{
		++mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline typename poolset<_T>::const_iterator& poolset<_T>::const_iterator::operator--()
	{
		--mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline typename poolset<_T>::const_iterator poolset<_T>::const_iterator::operator--(int)
	{
		--mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline bool poolset<_T>::const_iterator::operator==(const typename poolset<_T>::const_iterator& _other) const
	{
		return mIdx == _other.mIdx;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	bool poolset<_T>::const_iterator::operator != (const typename poolset<_T>::const_iterator& _other) const
	{
		return mIdx != _other.mIdx;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	_T& poolset<_T>::const_iterator::operator*()
	{
		return mPool->mArray[mIdx].mElement;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	_T* poolset<_T>::const_iterator::operator->()
	{
		return &(mPool->mArray[mIdx].mElement);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	poolset<_T>::iterator::iterator(): const_iterator()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	poolset<_T>::iterator::iterator(const typename poolset<_T>::iterator& _iter):const_iterator(_iter)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	typename poolset<_T>::iterator& poolset<_T>::iterator::operator=(const typename poolset<_T>::iterator& _i)
	{
		const_iterator::mIdx = _i.mIdx;
		const_iterator::mPool = _i.mPool;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline typename poolset<_T>::iterator& poolset<_T>::iterator::operator++()
	{
		++const_iterator::mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline typename poolset<_T>::iterator poolset<_T>::iterator::operator++(int)
	{
		++const_iterator::mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline typename poolset<_T>::iterator& poolset<_T>::iterator::operator--()
	{
		--const_iterator::mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline typename poolset<_T>::iterator poolset<_T>::iterator::operator--(int)
	{
		--const_iterator::mIdx;
		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Poolset
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	poolset<_T>::poolset()
		:mArray(0)
		,mRootIdx(0)
		,mSize(0)
		,mCapacity(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	poolset<_T>::poolset(const poolset<_T>& _other)
		:mSize(_other.mSize)
		,mCapacity(_other.mSize)
		,mRootIdx(_other.mRootIdx)
	{
		// Memcopy
		mArray = new elementT[mSize];
		for(size_type i = 0; i < mSize; ++i)
			mArray[i] = _other.mArray[i];
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	poolset<_T>::~poolset()
	{
		if(mCapacity)
			delete reinterpret_cast<unsigned char*>(mArray);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	poolset<_T>& poolset<_T>::operator=(const poolset<_T>& _other)
	{
		// Delete previous content
		if(mCapacity)
			delete mArray;
		// Copy indices and metrics
		mRootIdx = _other.mRootIdx;
		mSize = _other.mSize;
		mCapacity = _other.mSize;
		// Copy the array
		mArray = new elementT[mSize];
		for(size_type i = 0; i < mSize; ++i)
			mArray[i] = _other.mArray[i];
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	typename poolset<_T>::iterator poolset<_T>::begin()
	{
		iterator iter;
		iter.mPool = this;
		iter.mIdx = 0;
		return iter;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	typename poolset<_T>::const_iterator poolset<_T>::begin() const
	{
		const_iterator iter;
		iter.mPool = this;
		iter.mIdx = 0;
		return iter;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	typename poolset<_T>::iterator poolset<_T>::end()
	{
		iterator iter;
		iter.mPool = this;
		iter.mIdx = mSize;
		return iter;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	typename poolset<_T>::const_iterator poolset<_T>::end() const
	{
		const_iterator iter;
		iter.mPool = this;
		iter.mIdx = mSize;
		return iter;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	void poolset<_T>::reserve(unsigned _size)
	{
		enforceMinSize(_size);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline bool poolset<_T>::empty () const
	{
		return 0 == mSize;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	inline size_type poolset<_T>::size () const
	{
		return mSize;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	pair<typename poolset<_T>::iterator, bool> poolset<_T>::insert(const _T& _x)
	{
		if(0 == mSize)
		{
			// Reserve an array for at least two elements
			enforceMinSize(2);
			// Initialize first element
			elementT * element = new(&mArray[0]) elementT;
			element->mHighChildIdx = 0xffFFffFF;
			element->mLowChildIdx = 0xffFFffFF;
			element->mParentIdx = 0xffFFffFF;
			element->mElement = _x;
			// Update map metrics
			mSize = 1;
			mRootIdx = 0;
			return pair<iterator, bool>(begin(), true);
		}
		else
		{
			size_type lastIdx = 0xffFFffFF;
			size_type idx = mRootIdx;
			while(0xffFFffFF != idx)
			{
				lastIdx = idx;
				if(mArray[idx].mElement < _x)
				{
					idx = mArray[idx].mHighChildIdx;
				}
				else if( _x < mArray[idx].mElement)
				{
					idx = mArray[idx].mLowChildIdx;
				}
				else // Equal, so we've found the node we were looking for
				{
					mArray[idx].mElement = _x;
					iterator iter;
					iter.mIdx = idx;
					iter.mPool = this;
					return pair<iterator, bool>(iter, false);
				}
			}
			// If we get here, it means we've found no node, so we have to create one
			idx = mSize; // New nodes are always inserted at the end of the array
			++mSize;
			enforceMinSize(mSize);
			elementT * element = new(&mArray[idx]) elementT(); // Construct the node
			// Update hierarchy and map metrics
			element->mParentIdx = lastIdx;
			if(mArray[lastIdx].mElement < _x)
			{
				mArray[lastIdx].mHighChildIdx = idx;
			}else
			{
				mArray[lastIdx].mLowChildIdx = idx;
			}
			element->mHighChildIdx = 0xffFFffFF;
			element->mLowChildIdx = 0xffFFffFF;
			// Update element key
			element->mElement = _x;
			// Build the return pair
			iterator iter;
			iter.mIdx = idx;
			iter.mPool = this;
			return pair<iterator, bool>(iter, true);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	void poolset<_T>::erase(typename poolset<_T>::iterator _pos)
	{
		// We assume the given iterator is valid
		// This assumption will save us some branching
		size_type idx = _pos.mIdx;
		elementT * element = &mArray[idx]; // This is the element we want to erase
		if( mRootIdx == idx )
		{
			// Delete root node
			if(0xffFFffFF != element->mHighChildIdx)
			{
				mRootIdx = element->mHighChildIdx;
				mArray[mRootIdx].insertChild(mRootIdx, mArray, element->mLowChildIdx);
			}
			else mRootIdx = element->mLowChildIdx;
			
			if(0xffFFffFF != mRootIdx)
			{
				mArray[mRootIdx].mParentIdx = 0xffFFffFF;
			}
			// Update map metrics
			--mSize;
		}
		else
		{
			// Delete reference from parent node
			elementT * parent = &mArray[element->mParentIdx];
			if(idx == parent->mHighChildIdx)
				parent->mHighChildIdx = 0xffFFffFF;
			else parent->mLowChildIdx = 0xffFFffFF;

			// Move children into parent
			parent->insertChild(element->mParentIdx, mArray, element->mHighChildIdx);
			parent->insertChild(element->mParentIdx, mArray, element->mLowChildIdx);

			// Actual erase
			--mSize; // Decrese size
			// Always delete from the back of the queue to keep elements packed
			if(mSize == idx) // If we're trying to delete the last element
			{
				element->~elementT(); // Mamually call destructor to avoid memory deallocation
			}
			else
			{
				// Copy the last element into the slot of the element we want to erase
				*element = mArray[mSize];
				// Delete the last element in the queue
				mArray[mSize].~elementT();
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	size_type poolset<_T>::erase(const _T& _x)
	{
		iterator iter = find(_x);
		if(iter != end())
		{
			erase(iter);
			return 1;
		}
		else return 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	void poolset<_T>::clear()
	{
		// Call destructors of all elements (keep memory allocated)
		for(size_type i = 0; i < mSize; ++i)
		{
			mArray[i].~elementT();
		}
		// Resize to 0
		mSize = 0;
		// Update map metrics
		mRootIdx = 0xffFFffFF;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	typename poolset<_T>::iterator poolset<_T>::find(const _T& _x)
	{
		if(0 == mSize)
		{
			return begin();
		}
		else
		{
			size_type idx = mRootIdx;
			while(0xffFFffFF != idx)
			{
				if(mArray[idx].mElement < _x)
				{
					idx = mArray[idx].mHighChildIdx;
				}else if(_x < mArray[idx].mElement)
				{
					idx = mArray[idx].mLowChildIdx;
				}else // Equal, we've found what we were looking for
				{
					iterator iter;
					iter.mPool = this;
					iter.mIdx = idx;
					return iter;
				}
			}
			// Not found
			return end();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	typename poolset<_T>::const_iterator poolset<_T>::find(const _T& _x) const
	{
		if(0 == mSize)
		{
			return begin();
		}
		else
		{
			size_type idx = mRootIdx;
			while(0xffFFffFF != idx)
			{
				if(mArray[idx].mElement < _x)
				{
					idx = mArray[idx].mHighChildIdx;
				}else if(_x < mArray[idx].mElement)
				{
					idx = mArray[idx].mLowChildIdx;
				}else // Equal, we've found what we were looking for
				{
					const_iterator iter;
					iter.mPool = this;
					iter.mIdx = idx;
					return iter;
				}
			}
			// Not found
			return end();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	void poolset<_T>::enforceMinSize(size_type _size)
	{
		if(mCapacity < _size)
		{
			if(0 == mCapacity)
			{
				mCapacity = _size;
				// Simple reserve space for requested elements
				mArray = reinterpret_cast<elementT*>(new unsigned char[mCapacity*sizeof(elementT)]);
			}
			else
			{
				// Compute new size
				mCapacity = mCapacity * 2;
				// Allocate new memory
				elementT * bigArray = reinterpret_cast<elementT*>(new unsigned char[mCapacity * sizeof(elementT)]);
				// Copy elements (and delete old ones at the same time)
				for(size_type i = 0; i < mSize; ++i)
				{
					new(&bigArray[i]) elementT(mArray[i]);
					mArray[i].~elementT();
				}
				// Deallocate old elements
				delete reinterpret_cast<unsigned char*>(mArray);
				mArray = bigArray;
			}
		}
	}

}	// namespace rtl

#endif //_RTL_POOLSET_H_
