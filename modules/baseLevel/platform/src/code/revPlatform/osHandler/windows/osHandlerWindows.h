//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Windows handler for Operating System call

#ifndef _REV_PLATFORM_OSHANDLER_WINDOWS_OSHANDLERWINDOWS_H_
#define _REV_PLATFORM_OSHANDLER_WINDOWS_OSHANDLERWINDOWS_H_

#ifdef _WIN32

#include "../osHandler.h"

#include <functional>
#include <vector>
#include <Windows.h>

namespace rev { namespace platform {

	class OSHandlerWindows : public OSHandler
	{
	public:
		static OSHandlerWindows* get();
		void operator+= (std::function<bool (MSG)> _msgProcessor);

		bool update();
	private:
		static OSHandlerWindows* sHandler;
		OSHandlerWindows();
		std::vector<std::function<bool (MSG)> >	mMsgProcessors;
	};

}	// namespace platform
}	// namespace rev

#endif // _WIN32

#endif // _REV_PLATFORM_OSHANDLER_WINDOWS_OSHANDLERWINDOWS_H_