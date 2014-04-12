//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/12
//----------------------------------------------------------------------------------------------------------------------
// Generic smart pointer
#ifndef _REV_CORE_RESOURCES_OWNERSHIP_REFLINK_H_
#define _REV_CORE_RESOURCES_OWNERSHIP_REFLINK_H_

namespace rev {
	namespace core {

		//-----------------------------------------------------------------------------------------------------
		template<class T_, class Destroy_>
		class RefLink : Destroy_ {
		public:
			RefLink(T_* _ptr)
				:pointee(_ptr)
			{}

			RefLink(RefLink& _x)
				:prev(&_x)
				, post(_x.post)
			{
				_x.post = this;
				if (post)
					post->prev = this;
			}

			~RefLink() {
				if (prev)
					prev->post = post;
				if (post)
					post->prev = prev;
				if (!prev && !post)
					destroy(pointee);
			}

			RefLink& operator=(RefLink& _x) {
				if (!(_x.pointee == pointee)) {
					~RefLink();
					new(this)RefLink(_x);
				}
				return *this;
			}

		protected:
			T_*	pointee;
			RefLink* prev = nullptr;
			RefLink* post = nullptr;
		};

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_RESOURCES_OWNERSHIP_REFLINK_H_