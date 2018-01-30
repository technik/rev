//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "component.h"
#include <memory>
#include <graphics/scene/renderObj.h>
#include <graphics/driver/texture.h>
#include <math/algebra/vector.h>

namespace rev { namespace game {

	class Transform;

	class MeshRenderer : public Component {
	public:
		MeshRenderer(std::shared_ptr<graphics::RenderObj> mesh);

		void init		() override;
		void update		(float _dt) override;

		const	graphics::RenderObj& renderObj() const { return *mRenderable; }
				graphics::RenderObj& renderObj() { return *mRenderable; }

	private:
		std::shared_ptr<graphics::RenderObj>	mRenderable;
		Transform*								mSrcTransform;
	};

}}	// namespace rev::game