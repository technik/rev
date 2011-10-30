////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 17th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// map

#ifndef _RTL_MAP_H_
#define _RTL_MAP_H_

#include "baseTypes.h"
#include "pair.h"

namespace rtl
{
	template<typename _keyT, typename _T>
	class map
	{
	public:
		typedef pair<_keyT, _T>	valueT;
		// Iterators
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
			map<_keyT, _T>* mMap;
			size_type		mIdx;

			friend class map<_keyT,_T>;
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

			friend class map<_keyT,_T>;
		};
	public:
		// Construction, destruction and copy
		map();
		map(const map<_keyT, _T>& _map);
		~map();
		map<_keyT, _T>& operator= (const map<_keyT, _T>& _map);

		// Iterators
		iterator		begin		();
		const_iterator	begin		() const;
		iterator		end			();
		const_iterator	end			() const;
		//iterator		rbegin		();
		//const_iterator	rbegin		() const;
		//iterator		rend		();
		//const_iterator	rend		() const;

		// Capacity
		bool			empty		() const;
		size_type		size		() const;
		// size_type	max_size	() const;
		
		// Element access
		_T&				operator[]	(const _keyT& _key);

		// Modifiers
		pair<iterator, bool>	insert	(const valueT& _x);
		pair<iterator, bool>	insert	(const _keyT& _key, const _T& _x);
		//iterator				insert	(iterator _position, const valueT& _x);
		//template<class _iteratorT>
		//void					insert	(_iteratorT _first, _iteratorT _last);
		void					erase	( iterator _position);
		size_type				erase	(const _keyT& _key);
		//void					erase	(iterator _first, iterator _last);
		//void					swap	(map<_keyT, _T>& _map);
		void					clear	();

		// Operations
		iterator				find	(const _keyT& _key);
		const_iterator			find	(const _keyT& _key) const;
		//size_type				count	(const _keyT& _key) const;
		//iterator				lower_bound(const _keyT& _key);
		//const_iterator			lower_bound(const _keyT& _key) const;
		//iterator				upper_bound(const _keyT& _key);
		//const_iterator			upper_bound(const _keyT& _key) const;
		//pair<iterator,iterator	equal_range(const _keyT& _key);
		//pair<const_iterator, const_iterator> equal_range(const _keyT& _key) const;
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

			valueT		mElement;
			size_type	mParentIdx;
			size_type	mHighChildIdx;
			size_type	mLowChildIdx;

			size_type	nextElementIdx() const;
			size_type	prevElementIdx() const;

