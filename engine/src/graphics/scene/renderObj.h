//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Render scene primitive containing geometry and transform
#pragma once

#include <math/algebra/affineTransform.h>

namespace rev { namespace graphics {

	class RenderGeom;

	struct RenderObj {

		RenderObj()
			: transform(math::AffineTransform::identity())
		{}

		RenderObj(const RenderGeom* _mesh)
			: transform(math::AffineTransform::identity())
			, mesh(_mesh)
		{}

		const RenderGeom* mesh = nullptr;
		math::AffineTransform	transform;
	};

}}	// namespace rev::graphics
