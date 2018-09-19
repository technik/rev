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
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/driver/shader.h>
#include <graphics/renderer/RenderItem.h>
#include <graphics/renderer/material/material.h>
#include <graphics/scene/Light.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <map>
#include <unordered_map>

namespace rev{ namespace graphics {

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

		void render(const std::vector<gfx::RenderItem>& renderables, gfx::Texture2d _shadows);
		void showDebugInfo(bool show) { m_showDbgInfo = show; }

	private:
		void loadCommonShaderCode();

		gfx::Device& m_gfxDevice;
		gfx::RenderPass& m_gfxPass;
		gfx::CommandBuffer m_drawCommands;

		float mEV;
		std::unique_ptr<Material>	mErrorMaterial;
		using ShaderPtr = std::unique_ptr<Shader>;

		using ShaderOptions = std::pair<uint32_t,std::string>;
		using PipelineSet = std::map<ShaderOptions, ShaderPtr>;
		using EffectSelector = std::pair<uint32_t, const Effect*>;
		std::map<EffectSelector, PipelineSet>	mPipelines;

		std::string vertexFormatDefines(RenderGeom::VtxFormat);

		uint32_t effectCode(bool mirror, bool environment, bool shadows) { return ((mirror?1:0)<<2) | (environment?1:0)<<1) | (shadows?1:0); }

		Shader* getShader(Material&, RenderGeom::VtxFormat, const EnvironmentProbe* env, bool shadows);

		std::string mForwardShaderCommonCode;
		
		// Sky
		ShaderPtr mBackgroundShader;
		std::unique_ptr<RenderGeom> mSkyPlane;

		// Internal rendering structures
		struct MeshInfo
		{
			std::shared_ptr<const RenderGeom> geom;
			std::shared_ptr<const SkinInstance> skin;
			std::shared_ptr<Material> material;
			math::Mat44f world;
			math::Vec2f depth; // min, max
		};

		void renderMesh(
			const RenderGeom* _mesh,
			const SkinInstance* _skin,
			const math::Mat44f& _wvp,
			const math::Mat44f _worldMatrix,
			const math::Vec3f _wsEye,
			const std::shared_ptr<Material>& _material,
			const EnvironmentProbe* env,
			const std::vector<std::shared_ptr<Light>>& lights,
			ShadowMapPass* _shadows);

		bool m_showDbgInfo = false;
		void drawStats();
		void resetStats();
		unsigned m_numRenderables;
		unsigned m_numMeshes;
		int m_drawLimit;

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

}}
