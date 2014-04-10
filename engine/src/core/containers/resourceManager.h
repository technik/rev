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
		template<typename Key_, typename Val_>
		class ResourceMgr : Creator_ {
		public:
			Ptr_ get(const Key_&);
			static ResourceMgr* manager();

		private:
			ResourceMgr* sIntance;
		};

		template<class Key_, class Val_>
		class ManagedResource {
		public:
			typedef ResourceMgr<Key_, Val_>	ResourceMgr;
			inline static ResourceMgr*	manager() { ResourceMgr::manager(); }
		};
	}
}

#endif // _REV_CORE_CONTAINERS_RESOURCEMANAGER_H_