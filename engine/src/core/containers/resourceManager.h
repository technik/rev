//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/08
//----------------------------------------------------------------------------------------------------------------------
// Generic resource manager
#ifndef _REV_CORE_CONTAINERS_RESOURCEMANAGER_H_
#define _REV_CORE_CONTAINERS_RESOURCEMANAGER_H_

#include "map.h"
#include "../memory/defaultAllocator.h"

namespace rev {
	namespace core {
		//-----------------------------------------------------------------------------------------------------
		template<typename Key_, typename Val_, typename Creator_, typename Ownership_,
			typename MapAlloc_ = DefaultAllocator>
		class ResourceMgr : public Creator_ {
		public:
			// Singleton interface
			static ResourceMgr* manager();
			template<typename Alloc_>
			static void startUp(Alloc_& = Alloc_(), MapAlloc_& = MapAlloc_());
			template<typename Alloc_>
			static void shutDown(Alloc_& = Alloc_());

			// Resource manager interface
			Ownership_::Pointee get(const Key_&);

		private:
			ResourceMgr(MapAlloc_&);
			~ResourceMgr() = default;

			void release(Val_*);

			ResourceMgr*				sIntance;
			map<Key_, Val_, MapAlloc_>	mResources;
		};

		//-----------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, Ownership_, typename MapAlloc_ = DefaultAllocator>
		class ManagedResource {
		public:
			typedef ResourceMgr<Key_, Val_, Creator_, Ownership_, MapAlloc_>	ResourceMgr;
			inline static ResourceMgr*	manager() { ResourceMgr::manager(); }
		};
	}	// namespace core
}	// namespace rev

#include "resourceManager.inl"

#endif // _REV_CORE_CONTAINERS_RESOURCEMANAGER_H_