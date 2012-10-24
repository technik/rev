//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Video driver interface

#include "videoDriver.h"
#include <revCore/codeTools/assert/assert.h>
#ifdef WIN32
#include "windows/videoDriverWindows.h"
#endif // WIN32

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
#else // Unknown platform
		sInstance = 0;
#endif
	}

	//------------------------------------------------------------------------------------------------------------------
	void VideoDriver::init3d()
	{
		// Create a windows
		if(nullptr == mMainWindow)
			mMainWindow = createWindow(math::Vec2i(50, 50), math::Vec2u(640, 480));
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