//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with Windows operating system
#ifndef _REV_CORE_PLATFORM_WINDOWS_OSHANDLERWINDOWS_H_
#define _REV_CORE_PLATFORM_WINDOWS_OSHANDLERWINDOWS_H_

#ifdef _WIN32

#include <Windows.h>

#include <functional>

#include <vector>

namespace rev {
	namespace core {

		class OSHandlerBaseWindows {
		public:
			bool processMessage(MSG);
			/// \return true if the message was processed by the delegate
			typedef std::function<bool(MSG)>	OSDelegate;

			void operator+= (OSDelegate);
		protected:
			bool update();
		private:
			std::vector<OSDelegate>	mMsgProcessors;
			bool				mMustQuit = false;
		};

		typedef OSHandlerBaseWindows OSHandlerBase;

	}	// namespace core
}	// namespace rev

#endif // _WIN32

#endif // _REV_CORE_PLATFORM_WINDOWS_OSHANDLERWINDOWS_H_