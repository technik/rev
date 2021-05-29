//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/11
//----------------------------------------------------------------------------------------------------------------------
// Generic resource manager
#ifndef _REV_CORE_RESOURCES_RESOURCEMANAGER_INL_
#define _REV_CORE_RESOURCES_RESOURCEMANAGER_INL_

#include <cassert>

#include "resourceManager.h"

namespace rev {
	namespace core {
		//--------------------------------------------------------------------------------------------------------------
		template<typename Key_, typename Val_, typename Creator_>
		ResourceMgr<Key_, Val_, Creator_>*
			ResourceMgr<Key_, Val_, Creator_>::manager() 
		{
			if(!sInstance)
				startUp();
			return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename Key_, typename Val_, typename Creator_>
		void ResourceMgr<Key_, Val_, Creator_>::startUp()
		{
			assert(!sInstance);
			sInstance = new ResourceMgr();
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename Key_, typename Val_, typename Creator_>
		void ResourceMgr<Key_, Val_, Creator_>::shutDown()
		{
			assert(sInstance);
			delete sInstance;
			sInstance = nullptr;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename Key_, typename Val_, typename Creator_>
		typename ResourceMgr<Key_, Val_, Creator_>::Ptr
			ResourceMgr<Key_, Val_, Creator_>::get(const Key_& _key)
		{
			auto resIterator = mResources.find(_key);
			if (resIterator == mResources.end()) {
				Val_* newResource = Creator_::create(_key);
				mResources.insert(std::make_pair(_key, newResource));
				return Ptr(newResource, Destroy());
			}
			return Ptr(resIterator->second, Destroy());
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename Key_, typename Val_, typename Creator_>
		ResourceMgr<Key_, Val_, Creator_>::ResourceMgr()
		{
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename Key_, typename Val_, typename Creator_>
		void ResourceMgr<Key_, Val_, Creator_>::release(
			Val_* _v)
		{
			for (auto element : mResources) {
				if (element.second == _v) {
					onRelease(element.first, _v);
					Creator_::destroy(_v);
					mResources.erase(element.first);
					return;
				}
			}
			assert(false);
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_RESOURCES_RESOURCEMANAGER_INL_