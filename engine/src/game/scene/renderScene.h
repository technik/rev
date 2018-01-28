//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include <math/algebra/vector.h>
#include <memory>
#include "meshRenderer.h"

namespace rev { namespace game {

	class MeshRenderer;

	class RenderScene
	{
	public:
		RenderScene() { mLightClr = math::Vec3f::ones(); }
		std::unique_ptr<MeshRenderer> createMeshRenderer(const graphics::RenderGeom* _geom);

		const std::vector<std::weak_ptr<MeshRenderer>>&	renderables() const { return mMeshes; }
		const math::Vec3f& lightClr() const { return mLightClr; }

		void showDebugInfo();

	private:
		std::vector<std::weak_ptr<MeshRenderer>>	mMeshes;
		math::Vec3f					mLightClr;
	};

}}	// namespace rev::game