//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL
#ifndef _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGLDRIVER_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGLDRIVER_H_

#ifdef ANDROID

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#else // !ANDROID
#include "glew.h"
#endif // !ANDROID

#include <map>

#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>
#include <video/graphics/shader/shader.h>

namespace rev {
	namespace video{

		class Color;
		class RenderTarget;
		class Texture;

		class OpenGLDriver {
		public:
			enum class EPrimitiveType
			{
				triangles,
				triStrip,
				lines,
				lineStrip,
				points
			};

			enum class ECulling {
				eBack,
				eFront,
				eNone
			};


			// Note: position correspond to the lower left corner of the rectangle and the window, starting at (0,0)
			void		setViewport(const math::Vec2i& position, const math::Vec2u& size);
			void		clearZBuffer();
			void		clearColorBuffer();
			void		setClearColor(const Color&);
			void		setZCompare(bool _enable);
			void		setCulling(ECulling);
#ifndef ANDROID
			void		setMultiSampling(bool _enable);
#endif //!ANDROID

			void		beginFrame() { glFinish(); }
			void		finishFrame();

			// --- Vertex config and surface config ---
			void		setShader(const Shader::Ptr);

			int			getUniformLocation(const char* _uniformName);

			// --- Render targets ---
			void		setRenderTarget(const RenderTarget* _rt);

			// --- Attributes and uniforms
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const float * _buffer);
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const uint16_t* _buffer);
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const int16_t* _buffer);
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const math::Vec2f* _buffer);
			void		setAttribBuffer(unsigned _id, unsigned _nElements, const math::Vec3f* _buffer);

			void		setUniform(int _id, float _value);
			void		setUniform(int _id, const math::Vec2f& _value);
			void		setUniform(int _id, const math::Vec3f& _value);
			void		setUniform(int _id, const math::Mat33f& _value);
			void		setUniform(int _id, const math::Mat44f& _value);
			void		setUniform(int _id, const Color& _value);
			void		setUniform(int _id, const Texture* _rt, GLenum _texStage);

			// --- Draw ---
			void		drawIndexBuffer(int _nIndices, unsigned short * _indices, EPrimitiveType _primitive);

		private:
			GLuint	mProgram;
			GLuint mCurTexStage = GL_TEXTURE0;

			virtual void swapBuffers() = 0;
		};

	}	// namespace video
}	// namespave rev

#endif // _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGLDRIVER_H_