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
		template<typename Key_, typename Val_, typename Creator_, template<class, class> class Ownership_>
		ResourceMgr<Key_, Val_, Creator_, Ownership_>*
			ResourceMgr<Key_, Val_, Creator_, Ownership_>::manager() {
				assert(sInstance);
				return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_>
		template<class SingAlloc_>
		void ResourceMgr<Key_, Val_, Creator_, Ownership_>::startUp(
				SingAlloc_& _singAlloc)
		{
			assert(!sInstance);
			sInstance = _singAlloc.template allocate<ResourceMgr>();
			new(sInstance)ResourceMgr();
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_>
		template<class SingAlloc_>
		void ResourceMgr<Key_, Val_, Creator_, Ownership_>::shutDown(
			SingAlloc_& _singAlloc)
		{
			assert(sInstance);
			sInstance->~ResourceMgr();
			_singAlloc.deallocate(sInstance);
			sInstance = nullptr;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_>
		typename ResourceMgr<Key_, Val_, Creator_, Ownership_>::Ptr
			ResourceMgr<Key_, Val_, Creator_, Ownership_>::get(const Key_& _key)
		{
			auto resIterator = mResources.find(_key);
			if (resIterator == mResources.end()) {
				Val_* newResource = Creator_::create(_key);
				mResources.insert(std::make_pair(_key, newResource));
				return Ptr(newResource);
			}
			return Ptr(resIterator->second);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_>
		ResourceMgr<Key_, Val_, Creator_, Ownership_>::ResourceMgr()
		{
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_>
		void ResourceMgr<Key_, Val_, Creator_, Ownership_>::release(
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