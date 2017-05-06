//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple deferred renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_DEFERRED_DEFERREDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_DEFERRED_DEFERREDRENDERER_H_

#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <video/basicTypes/color.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>
#include <video/window/window.h>

namespace rev {
	namespace video {

		class RenderContext;
		class Window;
		class RenderObj;

		class DeferredRenderer {
		public:
			void init(GraphicsDriver* _driver);
			void end();

			void startFrame();
			void finishFrame();

			// Renderer's specific config
			void renderContext(const RenderContext&);
			//void renderDebug(const RenderObj&);

		private:
			video::RendererBackEnd*	mBackEnd;
			GraphicsDriver*			mDriver;
			Shader::Ptr				mGeometryShader;
			Shader::Ptr				mLightShader;
		};

		//--------------------------------------------------------------------------------------------------------------
		inline void DeferredRenderer::init(GraphicsDriver* _driver){
			mDriver = _driver;
			mBackEnd = new RendererBackEnd(mDriver);
			mGeometryShader = Shader::manager()->get("geometry");
			mLightShader = Shader::manager()->get("light");
		}

		//--------------------------------------------------------------------------------------------------------------
		inline void DeferredRenderer::end(){
			delete mBackEnd;
		}
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_DEFERRED_DEFERREDRENDERER_H_
