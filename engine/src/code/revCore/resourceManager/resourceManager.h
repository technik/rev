////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 28th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resource Manager

#ifndef _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_
#define _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_

#include <rtl/map.h>

namespace rev
{
	template <class _resourceT, class _keyT>
		class CResourceManager
	{
	public:
		_resourceT * get				(_keyT ) const;			// Get resource by key (Implies ownership)
		void		 registerResource	(_resourceT*, _keyT);	// Register a resource in the manager
		void		 release			(_keyT );				// Release resource by key
		void		 release			(_resourceT *);			// Release resource by pointer
	private:
		rtl::map<_keyT, _resourceT*>	mResources;
	};

	// Inline implementation
	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, class _keyT>
	_resourceT * CResourceManager<_resourceT, _keyT>::get(_keyT _x) const
	{
		typename rtl::map<_keyT,_resourceT*>::const_iterator i = mResources.find(_x);
		if(i != mResources.end())
		{
			return i->second;
		}
		else return 0;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, class _keyT>
	void CResourceManager<_resourceT,_keyT>::registerResource(_resourceT * _res, _keyT _x)
	{
		mResources[_x] = _res;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, class _keyT>
	void CResourceManager<_resourceT,_keyT>::release(_resourceT * _res)
	{
		typename rtl::map<_keyT,_resourceT*>::iterator i = mResources.begin();
		for(; i != mResources.end(); ++i)
		{
			if(i->second == _res)
			{
				mResources.erase(i);
			}
		}
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, class _keyT>
	void CResourceManager<_resourceT, _keyT>::release(_keyT _x)
	{
		mResources.erase(_x);
	}

}	// namespace rev

#endif // _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_
