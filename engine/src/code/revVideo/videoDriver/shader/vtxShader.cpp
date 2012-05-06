////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex shaders

#include "vtxShader.h"

#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/shader.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev {
	//------------------------------------------------------------------------------------------------------------------
	// static data
	video::CVtxShader::TManager	* video::CVtxShader::sManager = 0;

}	// namespace rev

namespace rev { namespace video
{
	// Static data
	rtl::vector<CShader*> CVtxShader::tempUserStorage;

	//------------------------------------------------------------------------------------------------------------------
	CVtxShader::CVtxShader(const char* _name)
	{
		SVideo * videoSystem = SVideo::get();
		if(videoSystem)
			mId = videoSystem->driver()->loadVtxShader(_name);
		if( 0 != tempUserStorage.size())
		{
			mUsers = tempUserStorage;
			for(rtl::vector<CShader*>::iterator i = tempUserStorage.begin(); i != tempUserStorage.end(); ++i)
			{
				(*i)->refresh();
			}
			tempUserStorage.clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CVtxShader::~CVtxShader()
	{
		SVideo::get()->driver()->releaseShader(mId);
		tempUserStorage = mUsers;
	}
}	// namespace video
}	// namespace rev
