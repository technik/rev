//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Windows' video window

#ifndef _REV_VIDEO_TYPES_WINDOW_WINDOWS_VIDEOWINDOWWINDOWS_H_
#define _REV_VIDEO_TYPES_WINDOW_WINDOWS_VIDEOWINDOWWINDOWS_H_

#ifdef WIN32

#include "../videoWindow.h"

namespace rev { namespace video
{
	class WindowWindows : public Window
	{
	public:
		WindowWindows	(const math::Vec2i& position, const math::Vec2u& resolution);

		virtual void		setPosition		(const math::Vec2i& position) {}
		virtual void		setResolution	(const math::Vec2u& resolution) {}
	};

}	// namespace video
}	// namespace rev

#endif // WIN32

#endif // _REV_VIDEO_TYPES_WINDOW_WINDOWS_VIDEOWINDOWWINDOWS_H_
