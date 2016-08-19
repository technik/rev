//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Draw debug helper geometry
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_DEBUGDRAWER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_DEBUGDRAWER_H_

#include <video/basicTypes/color.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>
#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>
#include <math/geometry/types.h>
#include <vector>

namespace rev {
	namespace video {

		class DebugDrawer {
		public:
			DebugDrawer(GraphicsDriver* driver, RendererBackEnd* backend)
				:mDriver(driver)
				,mBackEnd(backend)
				,mShader(Shader::manager()->get("debug"))
			{
			}

			void drawLine(const math::Vec3f& from, const math::Vec3f& to, const Color& c) {
				buffer.push_back({from, to, c});
			}

			void drawBasis(const math::Mat34f& basis) {
				drawLine(basis.col(3), basis.col(3) + basis.col(0), {1.f,0.f,0.f});
				drawLine(basis.col(3), basis.col(3) + basis.col(1), { 0.f,1.f,0.f });
				drawLine(basis.col(3), basis.col(3) + basis.col(2), { 0.f,0.f,1.f });
			}

			void setViewProj(const math::Mat44f& _vp) { mVp = _vp; }

			void drawFrustum(const math::Mat34f& viewMat, const math::Frustum&);

			void render() {
				mDriver->setShader((Shader*)mShader);
				unsigned uColor = mDriver->getUniformLocation("color");
				unsigned uVp = mDriver->getUniformLocation("vp");
				mDriver->setUniform(uVp, mVp);
				uint16_t indices[2] = { 0, 1 };
				for (auto line : buffer) {
					math::Vec3f * vertices = reinterpret_cast<math::Vec3f*>(&line);
					mDriver->setUniform(uColor, line.c);
					mDriver->setAttribBuffer(0, 2, vertices);
					mDriver->drawIndexBuffer(2, indices, GraphicsDriver::EPrimitiveType::lines);
				}
			}

		private:
			Shader::Ptr			mShader = nullptr;
			RendererBackEnd*	mBackEnd;
			GraphicsDriver*		mDriver;
			math::Mat44f		mVp;

		private:
			struct Line { 
				math::Vec3f from, to;
				Color c; 
			};

			std::vector<Line>	buffer;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_DEBUGDRAWER_H_