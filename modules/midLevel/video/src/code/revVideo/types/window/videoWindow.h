//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Video window interface

#ifndef _REV_VIDEO_TYPES_WINDOW_VIDEOWINDOW_H_
#define _REV_VIDEO_TYPES_WINDOW_VIDEOWINDOW_H_

#include <math/algebra/vector.h>

namespace rev { namespace video
{
	class Window
	{
	public:
		virtual ~Window() {}

		// Accessors
		virtual math::Vec2i	position		() const;
		virtual math::Vec2u	resolution		() const;
		virtual void		setPosition		(const math::Vec2i& position) = 0;
		virtual void		setResolution	(const math::Vec2u& resolution) = 0;

	protected:
		math::Vec2i	mPosition;
		math::Vec2u	mResolution;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	inline math::Vec2i Window::position() const		{ return mPosition; }
	inline math::Vec2u Window::resolution() const	{ return mResolution; }

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_WINDOW_VIDEOWINDOW_H_
