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

#include <core/platform/fileSystem/file.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/driver/shader.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/material.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderScene.h>
#include <math/algebra/affineTransform.h>

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
		mErrorMaterial->addTexture(5, std::make_shared<Texture>(ImageRGB8::proceduralXOR(256))); // Albedo texture
		mErrorMaterial->addParam(6, 0.5f); // Roughness
		mErrorMaterial->addParam(7, 0.05f); // Metallic
		mEV = 1.5f;
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
		auto shaderP = shader.get();
		mPipelines.insert(std::make_pair(fileName, std::move(shader)));
		return shaderP;
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::bindMaterial(const Material* mat)
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
					bindMaterial(mErrorMaterial.get());
					return;
				}
			}
			else
				shaderIter->second->bind();

			mat->bind(mDriver);
		}
		// Bind error material
		bindMaterial(mErrorMaterial.get());
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::render(const Camera& _eye, const RenderScene& _scene, const RenderTarget& _dst)
	{
		_dst.bind();
		glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto vp = _eye.viewProj(4.f/3.f);

		Vec4f lightDir = { 0.2f, -0.3f, 2.0f , 0.0f };

		// Setup pixel global uniforms
		auto& lightClr = _scene.lightClr();
		glUniform3f(3, lightClr.x(), lightClr.y(), lightClr.z()); // Light color
		glUniform1f(4, mEV); // EV

		auto worldMatrix = Mat44f::identity();
		// Performance counters
		for(auto& renderable : _scene.renderables())
		{
			auto renderObj = renderable.lock();
			// Get world matrix
			worldMatrix.block<3,4>(0,0) = renderObj->transform.matrix();
			// Set up vertex uniforms
			auto wvp = vp*worldMatrix;
			glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, wvp.data());
			auto& worldI = worldMatrix.transpose();
			auto msViewDir = worldI.block<3,3>(0,0) * _eye.position() + worldI.block<3,4>(0,0).col(3);
			glUniform3f(1, msViewDir.x(), msViewDir.y(), msViewDir.z());

			// Setup pixel global uniforms
			auto msLightDir = worldI * lightDir;
			glUniform3f(2, msLightDir.x(), msLightDir.y(), msLightDir.z());

			// render
			for(size_t i = 0; i < renderObj->meshes.size(); ++i)
			{
				// Setup material
				bindMaterial(renderObj->materials[i].get());
				// Render mesh
				renderObj->meshes[i]->render();
			}
		}
	}

}}