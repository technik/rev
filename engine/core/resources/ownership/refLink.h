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
			RefLink() = default;
			RefLink(T_* _ptr)
				:pointee(_ptr)
			{}

			RefLink(const RefLink& _x)
				:pointee(_x.pointee)
				, prev(&_x)
				, post(_x.post)
			{
				_x.post = this;
				if (post)
					post->prev = this;
			}

			RefLink(RefLink&& _x)
				:pointee(_x.pointee)
				, prev(_x.prev)
				, post(_x.post)
			{
				_x.prev = nullptr;
				_x.post = nullptr;
				_x.pointee = nullptr;
				if (prev)
					prev->post = this;
				if (post)
					post->prev = this;
			}

			~RefLink() {
				if (prev)
					prev->post = post;
				if (post)
					post->prev = prev;
				if (pointee && !prev && !post)
					Destroy_::destroy(pointee);
			}

			RefLink& operator=(const RefLink& _x) {
				if (!(_x.pointee == pointee)) {
					this->~RefLink();
					new(this)RefLink(_x);
				}
				return *this;
			}

			RefLink& operator=(RefLink&& _x) {
				if (!(_x.pointee == pointee)) {
					this->~RefLink();
					new(this)RefLink(_x);
				}
				return *this;
			}

		protected:
			T_*	pointee = nullptr;
			mutable RefLink const* prev = nullptr;
			mutable RefLink const* post = nullptr;
		};

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_RESOURCES_OWNERSHIP_REFLINK_H_