////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 28th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resource Manager

#ifndef _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_
#define _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_

#include <revCore/string.h>
#include <rtl/map.h>

namespace rev
{
	template <class _resourceT, class _keyT>
	class CResourceManager
	{
	public:
		_resourceT * get				(const _keyT& ) const;			// Get resource by key (Implies ownership)
		void		 registerResource	(_resourceT*, const _keyT&);	// Register a resource in the manager
		void		 release			(const _keyT& );				// Release resource by key
		void		 release			(_resourceT *);					// Release resource by pointer
	private:
		rtl::map<_keyT, _resourceT*>	mResources;
	};

	// Partial specialization for const char * keys
	template < class _resourceT >
	class CResourceManager <_resourceT, const char*>
	{
	public:
		_resourceT* get					(const char*) const;
		void		registerResource	(_resourceT*, const char*);
		void		release				(const char*);
		void		release				(_resourceT*);
	private:
		rtl::map<rev::string,_resourceT*>	mResources;
	};

	// Inline implementation
	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, class _keyT>
	_resourceT * CResourceManager<_resourceT, _keyT>::get(const _keyT& _x) const
	{
		typename rtl::map<_keyT,_resourceT*>::const_iterator i = mResources.find(_x);
		if(i != mResources.end())
		{
			return i->second;
		}
		else return 0;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	_resourceT * CResourceManager<_resourceT, const char*>::get(const char* _x) const
	{
		typename rtl::map<rev::string,_resourceT*>::const_iterator i = mResources.find(rev::string(_x));
		if(i != mResources.end())
		{
			return i->second;
		}
		else return 0;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, class _keyT>
	void CResourceManager<_resourceT,_keyT>::registerResource(_resourceT * _res, const _keyT& _x)
	{
		mResources[_x] = _res;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	void CResourceManager<_resourceT,const char*>::registerResource(_resourceT * _res, const char* _x)
	{
		mResources[rev::string(_x)] = _res;
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
	template<class _resourceT>
	void CResourceManager<_resourceT,const char*>::release(_resourceT * _res)
	{
		typename rtl::map<string,_resourceT*>::iterator i = mResources.begin();
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
	void CResourceManager<_resourceT, _keyT>::release(const _keyT& _x)
	{
		mResources.erase(_x);
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	void CResourceManager<_resourceT, const char*>::release(const char* _x)
	{
		mResources.erase(string(_x));
	}

}	// namespace rev

#endif // _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_
