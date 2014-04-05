//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Allocator using default platform memory system
#ifndef _REV_CORE_MEMORY_DEFAULTALLOCATOR_H_
#define _REV_CORE_MEMORY_DEFAULTALLOCATOR_H_

namespace rev {
	namespace core {

		class DefaultAllocator {
		public:
			// Object construction and destruction
			// 666 TODO: Maybe add interface for constructing arrays of objects?
			template<class T_, typename ... Args_>
			T_*		create(Args_ ... _args);
			template<class T_>
			void	destroy(const T_* _ptr);

			// Raw allocation
			template<class T_ = void>
			T_*		allocate(unsigned _n = 1);
			template<class T_>
			void	deallocate(const T_* _ptr, unsigned _n = 1);

			virtual ~DefaultAllocator() = default;
		};

	}	// namespace core
}	// namespace rev

#include "defaultAllocator.inl"

#endif // _REV_CORE_MEMORY_DEFAULTALLOCATOR_H_