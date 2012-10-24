//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 19th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Resource Manager

#ifndef _REV_CORE_RESOURCE_RESOURCEMANAGER_H_
#define _REV_CORE_RESOURCE_RESOURCEMANAGER_H_

#include <unordered_map>
namespace std { namespace tr1 { using namespace std; }} // To enable std::tr1::hash

#include <revCore/codeTools/assert/assert.h>

namespace rev
{
	template <class Resource_, class Key_, class Hasher_ = typename std::tr1::hash<Key_> >
	class ResourceManager
	{
	public:
		Resource_*	get					(const Key_& ) const;		// Get resource by key (Implies ownership)
		void		registerResource	(Resource_*, const Key_&);	// Register a resource in the manager
		bool		release				(const Key_& );				// Release resource by key
		bool		release				(Resource_ *);				// Release resource by pointer
	private:
		typedef		std::unordered_map<Key_,Resource_*,Hasher_>	ResourceTree;
		ResourceTree	mResources;
	};

	//----------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//----------------------------------------------------------------------------------------------------------------------
	template <class Resource_, class Key_, class Hasher_>
	Resource_* ResourceManager<Resource_,Key_, Hasher_>::get(const Key_& _x) const
	{
		auto i = mResources.find(_x); // Search the key in our registered resources
		if(i != mResources.end()) // If we find it
		{
			Resource_ * res = i->second;
			res->getOwnership();
			return res;
		}
		return nullptr;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Resource_, class Key_, class Hasher_>
	void ResourceManager<Resource_,Key_,Hasher_>::registerResource(Resource_* _res, const Key_& _key)
	{
#ifdef DEBUG
		assert(mResources.find(_key) != mResources.end(), "Error, there's already a resource registered with that key");
#endif // DEBUG
		mResources[_key] = _res;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Resource_, class Key_, class Hasher_>
	bool ResourceManager<Resource_, Key_, Hasher_>::release(Resource_* _x)
	{
		auto i = mResources.begin();
		auto e = mResources.end();
		for(; i != e; ++i)
		{
			if(i->second == _x)
			{
				if(!_x->release())
				{
					mResources.erase(i);
					return false;
				}
				else
					return true;
			}
		}
		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Resource_, class Key_, class Hasher_>
	bool ResourceManager<Resource_,Key_, Hasher_>::release(const Key_& _x)
	{
		auto i = mResources.find(_x);
		if(i != mResources.end())
		{
			if(!i->second->release())
			{
				mResources.erase(i);
				return false;
			}
			else return true;
		}
		return false;
	}

}	// namespace rev

#endif // _REV_CORE_RESOURCE_RESOURCEMANAGER_H_
