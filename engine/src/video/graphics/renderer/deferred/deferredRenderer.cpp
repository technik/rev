//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple deferred renderer
#include "deferredRenderer.h"

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		void DeferredRenderer::startFrame() {
			//mDriver->clearColorBuffer();
			//mDriver->clearZBuffer();
		}

		//--------------------------------------------------------------------------------------------------------------
		void DeferredRenderer::finishFrame()
		{
			//mDriver->finishFrame();
		}

		//--------------------------------------------------------------------------------------------------------------
		void DeferredRenderer::renderContext(const RenderContext& _rc) {

		}
	}
}