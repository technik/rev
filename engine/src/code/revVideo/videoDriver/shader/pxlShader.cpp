////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel shaders

#include "pxlShader.h"

#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// static data
	CPxlShader::managerT * CPxlShader::sManager = 0;

	//------------------------------------------------------------------------------------------------------------------
	CPxlShader::managerT * CPxlShader::manager()
	{
		if( 0 == sManager)
			sManager = new managerT();
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	CPxlShader::CPxlShader(const string& _name)
	{
		mId = SVideo::get()->driver()->loadPxlShader(_name.c_str());
	}
}	// namespace video
}	// namespace rev
