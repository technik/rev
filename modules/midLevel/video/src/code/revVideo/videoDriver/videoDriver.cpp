//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Video driver interface

#include "videoDriver.h"
#include <revCore/codeTools/assert/assert.h>
#include <revVideo/types/window/videoWindow.h>
#include <revVideo/driver3d/driver3d.h>
#ifdef WIN32
#include "windows/videoDriverWindows.h"
#endif // WIN32
#ifdef __linux__
#include "linux/videoDriverLinux.h"
#endif // __linux__

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data definition
	//------------------------------------------------------------------------------------------------------------------
	VideoDriver * VideoDriver::sInstance = nullptr;

	//------------------------------------------------------------------------------------------------------------------
	void VideoDriver::startUp()
	{
		// Create a different video driver depending on current platform
#ifdef WIN32
		sInstance = new VideoDriverWindows();
#elif defined(__linux__)
		sInstance = new VideoDriverLinux();
#else // Unknown platform
		sInstance = 0;
		revAssert(false, "This platform lacks an implementation for VideoDriver");
#endif
	}

	//------------------------------------------------------------------------------------------------------------------
	void VideoDriver::createMainWindow()
	{
		// Create a window
		if(nullptr == mMainWindow)
			mMainWindow = createWindow(math::Vec2i(50, 50), math::Vec2u(640, 480));
	}

	//------------------------------------------------------------------------------------------------------------------
	void VideoDriver::init3d()
	{
		// Create a 3d driver
		if(nullptr == mDriver3d)
			mDriver3d = createDriver3d();
	}

	//------------------------------------------------------------------------------------------------------------------
	VideoDriver::VideoDriver()
		:mDriver3d(nullptr)
		,mMainWindow(nullptr)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	VideoDriver::~VideoDriver()
	{
		if(nullptr != mMainWindow)
			delete mMainWindow;
		if(nullptr != mDriver3d)
			delete mDriver3d;
	}

}	// namespace video
}	// namespace rev