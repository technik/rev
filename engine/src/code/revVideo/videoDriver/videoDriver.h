////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// Video driver interface

#ifndef _REV_REVVIDEO_DRIVER_DRIVER_H_
#define _REV_REVVIDEO_DRIVER_DRIVER_H_

#include "revCore/math/matrix.h"

namespace rev	{
	class CVec2;

	namespace	video
{
	// Forward declarations
	class CColor;
	class CPxlShader;
	class CShader;
	class CTexture;
	class CVtxShader;

	class IVideoDriver
	{
	public:
		enum EAttribType
		{
			eVertex = 0,
			eNormal,
			eTexCoord,

			eAttribCount
		};

		enum EPrimitiveType
		{
			eTriangle,
			eTriStrip,
			eLine,
			eLineStrip
		};

	public:
		// Virtual destructor
		virtual ~IVideoDriver() {}
		// ----- Per frame tasks ---- //
		virtual	void	beginFrame	() = 0;
		virtual void	clearZ		() = 0;
		virtual void	finishPass	() = 0;
		virtual	void	endFrame	() = 0;

		// ---- Accessor methods ---- //
		virtual void	setBackgroundColor	(const CColor& _color) = 0;
		virtual void	setScreenSize		(const unsigned int _width,
											const unsigned int _height) = 0;
		virtual const CVec2& screenSize		() const = 0;

		// ---- Render related ---- //
		virtual void	setModelMatrix	(const CMat34& _mv) = 0;
		virtual void	setViewMatrix	(const CMat34& _mv) = 0;
		virtual void	setProjMatrix		(const CMat4& _proj) = 0;
		virtual void	setShader			(const int _shaderId) = 0;
		virtual int		getUniformId		(const char * _name) const = 0;
		virtual void	setRealAttribBuffer	(const int _attribId, unsigned _nElements, const unsigned _nComponents, const void * const _buffer) = 0;
		//virtual void	setIntAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer) = 0;
		virtual void	setUniform			(int _id, float _value) = 0;
		virtual	void	setUniform			(int _id, const CMat4& _value) = 0;
		virtual	void	setUniform			(int _id, const CColor& _value) = 0;
		virtual void	setUniform			(int _id, const CVec3& _value) = 0;
		virtual void	setUniform			(int _id, int _slot, const CTexture * _value) = 0;
		virtual void	drawIndexBuffer		(const int _nIndices, const unsigned short * _indices,
											EPrimitiveType _primitive = eTriangle) = 0;
		// --- Shader management -- //
		virtual int		linkShader			(CVtxShader* _vtx, CPxlShader* _pxl) = 0;
		virtual void	destroyShader		(int _id) = 0;
		virtual int		loadVtxShader		(const char * _name) = 0;
		virtual int		loadPxlShader		(const char * _name) = 0;
		virtual void	releaseShader		(int _id) = 0;

		// -- Texture management -- //
		virtual unsigned registerTexture	(void * _buffer, int _width, int _height) = 0;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_DRIVER_DRIVER_H_
