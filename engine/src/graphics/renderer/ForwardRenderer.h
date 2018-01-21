//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <memory>
#include <graphics/driver/shader.h>
#include <game/scene/renderScene.h>

namespace rev { namespace graphics {

	class Camera;

	class ForwardRenderer
	{
	public:
		void init	();
		void render	(const Camera& _pov, const game::RenderScene&);

	private:
		float mEV;
		std::unique_ptr<graphics::Shader>		mShader;
	};

}}
