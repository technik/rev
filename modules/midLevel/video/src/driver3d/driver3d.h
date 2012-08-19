//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 3d driver interface

#ifndef _REV_VIDEO_3DDRIVER_3DDRIVER_H_
#define _REV_VIDEO_3DDRIVER_3DDRIVER_H_

#include <math/algebra/vector.h>

namespace rev { namespace video
{
	// Forward declarations
	class Color;
	class Shader;

	class Driver3d
	{
	public:
		virtual ~Driver3d() {} // Virtual destructor

		// Note: position correspond to the lower left corner of the rectangle and the window, starting at (0,0)
		virtual void	setViewport			(const math::Vec2i& position, const math::Vec2u& size) = 0;
		virtual void	clearZBuffer		() = 0;
		virtual void	clearColorBuffer	() = 0;
		virtual void	setClearColor		(const Color&) = 0;

		virtual void	flush				() = 0;
		virtual void	finishFrame			() = 0;

		virtual void	setShader			(const Shader * _shader) = 0;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_3DDRIVER_3DDRIVER_H_