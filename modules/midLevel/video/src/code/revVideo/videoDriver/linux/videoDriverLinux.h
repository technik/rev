//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On October 24th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Linux video driver

#ifndef _REV_VIDEO_VIDEODRIVER_LINUX_VIDEODRIVERLINUX_H_
#define _REV_VIDEO_VIDEODRIVER_LINUX_VIDEODRIVERLINUX_H_

#ifdef __linux__

#include <revVideo/videoDriver/videoDriver.h>

namespace rev { namespace video
{
	class VideoDriverLinux : public VideoDriver
	{
	public:
		VideoDriverLinux();

		math::Vec2u		screenResolution	() const;
		bool			update				();

	private:
		// Polymorphic implementation
		Driver3d*		createDriver3d	();
		Window*			createWindow		(const math::Vec2i& position, const math::Vec2u& resolution);
	};

}	// namespace video
}	// namespace rev

#endif // __linux__

#endif // _REV_VIDEO_VIDEODRIVER_LINUX_VIDEODRIVERLINUX_H_
