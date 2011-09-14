////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// video driver

#include "videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriver::getShader(const char * _vtxName, const char * _pxlName)
	{
		return loadShader(_vtxName, _pxlName);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriver::releaseShader(int _shader)
	{
		deleteShader(_shader);
	}

}	// namespace video
}	// namespace rev
