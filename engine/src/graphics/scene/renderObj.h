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
		RenderObj() = default;

		RenderObj(const std::shared_ptr<const RenderMesh>& _mesh)
			: mesh(_mesh)
		{
		}

		bool visible = true;
		math::Mat44f transform;
		std::shared_ptr<const RenderMesh> mesh;
		std::shared_ptr<const SkinInstance> skin;
	};

}	// namespace rev::gfx
