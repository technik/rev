////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel shaders

#include "pxlShader.h"

#include <revCore/delegate/delegate.h>
#include <revCore/file/fileWatcher.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/shader/shader.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev {
	//------------------------------------------------------------------------------------------------------------------
	// static data
	video::CPxlShader::TManager * video::CPxlShader::sManager = 0;
}	// namespace rev

namespace rev { namespace video
{
	// Static data
	rtl::vector<CShader*> CPxlShader::tempUserStorage;

	//------------------------------------------------------------------------------------------------------------------
	CPxlShader::CPxlShader(const char* _name)
		:CRecreationFileBinding(_name)
		,mName(_name)
		,mId(-1)
	{
		// -- Delegates
		mPreShaderDelegate = new CObjectDelegate<CPxlShader,const char*>(this,&CPxlShader::load);
		mPostShaderDelegate = new CObjectDelegate<CPxlShader,const char*>(this,&CPxlShader::load);
		SFileWatcher::get()->addWatcher("preShader.pxl", mPreShaderDelegate);
		SFileWatcher::get()->addWatcher("postShader.pxl", mPostShaderDelegate);
		// -- Shader itself
		mUsers = tempUserStorage;
		load(_name);
	}

	//------------------------------------------------------------------------------------------------------------------
	CPxlShader::~CPxlShader()
	{
		SVideo::get()->driver()->releaseShader(mId);
		SFileWatcher::get()->removeWatcher(mPreShaderDelegate);
		SFileWatcher::get()->removeWatcher(mPostShaderDelegate);
		tempUserStorage = mUsers;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPxlShader::load(const char * _filename)
	{
		// Notice we don't load from the passed name but from our stored name
		_filename;
		// Destroy old shader, if any
		if(-1 != mId)
			SVideo::get()->driver()->releaseShader(mId);
		// Load the shader
		mId = SVideo::get()->driver()->loadPxlShader(mName.c_str());
		if(-1 != mId)
		{
			for(rtl::vector<CShader*>::iterator i = mUsers.begin(); i != mUsers.end(); ++i)
			{
				(*i)->refresh();
			}
		}
	}
}	// namespace video
}	// namespace rev
