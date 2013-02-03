//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On January 28th, 2013
//----------------------------------------------------------------------------------------------------------------------
// Simple canvas

#ifndef _REV_CANVAS_CANVAS_CANVAS_H_
#define _REV_CANVAS_CANVAS_CANVAS_H_

#include <revCore/math/algebra/vector.h>

namespace rev { namespace video {
	class Driver3d;
}}

namespace rev { namespace canvas {

	class Canvas
	{
	public:
		Canvas();

		void rect(const math::Vec2f& _topLeft, const math::Vec2f& _size);
		void line(const math::Vec2f& _start, const math::Vec2f& _end);

	private:
		video::Driver3d* mDriver3d;
	};

}	// namespace canvas
}	// namespace rev

#endif // _REV_CANVAS_CANVAS_CANVAS_H_