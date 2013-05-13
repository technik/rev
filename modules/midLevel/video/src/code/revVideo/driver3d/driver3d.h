//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 3d driver interface

#ifndef _REV_VIDEO_3DDRIVER_3DDRIVER_H_
#define _REV_VIDEO_3DDRIVER_3DDRIVER_H_

#include <cstdint>
#include <utility>

#include <revCore/math/algebra/vector.h>

namespace rev { namespace video
{
	// Forward declarations
	class Color;
	class PxlShader;
	class Shader;
	class VtxShader;

	class Driver3d
	{
	public:
		enum class EPrimitiveType
		{
			triangles,
			triStrip,
			lines,
			lineStrip
		};
	public:
		virtual ~Driver3d() {} // Virtual destructor

		// Note: position correspond to the lower left corner of the rectangle and the window, starting at (0,0)
		virtual void		setViewport			(const math::Vec2i& position, const math::Vec2u& size) = 0;
		virtual void		clearZBuffer		() = 0;
		virtual void		clearColorBuffer	() = 0;
		virtual void		setClearColor		(const Color&) = 0;

		virtual void		flush				() = 0;
		virtual void		finishFrame			() = 0;

		// --- Shaders -----
		virtual void		setShader			(const Shader * _shader) = 0;
		virtual PxlShader*	createPxlShader		(const char * _fileName) = 0;
		virtual Shader*		createShader		(VtxShader*, PxlShader*) = 0;
		virtual VtxShader*	createVtxShader		(const char * _fileName) = 0;

		// --- Attributes and uniforms
		virtual void		setAttribBuffer		(unsigned _id, unsigned _nElements, const float * _buffer) = 0;
		virtual void		setAttribBuffer		(unsigned _id, unsigned _nElements, const uint16_t* _buffer) = 0;
		virtual void		setAttribBuffer		(unsigned _id, unsigned _nElements, const int16_t* _buffer) = 0;
		virtual void		setAttribBuffer		(unsigned _id, unsigned _nElements, const math::Vec2f* _buffer) = 0;
		virtual void		setAttribBuffer		(unsigned _id, unsigned _nElements, const math::Vec3f* _buffer) = 0;

		virtual void		setUniform			(int _id, float _value) = 0;
		virtual void		setUniform			(int _id, const math::Vec2f& _value) = 0;
		virtual void		setUniform			(int _id, const math::Vec3f& _value) = 0;
		virtual void		setUniform			(int _id, const Color& _value) = 0;

		// --- Draw ---
		virtual void		drawIndexBuffer		(int _nIndices, unsigned short * _indices, EPrimitiveType _primitive) = 0;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_3DDRIVER_3DDRIVER_H_
