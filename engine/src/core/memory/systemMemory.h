//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Manage memory at the base level
#ifndef _REV_CORE_MEMORY_SYSTEMMEMORY_H_
#define _REV_CORE_MEMORY_SYSTEMMEMORY_H_

namespace rev {
	namespace core {

		class SystemMemory {
		public:
			template<typename Alloc_>
			static void startUp(Alloc_&);
			template<typename Alloc_>
			static void shutDown(Alloc_&);
			static SystemMemory* get();

			template<typename T_>
			static T_*	allocate(size_t _n = 1);
			template<typename T_>
			static void deallocate(const T_* _ptr, size_t _n = 1);

			virtual ~SystemMemory() = default;

		private:
			static SystemMemory* sInstance;
			virtual void*	allocBuffer	(size_t _size) = 0;
			virtual void	freeBuffer	(const void* _ptr, size_t _n) = 0;
		};

	}	// namespace core
}	// namespace rev

#include "systemMemory.inl"

#endif // _REV_CORE_MEMORY_SYSTEMMEMORY_H_