//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL
#ifndef _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGLDRIVER_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGLDRIVER_H_

#ifdef _WIN32
#include "windows/openGLDriverWindows.h"
#endif // _WIN32
#ifdef __linux__
#include "linux/openGLDriverLinux.h"
#endif // __linux__
#include "glew.h"
#include <core/containers/map.h>

#include <utility>

namespace rev {
	namespace video{

		class OpenGLDriver : public OpenGLDriverBase {
		public:
			OpenGLDriver(const Window* _window);

			// Note: position correspond to the lower left corner of the rectangle and the window, starting at (0,0)
			void		setViewport(const math::Vec2i& position, const math::Vec2u& size) override;
			void		clearZBuffer() override;
			void		clearColorBuffer() override;
			void		setClearColor(const Color&) override;
			void		setZCompare(bool _enable) override;

			void		finishFrame() override;

			// --- Vertex config and surface config ---
			void		setShader(const Shader*) override;

			int			getUniformLocation(const char* _uniformName) override;

			// --- Attributes and uniforms
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const float * _buffer) override;
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const uint16_t* _buffer) override;
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const int16_t* _buffer) override;
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const math::Vec2f* _buffer) override;
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const math::Vec3f* _buffer) override;

			void		setUniform(int _id, float _value) override;
			void		setUniform(int _id, const math::Vec2f& _value) override;
			void		setUniform(int _id, const math::Vec3f& _value) override;
			void		setUniform(int _id, const math::Mat33f& _value) override;
			void		setUniform(int _id, const math::Mat44f& _value) override;
			void		setUniform(int _id, const Color& _value) override;

			// --- Draw ---
			void		drawIndexBuffer(int _nIndices, unsigned short * _indices, EPrimitiveType _primitive) override;

		private:
			GLuint								mProgram;
		};

	}	// namespace video
}	// namespave rev

#endif // _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGLDRIVER_H_