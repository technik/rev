//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Mapping to the standard vector using engine allocator as default allocator
#ifndef _REV_CORE_CONTAINERS_VECTOR_H_
#define _REV_CORE_CONTAINERS_VECTOR_H_

#include <vector>
#include <core/memory/stdAllocator.h>
#include <core/memory/defaultAllocator.h>

namespace rev {
	namespace core {

		template<typename T_, typename Allocator_ = DefaultAllocator>
		using vector = std::vector < T_, StdAllocator<Allocator_,T_>>;

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_CONTAINERS_VECTOR_H_