//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include "meshRenderer.h"

namespace rev { namespace game {

	class MeshRenderer;

	class RenderScene
	{
	public:
		MeshRenderer* createMeshRenderer(const graphics::RenderGeom* _geom);

		const std::vector<MeshRenderer*>&	renderables() const { return mMeshes; }

	private:
		std::vector<MeshRenderer*>	mMeshes;
	};

}}	// namespace rev::game