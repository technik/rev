//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "meshRenderer.h"
#include "sceneNode.h"
#include "transform/transform.h"
#include <graphics/scene/renderObj.h>

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	MeshRenderer::MeshRenderer(
		std::shared_ptr<graphics::RenderObj> renderable
	)
		: mRenderable(renderable)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::init() {
		mSrcTransform = node()->component<Transform>();
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::update(float _dt) {
		mRenderable->transform = mSrcTransform->absoluteXForm();
	}

}}	// namespace rev::game