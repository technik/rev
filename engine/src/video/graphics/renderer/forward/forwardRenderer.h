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
#include <video/graphics/renderer/renderPass/shadowPass.h>
#include <video/graphics/renderer/debugDrawer.h>

namespace rev {
	namespace video {

		class RenderContext;
		class Window;
		class RenderObj;
		class Camera;

		class ForwardRenderer {
		public:
			template<class Alloc_> void init(GraphicsDriver* _driver, Alloc_& _alloc);
			template<class Alloc_> void end(Alloc_& _alloc);

			void startFrame();
			void finishFrame();

			// Renderer's specific config
			void setShadowLight(const math::Vec3f& _pos, const math::Vec3f& dir, const Color& _color);
			void setAmbientLight(const Color& _color);
			void setWindowSize(const rev::math::Vec2u& _size) { mWindowSize = _size; }

			void renderContext(const RenderContext&);
			void setDebugCamera(const Camera* _dbg) { mDebugCamera = _dbg; }

		private:
			void renderObject(const RenderObj& _obj);
			math::Frustum adjustShadowFrustum(const RenderContext&);

		private:
			DebugDrawer*		mDebug;
			const Camera*		mDebugCamera;
			Shader::Ptr			mShader = nullptr;
			Shader::Ptr			mSkyShader = nullptr;
			RendererBackEnd*	mBackEnd;
			rev::math::Vec2u	mWindowSize;
			GraphicsDriver*		mDriver;
			ShadowPass*			mShadowPass[3];
		};

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::init(GraphicsDriver* _driver, Alloc_& _alloc){
			mDriver = _driver;
			mBackEnd = _alloc.template create<RendererBackEnd>(mDriver);
			mShader = Shader::manager()->get("forward");
			mSkyShader = Shader::manager()->get("skybox");
			mDebug = new DebugDrawer(mDriver, mBackEnd);

			mShadowPass[0] = new ShadowPass(mDriver, mBackEnd, 1024);
			mShadowPass[1] = new ShadowPass(mDriver, mBackEnd, 1024);
			mShadowPass[2] = new ShadowPass(mDriver, mBackEnd, 1024);

			mDriver->setShader(mShader);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::end(Alloc_& _alloc){
			delete mShadowPass[0];
			delete mShadowPass[1];
			delete mShadowPass[2];
			_alloc.destroy(mBackEnd);
		}
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_