			void		insertChild(size_type _myIdx, elementT * _array, size_type _childIdx)
			{
				if(0xffFFffFF == _childIdx)
					return;
				else
				{
					if(_array[_childIdx].mElement.first < mElement.first)
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
		size_type	mLowestIdx;
		size_type	mHighestIdx;
		size_type	mSize;
		size_type	mCapacity;

		friend class const_iterator;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline map<_keyT, _T>::const_iterator::const_iterator():mMap(0), mIdx(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline map<_keyT, _T>::const_iterator::const_iterator(const typename map<_keyT,_T>::const_iterator& _i):
		mMap(_i.mMap), mIdx(_i.mIdx)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	typename map<_keyT,_T>::const_iterator& map<_keyT,_T>::const_iterator::operator=(const const_iterator& _i)
	{
		mMap = _i.mMap;
		mIdx = _i.mIdx;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline typename map<_keyT,_T>::const_iterator& map<_keyT,_T>::const_iterator::operator++()
	{
		mIdx = mMap->mArray[mIdx].nextElementIdx();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline typename map<_keyT,_T>::const_iterator map<_keyT,_T>::const_iterator::operator++(int)
	{
		mIdx = mMap->mArray[mIdx].nextElementIdx();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline typename map<_keyT,_T>::const_iterator& map<_keyT,_T>::const_iterator::operator--()
	{
		mIdx = mMap->mArray[mIdx].prevElementIdx();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline typename map<_keyT,_T>::const_iterator map<_keyT,_T>::const_iterator::operator--(int)
	{
		mIdx = mMap->mArray[mIdx].prevElementIdx();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline bool map<_keyT,_T>::const_iterator::operator==(const const_iterator& _i) const
	{
		return mIdx == _i.mIndex;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline bool map<_keyT,_T>::const_iterator::operator!=(const const_iterator& _i) const
	{
		return mIdx != _i.mIdx;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline _T& map<_keyT, _T>::const_iterator::operator*()
	{
		return (mMap->mArray[mIdx]).mElement.second;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline _T* map<_keyT,_T>::const_iterator::operator->()
	{
		return &((mMap->mArray[mIdx]).mElement.second);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline map<_keyT,_T>::iterator::iterator():const_iterator()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline map<_keyT,_T>::iterator::iterator(const typename map<_keyT,_T>::iterator& _i):const_iterator(_i)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline typename map<_keyT,_T>::iterator& map<_keyT,_T>::iterator::operator++()
	{
		const_iterator::mIdx = const_iterator::mMap->mArray[const_iterator::mIdx].nextElementIdx();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline typename map<_keyT,_T>::iterator map<_keyT,_T>::iterator::operator++(int)
	{
		const_iterator::mIdx = const_iterator::mMap->mArray[const_iterator::mIdx].nextElementIdx();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline typename map<_keyT,_T>::iterator& map<_keyT,_T>::iterator::operator--()
	{
		const_iterator::mIdx = const_iterator::mMap->mArray[const_iterator::mIdx].prevElementIdx();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline typename map<_keyT,_T>::iterator map<_keyT,_T>::iterator::operator--(int)
	{
		const_iterator::mIdx = const_iterator::mMap->mArray[const_iterator::mIdx].prevElementIdx();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline map<_keyT, _T>::map():
		mArray(0),
		mRootIdx(0),
		mLowestIdx(0),
		mHighestIdx(0),
		mSize(0),
		mCapacity(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	map<_keyT, _T>::map(const map<_keyT, _T>& _map):
		mRootIdx(_map.mRootIdx),
		mLowestIdx(_map.mLowestIdx),
		mHighestIdx(_map.mHighestIdx),
		mSize(_map.mSize),
		mCapacity(_map.mCapacity)
	{
		// Memcopy
		mArray = new elementT[mSize];
		for(size_type i = 0; i < mSize; ++i)
			mArray[i] = _map.mArray[i];
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline map<_keyT,_T>::~map()
	{
		if(mCapacity)
			delete reinterpret_cast<unsigned char*>(mArray);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	map<_keyT,_T>& map<_keyT,_T>::operator=(const map<_keyT,_T>& _map)
	{
		// Delete previous content
		if(mCapacity)
			delete mArray;
		// Copy indices and metrics
		mRootIdx = _map.mRootIdx;
		mLowestIdx = _map.mLowestIdx;
		mHighestIdx = _map.mHighestIdx;
		mSize = _map.mSize;
		mCapacity = _map.mCapacity;
		// Copy the array
		mArray = new elementT[mSize];
		for(size_type i = 0; i < mSize; ++i)
			mArray[i] = _map.mArray[i];
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	typename map<_keyT,_T>::iterator map<_keyT,_T>::begin()
	{
		iterator iter;
		iter.mMap = this;
		iter.mIdx = mLowestIdx;
		return iter;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	typename map<_keyT,_T>::const_iterator map<_keyT,_T>::begin() const
	{
		const_iterator iter;
		iter.mMap = this;
		iter.mIdx = mLowestIdx;
		return iter;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	typename map<_keyT,_T>::iterator map<_keyT,_T>::end()
	{
		iterator iter;
		iter.mMap = this;
		iter.mIdx = 0xffFFffFF;
		return iter;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	typename map<_keyT,_T>::const_iterator map<_keyT,_T>::end() const
	{
		const_iterator iter;
		iter.mMap = this;
		iter.mIdx = 0xffFFffFF;
		return iter;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline bool map<_keyT,_T>::empty() const
	{
		return 0 == mSize;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline size_type map<_keyT,_T>::size() const
	{
		return mSize;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	_T& map<_keyT,_T>::operator[] (const _keyT& _key)
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
			element->mElement.first = _key;
			// Update map metrics
			mSize = 1;
			mLowestIdx = 0;
			mHighestIdx = 0;
			mRootIdx = 0;
			return element->mElement.second;
		}
		else
		{
			size_type lastIdx = 0xffFFffFF;
			size_type idx = mRootIdx;
			while(0xffFFffFF != idx)
			{
				lastIdx = idx;
				if(mArray[idx].mElement.first < _key)
				{
					idx = mArray[idx].mHighChildIdx;
				}
				else if( _key < mArray[idx].mElement.first)
				{
					idx = mArray[idx].mLowChildIdx;
				}
				else // Equal, so we've found the node we were looking for
				{
					return mArray[idx].mElement.second;
				}
			}
			// If we get here, it means we've found no node, so we have to create one
			enforceMinSize(mSize+1);
			idx = mSize; // New nodes are always inserted at the end of the array
			elementT * element = new(&mArray[idx]) elementT(); // Construct the node
			// Update hierarchy and map metrics
			element->mParentIdx = lastIdx;
			if(mArray[lastIdx].mElement.first < _key)
			{
				mArray[lastIdx].mHighChildIdx = idx;
				if(mHighestIdx == lastIdx)
					mHighestIdx = idx;
			}else
			{
				mArray[lastIdx].mLowChildIdx = idx;
				if(mLowestIdx == lastIdx)
					mLowestIdx = idx;
			}
			element->mHighChildIdx = 0xffFFffFF;
			element->mLowChildIdx = 0xffFFffFF;
			// Update element key
			element->mElement.first = _key;
			return element->mElement.second;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline pair<typename map<_keyT,_T>::iterator, bool> map<_keyT,_T>::insert(const typename map<_keyT,_T>::valueT& _x)
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
			mLowestIdx = 0;
			mHighestIdx = 0;
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
				if(mArray[idx].mElement.first < _x.first)
				{
					idx = mArray[idx].mHighChildIdx;
				}
				else if( _x.first < mArray[idx].mElement.first)
				{
					idx = mArray[idx].mLowChildIdx;
				}
				else // Equal, so we've found the node we were looking for
				{
					mArray[idx].mElement.second = _x.second;
					iterator iter;
					iter.mIdx = idx;
					iter.mMap = this;
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
			if(mArray[lastIdx].mElement.first < _x.first)
			{
				mArray[lastIdx].mHighChildIdx = idx;
				if(mHighestIdx == lastIdx)
					mHighestIdx = idx;
			}else
			{
				mArray[lastIdx].mLowChildIdx = idx;
				if(mLowestIdx == lastIdx)
					mLowestIdx = idx;
			}
			element->mHighChildIdx = 0xffFFffFF;
			element->mLowChildIdx = 0xffFFffFF;
			// Update element key
			element->mElement = _x;
			// Build the return pair
			iterator iter;
			iter.mIdx = idx;
			iter.mMap = this;
			return pair<iterator, bool>(iter, true);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	inline pair<typename map<_keyT,_T>::iterator, bool> map<_keyT,_T>::insert(const _keyT& _key, const _T& _x)
	{
		return insert(valueT(_key, _x));
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	void map<_keyT,_T>::erase(typename map<_keyT,_T>::iterator _pos)
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
			if(idx == mLowestIdx)
				mLowestIdx = mRootIdx;
			if(idx == mHighestIdx)
				mHighestIdx = mRootIdx;
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
	template<typename _keyT, typename _T>
	size_type map<_keyT,_T>::erase(const _keyT& _key)
	{
		iterator iter = find(_key);
		if(iter != end())
		{
			erase(iter);
			return 1;
		}
		else return 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	void map<_keyT,_T>::clear()
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
		mLowestIdx = 0xffFFffFF;
		mHighestIdx = 0xffFFffFF;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	typename map<_keyT,_T>::iterator map<_keyT,_T>::find(const _keyT& _key)
	{
		if(0 == mSize)
			return end();
		else
		{
			size_type idx = mRootIdx;
			while(0xffFFffFF != idx)
			{
				if(mArray[idx].mElement.first < _key)
				{
					idx = mArray[idx].mHighChildIdx;
				}else if(_key < mArray[idx].mElement.first)
				{
					idx = mArray[idx].mLowChildIdx;
				}else // Equal, we've found what we were looking for
				{
					iterator iter;
					iter.mMap = this;
					iter.mIdx = idx;
					return iter;
				}
			}
			// Not found
			return end();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	typename map<_keyT,_T>::const_iterator map<_keyT,_T>::find(const _keyT& _key) const
	{
		if(0 == mSize)
			return begin();
		else
		{
			size_type idx = mRootIdx;
			while(0xffFFffFF != idx)
			{
				if(mArray[idx].mElement.first < _key)
				{
					idx = mArray[idx].mHighChildIdx;
				}else if(_key < mArray[idx].mElement.first)
				{
					idx = mArray[idx].mLowChildIdx;
				}else // Equal, we've found what we were looking for
				{
					const_iterator iter;
					iter.mMap = this;
					iter.mIdx = idx;
					return pair<const_iterator, bool>(iter, true);
				}
			}
			// Not found
			return pair<const_iterator, bool>(end(), false);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _T>
	void map<_keyT, _T>::enforceMinSize(size_type _size)
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

#endif // _RTL_MAP_H_
