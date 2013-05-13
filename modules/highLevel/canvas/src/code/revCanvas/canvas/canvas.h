//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On January 28th, 2013
//----------------------------------------------------------------------------------------------------------------------
// Simple canvas

#ifndef _REV_CANVAS_CANVAS_CANVAS_H_
#define _REV_CANVAS_CANVAS_CANVAS_H_

#include <cstdint>
#include <revCore/math/algebra/vector.h>

namespace rev { namespace video {
	class Driver3d;
	class Shader;
	class Color;
}}

namespace rev { namespace canvas {

	class Canvas
	{
	public:
		Canvas();

		void rect(const math::Vec2f& _topLeft, const math::Vec2f& _size);
		void line(const math::Vec2f& _start, const math::Vec2f& _end);

		void setDrawColor(const video::Color& _color);

	private:
		video::Driver3d*	mDriver3d;
		video::Shader*		mShader;

		int					mColorUniformId;
		uint16_t			mRectIndices[6];
	};

}	// namespace canvas
}	// namespace rev

#endif // _REV_CANVAS_CANVAS_CANVAS_H_