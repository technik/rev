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
			static void startUp();
			static void shutDown();
			static SystemMemory* get();

			template<typename T_>
			static T_*	allocate(size_t _n);
			template<typename T_>
			static void free(T_* _ptr, size_t _n);

			virtual ~SystemMemory();

		private:
			virtual void*	allocBuffer	(size_t _size) = 0;
			virtual void	freeBuffer	(void* _ptr) = 0;
		};

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_MEMORY_SYSTEMMEMORY_H_