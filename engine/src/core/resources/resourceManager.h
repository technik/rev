//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/08
//----------------------------------------------------------------------------------------------------------------------
// Generic resource manager
#ifndef _REV_CORE_RESOURCES_RESOURCEMANAGER_H_
#define _REV_CORE_RESOURCES_RESOURCEMANAGER_H_

#include <core/resources/smartPtr.h>

#include <cassert>
#include <functional>
#include <map>

namespace rev {
	namespace core {
		//-----------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_>
		class ResourceMgr : public Creator_ {
		public:

			// Singleton interface
			static ResourceMgr* manager();
			static void startUp();
			static void shutDown();
			void setOnRelease(std::function<void(const Key_&, Val_*)> _fn) { onRelease = _fn; }

		private:
			class Destroy {
			protected:
				void destroy(Val_* _v) {
					ResourceMgr::manager()->release(_v);
				}
			};

		public:
			typedef SmartPtr<Val_, Ownership_, Destroy>	Ptr;
			// Resource manager interface
			Ptr get(const Key_&);

		private:
			ResourceMgr();
			~ResourceMgr() = default;

			void release(Val_*);

			static ResourceMgr*			sInstance;
			std::function<void(const Key_&, Val_*)>	onRelease = [](const Key_&, Val_*){ assert(false); };
			std::map<Key_, Val_*>	mResources;
		};

		//-----------------------------------------------------------------------------------------------------
		template<class Key_, class Val_, class Creator_, template<class, class> class Ownership_>
		class ManagedResource {
		public:
			typedef ResourceMgr<Key_, Val_, Creator_, Ownership_>	Mgr;
			typedef typename Mgr::Ptr	Ptr;
			inline static Mgr*	manager() { return Mgr::manager(); }
		};

	}	// namespace core
}	// namespace rev

#include "resourceManager.inl"

#endif // _REV_CORE_RESOURCES_RESOURCEMANAGER_H_