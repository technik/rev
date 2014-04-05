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
			template<typename Allocator_>
			static void startUp();
			static void shutDown();
		};

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_PLATFORM_PLATFORM_H_