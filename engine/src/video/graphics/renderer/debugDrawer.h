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

			void setViewProj(const math::Mat34f& _view, const math::Mat44f& _proj) {
				math::Mat34f invView;
				_view.inverse(invView);
				mVp = _proj * invView; 
			}

			void drawFrustum(const math::Mat34f& viewMat, const math::Frustum& _f, const Color& _c) {
				math::Vec3f vertices[8];
				for (size_t i = 0; i < 8; ++i) {
					vertices[i] = viewMat * _f.vertices()[i];
				}

				for (size_t i = 0; i < 4; ++i) {
					auto j = (i+1)%4;
					drawLine(vertices[i], vertices[j], _c);
					drawLine(vertices[i+4], vertices[j+4], _c);
					drawLine(vertices[i], vertices[i + 4], _c);
				}
			}

			void render() {
				mDriver->setCulling(GraphicsDriver::ECulling::eNone);
				//mDriver->setZCompare(false);
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
				buffer.clear();
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