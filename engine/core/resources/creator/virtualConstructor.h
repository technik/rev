//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/12
//----------------------------------------------------------------------------------------------------------------------
// Construction for single-line inheritance with run-time defined construction method
#ifndef _REV_CORE_RESOURCES_CREATOR_VIRTUALONSTRUCTOR_H_
#define _REV_CORE_RESOURCES_CREATOR_VIRTUALONSTRUCTOR_H_

#include <functional>

namespace rev {
	namespace core {
		//-----------------------------------------------------------------------------------------------------
		template<class T_, class ... Arg_>
		class VirtualConstructor {
		public:
			T_* create(Arg_ ...);
			void destroy(const T_*);

			typedef std::function < T_* (Arg_ ...)>		Creator;
			void setCreator(Creator);

		private:
			Creator	mCreator;
		};

		//-----------------------------------------------------------------------------------------------------
		// Inline implementation
		template<class T_, class ... Arg_>
		T_* VirtualConstructor<T_,Arg_...>::create(Arg_ ... _args) {
			return mCreator(_args...);
		}

		//-----------------------------------------------------------------------------------------------------
		template<class T_, class ... Arg_>
		void VirtualConstructor<T_, Arg_...>::destroy(const T_* _ptr) {
			delete _ptr;
		}

		//-----------------------------------------------------------------------------------------------------
		template<class T_, class ... Arg_>
		void VirtualConstructor<T_, Arg_...>::setCreator(Creator _creator){
			mCreator = _creator;
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_RESOURCES_CREATOR_VIRTUALONSTRUCTOR_H_