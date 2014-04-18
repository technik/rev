//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// Generic event
#ifndef _REV_CORE_EVENT_H_
#define _REV_CORE_EVENT_H_

#include <functional>
#include <core/containers/vector.h>

namespace rev {
	namespace core {
		template<typename ... Arg_>
		class Event {
		public:
			typedef std::function<void(Arg_...)>	Delegate;

			// Handle delegates
			void operator+=(Delegate);
			void operator-=(Delegate);
			void clear() { mDelegates.clear(); }

			// Invoke
			void operator()(Arg_...) const;

		private:
			vector<Delegate>	mDelegates;
		};

		//--------------------------------------------------------------------------------------------------------------
		// Inline
		//--------------------------------------------------------------------------------------------------------------
		template<typename...Arg_>
		void Event<Arg_...>::operator+=(Delegate _delegate) {
			mDelegates.push_back(_delegate);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename...Arg_>
		void Event<Arg_...>::operator-= (Delegate _delegate)
		{
			for (auto i = mDelegates.begin(); i != mDelegates.end(); ++i) {
				if (*i == _delegate) {
					mDelegates.erase(i);
					return;
				}
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename...Arg_>
		void Event<Arg_...>::operator()(Arg_ ... _args) const
		{
			for (auto curDelegate : mDelegates)
				curDelegate(_args...);
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_EVENT_H_