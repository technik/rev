//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Wrapper to map rev allocators into stl-compliant allocators
#ifndef _REV_CORE_MEMORY_STDALLOCATOR_H_
#define _REV_CORE_MEMORY_STDALLOCATOR_H_

#include <limits>

namespace rev {
	namespace core {

		template<class BaseAlloc_, class T_>
		class StdAllocator : public BaseAlloc_ {
		public:
			// Public types
			typedef T_				value_type;
			typedef	T_*				pointer;
			typedef const T_*		const_pointer;
			typedef T_&				reference;
			typedef const T_&		const_reference;
			typedef std::size_t		size_type;
			typedef std::ptrdiff_t	difference_type;

		public:
			// Convert an allocator<T_> to allocator<U_>
			template<typename U_>
			struct rebind {
				typedef StdAllocator<BaseAlloc_,U_> other;
			};

		public:
			StdAllocator():BaseAlloc_() {}
			explicit StdAllocator(BaseAlloc_ _b) :BaseAlloc_(_b) {}
			~StdAllocator() {}
			StdAllocator(StdAllocator const& _x) :BaseAlloc_(_x) {}
			template<typename U>
			explicit StdAllocator(StdAllocator<BaseAlloc_,U> const&) :BaseAlloc_() {}

			//    address
			pointer address(reference r) { return &r; }
			const_pointer address(const_reference r) { return &r; }

			//    memory allocation
			pointer allocate(size_type _cnt,
				typename std::allocator<void>::const_pointer = 0) {
				return BaseAlloc_::template allocate<T_>(_cnt);
			}
			void deallocate(pointer _p, size_type _cnt) {
				BaseAlloc_::deallocate(_p,_cnt);
			}

			//    size
			size_type max_size() const {
				return std::numeric_limits<size_type>::max() / sizeof(T_);
			}

			//    construction/destruction
			void construct(pointer p, const T_& t) { new(p)T_(t); }
			void destroy(pointer _p) { 
				#ifdef _WIN32
				_p; // Work around for visual studio incorrect warning
				#endif // _WIN32
				_p->~T_();
			}

			bool operator==(StdAllocator const&) { return true; }
			bool operator!=(StdAllocator const& a) { return !operator==(a); }
		};

	} // namespace core
}	// namespace rev

#include "stdAllocator.inl"

#endif // _REV_CORE_MEMORY_STDALLOCATOR_H_