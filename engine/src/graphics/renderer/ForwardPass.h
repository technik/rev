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
//#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/driver/shader.h>
#include <graphics/renderer/RenderItem.h>
#include <graphics/renderer/material/material.h>
#include <graphics/scene/Light.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <map>
#include <unordered_map>

namespace rev::gfx {

	class BackEndRenderer;
	class Camera;
	class EnvironmentProbe;
	class GraphicsDriverGL;
	class RenderScene;
	class RenderTarget;
	class ShadowMapPass;

	class ForwardPass
	{
	public:
		ForwardPass(gfx::Device&, const math::Vec2u& viewportSize, gfx::FrameBuffer target);

		/*void render(
			const Camera& eye,
			const EnvironmentProbe* env,
			bool shadows,
			const std::vector<gfx::RenderItem>& renderables,
			const CommandBuffer::UniformBucket& sharedUniforms,
			CommandBuffer& dst);*/

		void onResizeTarget(const math::Vec2u& newSize) {
			m_viewportSize = newSize;
			m_pass->setViewport({0,0}, newSize);
		}

	private:
		void loadCommonShaderCode();

		gfx::Device& m_gfxDevice;
		gfx::RenderPass* m_gfxPass;
		gfx::RenderPass*	m_pass;
		math::Vec2u m_viewportSize;

		float mEV;
		std::unique_ptr<Material>	mErrorMaterial;

		using ShaderOptions = std::pair<uint32_t,std::string>;
		using PipelineSet = std::map<ShaderOptions, gfx::Pipeline>;
		using EffectSelector = std::pair<uint32_t, const Effect*>;
		std::map<EffectSelector, PipelineSet>	mPipelines;

		std::string vertexFormatDefines(RenderGeom::VtxFormat);

		uint32_t effectCode(bool mirror, bool environment, bool shadows)
		{
			return ((mirror?1:0)<<2) | ((environment?1:0)<<1) | (shadows?1:0);
		}

		gfx::Pipeline getPipeline(const Material&, RenderGeom::VtxFormat, const EnvironmentProbe* env, bool shadows, bool mirror);

		std::string mForwardShaderCommonCode;
		gfx::Pipeline::PipielineDesc m_commonPipelineDesc; // Config common to all shadow pipelines

		// Internal rendering structures
		struct MeshInfo
		{
			std::shared_ptr<const RenderGeom> geom;
			std::shared_ptr<const SkinInstance> skin;
			std::shared_ptr<Material> material;
			math::Mat44f world;
			math::Vec2f depth; // min, max
		};

		struct ShaderPragma
		{
			std::string name;
			bool state;
			std::string preprocessorLine() const
			{
				return "#define " + name + "\n";
			}
		};

		std::map<std::string, ShaderPragma> m_pragmas;
	};

}
