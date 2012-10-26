//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Video driver interface

#ifndef _REV_VIDEO_VIDEODRIVER_VIDEODRIVER_H_
#define _REV_VIDEO_VIDEODRIVER_VIDEODRIVER_H_

#include <revCore/codeTools/usefulMacros.h>
#include <revCore/math/algebra/vector.h>

namespace rev { namespace video
{
	// Forward declarations
	class Driver3d;
	class Window;

	// Video driver interface
	class VideoDriver
	{
	public:
		// Singleton interface
		static void				startUp			();
		static void				shutDown		();
		static VideoDriver*		get				();
		static Driver3d*		getDriver3d		();

		// --- Actual video driver interface ---
		// Window creation and destruction
		virtual	bool			update			(); // Return false means application must exit
				void			createMainWindow();
				void			init3d			();
				Driver3d*		driver3d		();
				Window*			mainWindow		();
		// Screen resolution
		virtual math::Vec2u		screenResolution() const = 0;

	protected:
		VideoDriver();
		virtual ~VideoDriver();

	private:
		virtual Driver3d*	createDriver3d	() = 0;
		virtual Window*		createWindow	(const math::Vec2i& position, const math::Vec2u& resolution) = 0;

	private:
		REV_DECLARE_COPY(VideoDriver);
		static VideoDriver * sInstance;

	protected:
		Driver3d*	mDriver3d;
		Window *	mMainWindow;
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

	//------------------------------------------------------------------------------------------------------------------
	inline Driver3d * VideoDriver::getDriver3d()
	{
		return get()->driver3d();
	}

	//------------------------------------------------------------------------------------------------------------------
	inline Driver3d * VideoDriver::driver3d()
	{
		return mDriver3d;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline Window * VideoDriver::mainWindow()
	{
		return mMainWindow;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_VIDEODRIVER_VIDEODRIVER_H_
