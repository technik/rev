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
	MeshRenderer::MeshRenderer(std::shared_ptr<graphics::RenderObj> renderable)
		: mRenderable(renderable)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::init() {
		mSrcTransform = &node()->component<Transform>()->xForm;
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::update(float _dt) {
		mRenderable->transform = *mSrcTransform;
	}

}}	// namespace rev::game