//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "component.h"
#include <memory>
#include <graphics/scene/renderObj.h>
#include <graphics/driver/texture.h>
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
			std::shared_ptr<graphics::Texture> albedo;
			float roughness;
			float metallic;
		};

		const Material& material() const { return mMaterial; }
		Material& material() { return mMaterial; }

		void showDebugInfo() override;
		void serialize(std::ostream& out) const {
			out << "MeshRenderer";
			// TODO: Save actual indices
			int32_t meshIdx = -1;
			int32_t materialIdx = -1;
			out.write((const char*)meshIdx, sizeof(meshIdx));
			out.write((const char*)materialIdx, sizeof(materialIdx));
		}
	private:
		std::unique_ptr<graphics::RenderObj>	mRenderMesh;
		math::AffineTransform*	mTransform = nullptr;

		// Material info
		Material mMaterial;
	};

}}	// namespace rev::game