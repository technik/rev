//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Windows video driver

#ifndef _REV_VIDEO_VIDEODRIVER_WINDOWS_VIDEODRIVERWINDOWS_H_
#define _REV_VIDEO_VIDEODRIVER_WINDOWS_VIDEODRIVERWINDOWS_H_

#ifdef WIN32

#include <videoDriver/videoDriver.h>

namespace rev { namespace video
{
	class VideoDriverWindows : public VideoDriver
	{
	public:
		Window* createWindow	();
		void	destroyWindow	(Window*);
	};
}	// namespace video
}	// namespace rev

#endif // WIN32

#endif // _REV_VIDEO_VIDEODRIVER_WINDOWS_VIDEODRIVERWINDOWS_H_
