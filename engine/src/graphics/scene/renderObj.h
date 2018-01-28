//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Render scene primitive containing geometry and transform
#pragma once

#include <math/algebra/affineTransform.h>
#include <memory>

namespace rev { namespace graphics {

	class RenderGeom;

	struct RenderObj
	{
		RenderObj()
			: transform(math::AffineTransform::identity())
		{}

		RenderObj(std::shared_ptr<const RenderGeom> _mesh)
			: transform(math::AffineTransform::identity())
			, mesh(_mesh)
		{}

		math::AffineTransform	transform;
		std::shared_ptr<const RenderGeom> mesh;
	};

}}	// namespace rev::graphics
