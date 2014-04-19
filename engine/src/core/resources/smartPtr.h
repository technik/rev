//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/12
//----------------------------------------------------------------------------------------------------------------------
// Generic smart pointer
#ifndef _REV_CORE_RESOURCES_SMARTPTR_H_
#define _REV_CORE_RESOURCES_SMARTPTR_H_

namespace rev {
	namespace core {

		//-----------------------------------------------------------------------------------------------------
		template<class T_>
		class JustDelete {
			inline void destroy(const T_* _ptr) { delete _ptr; }
		};

		//-----------------------------------------------------------------------------------------------------
		template<class T_, template<class,class> class Ownership_, class Destroy_ = JustDelete>
		class SmartPtr : public Ownership_<T_,Destroy_>{
		public:
			SmartPtr() {}

			SmartPtr(const SmartPtr& _x)
				:Ownership_<T_, Destroy_>(_x)
			{}

			SmartPtr(SmartPtr&& _x)
				:Ownership_<T_, Destroy_>(_x)
			{}

			SmartPtr(T_* _ptr)
				:Ownership_<T_, Destroy_>(_ptr)
			{}

			SmartPtr& operator=(const SmartPtr& _x) {
				Ownership_<T_, Destroy_>::operator= (_x);
				return *this;
			}

			SmartPtr& operator=(SmartPtr&& _x) {
				Ownership_<T_, Destroy_>::operator= (_x);
				return *this;
			}

			explicit operator T_*() const { return pointee; }

			bool operator==(const SmartPtr& _x) const {
				return pointee == _x.pointee;
			}

			bool operator!=(const SmartPtr& _x) const {
				return pointee != _x.pointee;
			}

			T_* operator->() const { return pointee; }
			T_& operator&() const { return *pointee; }
			T_& operator*() const { return *pointee; }
		};

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_RESOURCES_SMARTPTR_H_