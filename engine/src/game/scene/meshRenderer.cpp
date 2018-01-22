//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "meshRenderer.h"
#include "sceneNode.h"
#include "transform/transform.h"
#include <graphics/debug/debugGUI.h>

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	MeshRenderer::MeshRenderer(const RenderGeom* _mesh) {
		mRenderMesh = std::make_unique<RenderObj>(_mesh);
		mMaterial.roughness = 0.5f;
		mMaterial.metallic = 0.1f;
	}
	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::init() {
		mMaterial.albedo = math::Vec3f(1.f,1.f,1.f);
		mTransform = &node()->component<Transform>()->xForm;
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::update(float _dt) {
		mRenderMesh->transform = *mTransform;
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::showDebugInfo() {
		gui::colorPicker("Albedo", mMaterial.albedo);
		gui::slider("Roughness", mMaterial.roughness, 0.f, 1.f);
		gui::slider("Metallic", mMaterial.metallic, 0.f, 1.f);
	}


}}	// namespace rev::game