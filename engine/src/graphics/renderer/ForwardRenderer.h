//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <memory>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/driver/shader.h>
#include <graphics/driver/texture.h>
#include <graphics/scene/renderScene.h>
#include <graphics/scene/material.h>

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
		std::unique_ptr<Material>		mErrorMaterial;
		std::unique_ptr<Texture>		mErrorTexture;
		std::unique_ptr<Shader>			mShader;
	};

}}
