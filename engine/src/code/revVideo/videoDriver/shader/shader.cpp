////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shaders

#include "shader.h"

#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/shader/vtxShader.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev {
//------------------------------------------------------------------------------------------------------------------
// Static data definitions
TResource<video::CShader, pair<video::CVtxShader*,video::CPxlShader*> >::managerT*
	TResource<video::CShader, pair<video::CVtxShader*,video::CPxlShader*> >::sManager = 0;
}	// namespace rev

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CShader::CShader(pair<CVtxShader*, CPxlShader*> _baseShaders)
		:mVtx(_baseShaders.first)
		,mPxl(_baseShaders.second)
	{
		mPxl->users().push_back(this);
		mId = SVideo::get()->driver()->linkShader(mVtx, mPxl);
	}

	//------------------------------------------------------------------------------------------------------------------
	CShader::~CShader()
	{
		rtl::vector<CShader*>& users = mPxl->users();
		rtl::vector<CShader*>::iterator i = users.begin();
		while(i != users.end())
		{
			if(*i == this)
			{
				users.erase(i);
				break;
			}
			++i;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CShader::setEnviroment() const
	{
		SVideo::get()->driver()->setShader(mId);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CShader::refresh()
	{
		SVideo::get()->driver()->destroyShader(mId);
		mId = SVideo::getDriver()->linkShader(mVtx, mPxl);
	}
}	// namespace video
}	// namespace rev
