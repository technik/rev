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

			// Actual draw
			StaticRenderMesh* mesh = _dc.mesh;
			glBindVertexArray(mesh->vao);
			glDrawElements(GL_TRIANGLES, mesh->nIndices, GL_UNSIGNED_SHORT, mesh->indices);
		}

		//------------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::flush() {
			glBindVertexArray(0);
		}
	}
}