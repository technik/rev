////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// basic game client
#ifdef WIN32
#include <Windows.h>
#include <revCore/resourceManager/resourceManager.h>
#include <revGame/gameClient/windows/windowsHandler.h>
#include <revVideo/texture/texture.h>
#include <revVideo/videoDriver/shader/vtxShader.h>
#include <revVideo/videoDriver/shader/pxlShader.h>
#endif // WIN32

#include "gameClient.h"

// --- Engine headers -----------
#include <revCore/codeTools/log/log.h>
#include <revCore/string.h>
#include <revCore/time/time.h>
#include <revInput/keyboardInput/keyboardInput.h>
#include <revInput/touchInput/touchInput.h>
#include <revVideo/video.h>

// --- Active namespaces --------
using namespace rev;
using namespace rev::codeTools;
using namespace rev::input;
using namespace rev::video;

namespace rev { namespace game
{
#ifdef WIN32
		HANDLE hDirectory;
		u32 longBuffer[1024];
		DWORD recievedBytes = 0;
		OVERLAPPED overlapped;
		HANDLE hPort;
		ULONG_PTR key = 1;
#endif // WIN32
	//------------------------------------------------------------------------------------------------------------------
	CGameClient::CGameClient()
	{
		// Init log system
		SLog::init();
		revLogN("----- Creating base game client -----");
#ifdef WIN32
		mWindowsHandler = new CWindowsHandler();
#endif // WIN32
		// Init time system
		STime::init();
		// Init video system
		SVideo::init();
		// Init audio system
		// Init input system
		STouchInput::init();
		SKeyboardInput::init();
	}

	//------------------------------------------------------------------------------------------------------------------
	CGameClient::~CGameClient()
	{
		// End input system
		// End audio system
		// End video system
		SVideo::end();
		// End time system
		STime::end();
#ifdef WIN32
		delete mWindowsHandler;
#endif // WIN32
		// End log system
		revLogN("----- Base game client destroyed -----");
		SLog::end();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CGameClient::update()
	{
		// Update time system
		STime::get()->update();
		// Update video system and render
		SVideo::get()->update();
		// Update input
		SKeyboardInput::get()->refresh();	// Input must be refreshed before we handle os messages. Otherways key presses
#ifdef WIN32								// be missinterpreted as key holdings
		mWindowsHandler->update();
#endif // WIN32
		// Run time reloading of modified files
#ifdef WIN32
		static bool registeredForModifications = false;
		if(!registeredForModifications)
		{
			registeredForModifications = true;
			// Obtain a handle to the working directory
			hDirectory = CreateFile(".\\", 
				FILE_LIST_DIRECTORY,	// Request permission to list directory contents
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED ,
				NULL);
			if(0 == ReadDirectoryChangesW(
				hDirectory,
				longBuffer,
				1024 * sizeof(u32),
				true,					// Watch subtree
				FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE,
				&recievedBytes,
				&overlapped,
				NULL))
			{
				DWORD error = GetLastError();
				printf("%d", error);
			}
			hPort = CreateIoCompletionPort(
				hDirectory,
				NULL,
				key,
				0
			);
		}
		else
		{
			// Check
			LPOVERLAPPED over;
			if(0 != GetQueuedCompletionStatus(hPort, 
                &recievedBytes, &key, 
                &over, 0))
			{
				if(0 != recievedBytes)
				{
					// Copy file name
					FILE_NOTIFY_INFORMATION * notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(longBuffer);
					string filename;
					//filename.resize(notifyInfo->FileNameLength+1);
					DWORD i = 0;
					DWORD length = notifyInfo->FileNameLength / sizeof(wchar_t);
					for(; i < length; ++i)
					{
						char c = (char)notifyInfo->FileName[i];
						if(c == '\0')
							break;
						filename += c;
					}
					CTexture::manager()->reload(filename);
					CVtxShader::manager()->reload(filename);
					CPxlShader::manager()->reload(filename);
				}

				ReadDirectoryChangesW(
					hDirectory,
					longBuffer,
					1024 * sizeof(u32),
					true,					// Watch subtree
					FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
					&recievedBytes,
					&overlapped,
					NULL);
			}
		}

#endif // WIN32
		return true;
	}

} // namespace game
} // namespace rev

