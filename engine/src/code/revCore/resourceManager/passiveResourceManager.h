////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on February 19th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Passive resource manager

#ifndef _REV_REVCORE_RESOURCEMANAGER_PASSIVERESOURCEMANAGER_H_
#define _REV_REVCORE_RESOURCEMANAGER_PASSIVERESOURCEMANAGER_H_

#include "passiveResource.h"
#include "revCore/string.h"
#include "rtl/map.h"

using rtl::map;
using rtl::pair;

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT> // _resourceT must inherit from TPassiveResource<_T>
	class TPassiveResourceManager
	{
	public:
		_resourceT *	get					(const string& _key);
		// Notice you have ownership of all resources you manually register
		void			registerResource	(_resourceT * _resource, const string& _key);
		void			release				(_resourceT * _resource);
		void			release				(const string& _key);
		void			clear				()	{ mResources.clear();	}
	protected:
		typedef map<string, _resourceT*>	resourceMapT;

		resourceMapT	mResources;
	};

	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	_resourceT* TPassiveResourceManager<_resourceT>::get(const string& _key)
	{
		typename resourceMapT::iterator searchResult = mResources.find(_key);
		if(mResources.end() != searchResult) // We found it!
		{
			_resourceT * resource = searchResult->second;
			// Add a reference of ownership to the resource and return it
			TPassiveResource<_resourceT>* baseResource = static_cast<TPassiveResource<_resourceT>*>(resource);
			++baseResource->mReferences;
			baseResource->mKey = _key;
			return resource;
		}
		else return 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	void TPassiveResourceManager<_resourceT>::registerResource(_resourceT * _resource, const string& _key)
	{
		mResources[_key] = _resource;
		TPassiveResource<_resourceT>* baseResource = static_cast<TPassiveResource<_resourceT>*>(_resource);
		++baseResource->mReferences;
		baseResource->mKey = _key;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	void TPassiveResourceManager<_resourceT>::release(_resourceT * _resource)
	{
		TPassiveResource<_resourceT> * baseResource = static_cast<TPassiveResource<_resourceT>*>(_resource);
		// Decrease references
		--baseResource->mReferences;
		if(0 == baseResource->mReferences) // No one owns this resource anymore
		{
			// Delete the resource
			if(!baseResource->mKey.empty()) // Registered resource
			{
				typename resourceMapT::iterator iter = mResources.find(baseResource->mKey);
				revAssert(mResources.end() != iter);
				mResources.erase(iter);
			}
			delete _resource;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	void TPassiveResourceManager<_resourceT>::release(const string& _key)
	{
		typename resourceMapT::iterator iter = mResources.find(_key);
		TPassiveResource<_resourceT> * baseResource = iter->second;
		// Decrease references
		--baseResource->mReferences;
		if(0 == baseResource->mReferences) // No one owns this resource anymore
		{
			// Delete the resource
			mResources.erase(iter);
			delete baseResource;
		}
	}

}	// namespace rev

#endif // _REV_REVCORE_RESOURCEMANAGER_PASSIVERESOURCEMANAGER_H_
