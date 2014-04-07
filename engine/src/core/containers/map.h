//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/06
//----------------------------------------------------------------------------------------------------------------------
// Mapping to the standard map using engine allocator as default allocator
#ifndef _REV_CORE_CONTAINERS_MAP_H_
#define _REV_CORE_CONTAINERS_MAP_H_

#include <map>
#include <core/memory/stdAllocator.h>
#include <core/memory/defaultAllocator.h>

namespace rev {
	namespace core {

		template<typename Key_, typename Val_, typename Allocator_ = DefaultAllocator>
		using map = std::map < Key_, Val_, std::less<Key_>, StdAllocator<Allocator_, std::pair<Key_,Val_>>>;

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_CONTAINERS_MAP_H_