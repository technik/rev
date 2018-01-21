#include "ForwardRenderer.h"
#include <core/platform/fileSystem/file.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderGeom.h>
#include <math/algebra/affineTransform.h>
#include <graphics/debug/debugGUI.h>

using namespace rev::math;

namespace rev { namespace graphics {

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::init()
	{
		core::File shaderFile("pbr.fx");
		mShader = Shader::createShader(shaderFile.bufferAsText());
		mEV = 0.f;
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const Camera& eye, const game::RenderScene& scene) {
		glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto vp = eye.viewProj(4.f/3.f);

		Vec4f lightDir = { 0.2f, -0.3f, 2.0f , 0.0f };
		mShader->bind();

		gui::beginWindow("Renderer");
		gui::slider("EV", mEV, -4.f, 4.f);
		gui::endWindow();
		float ev = pow(10.f,mEV);

		auto worldMatrix = Mat44f::identity();
		for(auto renderable : scene.renderables()) {
			auto& renderObj = renderable->renderObj();
			// Get world matrix
			worldMatrix.block<3,4>(0,0) = renderObj.transform.matrix();
			// Set up vertex uniforms
			auto wvp = vp*worldMatrix;
			glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, wvp.data());
			auto& worldI = worldMatrix.transpose();
			auto msViewDir = worldI.block<3,3>(0,0) * eye.position() + worldI.block<3,4>(0,0).col(3);
			glUniform3f(1, msViewDir.x(), msViewDir.y(), msViewDir.z());
			// Setup pixel global uniforms
			auto msLightDir = worldI * lightDir;
			glUniform3f(2, msLightDir.x(), msLightDir.y(), msLightDir.z());
			glUniform3f(3, 1.f, 1.f, 1.f); // Light color
			glUniform1f(4, ev); // EV

			// Setup material
			auto& material = renderable->material();
			auto albedo = material.albedo;
			glUniform3f(5, albedo.x(), albedo.y(), albedo.z()); // Albedo
			glUniform1f(6, material.roughness);
			glUniform1f(7, material.metallic);
			// render
			renderObj.mesh->render();
		}
	}

}}
