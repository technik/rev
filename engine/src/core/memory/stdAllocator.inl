//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_CORE_MEMORY_STDALLOCATOR_INL_
#define _REV_CORE_MEMORY_STDALLOCATOR_INL_

#include "stdAllocator.h"

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		template<class T_, typename ... Args_>
		T_*	StdAllocator::create(Args_ ... _args, unsigned _n) {
			return new T_(_args)[_n];
		}

	} // namespace core
}	// namespace rev

#endif // _REV_CORE_MEMORY_STDALLOCATOR_INL_