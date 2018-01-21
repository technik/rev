//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "component.h"
#include <memory>
#include <graphics/scene/renderObj.h>
#include <math/algebra/vector.h>

namespace rev { namespace game {

	class Transform;

	class MeshRenderer : public Component{
	public:
		MeshRenderer(const graphics::RenderGeom* _mesh);

		void init		() override;
		void update		(float _dt) override;

		const graphics::RenderObj& renderObj() const { return *mRenderMesh; }

		struct Material {
			math::Vec3f albedo;
			float roughness;
			float metallic;
		};

		const Material& material() const { return mMaterial; }

		void showDebugInfo() override;
	private:
		std::unique_ptr<graphics::RenderObj>	mRenderMesh;
		math::AffineTransform*	mTransform = nullptr;

		// Material info
		Material mMaterial;
	};

}}	// namespace rev::game