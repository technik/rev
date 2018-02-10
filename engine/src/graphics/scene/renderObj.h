//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Render scene primitive containing geometry and transform
#pragma once

#include <math/algebra/affineTransform.h>
#include <memory>
#include <vector>

namespace rev { namespace graphics {

	class Material;
	class RenderGeom;

	class RenderObj
	{
	public:
		RenderObj()
			: transform(math::AffineTransform::identity())
		{}

		// Sugar constructor for single mesh objects
		RenderObj(std::shared_ptr<const RenderGeom> _mesh, std::shared_ptr<Material> _material)
			: RenderObj()
		{
			meshes.push_back(_mesh);
			materials.push_back(_material);
		}

		math::AffineTransform	transform;
		std::vector<std::shared_ptr<const RenderGeom>>	meshes;
		std::vector<std::shared_ptr<Material>>	materials;
	};

}}	// namespace rev::graphics
