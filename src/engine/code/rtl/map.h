////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 22nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// map

#ifndef _REV_RTL_MAP_H_
#define _REV_RTL_MAP_H_

#include <vector.h>

namespace rev { namespace rtl
{
	//------------------------------------------------------------------------------------------------------------------
	// map class declaration
	//------------------------------------------------------------------------------------------------------------------
	template<typename _keyT, typename _vauleT>
	class map<_keyT, _valueT>
	{
	public:
		map();
		~map();
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
