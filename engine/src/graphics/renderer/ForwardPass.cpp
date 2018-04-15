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
#include "ForwardPass.h"
#include "ShadowMapPass.h"

#include <core/platform/fileSystem/file.h>
#include <core/time/time.h>
#include <graphics/debug/imgui.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/driver/shader.h>
#include <graphics/renderer/material/material.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderScene.h>
#include <math/algebra/affineTransform.h>

#ifdef _WIN32
#include <input/keyboard/keyboardInput.h>
using namespace rev::input;
#endif // _WIN32

using namespace std;
using namespace rev::math;

namespace rev { namespace graphics {

	//----------------------------------------------------------------------------------------------
	ForwardPass::ForwardPass(GraphicsDriverGL& _gfxDriver)
		: mDriver(_gfxDriver)
		, mBackEnd(_gfxDriver)
	{
		loadCommonShaderCode();
		mErrorMaterial = std::make_unique<Material>(Effect::loadFromFile("plainColor.fx"));
		mErrorMaterial->addTexture(string("albedo"), std::make_shared<Texture>(Image::proceduralXOR(256, 4), false));

		mEV = 0.0f;
		// Init sky resources
		mSkyPlane = std::make_unique<RenderGeom>(RenderGeom::quad(2.f*Vec2f::ones()));
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::loadCommonShaderCode()
	{
		core::File code("forward.fx");
		mForwardShaderCommonCode = code.bufferAsText();
	}

	//----------------------------------------------------------------------------------------------
	Shader* ForwardPass::getShader(const Material& mat)
	{
		// Locate the proper pipeline set
		auto setIter = mPipelines.find(&mat.effect());
		if(setIter == mPipelines.end())
		{
			setIter = mPipelines.emplace(
				&mat.effect(),
				PipelineSet()
			).first;
		}
		auto& pipelineSet = setIter->second;
		// Locate the proper shader in the set
		const auto& descriptor = mat.bakedOptions(); // TODO: Hash this once during material setup. Use hash for faster indexing. Maybe incorporate effect in the hash.
		auto iter = pipelineSet.find(descriptor);
		if(iter == pipelineSet.end())
		{
			iter = pipelineSet.emplace(
				descriptor,
				Shader::createShader({
					mat.bakedOptions().c_str(),
					mForwardShaderCommonCode.c_str(),
					mat.effect().code().c_str()
					})
			).first;
		}
		return iter->second.get();
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::renderBackground(const math::Mat44f& viewProj, float exposure, Texture* bgTexture)
	{
		if(!mBackgroundShader)
		{
			// Try to load shader
			core::File code("sky.fx");
			mBackgroundShader = Shader::createShader( code.bufferAsText() );
		}
		if(mBackgroundShader)
		{
			auto& cmd = mBackEnd.beginCommand();
			mBackgroundShader->bind();
			
			// View projection matrix
			mBackEnd.addParam(0, viewProj);
			// Lighting
			mBackEnd.addParam(3, exposure);
			mBackEnd.addParam(7, bgTexture);
			cmd.shader = mBackgroundShader.get();
			cmd.vao = mSkyPlane->getVao();
			cmd.nIndices = mSkyPlane->nIndices();

			mBackEnd.endCommand();
		}
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::render(const Camera& _eye, const RenderScene& _scene, const RenderTarget& _dst, ShadowMapPass* _shadows)
	{
#ifdef _WIN32
		// Shader reload
		KeyboardInput* input = KeyboardInput::get();
		if (input->pressed('R'))
		{
			mPipelines.clear();
			loadCommonShaderCode();
		}
#endif
		resetStats();
		mBackEnd.beginFrame();
		mBackEnd.beginPass();

		_dst.bind();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, _dst.size().x(), _dst.size().y());

		if(ImGui::Begin("Forward pass"))
		{
			ImGui::SliderFloat("EV", &mEV, -2.f, 3.f);
			ImGui::End();
		}

		// Compute global variables
		auto vp = _eye.viewProj(_dst.aspectRatio());
		auto wsEye = _eye.position();
		float exposure = std::pow(2.f,mEV);
		auto& lightClr = _scene.lightClr();
		Vec3f lightDir = -_scene.mLightDir;

		// Iterate over renderables
		Mat44f worldMatrix = Mat44f::identity();
		mBackEnd.reserve(_scene.renderables().size());
		for(auto& renderable : _scene.renderables())
		{
			// Skip invalid objects
			auto renderObj = renderable.lock();
			if(renderObj->materials.size() < renderObj->meshes.size())
				continue;

			++m_numRenderables;
			for(size_t i = 0; i < renderObj->meshes.size(); ++i)
			{
				// Reuse commands from previous frames
				auto& command = mBackEnd.beginCommand();
				// Optional sky
				if(_scene.sky)
					mBackEnd.addParam(7, _scene.sky.get());
				if(_scene.irradiance)
					mBackEnd.addParam(8,_scene.irradiance.get());
				// Bind shadows
				if(_shadows)
				{
					// TODO: Bind shadows to renderer
					//glActiveTexture(GL_TEXTURE0+9);
					//glBindTexture(GL_TEXTURE_2D, _shadows->texName());
				}
				// Get world matrix
				worldMatrix = renderObj->transform.matrix();
				// Set up vertex uniforms
				Mat44f wvp = vp*worldMatrix;
				Mat44f model2Shadow;
				if(_shadows) // TODO: This should be world 2 shadow matrix
					model2Shadow = _shadows->shadowProj() * worldMatrix;

				// render
				const auto& material = *renderObj->materials[i];
				// Setup material
				auto shader = getShader(material);
				if(!shader)
					continue;
				command.shader = shader;
				material.bindParams(mBackEnd);
				// Matrices
				mBackEnd.addParam(0, wvp);
				mBackEnd.addParam(1, worldMatrix);
				if(_shadows) // TODO: This should be world 2 shadow matrix
					mBackEnd.addParam(2, model2Shadow);
				// Lighting
				mBackEnd.addParam(3, exposure); // EV
				mBackEnd.addParam(4, wsEye);
				mBackEnd.addParam(5, lightClr);
				mBackEnd.addParam(6, lightDir);
				// Render mesh
				command.vao = renderObj->meshes[i]->getVao();
				command.nIndices = renderObj->meshes[i]->nIndices();
				++m_numMeshes;
				mBackEnd.endCommand();
			}
		}

		// Render skybox
		if(_scene.sky)
			renderBackground(vp, exposure, _scene.sky.get());

		mBackEnd.endPass();
		mBackEnd.submitDraws();

		drawStats();
	}

	void ForwardPass::resetStats(){
		m_numMeshes = 0;
		m_numRenderables = 0;
	}

	void ForwardPass::drawStats(){
		const float DISTANCE = 10.f;
		ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - DISTANCE, ImGui::GetIO().DisplaySize.y - DISTANCE - 40);
		ImVec2 window_pos_pivot = ImVec2(1.0f, 1.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.3f)); // Transparent background
		bool isOpen = true;
		if (ImGui::Begin("Render Counters", &isOpen, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text("Renderables: %d", m_numRenderables);
			ImGui::Text("Meshes: %d", m_numMeshes);
			mBackEnd.drawStats();
			ImGui::End();
		}
		ImGui::PopStyleColor();
		resetStats();
	}

}}