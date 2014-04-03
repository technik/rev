//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with Windows operating system

#ifdef _WIN32

#include "osHandlerWindows.h"

#include <Windows.h>

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		// Static data definition
		OSHandler* OSHandler::sInstance = nullptr;

		//--------------------------------------------------------------------------------------------------------------
		bool OSHandlerWindows::update() {
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) // If exit requested, don't bother processing anything else
					return false;
				else {
					bool processed = false; // Try to process the message internally
					for (auto proc : mMsgProcessors)
					if (proc(msg)) {
						processed = true;
						break;
					}
					if (!processed) { // Let windows process the message
						TranslateMessage(&msg);	// Translate The Message
						DispatchMessage(&msg);
					}
				}
			}
			return true;
		}

	}	// namespace core
}	// namespace rev

#endif // _WIN32