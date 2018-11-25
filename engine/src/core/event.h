//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// Generic event
#ifndef _REV_CORE_EVENT_H_
#define _REV_CORE_EVENT_H_

#include <functional>
#include <memory>
#include <vector>

namespace rev {
	namespace core {
		template<typename ... Arg_>
		class Event {
		public:
			typedef std::function<void(Arg_...)>	Delegate;

			struct Listener
			{
				Listener(Delegate _d) : m_delegate(_d) {}
				Delegate m_delegate;
			};

			// Handle delegates
			std::shared_ptr<Listener> operator+=(Delegate);
			void operator-=(const std::shared_ptr<Listener>&);
			void clear() { mDelegates.clear(); }

			// Invoke
			void operator()(Arg_...) const;

		private:
			std::vector<std::weak_ptr<Listener>>	mListeners;
		};

		//--------------------------------------------------------------------------------------------------------------
		// Inline
		//--------------------------------------------------------------------------------------------------------------
		template<typename...Arg_>
		auto Event<Arg_...>::operator+=(Delegate _delegate) -> std::shared_ptr<Listener>
		{
			auto listener = std::make_shared<Listener>(_delegate);
			mListeners.push_back(listener);
			return listener;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename...Arg_>
		void Event<Arg_...>::operator-= (const std::shared_ptr<Listener>& listener)
		{
			for (auto i = mListeners.begin(); i != mListeners.end(); ++i) {
				if (*i == listener) {
					i = mListeners.erase(i);
					return;
				}
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename...Arg_>
		void Event<Arg_...>::operator()(Arg_ ... _args) const
		{
			for (auto& listener : mListeners)
			{
				auto p = listener.lock();
				if(p)
					p->m_delegate(_args...);
			}
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_EVENT_H_