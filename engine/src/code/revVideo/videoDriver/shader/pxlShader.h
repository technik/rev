////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel shaders

#ifndef _REV_REVVIDEO_SHADER_PXLSHADER_H_
#define _REV_REVVIDEO_SHADER_PXLSHADER_H_

#include "revCore/resourceManager/resource.h"
#include "revCore/string.h"
#include <rtl/vector.h>

namespace rev { namespace video
{
	// Forward declarations
	class CShader;

	class CPxlShader: public TResource<CPxlShader, string>
	{
	public:
		CPxlShader(const string& _name);
		~CPxlShader();
		int		id() const	{	return mId;	}
		rtl::vector<CShader*>& users () { return mUsers; }

	private:
		int mId;
		rtl::vector<CShader*>	mUsers;	///< Shaders that use this pixel shader

		static rtl::vector<CShader*> tempUserStorage;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SHADER_PXLSHADER_H_
