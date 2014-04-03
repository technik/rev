//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Wrapper to map rev allocators into stl-compliant allocators
#ifndef _REV_CORE_MEMORY_STDALLOCATOR_H_
#define _REV_CORE_MEMORY_STDALLOCATOR_H_

namespace rev {
	namespace core {

		template<class BaseAlloc_, class T_>
		class StdAllocator {
		public:
			// Public types
			typedef T_				value_type;
			typedef	T_*				pointer;
			typedef const T_*		const_pointer;
			typedef T_&				reference;
			typedef const T_&		const_reference;
			typedef std::size_t		size_type;
			typedef std::ptrdiff_t	difference_type;


		};

	} // namespace core
}	// namespace rev

#include "stdAllocator.inl"

#endif // _REV_CORE_MEMORY_STDALLOCATOR_H_