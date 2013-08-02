//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Windows handler for Operating System call

#ifdef _WIN32

#include "osHandlerWindows.h"
#include <revPlatform/fileSystem/windows/fileSystemWindows.h>

namespace rev { namespace platform {

	//-----------------------------------------------------------------------------------------------------------------
	OSHandlerWindows* OSHandlerWindows::sHandler = nullptr;

	//-----------------------------------------------------------------------------------------------------------------
	OSHandler* OSHandler::get() {
		return OSHandlerWindows::get();
	}

	//-----------------------------------------------------------------------------------------------------------------
	OSHandlerWindows* OSHandlerWindows::get() {
		if(nullptr == sHandler)
			sHandler = new OSHandlerWindows;
		return sHandler;
	}

	//-----------------------------------------------------------------------------------------------------------------
	OSHandlerWindows::OSHandlerWindows()
	{
	}

	//-----------------------------------------------------------------------------------------------------------------
	void OSHandlerWindows::operator+=(std::function<bool (MSG)> _msgProcessor)
	{
		mMsgProcessors.push_back(_msgProcessor);
	}

	//-----------------------------------------------------------------------------------------------------------------
	bool OSHandlerWindows::update()
	{
		MSG msg;
		while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT) // If exit requested, don't bother processing anything else
				return false;
			else {
				bool processed = false; // Try to process the message internally
				for(auto proc : mMsgProcessors)
					if(proc(msg)) {
						processed = true;
						break;
					}
				if(!processed) { // Let windows process the message
					TranslateMessage(&msg);	// Translate The Message
					DispatchMessage(&msg);
				}
			}
		}
		// Update filesystem
		FileSystemWindows::get()->update();
		return true;
	}

}	// namespace platform
}	// namespace rev

#endif // _WIN32
