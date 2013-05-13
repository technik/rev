//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On October 27th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Basic canvas application

#include "canvasApplication.h"
#include "../canvas/canvas.h"
#include <revCore/codeTools/log/log.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/color/color.h>
#include <revVideo/videoDriver/videoDriver.h>

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

using rev::codeTools::Log;
using namespace rev::video;

namespace rev { namespace canvas
{
	//------------------------------------------------------------------------------------------------------------------
	CanvasApplication::CanvasApplication()
	{
		// Note: The order of initialization is important because video system assumes log is initialized
		// Init log system
		Log::init();
		// Init video systems and drivers
		initializeVideoSystem();
		// Create canvas
		mCanvas = new Canvas();
	}

	//------------------------------------------------------------------------------------------------------------------
	CanvasApplication::~CanvasApplication()
	{
		delete mCanvas;
		VideoDriver::shutDown();
		Log::end();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CanvasApplication::update()
	{
#ifdef _WIN32
		// TODO: Move this to a platform specific handler that can process and dispatch messages
		MSG msg;
		while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
			{
				return false;
			}
			else
			{
                TranslateMessage(&msg);	// Translate The Message
				DispatchMessage(&msg);
			}
		}
#endif // _WIN32
		mDriver3d->clearColorBuffer();
		this->draw();
		mDriver3d->finishFrame();
		return mVideoDriver->update();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CanvasApplication::initializeVideoSystem()
	{
		VideoDriver::startUp();
		mVideoDriver = VideoDriver::get();
		mVideoDriver->createMainWindow();
		mVideoDriver->init3d();
		mDriver3d = mVideoDriver->driver3d();
		mDriver3d->setClearColor(rev::video::Color(1.f));
	}

}	// namespace canvas
}	// namespace rev