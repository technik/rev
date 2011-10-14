////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 17th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pair

#ifndef _REV_RTL_PAIR_H_
#define _REV_RTL_PAIR_H_

namespace rev { namespace rtl
{
	template<typename _T1, typename _T2>
	class pair
	{
	public:
		pair(_T1 _first, _T2 _second): first(_first), second(_second)
		{
		}

		_T1 first;
		_T2 second;
	};

}	// namespace rtl
}	// namespace rev

#endif // _REV_RTL_PAIR_H_
