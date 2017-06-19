//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Render system's back end, the layer right on top of the video driver
#include "rendererBackEnd.h"

#include <math/algebra/matrix.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/staticRenderMesh.h>

using namespace rev::math;

namespace rev {
	namespace video {
		
		//------------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::draw(const DrawCall& _dc) {
			const DrawInfo& info = _dc.renderStateInfo;
			// Set shader
			mDriver->setShader(info.program);

			// Set global uniforms
			int uWorldViewProj = mDriver->getUniformLocation("uWorldViewProj");
			mDriver->setUniform(uWorldViewProj, info.wvp);

			int uLightClr = mDriver->getUniformLocation("uLightClr");
			mDriver->setUniform(uLightClr, info.lightClr);

			int uLightDir = mDriver->getUniformLocation("uLightDir");
			mDriver->setUniform(uLightDir, info.lightDir);

			int uViewPos = mDriver->getUniformLocation("uViewPos");
			mDriver->setUniform(uViewPos, info.viewPos);

			GLenum stage = 0;
			for (const auto& map : info.texUniforms) {
				int uLoc = mDriver->getUniformLocation(map.first.c_str());
				if (uViewPos >= 0) {
					mDriver->setUniform(uLoc, map.second, stage);
					++stage;
				}
			}

			for (const auto& v : info.vec3Uniforms) {
				int uLoc = mDriver->getUniformLocation(v.first.c_str());
				if (uViewPos >= 0) {
					mDriver->setUniform(uLoc, v.second);
				}
			}

			for (const auto& f : info.floatUniforms) {
				int uLoc = mDriver->getUniformLocation(f.first.c_str());
				if (uViewPos >= 0) {
					mDriver->setUniform(uLoc, f.second);
				}
			}

			// Actual draw
			StaticRenderMesh* mesh = _dc.mesh;
			glBindVertexArray(mesh->vao);
			glDrawElements(GL_TRIANGLES, mesh->nIndices, GL_UNSIGNED_SHORT, mesh->indices);
		}

		//------------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::drawSkybox(const DrawInfo& _info) {
			if (!mSkyBox) {
				mSkyBox = game::Procedural::box(Vec3f(100.f));
			}
			DrawCall dc;
			dc.renderStateInfo = _info;
			dc.mesh = mSkyBox;
			glDisable(GL_CULL_FACE);
			draw(dc);
			glEnable(GL_CULL_FACE);
		}

		//------------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::flush() {
			//glBindVertexArray(0);
		}
	}
}