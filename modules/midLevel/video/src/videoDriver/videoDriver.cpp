//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Video driver interface

#include "videoDriver.h"
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

}	// namespace video
}	// namespace rev