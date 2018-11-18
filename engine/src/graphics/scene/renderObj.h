//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Render scene primitive containing geometry and transform
#pragma once

#include <math/algebra/affineTransform.h>
#include <graphics/scene/animation/skinning.h>
#include <memory>
#include <vector>

namespace rev::gfx {

	class RenderMesh;

	class RenderObj
	{
	public:
		RenderObj()
			: transform(math::AffineTransform::identity())
		{}

		RenderObj(const std::shared_ptr<const RenderMesh>& _mesh)
			: RenderObj()
		{
			mesh = _mesh;
		}

		bool visible = true;
		math::AffineTransform	transform;
		std::shared_ptr<const RenderMesh> mesh;
		std::shared_ptr<const SkinInstance> skin;
	};

}	// namespace rev::gfx
