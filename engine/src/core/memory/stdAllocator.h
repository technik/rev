//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_CORE_MEMORY_STDALLOCATOR_H_
#define _REV_CORE_MEMORY_STDALLOCATOR_H_

namespace rev {
	namespace core {

		class StdAllocator {
		public:
			// Object construction and destruction
			template<class T_, typename ... Args_>
			T_*		create		(Args_ ... _args, unsigned _n = 1);
			template<class T_>
			void	destroy		(const T_* _ptr, unsigned _n = 1);

			// Raw allocation
			template<class T_ = void>
			T_*		allocate	(unsigned _n = 1);
			template<class T_>
			void	deallocate	(const T_* _ptr, unsigned _n = 1);
		};

	} // namespace core
}	// namespace rev

#endif // _REV_CORE_MEMORY_STDALLOCATOR_H_