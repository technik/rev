//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "meshRenderer.h"
#include "sceneNode.h"
#include "transform/transform.h"

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	MeshRenderer::MeshRenderer(RenderGeom* _mesh) {
		mRenderMesh = std::make_unique<RenderObj>(_mesh);
	}
	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::init() {
		mTransform = &node()->component<Transform>()->xForm;
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::update(float _dt) {
		mRenderMesh->transform = *mTransform;
	}


}}	// namespace rev::game