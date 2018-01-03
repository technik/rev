//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Render scene primitive containing geometry and transform
#pragma once

#include <math/algebra/affineTransform.h>

namespace rev { namespace graphics {

	class RenderGeom;

	struct RenderObj {
		RenderGeom* model = nullptr;
		math::AffineTransform	transform;
	};

}}	// namespace rev::graphics
