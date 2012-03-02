////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel shaders

#include "pxlShader.h"

#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/shader.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev {
	//------------------------------------------------------------------------------------------------------------------
	// static data
	TResource<video::CPxlShader, string>::managerT * TResource<video::CPxlShader, string>::sManager = 0;
}	// namespace rev

namespace rev { namespace video
{
	// Static data
	rtl::vector<CShader*> CPxlShader::tempUserStorage;

	//------------------------------------------------------------------------------------------------------------------
	CPxlShader::CPxlShader(const string& _name)
	{
		mId = SVideo::get()->driver()->loadPxlShader(_name.c_str());
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
	CPxlShader::~CPxlShader()
	{
		SVideo::get()->driver()->releaseShader(mId);
		tempUserStorage = mUsers;
	}
}	// namespace video
}	// namespace rev
