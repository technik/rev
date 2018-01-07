//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "meshRenderer.h"
#include "sceneNode.h"
#include "transform/transform.h"

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::init() {
		mTransform = node()->component<Transform>();
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::update(float _dt) {
		mRenderMesh->transform = mTransform->xForm;
	}


}}	// namespace rev::game