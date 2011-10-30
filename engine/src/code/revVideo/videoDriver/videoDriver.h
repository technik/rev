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

			eAttribCount
		};

	public:
		// Virtual destructor
		virtual ~IVideoDriver() {}
		// ----- Per frame tasks ---- //
		virtual	void	beginFrame	() = 0;
		virtual	void	endFrame	() = 0;

		// ---- Accessor methods ---- //
		virtual void	setBackgroundColor	(const CColor& _color) = 0;
		virtual void	setScreenSize		(const unsigned int _width,
											const unsigned int _height) = 0;

		// ---- Render related ---- //
		virtual void	setShader			(const int _shaderId) = 0;
		virtual int		getUniformId		(const char * _name) const = 0;
		virtual void	setRealAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer) = 0;
		//virtual void	setIntAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer) = 0;
		//virtual void	setUniform		(const int _uniformId, const TReal _value) = 0;
		virtual	void	setUniform			(int _id, const CMat4& _value) = 0;
		virtual	void	setUniform			(int _id, const CColor& _value) = 0;
		virtual void	setUniform			(int _id, int _slot, const CTexture * _value) = 0;
		virtual void	drawIndexBuffer		(const int _nIndices, const unsigned short * _indices, const bool _strip) = 0;

		// --- Shader management -- //
		virtual int		linkShader			(CVtxShader* _vtx, CPxlShader* _pxl) = 0;
		virtual int		loadVtxShader		(const char * _name) = 0;
		virtual int		loadPxlShader		(const char * _name) = 0;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_DRIVER_DRIVER_H_
