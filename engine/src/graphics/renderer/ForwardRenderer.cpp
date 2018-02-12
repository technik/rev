#include "ForwardRenderer.h"
#include <core/platform/fileSystem/file.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/material.h>
#include <math/algebra/affineTransform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>
#include <graphics/scene/renderObj.h>

using namespace rev::math;

namespace rev { namespace graphics {

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::init(GraphicsDriverGL& driver)
	{
		mDriver = &driver;
		core::File shaderFile("pbr.fx");
		mShader = Shader::createShader(shaderFile.bufferAsText());
		mErrorMaterial = std::make_unique<Material>();
		mErrorMaterial->name = "XOR-ErrorMaterial";
		mErrorMaterial->addTexture(5, std::make_shared<Texture>(ImageRGB8::proceduralXOR(256))); // Albedo texture
		mErrorMaterial->addParam(6, 0.5f); // Roughness
		mErrorMaterial->addParam(7, 0.05f); // Metallic
		mEV = 1.5f;
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const Camera& eye, const RenderScene& scene) {
		glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto vp = eye.viewProj(4.f/3.f);

		Vec4f lightDir = { 0.2f, -0.3f, 2.0f , 0.0f };
		mShader->bind();

		gui::beginWindow("Renderer");
		gui::slider("EV", mEV, -4.f, 4.f);
		gui::endWindow();
		float ev = pow(10.f,-mEV);

		// Setup pixel global uniforms
		auto& lightClr = scene.lightClr();
		glUniform3f(3, lightClr.x(), lightClr.y(), lightClr.z()); // Light color
		glUniform1f(4, ev); // EV

		auto worldMatrix = Mat44f::identity();
		auto& driver = *mDriver;
		// Performance counters
		size_t nRenderObjs = 0;
		size_t nDrawCalls = 0;
		for(auto renderable : scene.renderables())
		{
			nRenderObjs++;
			auto renderObj = renderable.lock();
			// Get world matrix
			worldMatrix.block<3,4>(0,0) = renderObj->transform.matrix();
			// Set up vertex uniforms
			auto wvp = vp*worldMatrix;
			glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, wvp.data());
			auto& worldI = worldMatrix.transpose();
			auto msViewDir = worldI.block<3,3>(0,0) * eye.position() + worldI.block<3,4>(0,0).col(3);
			glUniform3f(1, msViewDir.x(), msViewDir.y(), msViewDir.z());

			// Setup pixel global uniforms
			auto msLightDir = worldI * lightDir;
			glUniform3f(2, msLightDir.x(), msLightDir.y(), msLightDir.z());

			// render
			for(size_t i = 0; i < renderObj->meshes.size(); ++i)
			{
				nDrawCalls++;
				// Setup material
				auto& material = renderObj->materials[i];
				if(material)
					material->bind(driver);
				else
					mErrorMaterial->bind(driver);
				// Render mesh
				renderObj->meshes[i]->render();
			}
		}

		if(ImGui::Begin("Render metrics"))
		{
			ImGui::Text("Objects: %d, Draws: %d", nRenderObjs, nDrawCalls);
			ImGui::End();
		}
	}

}}
