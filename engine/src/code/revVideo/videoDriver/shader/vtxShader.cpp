////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex shaders

#include "vtxShader.h"

#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// static data
	CVtxShader::managerT * CVtxShader::sManager = 0;

	//------------------------------------------------------------------------------------------------------------------
	CVtxShader::managerT * CVtxShader::manager()
	{
		if( 0 == sManager)
			sManager = new managerT();
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVtxShader::CVtxShader(const string& _name)
	{
		mId = SVideo::get()->driver()->loadVtxShader(_name.c_str());
	}
}	// namespace video
}	// namespace rev
