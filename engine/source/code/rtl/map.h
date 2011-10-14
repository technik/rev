////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 17th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// map

#ifndef _REV_RTL_MAP_H_
#define _REV_RTL_MAP_H_

#include "pair.h"

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

		class iterator
		{
		public:
			operator++	();
			operator*	();
		};

	public:
		map();
		~map();
		// Operator =

		iterator	begin	();
		iterator	end		();

		bool		empty	();
		unsigned	size	();

		iterator	insert	(const valueT& _value);
		void		erase	(const iterator _position);
		void		clear	();

		iterator	find	(const _keyT& key);
	}

	//------------------------------------------------------------------------------------------------------------------
	// map implementation
	//------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------
	template<typename _ketT, typename _valueT>
	inline map<_keyT, _valueT>::map()
	{
		// Intentionally blank
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _ketT, typename _valueT>
	inline map<_keyT, _valueT>::~map()
	{
		// Intentionally blank
	}

}	// namespace rtl
}	// namespace rev

#endif // _REV_RTL_MAP_H_
