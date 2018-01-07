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
		MeshRenderer(graphics::RenderGeom* _mesh);

		void init		() override;
		void update		(float _dt) override;

		graphics::RenderObj& renderObj() const { return *mRenderMesh; }

	private:
		std::unique_ptr<graphics::RenderObj>	mRenderMesh;
		math::AffineTransform*	mTransform = nullptr;
	};

}}	// namespace rev::game