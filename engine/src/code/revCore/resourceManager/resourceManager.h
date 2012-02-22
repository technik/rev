////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 28th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resource Manager

#ifndef _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_
#define _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_

#include "resource.h"
#include "revCore/string.h"
#include "rtl/map.h"

using rtl::map;
using rtl::pair;

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, typename _keyT> // _T must inherit from TResource<_T,_keyT>
	class TResourceManager
	{
	public:
		_resourceT *	get					(const _keyT& _key);
		// Notice you have ownership of all resources you manually register
		void			registerResource	(_resourceT * _resource, const _keyT& _key);
		void			release				(_resourceT * _resource);
		void			clear				()	{	mResources.clear();	}
	protected:
		typedef map<_keyT, _resourceT*>	resourceMapT;

		resourceMapT	mResources;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Partial specialization for strings, so we can reload files in run time
	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT> // _T must inherit from TResource<_T,_keyT>
	class TResourceManager<_resourceT, string>
	{
	public:
		_resourceT *	get					(const string& _key);
		// Notice you have ownership of all resources you manually register
		void			registerResource	(_resourceT * _resource, const string& _key);
		void			release				(_resourceT * _resource);
		void			clear				()	{	mResources.clear();	}
	protected:
		typedef map<string, _resourceT*>	resourceMapT;

		resourceMapT	mResources;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Generic implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, typename _keyT>
	_resourceT* TResourceManager<_resourceT, _keyT>::get(const _keyT& _key)
	{
		typename resourceMapT::iterator searchResult = mResources.find(_key);
		_resourceT * resource;
		if(mResources.end() != searchResult) // We found it!
		{
			resource = *searchResult;
		}
		else
		{
			// Resource not found, so we create it and cache it
			resource = new _resourceT(_key);
			mResources.insert(pair<_keyT, _resourceT*>(_key, resource));
		}
		// Add a reference of ownership to the resource and return it
		TResource<_resourceT,_keyT>* baseResource = static_cast<TResource<_resourceT,_keyT>*>(resource);
		++baseResource->mReferences;
		baseResource->mKey = _key;
		return resource;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, typename _keyT>
	void TResourceManager<_resourceT, _keyT>::registerResource(_resourceT * _resource, const _keyT& _key)
	{
		mResources[_key] = _resource;
		TResource<_resourceT,_keyT>* baseResource = static_cast<TResource<_resourceT,_keyT>*>(_resource);
		++baseResource->mReferences;
		baseResource->mKey = _key;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT, typename _keyT>
	void TResourceManager<_resourceT, _keyT>::release(_resourceT * _resource)
	{
		TResource<_resourceT, _keyT> * baseResource = static_cast<TResource<_resourceT,_keyT>*>(_resource);
		// Decrease references
		--baseResource->mReferences;
		if(0 == baseResource->mReferences) // No one owns this resource anymore
		{
			// Delete the resource
			mResources.erase(baseResource->mKey);
			delete _resource;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// Specialized implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	_resourceT * TResourceManager<_resourceT, string>::get(const string& _key)
	{
		typename resourceMapT::iterator searchResult = mResources.find(_key);
		_resourceT * resource;
		if(mResources.end() != searchResult) // We found it!
		{
			resource = *searchResult;
		}
		else
		{
			// Resource not found, so we create it and cache it
			resource = new _resourceT(_key);
			mResources.insert(pair<string, _resourceT*>(_key, resource));
		}
		// Add a reference of ownership to the resource and return it
		TResource<_resourceT,string>* baseResource = static_cast<TResource<_resourceT,string>*>(resource);
		++baseResource->mReferences;
		baseResource->mKey = _key;
		return resource;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	void TResourceManager<_resourceT, string>::registerResource(_resourceT * _resource, const string& _key)
	{
		mResources[_key] = _resource;
		TResource<_resourceT,string>* baseResource = static_cast<TResource<_resourceT,string>*>(_resource);
		++baseResource->mReferences;
		baseResource->mKey = _key;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _resourceT>
	void TResourceManager<_resourceT, string>::release(_resourceT * _resource)
	{
		TResource<_resourceT, string> * baseResource = static_cast<TResource<_resourceT,string>*>(_resource);
		// Decrease references
		--baseResource->mReferences;
		if(0 == baseResource->mReferences) // No one owns this resource anymore
		{
			// Delete the resource
			mResources.erase(baseResource->mKey);
			delete _resource;
		}
	}

}	// namespace rev

#endif // _REV_REVCORE_RESOURCEMANAGER_RESOURCEMANAGER_H_
