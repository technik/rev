//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "meshRenderer.h"
#include "sceneNode.h"
#include "transform/transform.h"
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	MeshRenderer::MeshRenderer(const RenderGeom* _mesh) {
		mRenderMesh = std::make_unique<RenderObj>(_mesh);
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
		ImGui::ColorEdit3("Albedo", mMaterial.albedo.data());
	}


}}	// namespace rev::game