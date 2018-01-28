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
	MeshRenderer::MeshRenderer(std::shared_ptr<const graphics::RenderGeom> _mesh) 
	{
		mRenderMesh = std::make_unique<RenderObj>(_mesh);
		mMaterial.roughness = 0.5f;
		mMaterial.metallic = 0.1f;
	}
	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::init() {
		mTransform = &node()->component<Transform>()->xForm;
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::update(float _dt) {
		assert(mTransform);
		mRenderMesh->transform = *mTransform;
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::showDebugInfo() {
		gui::slider("Roughness", mMaterial.roughness, 0.f, 1.f);
		gui::slider("Metallic", mMaterial.metallic, 0.f, 1.f);
	}


}}	// namespace rev::game