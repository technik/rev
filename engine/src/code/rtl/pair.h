////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, revolution template library
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 17th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pair

#ifndef _REV_RTL_PAIR_H_
#define _REV_RTL_PAIR_H_

namespace rtl
{
	template<typename _T1, typename _T2>
	class pair
	{
	public:
		pair() {}
		pair(_T1 _first, _T2 _second): first(_first), second(_second)
		{
		}

		bool operator < (const pair<_T1, _T2>& _x) const
		{
			return (first<_x.first)?
				true:
				(_x.first<first?
					false:
					second < _x.second);
		}

		_T1 first;
		_T2 second;
	};

}	// namespace rtl

#endif // _REV_RTL_PAIR_H_
