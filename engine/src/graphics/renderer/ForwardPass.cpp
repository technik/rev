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
#include <graphics/scene/camera.h>
#include <graphics/scene/material.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderScene.h>
#include <math/algebra/affineTransform.h>

#ifdef _WIN32
#include <input/keyboard/keyboardInput.h>
using namespace rev::input;
#endif // _WIN32

using namespace rev::math;

namespace rev { namespace graphics {

	//----------------------------------------------------------------------------------------------
	ForwardPass::ForwardPass(GraphicsDriverGL& _gfxDriver)
		: mDriver(_gfxDriver)
	{
		loadCommonShaderCode();
		mErrorMaterial = std::make_unique<Material>();
		mErrorMaterial->name = "XOR-ErrorMaterial";
		mErrorMaterial->shader = "simplePBR.fx";
		mErrorMaterial->addTexture(5, std::make_shared<Texture>(Image::proceduralXOR(256, 4), false)); // Albedo texture
		mErrorMaterial->addParam(6, 0.5f); // Roughness
		mErrorMaterial->addParam(7, 0.05f); // Metallic
		mEV = 1.5f;

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
	Shader* ForwardPass::loadShader(const std::string& fileName)
	{
		core::File code(fileName);
		auto shader = Shader::createShader({mForwardShaderCommonCode.c_str(), code.bufferAsText()});
		if(shader)
		{
			auto shaderP = shader.get();
			mPipelines.insert(std::make_pair(fileName, std::move(shader)));
			return shaderP;
		} else
			return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	bool ForwardPass::bindMaterial(const Material* mat)
	{
		if(mat && !mat->shader.empty()) // Bind proper material
		{
			// Find shader
			auto shaderIter = mPipelines.find(mat->shader);
			if(shaderIter == mPipelines.end())
			{
				// Try to load shader
				auto shader = loadShader(mat->shader);
				if(shader)
					shader->bind();
				else
				{
					if(mat != mErrorMaterial.get())
						return bindMaterial(mErrorMaterial.get());
					return false;
				}
			}
			else
				shaderIter->second->bind();

			mat->bind(mDriver);
			return true;
		}
		// Bind error material
		return bindMaterial(mErrorMaterial.get());
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
		_dst.bind();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
		glClearDepth(1.0);
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
		Vec3f lightDir = { 0.f, 0.f, 1.0f };

		if(_scene.sky)
		{
			glActiveTexture(GL_TEXTURE0+7);
			glBindTexture(GL_TEXTURE_2D, _scene.sky->glName());
		}
		if(_scene.irradiance)
		{
			glActiveTexture(GL_TEXTURE0+8);
			glBindTexture(GL_TEXTURE_2D, _scene.irradiance->glName());
		}
		// Bind shadows
		if(_shadows)
		{
			glActiveTexture(GL_TEXTURE0+9);
			glBindTexture(GL_TEXTURE_2D, _shadows->texName());
		}

		auto worldMatrix = Mat44f::identity();
		// TODO: Performance counters
		for(auto& renderable : _scene.renderables())
		{
			auto renderObj = renderable.lock();
			// Get world matrix
			worldMatrix.block<3,4>(0,0) = renderObj->transform.matrix();
			// Set up vertex uniforms
			auto wvp = vp*worldMatrix;

			// render
			for(size_t i = 0; i < renderObj->meshes.size(); ++i)
			{
				// Setup material
				if(bindMaterial(renderObj->materials[i].get()))
				{
					// Matrices
					glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, wvp.data());
					glUniformMatrix4fv(1, 1, !Mat44f::is_col_major, worldMatrix.data());
					if(_shadows) // TODO: This should be world 2 shadow matrix
						glUniformMatrix4fv(2, 1, !Mat44f::is_col_major, worldMatrix.data());
					// Lighting
					glUniform1f(3, exposure); // EV
					glUniform3f(4, wsEye.x(), wsEye.y(), wsEye.z());
					glUniform3f(5, lightClr.x(), lightClr.y(), lightClr.z()); // Light color
					glUniform3f(6, lightDir.x(), lightDir.y(), lightDir.z());
					if(_scene.sky)
						glUniform1i(7, 7);
					if(_scene.irradiance)
						glUniform1i(8, 8);
					// Bind shadows
					if(_shadows)
						glUniform1i(9, 9);
					// Render mesh
					renderObj->meshes[i]->render();
				}
			}
		}

		// Render skybox
		if(!_scene.sky)
			return;
		auto skyShaderIter = mPipelines.find("sky.fx");
		Shader* skyShader = nullptr;
		if(skyShaderIter == mPipelines.end())
		{
			// Try to load shader
			core::File code("sky.fx");
			auto skyShaderPtr = Shader::createShader( code.bufferAsText() );
			if(skyShaderPtr)
			{
				skyShader = skyShaderPtr.get();
				mPipelines.insert(std::make_pair("sky.fx", std::move(skyShaderPtr)));
			}
		} else
			skyShader = skyShaderIter->second.get();
		if(skyShader)
		{
			skyShader->bind();
			// View projection matrix
			glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, vp.data());
			// Lighting
			glUniform1f(3, exposure); // EV
			// Sky texture
			glUniform1i(7, 7);
			mSkyPlane->render();
		}
	}

}}