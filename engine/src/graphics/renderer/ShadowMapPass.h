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
#include <map>

namespace rev{ namespace graphics {

	class BackEndRenderer;
	class Camera;
	class GraphicsDriverGL;
	class RenderScene;
	class RenderTarget;

	class ShadowMapPass
	{
	public:
		ShadowMapPass(BackEndRenderer&, GraphicsDriverGL&, const math::Vec2u& _size);

		void render(const RenderScene& _scene);

		auto texture() const { return mDepthBuffer->texture(); }
		const math::Mat44f& shadowProj() const { return mUnbiasedShadowProj; }

	private:

		void setUpGlobalState();
		void adjustViewMatrix(const math::Vec3f& lightDir);
		void renderMeshes(const RenderScene& _scene);

		void loadCommonShaderCode();
		Shader* getShader(RenderGeom::VtxFormat);
		// Used to combine different vertex formats as long as they share the data needed to render shadows.
		// This saves a lot on shader permutations.
		uint32_t mVtxFormatMask;

	private:
		using ShaderPtr = std::unique_ptr<Shader>;
		using PipelineSet = std::map<uint32_t, ShaderPtr>;

		GraphicsDriverGL&	mDriver;
		std::unique_ptr<FrameBuffer>	mDepthBuffer;
		math::Mat44f					mShadowProj;
		math::Mat44f					mUnbiasedShadowProj;
		BackEndRenderer&				mBackEnd;
		PipelineSet						mPipelines;
		std::string						mCommonShaderCode;
		float mBias = 0.001f;
	};

}}
