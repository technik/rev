//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include <math/algebra/vector.h>
#include "meshRenderer.h"

namespace rev { namespace game {

	class MeshRenderer;

	class RenderScene
	{
	public:
		RenderScene() { mLightClr = math::Vec3f::ones(); }
		MeshRenderer* createMeshRenderer(const graphics::RenderGeom* _geom);

		const std::vector<MeshRenderer*>&	renderables() const { return mMeshes; }
		const math::Vec3f& lightClr() const { return mLightClr; }

		void showDebugInfo();

	private:
		std::vector<MeshRenderer*>	mMeshes;
		math::Vec3f					mLightClr;
	};

}}	// namespace rev::game