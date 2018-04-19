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

		mEV = 1.0f;
		// Init sky resources
		mSkyPlane = std::make_unique<RenderGeom>(RenderGeom::quad(2.f*Vec2f::ones()));
		m_drawLimit = -1;
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
	void setupOpenGLState()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

		// Prepare graphics device's global state
		_dst.bind();
		setupOpenGLState();
		glViewport(0, 0, _dst.size().x(), _dst.size().y());

		// Compute global variables
		auto vp = _eye.viewProj(_dst.aspectRatio());
		auto wsEye = _eye.position();
		auto& lightClr = _scene.lightClr();
		Vec3f lightDir = -_scene.mLightDir;

		// Iterate over renderables
		Mat44f worldMatrix = Mat44f::identity();
		Mat44f model2Shadow;
		Mat44f wvp;
		mBackEnd.reserve(_scene.renderables().size());
		// Prepare skybox environment probe
		EnvironmentProbe environmentProbe;
		EnvironmentProbe* environmentPtr = nullptr;
		if(_scene.sky && _scene.irradiance)
		{
			environmentProbe.environment = _scene.sky;
			environmentProbe.irradiance = _scene.irradiance;
			environmentPtr = &environmentProbe;
		}

		depthSort(wsEye, _eye.viewDir(), _scene.renderables());

		for(const auto& mesh : mZSortedQueue)
		{
			if(m_drawLimit >= 0 && m_numRenderables >= m_drawLimit)
				break;

			// Set up vertex uniforms
			wvp = vp* mesh.second.world;

			++m_numRenderables;
			{
				renderMesh(
					mesh.second.geom,
					wvp,
					mesh.second.world,
					wsEye,
					mesh.second.material,
					environmentPtr);
			}
		}

		// Render skybox
		if(_scene.sky)
			renderBackground(vp, mEV, _scene.sky.get());

		mBackEnd.endPass();
		mBackEnd.submitDraws();

		drawStats();
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::depthSort(
		const math::Vec3f& camPos,
		const math::Vec3f& viewDir,
		const std::vector<std::shared_ptr<RenderObj>>& renderables)
	{
		mZSortedQueue.clear();
		MeshInfo meshDrawInfo;
		for(auto obj : renderables)
		{
			// Skip invalid objects
			if(obj->materials.size() < obj->meshes.size())
				continue;

			// TODO: Transform bounding box to world
			meshDrawInfo.world = obj->transform.matrix();
			int matNdx = 0;
			for(auto mesh : obj->meshes)
			{
				auto center = obj->transform.transformPosition(mesh->bbox.center());
				float radius = mesh->bbox.radius();
				float medDepth = (center - camPos).dot(viewDir);
				if(medDepth > -radius)
				{
					meshDrawInfo.geom = mesh.get();
					meshDrawInfo.material = obj->materials[matNdx].get();
					mZSortedQueue.emplace(medDepth-radius, meshDrawInfo);
				}
				matNdx++;
			}
			auto objPos = obj->transform.position();
			float depth = (objPos - camPos).dot(viewDir);
		}
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::renderMesh(
		const RenderGeom* _mesh,
		const Mat44f& _wvp,
		const Mat44f _worldMatrix,
		const Vec3f _wsEye,
		const Material* _material,
		const EnvironmentProbe* env)
	{
		// Reuse commands from previous frames
		auto& command = mBackEnd.beginCommand();
		// Optional sky
		if(env)
		{
			mBackEnd.addParam(7, env->environment.get());
			mBackEnd.addParam(8, env->irradiance.get());
		}

		// Setup material
		auto shader = getShader(*_material);
		if(!shader)
			return;
		command.shader = shader;
		_material->bindParams(mBackEnd);
		// Matrices
		mBackEnd.addParam(0, _wvp);
		mBackEnd.addParam(1, _worldMatrix);
		//if(_shadows) // TODO: This should be world 2 shadow matrix
		//	mBackEnd.addParam(2, model2Shadow);
		// Lighting
		mBackEnd.addParam(3, mEV); // Exposure value
		mBackEnd.addParam(4, _wsEye);
		// Render mesh
		command.vao = _mesh->getVao();
		command.nIndices = _mesh->nIndices();
		++m_numMeshes;
		mBackEnd.endCommand();
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::resetStats(){
		m_numMeshes = 0;
		m_numRenderables = 0;
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::drawStats(){
		// Debug interface
		if(ImGui::Begin("Forward pass"))
		{
			float exposure = std::log2(mEV);
			ImGui::SliderFloat("EV", &exposure, -2.f, 3.f);
			mEV = std::pow(2.f,exposure);
			int maxDraws = m_drawLimit >= 0 ? m_drawLimit : mZSortedQueue.size();
			ImGui::SliderInt("Max Draws", &maxDraws, 0, mZSortedQueue.size());
			m_drawLimit = maxDraws < mZSortedQueue.size() ? maxDraws : -1;
			ImGui::End();
		}

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