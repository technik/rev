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
		virtual	void	beginFrame	();
		virtual	void	endFrame	();

		// ---- Accessor methods ---- //
		void	setBackgroundColor	(const CColor& _color);
		void	setScreenSize		(const unsigned int _width,
									const unsigned int _height);

		// ---- Render tasks ---
		virtual void	setShader		(const int _shader);
		virtual void	setRealAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer);
		virtual void	drawIndexBuffer	(const int _nIndices, const unsigned short * _indices, const bool _strip);

	private:
		virtual int 	loadShader		(const char * _vtxName, const char * _pxlName);
		virtual	void	deleteShader	(const int _id);
		void			bindAttributes			(int _shader);
	private:
		int				mCurShader;
		unsigned int 	mScreenWidth;
		unsigned int 	mScreenHeight;
	};

}	// namespace video
}	// namespace rev

#endif // REV_REVVIDEO_DRIVER_ANDROID_VIDEODRIVERANDROID_H_
