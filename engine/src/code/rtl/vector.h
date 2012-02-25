////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 22nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// vector

#ifndef _RTL_VECTOR_H_
#define _RTL_VECTOR_H_

#if defined(_WIN32) || defined(_linux) || defined (ANDROID)
#include <new>
#endif

namespace rtl
{
	//------------------------------------------------------------------------------------------------------------------
	// vector class declaration
	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	class vector
	{
	public:
		// vector data types
		class iterator
		{
		public:
			iterator(): mIndex(0) {}
			iterator& operator++	()					{ mIndex = &mIndex[1];	return *this; }
			iterator  operator++	(int)				{ mIndex = &mIndex[1];	return *this; }
			bool operator==	(const iterator& _i) const	{ return mIndex == _i.mIndex;	}
			bool operator!= (const iterator& _i) const	{ return mIndex != _i.mIndex;	}
			_dataT&	operator *	() const				{ return *mIndex;				}
			_dataT* operator->	() const				{ return mIndex;				}

		private:
			iterator(_dataT* _index):mIndex(_index)	{}
			_dataT* mIndex;
			friend class vector<_dataT>;
		};
		class const_iterator: public iterator {};
	public:
		// --- Default constructor and destructor ---
		vector();
		vector(const vector<_dataT>& _x);
		~vector();
		vector<_dataT>& operator= (const vector<_dataT>& _x)
		{
			m_size = _x.m_size;
			m_capacity = m_size;
			m_pData = reinterpret_cast<_dataT*>(new char[m_size * sizeof(_dataT)]);
			for(unsigned i = 0; i < m_size; ++i)
			{
				new(&m_pData[i]) _dataT(_x[i]);
			}
			return *this;
		}

		// --- accessors and parsing ---
		unsigned int	size		()	const;
		unsigned int	capacity	()	const;

		iterator		begin		();
		const_iterator	begin		()	const;

		iterator		end			();
		// TODO: const_iterator	end			()	const;
		_dataT&			operator[]	(unsigned int _idx)			{ return m_pData[_idx];	}
		const _dataT&	operator[]	(unsigned int _idx) const	{ return m_pData[_idx];	}


		// TODO: rbegin rend

		// --- modifiers ---
		void			push_back	(const _dataT& _data);
		void			clear		()
		{
			for(unsigned i = 0; i < m_size; ++i)
			{
				m_pData[i].~_dataT(); // Destroy data
			}
			m_size = 0;
		}
		void			erase		(iterator _position)
		{
			if(_position == end())
				return;
			m_size--;
			iterator i = _position;
			iterator j;
			j++;
			while(j != end())
			{
				*i = *j;
				++i;
				++j;
			}
			m_pData[m_size].~_dataT();
			m_size--;
		}
		// TODO: pop_back

		void			resize		(const unsigned int _size);
		void			reserve		(const unsigned int _size);

	private:
		// Internal data storage
		unsigned int	m_size;
		unsigned int	m_capacity;
		_dataT	*		m_pData;
	};

	//------------------------------------------------------------------------------------------------------------------
	// vector implementation
	//------------------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline vector<_dataT>::vector():
		m_size(0),
		m_capacity(0),
		m_pData(0)
	{
		m_pData = 0;// new _dataT[m_capacity];
	}
	
	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline vector<_dataT>::vector(const vector<_dataT>& _x)
	{
		m_size = _x.m_size;
		m_capacity = m_size;
		m_pData = reinterpret_cast<_dataT*>(new char[m_size * sizeof(_dataT)]);
		for(unsigned i = 0; i < m_size; ++i)
		{
			new(&m_pData[i]) _dataT(_x[i]);
		}
	}
	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline vector<_dataT>::~vector()
	{
		if(m_pData)
			delete[] m_pData;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline unsigned int vector<_dataT>::size() const
	{
		return m_size;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline unsigned int vector<_dataT>::capacity() const
	{
		return m_capacity;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline typename vector<_dataT>::iterator vector<_dataT>::begin()
	{
		return iterator(m_pData);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline typename vector<_dataT>::const_iterator vector<_dataT>::begin() const
	{
		return const_iterator(m_pData);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline typename vector<_dataT>::iterator vector<_dataT>::end()
	{
		return iterator(&m_pData[m_size]);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline void vector<_dataT>::push_back(const _dataT& _element)
	{
		if(!m_capacity)
		{
			reserve(1);
		}
		else if(m_size == m_capacity)
		{
			reserve(m_capacity * 2);
		}
		m_pData[m_size] = _element;
		++m_size;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline void vector<_dataT>::resize(const unsigned int _size)
	{
		// Reserve extra memory (if neccessary)
		if(_size > m_capacity)
		{
			reserve(_size);
			// Initialize new active members
			for(iterator i = m_size; i < _size; ++i)
			{
				// Call approipate constructor
				new(&m_pData[i]) _dataT;
			}
			m_size = _size;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _dataT>
	inline void vector<_dataT>::reserve(const unsigned int _size)
	{
		if( _size > m_capacity)
		{
			// Allocate new memory
			// Lazy initialization, avoid calling unnecessary constructors
			_dataT *newArray = reinterpret_cast<_dataT*>(new char[_size * sizeof(_dataT)]);
			// Copy stored data
			for(unsigned int i = 0; i < m_size; ++i)
			{
				new(&newArray[i]) _dataT(m_pData[i]);
			}
			// delete old data
			if(m_pData)
				delete[] reinterpret_cast<char*>(m_pData);
			// array substitution
			m_pData = newArray;
			// Update capacity
			m_capacity = _size;
		}
	}

}	// namespace rtl

#endif // _RTL_VECTOR_H_
