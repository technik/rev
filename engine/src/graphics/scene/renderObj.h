//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Render scene primitive containing geometry and transform
#pragma once

#include <math/algebra/affineTransform.h>
#include <memory>
#include <vector>

namespace rev { namespace graphics {

	class RenderGeom;

	struct RenderObj
	{
		RenderObj()
			: transform(math::AffineTransform::identity())
		{}

		RenderObj(std::shared_ptr<const RenderGeom> _mesh)
			: RenderObj()
		{
			meshes.push_back(_mesh);
		}

		math::AffineTransform	transform;
		std::vector<std::shared_ptr<const RenderGeom>> meshes;
	};

}}	// namespace rev::graphics
