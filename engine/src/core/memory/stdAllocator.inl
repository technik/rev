//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Standard allocator using new and delete
#ifndef _REV_CORE_MEMORY_STDALLOCATOR_INL_
#define _REV_CORE_MEMORY_STDALLOCATOR_INL_

#include "stdAllocator.h"

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		template<class T_, typename ... Args_>
		T_*	StdAllocator::create(Args_ ... _args) {
			return new T_(_args...);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class T_>
		void StdAllocator::destroy(const T_* _ptr) {
			delete _ptr;
		}

	} // namespace core
}	// namespace rev

#endif // _REV_CORE_MEMORY_STDALLOCATOR_INL_