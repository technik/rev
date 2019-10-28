//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "meshRenderer.h"
#include "sceneNode.h"
#include "transform/transform.h"
#include <graphics/scene/renderObj.h>

using namespace rev::gfx;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	MeshRenderer::MeshRenderer(
		const std::shared_ptr<gfx::RenderObj>& renderObj
	)
		: mRenderable(renderObj)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::init() {
		mSrcTransform = node()->component<Transform>();
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::update(float _dt) {
		mRenderable->transform = mSrcTransform->absoluteXForm().matrix();
	}

}}	// namespace rev::game