//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Windows handler for Operating System call

#ifdef _WIN32

#include "osHandlerWindows.h"
#include <revGame/application/fileSystem/windows/fileSystemWindows.h>
#include <revInput/keyboard/windows/keyboardInputWindows.h>

using namespace rev::input;

namespace rev { namespace game {

	//-----------------------------------------------------------------------------------------------------------------
	OSHandler* OSHandler::sHandler = nullptr;

	//-----------------------------------------------------------------------------------------------------------------
	void OSHandler::createHandler() {
		OSHandlerWindows::createHandler();
	}

	//-----------------------------------------------------------------------------------------------------------------
	void OSHandlerWindows::createHandler() {
		sHandler = new OSHandlerWindows;
	}

	//-----------------------------------------------------------------------------------------------------------------
	OSHandlerWindows::OSHandlerWindows()
	{
		KeyboardInputWindows * kb = static_cast<KeyboardInputWindows*>(KeyboardInput::get());
		*this += ([=](MSG _msg){
			return kb->processWindowsMessage(_msg);
		});
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

}	// namespace game
}	// namespace rev

#endif // _WIN32
