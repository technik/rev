////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernndez-Agera Tortosa (a.k.a. Technik)
// Created on November 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Png utils for android

#ifdef ANDROID
#ifndef _REV_REVVIDEO_TEXTURE_ANDROID_ANDROIDPNGUTILS_H_
#define _REV_REVVIDEO_TEXTURE_ANDROID_ANDROIDPNGUTILS_H_

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	void * imageFromPngBuffer(char * _buffer, int& _width, int& _height);

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_TEXTURE_ANDROID_ANDROIDPNGUTILS_H_
#endif // ANDROID
