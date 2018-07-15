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
#include <core/platform/fileSystem/fileSystem.h>
#include <core/time/time.h>
#include <graphics/debug/imgui.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/driver/shaderProcessor.h>
#include <graphics/renderer/material/material.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderMesh.h>
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
	ForwardPass::ForwardPass(BackEndRenderer& _backEnd, GraphicsDriverGL& _gfxDriver)
		: mDriver(_gfxDriver)
		, mBackEnd(_backEnd)
	{
		loadCommonShaderCode();
		mErrorMaterial = std::make_unique<Material>(make_shared<Effect>("plainColor.fx"));
		mErrorMaterial->addTexture(string("albedo"), std::make_shared<Texture>(Image::proceduralXOR(256, 4), false));

		mEV = 1.0f;
		// Init sky resources
		mSkyPlane = std::make_unique<RenderGeom>(RenderGeom::quad(2.f*Vec2f::ones()));
		m_drawLimit = -1;
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::loadCommonShaderCode()
	{
		ShaderProcessor::MetaData metadata;
		ShaderProcessor::loadCodeFromFile("forward.fx", mForwardShaderCommonCode, metadata);
		// TODO: Actualle use the metadata (unifrom layouts)
		for(auto& file : metadata.dependencies)
		{
			core::FileSystem::get()->onFileChanged(file) += [this](const char*) {
				mPipelines.clear();
				ShaderProcessor::MetaData metadata;
				mForwardShaderCommonCode.clear();
				ShaderProcessor::loadCodeFromFile("forward.fx", mForwardShaderCommonCode, metadata);
			};
		}
	}	

	//----------------------------------------------------------------------------------------------
	std::string ForwardPass::vertexFormatDefines(RenderGeom::VtxFormat vertexFormat)
	{
		// TODO: Create this defines procedurally with more information from the actual rendergeom
		string defines;
		if(vertexFormat.position() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_POSITION_FLOAT 0\n";
		if(vertexFormat.normal() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_NORMAL_FLOAT 1\n";
		if(vertexFormat.tangent() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_TANGENT_FLOAT 2\n";
		if(vertexFormat.uv() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_UV_FLOAT 3\n";
		return defines;
	}

	//----------------------------------------------------------------------------------------------
	Shader* ForwardPass::getShader(Material& mat, RenderGeom::VtxFormat vtxFormat, const EnvironmentProbe* env, bool shadows)
	{
		// Locate the proper pipeline set
		auto code = effectCode(env, shadows);
		auto setIter = mPipelines.find({code, &mat.effect()});
		if(setIter == mPipelines.end())
		{
			setIter = mPipelines.emplace(
				std::pair(code, &mat.effect()),
				PipelineSet()
			).first;
			auto& effect = mat.effect();
			// Listen to effect reload
			mat.effect().onReload([this, code](Effect& effect){
				auto setIter = mPipelines.find({code, &effect});
				if(setIter != mPipelines.end())
					setIter->second.clear();
			});
		}
		auto& pipelineSet = setIter->second;

		std::string environmentDefines = env ? "#define sampler2D_uEnvironment\n#define sampler2D_uIrradiance\n" : "";
		std::string shadowDefines = shadows ? "#define sampler2D_uShadowMap\n#define mat4_uMs2Shadow\n" : "";
		std::string skinningDefines = "";
		if(vtxFormat.weights() != RenderGeom::VtxFormat::Storage::None)
		{
			skinningDefines = "#define HW_SKINNING\n";
		}

		// Locate the proper shader in the set
		const auto& descriptor = std::pair(vtxFormat.code(),  mat.bakedOptions()); // TODO: Hash this once during material setup. Use hash for faster indexing. Maybe incorporate effect in the hash.
		auto iter = pipelineSet.find(descriptor);
		if(iter == pipelineSet.end())
		{
			iter = pipelineSet.emplace(
				descriptor,
				Shader::createShader({
					vertexFormatDefines(vtxFormat).c_str(),
					environmentDefines.c_str(),
					shadowDefines.c_str(),
					skinningDefines.c_str(),
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
			core::FileSystem::get()->onFileChanged("sky.fx") += [this](const char*) {
				mBackgroundShader = nullptr;
			};
		}
		if(mBackgroundShader)
		{
			auto& cmd = mBackEnd.beginCommand();
			cmd.cullMode = GL_BACK;
			mBackgroundShader->bind();
			
			// View projection matrix
			mBackEnd.addParam(0, viewProj);
			// Lighting
			mBackEnd.addParam(3, exposure);
			mBackEnd.addParam(7, bgTexture);
			cmd.shader = mBackgroundShader.get();
			cmd.vao = mSkyPlane->getVao();
			cmd.nIndices = mSkyPlane->indices().count;
			cmd.indexType= mSkyPlane->indices().componentType;

			mBackEnd.endCommand();
		}
	}

	//----------------------------------------------------------------------------------------------
	void setupOpenGLState()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GEQUAL);// GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glClearColor(1,1,1.f,1.f);
		//glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
		glClearDepthf(0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::render(const RenderScene& _scene, const RenderTarget& _dst, ShadowMapPass* _shadows)
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
		// Get active camera
		//assert(_scene.cameras().size() <= 1); // Only one camera per scene currently supported
		auto eye = _scene.cameras()[0].lock(); // TODO: Check for deleted cameras
		assert(eye);

		// Cull and sort
		cullAndSortScene(*eye, _scene);

		// Prepare skybox environment probe
		EnvironmentProbe environmentProbe;
		EnvironmentProbe* environmentPtr = nullptr;
		if(_scene.sky && _scene.irradiance)
		{
			environmentProbe.environment = _scene.sky;
			environmentProbe.irradiance = _scene.irradiance;
			environmentPtr = &environmentProbe;
		}

		// Compute global variables
		auto vp = eye->viewProj(_dst.aspectRatio());
		// Prepare graphics device's global state
		_dst.bind();
		setupOpenGLState();
		glViewport(0, 0, _dst.size().x(), _dst.size().y());
		// Render
		resetStats();
		mBackEnd.beginPass();
		// Iterate over renderables
		mBackEnd.reserve(_scene.renderables().size());
		// Record render commands
		resetRenderCache();
		for(const auto& mesh : mZSortedQueue)
		{
			if(m_drawLimit >= 0 && m_numRenderables >= unsigned(m_drawLimit))
				break;
			// Set up vertex uniforms
			renderMesh(
				mesh.geom.get(),
				mesh.skin.get(),
				vp* mesh.world, // World-View-Projection
				mesh.world,
				eye->position(),
				mesh.material,
				environmentPtr,
				_scene.lights(),
				_shadows);
			++m_numRenderables;
		}

		// Render skybox
		if(_scene.sky)
			renderBackground(vp, mEV, _scene.sky.get());

		// Finish pass
		mBackEnd.endPass();
		mBackEnd.submitDraws();

		drawStats();
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::cull(
		const math::Vec3f& camPos,
		const math::Vec3f& viewDir,
		const std::vector<std::shared_ptr<RenderObj>>& renderables)
	{
		mZSortedQueue.clear();
		MeshInfo meshDrawInfo;
		for(auto obj : renderables)
		{
			auto mesh = obj->mesh;
			// Set world matrix
			meshDrawInfo.world = obj->transform.matrix();
			meshDrawInfo.skin = obj->skin;

			for(size_t i = 0; i < mesh->mPrimitives.size(); ++i)
			{
				auto& primitive = mesh->mPrimitives[i];
				auto& geom = primitive.first;
				// Transform BBox to world space
				auto worldAABB = geom->bbox().transform(obj->transform);
				// Get min and max depths along view direction
				auto aDepth = viewDir.dot(worldAABB.min());
				auto bDepth = viewDir.dot(worldAABB.max());
				auto minDepth = math::min(aDepth, bDepth);
				auto maxDepth = math::max(aDepth, bDepth);
				// Transform depths relative to the camera
				auto center = obj->transform.position();
				float medDepth = (center - camPos).dot(viewDir);
				meshDrawInfo.depth = {medDepth-minDepth, medDepth+maxDepth};

				if(meshDrawInfo.depth.y() > 0) // Object may be visible
				{
					meshDrawInfo.geom = geom;
					meshDrawInfo.material = primitive.second;
					mZSortedQueue.push_back(meshDrawInfo);
				}
			}
			auto objPos = obj->transform.position();
			float depth = (objPos - camPos).dot(viewDir);
		}
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::resetRenderCache()
	{
		mBoundShader = nullptr;
		mBoundMaterial = nullptr;
		mBoundProbe = nullptr;
		mLastVtxFormatCode = RenderGeom::VtxFormat::invalid();
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::renderMesh(
		const RenderGeom* _mesh,
		const SkinInstance* _skin,
		const Mat44f& _wvp,
		const Mat44f _worldMatrix,
		const Vec3f _wsEye,
		const shared_ptr<Material>& _material,
		const EnvironmentProbe* env,
		const std::vector<std::shared_ptr<Light>>& lights,
		ShadowMapPass* shadows
	)
	{
		// Select material
		bool changedShader = false;
		bool changedMaterial = false;
		if(_material != mBoundMaterial || mLastVtxFormatCode != _mesh->vertexFormat().code())
		{
			auto shader = getShader(*_material, _mesh->vertexFormat(), env, shadows);
			if(!shader)
				return;
			if(shader != mBoundShader)
			{
				resetRenderCache();
				mBoundShader = shader;
				changedShader = true;
				mBoundProbe = nullptr;
			}
			mBoundMaterial = _material;
			changedMaterial = true;
		}
		// Begin recording command
		auto& command = mBackEnd.beginCommand();
		command.cullMode = affineTransformDeterminant(_worldMatrix) > 0.f ? GL_BACK : GL_FRONT;
		command.shader = mBoundShader;
		// Optional sky
		if(env != mBoundProbe)
		{
			mBoundProbe = env;
			mBackEnd.addParam(7, env->environment.get());
			mBackEnd.addParam(8, env->irradiance.get());
		}
		if(!lights.empty())
		{
			auto& light = lights[0];
			Vec3f lightDir = light->worldMatrix.rotateDirection({0.f, 1.f, 0.f});
			mBackEnd.addParam(6, lightDir);
			mBackEnd.addParam(5, light->color);
		}
		// Matrices
		mBackEnd.addParam(0, _wvp);
		mBackEnd.addParam(1, _worldMatrix);
		// Skinning matrix
		if(_skin)
			mBackEnd.addParam(30, _skin->appliedPose);
		// Material
		if(changedMaterial)
		{
			mBoundMaterial->bindParams(mBackEnd);
		}
		// Lighting
		if(changedShader)
		{
			mBackEnd.addParam(3, mEV); // Exposure value
			mBackEnd.addParam(4, _wsEye);
		}
		if(shadows)
		{
			Mat44f shadowProj = shadows->shadowProj() * _worldMatrix;
			mBackEnd.addParam(2, shadowProj);
			mBackEnd.addParam(9, shadows->texture().get());
		}
		// Render mesh
		command.vao = _mesh->getVao();
		command.nIndices = _mesh->indices().count;
		command.indexType = _mesh->indices().componentType;
		++m_numMeshes;
		mBackEnd.endCommand();
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::sortByRenderInfo()
	{
		std::sort(
			mZSortedQueue.begin(),
			mZSortedQueue.end(),
			[](const MeshInfo& a, const MeshInfo& b) {
				return a.material == b.material ?
					a.geom < b.geom :
					a.material < b.material;
				});
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
		}
		ImGui::End();

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
		}
		ImGui::End();
		ImGui::PopStyleColor();
		resetStats();
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::cullAndSortScene(const Camera& eye, const RenderScene& scene)
	{
		cull(eye.position(), eye.viewDir(), scene.renderables());
		std::sort(mZSortedQueue.begin(), mZSortedQueue.end(), [](const MeshInfo& a, const MeshInfo& b) { return a.depth.y() < b.depth.y(); });
		sortByRenderInfo();
	}
}}