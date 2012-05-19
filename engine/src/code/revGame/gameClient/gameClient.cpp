////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// basic game client
#ifdef WIN32
#include <Windows.h>
#include <revGame/gameClient/windows/windowsHandler.h>
#include <revVideo/texture/texture.h>
#include <revVideo/videoDriver/shader/vtxShader.h>
#include <revVideo/videoDriver/shader/pxlShader.h>
#endif // WIN32

#include "gameClient.h"

// --- Engine headers -----------
#include <revCore/codeTools/log/log.h>
#include <revCore/codeTools/profiler/profiler.h>
#include <revCore/file/fileWatcher.h>
#include <revCore/node/node.h>
#include <revCore/string.h>
#include <revCore/time/time.h>
#include <revGame/gui/webPanel/html/lexer/htmlLexer.h>
#include <revGame/gui/webPanel/html/parser/htmlParser.h>
#include <revInput/keyboardInput/keyboardInput.h>
#include <revInput/touchInput/touchInput.h>
#include <revVideo/scene/videoScene.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>
#include <revScript/analisys/lexer/lexicalAnalizer.h>
#include <revScript/analisys/parser/syntaxParser.h>

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

	// Static data
	SGameClient * SGameClient::sInstance = 0;
	//------------------------------------------------------------------------------------------------------------------
	SGameClient * SGameClient::get()
	{
		return sInstance;
	}

	//------------------------------------------------------------------------------------------------------------------
	void SGameClient::destroy()
	{
		delete sInstance;
		sInstance = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	SGameClient::SGameClient()
	{
		initEngineSystems();
		initBasic2d();
		initBasic3d();
		// Init html loader
		CHtmlLexer::init();
		CHtmlParser::init();
	}

	//------------------------------------------------------------------------------------------------------------------
	SGameClient::~SGameClient()
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
#ifdef REV_PROFILER
		SProfiler::end();
#endif // REV_PROFILER
		// End log system
		revLogN("----- Base game client destroyed -----");
		SLog::end();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool SGameClient::update()
	{
#ifdef REV_PROFILER
		SProfiler::get()->resetFrame();
#endif // REV_PROFILER
		CProfileFunction profilerIntance("SGameClient::update"); 
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
					SFileWatcher::get()->notifyFileChanged(filename.c_str());
					// CTexture::manager()->reload(filename);
					// CVtxShader::manager()->reload(filename);
					// CPxlShader::manager()->reload(filename);
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

	//------------------------------------------------------------------------------------------------------------------
	void SGameClient::initEngineSystems()
	{
		// Init log system
		SLog::init();
		revLogN("----- Creating base game client -----");
#ifdef REV_PROFILER
		SProfiler::init();
#endif // REV_PROFILER
#ifdef WIN32
		mWindowsHandler = new CWindowsHandler();
		SFileWatcher::init();
#endif // WIN32
		// Init script system
		script::CRevScriptLexer::init();
		script::CRevScriptParser::init();
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
	void SGameClient::initBasic2d()
	{
		CVec2 screenSize = SVideo::getDriver()->screenSize();
		// Create the camera
		m2dCamera = new COrthoCamera(screenSize, -100.f, 100.f);
		CNode * camNode = new CNode();
		m2dCamera->attachTo(camNode);
		// Create a viewport
		CViewport * view = new CViewport(rev::CVec2::zero, screenSize, 1.f);
		view->setCamera(m2dCamera);
		// Create a scene
		m2dScene = new CVideoScene();
		m2dCamera->setScene(m2dScene);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SGameClient::initBasic3d()
	{
		// Create the camera
		m3dCamera = new CPerspectiveCamera(45.f, 1.5f, CVec2(0.f, 10000.f));
		CNode * camNode = new CNode();
		camNode->addComponent(m3dCamera);
		// Create a viewport and attach the camera to it
		CViewport * view = new CViewport(rev::CVec2::zero, SVideo::getDriver()->screenSize(), 0.f);
		view->setCamera(m3dCamera);
		// Create a scene
		m3dScene = new CVideoScene();
		m3dCamera->setScene(m3dScene);
	}

} // namespace game
} // namespace rev

