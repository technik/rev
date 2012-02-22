////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex shaders

#include "vtxShader.h"

#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev {
	//------------------------------------------------------------------------------------------------------------------
	// static data
	TResource<video::CVtxShader*, string>::managerT * TResource<video::CVtxShader*, string>::sManager = 0;
}	// namespace rev

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CVtxShader::CVtxShader(const string& _name)
	{
		mId = SVideo::get()->driver()->loadVtxShader(_name.c_str());
	}
}	// namespace video
}	// namespace rev
