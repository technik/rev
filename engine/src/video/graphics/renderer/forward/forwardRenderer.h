//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_

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

		class ForwardRenderer {
		public:
			template<class Alloc_> void init(GraphicsDriver* _driver, Alloc_& _alloc);
			template<class Alloc_> void end(Alloc_& _alloc);

			void startFrame();
			void finishFrame();

			// Renderer's specific config
			void setAmbientLight(const Color& _color);

			void renderContext(const RenderContext&);
			//void renderDebug(const RenderObj&);

		private:
			void renderObject(const RenderObj& _obj);

		private:
			video::Shader::Ptr		mShader = nullptr;
			video::Shader::Ptr		mShadowShader = nullptr;
			video::RendererBackEnd*	mBackEnd;
			RenderTarget*			mShadowBuffer;
			GraphicsDriver*			mDriver;
		};

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::init(GraphicsDriver* _driver, Alloc_& _alloc){
			mDriver = _driver;
			mBackEnd = _alloc.template create<RendererBackEnd>(mDriver);
			mShader = Shader::manager()->get("forward");
			mShadowShader = Shader::manager()->get("shadow");

			mShadowBuffer = mDriver->createRenderTarget({512, 512}, Texture::EImageFormat::rgba, Texture::EByteFormat::eFloat);

			mDriver->setShader((Shader*)mShader);
			mDriver->setClearColor(Color(0.7f, 0.8f, 1.f, 1.f));
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::end(Alloc_& _alloc){
			mDriver->destroyRenderTarget(mShadowBuffer);
			_alloc.destroy(mBackEnd);
		}
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_