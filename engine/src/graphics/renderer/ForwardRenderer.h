//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <memory>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/driver/shader.h>
#include <graphics/driver/texture.h>
#include <graphics/scene/renderScene.h>

namespace rev { namespace graphics {

	class Camera;

	class ForwardRenderer
	{
	public:
		void init	(GraphicsDriverGL& driver);
		void render	(const Camera& _pov, const RenderScene&);

	private:
		GraphicsDriverGL*	mDriver = nullptr;
		float mEV;
		std::unique_ptr<graphics::Texture>		mErrorTexture;
		std::unique_ptr<graphics::Shader>		mShader;
	};

}}
