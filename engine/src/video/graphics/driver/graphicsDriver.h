//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Interface with graphics hardware

#ifndef _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_

#include <cstdint>

#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <video/basicTypes/texture.h>

namespace rev {
	namespace video
	{
		// Forward declarations
		class Color;
		class Image;
		class Shader;
		class RenderTarget;

		class GraphicsDriver
		{
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

		public:
			template<class Alloc_>
			static GraphicsDriver* createDriver(Alloc_&); // 666TODO: Unimplemented

			virtual ~GraphicsDriver() = default; // Virtual destructor

			// Note: position corresponds to the lower left corner of the rectangle and the window, starting at (0,0)
			virtual void		setViewport(const math::Vec2i& position, const math::Vec2u& size) = 0;
			virtual void		clearZBuffer() = 0;
			virtual void		clearColorBuffer() = 0;
			virtual void		setClearColor(const Color&) = 0;
			virtual void		setZCompare(bool _enable) = 0;
			virtual void		setCulling(ECulling) = 0;
			virtual void		setMultiSampling(bool _enable) = 0;

			virtual void		finishFrame() = 0;

			virtual void				setRenderTarget		(RenderTarget* _rt) = 0;

			// --- Vertex config and surface config ---
			virtual	void		setShader(const Shader*) = 0;

			virtual int			getUniformLocation(const char* _uniformName) = 0;

			// --- Attributes and uniforms
			virtual void		setAttribBuffer(unsigned _id, unsigned _nElements, const float * _buffer) = 0;
			virtual void		setAttribBuffer(unsigned _id, unsigned _nElements, const uint16_t* _buffer) = 0;
			virtual void		setAttribBuffer(unsigned _id, unsigned _nElements, const int16_t* _buffer) = 0;
			virtual void		setAttribBuffer(unsigned _id, unsigned _nElements, const math::Vec2f* _buffer) = 0;
			virtual void		setAttribBuffer(unsigned _id, unsigned _nElements, const math::Vec3f* _buffer) = 0;

			virtual void		setUniform(int _id, float _value) = 0;
			virtual void		setUniform(int _id, const math::Vec2f& _value) = 0;
			virtual void		setUniform(int _id, const math::Vec3f& _value) = 0;
			virtual void		setUniform(int _id, const math::Mat33f& _value) = 0;
			virtual void		setUniform(int _id, const math::Mat44f& _value) = 0;
			virtual void		setUniform(int _id, const Color& _value) = 0;
			virtual void		setUniform(int _id, const Texture* _rt) = 0;

			// --- Draw ---
			virtual void		drawIndexBuffer(int _nIndices, unsigned short * _indices, EPrimitiveType _primitive) = 0;
		};

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_
