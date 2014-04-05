//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Allocator using default platform memory system
#ifndef _REV_CORE_MEMORY_DEFAULTALLOCATOR_INL_
#define _REV_CORE_MEMORY_DEFAULTALLOCATOR_INL_

#include "defaultAllocator.h"
#include "systemMemory.h"

namespace rev {
	namespace core {

		//----------------------------------------------------------------------------------------------------------------------
		template<class T_, typename ... Args_>
		T_* DefaultAllocator::create(Args_ ... _args) {
			T_* buffer = SystemMemory::allocate<T_>();
			return new(buffer) T_(_args ...);
		}

		//----------------------------------------------------------------------------------------------------------------------
		template<class T_>
		void DefaultAllocator::destroy(const T_* _ptr) {
			_ptr->~T_();
			SystemMemory::deallocate(_ptr);
		}

		//----------------------------------------------------------------------------------------------------------------------
		template<class T_>
		T_* DefaultAllocator::allocate(unsigned _n) {
			return SystemMemory::allocate<T_>(_n);
		}

		//----------------------------------------------------------------------------------------------------------------------
		template<class T_>
		void DefaultAllocator::deallocate(const T_* _ptr, unsigned _n) {
			SystemMemory::deallocate(_ptr, _n);
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_MEMORY_DEFAULTALLOCATOR_INL_