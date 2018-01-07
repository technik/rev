//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "component.h"
#include <memory>
#include <graphics/scene/renderObj.h>

namespace rev { namespace game {

	class Transform;

	class MeshRenderer : public Component{
	public:
		void update(float _dt) override;

	private:
		std::unique_ptr<graphics::RenderObj>	mRenderMesh;
	};

}}	// namespace rev::game