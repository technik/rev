//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once
#include <memory>
#include <graphics/driver/frameBuffer.h>
#include <graphics/driver/shader.h>
#include <graphics/scene/renderGeom.h>

namespace rev{ namespace graphics {

	class Camera;
	class GraphicsDriverGL;
	class RenderScene;
	class RenderTarget;

	class ShadowMapPass
	{
	public:
		ShadowMapPass(GraphicsDriverGL&, const math::Vec2u& _size);

		void render(const RenderScene& _scene, const math::Vec3f& lightDir);

		GLuint texName() const { return mDepthBuffer->textureGLName(); }
		const math::Mat44f& shadowProj() const { return mShadowProj; }
	private:
		GraphicsDriverGL&	mDriver;
		std::unique_ptr<FrameBuffer>	mDepthBuffer;
		std::unique_ptr<Shader>			mShader;
		math::Mat44f					mShadowProj;
		float mBias = 0.02f;
	};

}}
