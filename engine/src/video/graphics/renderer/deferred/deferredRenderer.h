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
#include <video/graphics/driver/openGL/openGLDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>
#include <video/window/window.h>

namespace rev {
	namespace video {

		class GraphicsDriver;
		class RenderContext;
		class Window;
		class RenderObj;

		class DeferredRenderer {
		public:
			template<class Alloc_> void init(GraphicsDriver* _driver, Alloc_& _alloc);
			template<class Alloc_> void end(Alloc_& _alloc);

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
		template<class Alloc_> void DeferredRenderer::init(GraphicsDriver* _driver, Alloc_& _alloc){
			mDriver = _driver;
			mBackEnd = _alloc.template create<RendererBackEnd>(mDriver);
			mGeometryShader = Shader::manager()->get("geometry");
			mLightShader = Shader::manager()->get("light");

			mDriver->setShader((Shader*)mShader);
			mDriver->setClearColor(Color(0.7f, 0.8f, 1.f, 1.f));
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void DeferredRenderer::end(Alloc_& _alloc){
			_alloc.destroy(mBackEnd);
		}
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_DEFERRED_DEFERREDRENDERER_H_
