//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Singleton to interact with platform dependent systems
#ifndef _REV_CORE_PLATFORM_PLATFORM_H_
#define _REV_CORE_PLATFORM_PLATFORM_H_

namespace rev {
	namespace core {

		class Platform {
		public:
			template<typename Allocator_>
			static void startUp(Allocator_&);
			template<typename Allocator_>
			static void shutDown(Allocator_&);
		};

	}	// namespace core
}	// namespace rev

#include "platform.inl"

#endif // _REV_CORE_PLATFORM_PLATFORM_H_