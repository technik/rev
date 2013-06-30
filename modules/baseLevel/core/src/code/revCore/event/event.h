//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Creted by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// on 2013/May/13
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_CORE_EVENT_EVENT_H_
#define _REV_CORE_EVENT_EVENT_H_

#include <functional>
#include <vector>

namespace rev
{
	template<typename... Arg_>
	class Event
	{
	public:
		void	operator()	(Arg_...) const;					///\ Invoke events
		void	operator+=	(std::function<void (Arg_...)>*);	///\ Add a deletage to the event
		void	operator-=	(std::function<void (Arg_...)>*);	///\ Remove a delegate from the event

	private:
		std::vector<std::function<void (Arg_...)>*> mDelegates;
	};

	//----------------------------------------------------------
	// Inline implementation
	//----------------------------------------------------------
	template<typename... Arg_>
	void Event<Arg_...>::operator() 
		(Arg_... _argument) const
	{
		for(auto del : mDelegates)
			(*del)(_argument...);
	}

}	// namespace rev

#endif // _REV_CORE_EVENT_EVENT_H_
