////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel shaders

#ifndef _REV_REVVIDEO_SHADER_PXLSHADER_H_
#define _REV_REVVIDEO_SHADER_PXLSHADER_H_

#include "revCore/resourceManager/resource.h"
#include "revCore/string.h"

namespace rev { namespace video
{
	class CPxlShader: public TResource<CPxlShader, string>
	{
	public:
		CPxlShader(const string& _name);
		~CPxlShader();
		int		id() const	{	return mId;	}

	private:
		int mId;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SHADER_PXLSHADER_H_
