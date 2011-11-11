////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 10th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// android video driver

#ifndef REV_REVVIDEO_DRIVER_ANDROID_VIDEODRIVERANDROID_H_
#define REV_REVVIDEO_DRIVER_ANDROID_VIDEODRIVERANDROID_H_

// Engine headers
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	class CColor;

	class CVideoDriverAndroid: public IVideoDriver
	{
	public:
		// ---- Ctors and dtors ---- //
		CVideoDriverAndroid();

		// ---- Per frame tasks ---- //
		void	beginFrame	();
		void	endFrame	();

		// ---- Accessor methods ---- //
		void	setBackgroundColor	(const CColor& _color);
		void	setScreenSize		(const unsigned int _width,
									const unsigned int _height);

		// ---- Render tasks ---
		void	setShader			(const int _shader);
		int		getUniformId		(const char * _name) const;
		void	setRealAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer);
		void	setUniform			(int _id, float _f);
		void	setUniform			(int _id, const CMat4& _value);
		void	setUniform			(int _id, const CColor& _value);
		void	setUniform			(int /*_id*/, int /*_slot*/, const CTexture* /*_texture*/) {}
		void	drawIndexBuffer		(const int _nIndices, const unsigned short * _indices, const bool _strip);

	private:
		// --- Shader management -- //
		int		linkShader			(CVtxShader* _vtx, CPxlShader* _pxl);
		int		loadVtxShader		(const char * _name);
		int		loadPxlShader		(const char * _name);
		void	bindAttributes		(int _shader);

	private:
		int				mCurShader;
		unsigned int 	mScreenWidth;
		unsigned int 	mScreenHeight;
	};

}	// namespace video
}	// namespace rev

#endif // REV_REVVIDEO_DRIVER_ANDROID_VIDEODRIVERANDROID_H_
