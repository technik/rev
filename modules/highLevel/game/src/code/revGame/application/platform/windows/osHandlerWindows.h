//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Windows handler for Operating System call

#ifndef _REV_GAME_APPLICATION_PLATFORM_WINDOWS_OSHANDLER_WINDOWS_H_
#define _REV_GAME_APPLICATION_PLATFORM_WINDOWS_OSHANDLER_WINDOWS_H_

#ifdef _WIN32

#include "../osHandler.h"

#include <functional>
#include <vector>
#include <Windows.h>

namespace rev { namespace game {

	class OSHandlerWindows : public OSHandler
	{
	public:
		static void createHandler();
		void operator+= (std::function<bool (MSG)> _msgProcessor);

		bool update();
	private:
		OSHandlerWindows();
		std::vector<std::function<bool (MSG)> >	mMsgProcessors;
	};

}	// namespace game
}	// namespace rev

#endif // _WIN32

#endif // _REV_GAME_APPLICATION_PLATFORM_WINDOWS_OSHANDLER_WINDOWS_H_