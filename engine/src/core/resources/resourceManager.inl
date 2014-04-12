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
		template<typename Key_, typename Val_, typename Creator_, template<class, class> class Ownership_, typename MapAlloc_>
		ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>*
			ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>::manager() {
				assert(sInstance);
				return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_, class MapAlloc_>
		template<class SingAlloc_>
		void ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>::startUp(
				SingAlloc_& _singAlloc, MapAlloc_& _mapAlloc)
		{
			assert(!sInstance);
			sInstance = _singAlloc.create<ResourceMgr>(_mapAlloc);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_, class MapAlloc_>
		template<class SingAlloc_>
		void ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>::shutDown(
			SingAlloc_& _singAlloc)
		{
			assert(sInstance);
			_singAlloc.destroy(sInstance);
			sInstance = nullptr;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_, class MapAlloc_>
		typename ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>::Ptr
			ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>::get(const Key_& _key)
		{
			auto resIterator = mResources.find(_key);
			if (resIterator == mResources.end()) {
				Val_* newResource = create<Val_>(_key);
				auto newEntry = std::make_pair(_key, _newResource);
				mResources.insert(newEntry);
				return Ptr(newResource);
			}
			return Ptr(nullptr);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_, class MapAlloc_>
		ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>::ResourceMgr(MapAlloc_& _mapAlloc)
			:mResources(_mapAlloc)
		{
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_, class MapAlloc_>
		void ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>::release(
			Val_* _v)
		{
			for (auto element : mResources) {
				if (element->second == _v) {
					destroy(resource);
					mResources.erase(elenment);
				}
			}
			assert(false);
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_RESOURCES_RESOURCEMANAGER_INL_