//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Video driver interface

#ifndef _REV_VIDEO_VIDEODRIVER_VIDEODRIVER_H_
#define _REV_VIDEO_VIDEODRIVER_VIDEODRIVER_H_

#include <codeTools/usefulMacros.h>

namespace rev { namespace video
{
	// Forward declarations
	class Window;

	// Video driver interface
	class VideoDriver
	{
	public:
		// Singleton interface
		static void			startUp();
		static void			shutDown();
		static VideoDriver*	get();

		// Actual video driver interface
		virtual Window*	createWindow() = 0;
		virtual void	destroyWindow(Window*) = 0;

	protected:
		VideoDriver() {}
		virtual ~VideoDriver() {}

	private:
		DECLARE_COPY(VideoDriver);

		static VideoDriver * sInstance;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	inline void VideoDriver::shutDown()
	{
		delete sInstance;
		sInstance = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline VideoDriver * VideoDriver::get()
	{
		return sInstance;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_VIDEODRIVER_VIDEODRIVER_H_
