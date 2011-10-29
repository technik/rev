////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel shaders

#ifndef _REV_REVVIDEO_SHADER_PXLSHADER_H_
#define _REV_REVVIDEO_SHADER_PXLSHADER_H_

#include "revCore/resourceManager/resource.h"
#include "revCore/resourceManager/resourceManager.h"
#include "revCore/types.h"

namespace rev { namespace video
{
	class CPxlShader: public TResource<CPxlShader, string>
	{
	public:
		typedef TResourceManager<CPxlShader, string> managerT;

	public:
		static managerT * manager();
		CPxlShader(const string& _name);
		int		id() const	{	return mId;	}

	private:
		static managerT* sManager;
		int mId;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SHADER_PXLSHADER_H_
