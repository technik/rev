////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 27th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// scripted material

#include "scriptedMaterial.h"

#include <revCore/file/file.h>
#include <revCore/variant/variant.h>
#include <revVideo/renderer/renderer3d.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/shader/pxlShader.h>
#include <revVideo/videoDriver/videoDriver.h>
#include <revScript/script.h>

using namespace rev::script;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CScriptedMaterial::CScriptedMaterial(const char * _scriptFileName)
		:mUseAlpha(false)
	{
		// Get parameters from script
		CVariant params;
		CFile scriptFile(_scriptFileName);
		CScript	script(scriptFile.textBuffer());
		script.run();
		CScript::getVar("shader", params);
		mShader = CPxlShader::get(params[0].asString());
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptedMaterial::setEnvironment() const
	{
		// ViewPos
		IVideoDriver * driver = SVideo::getDriver();
		IRenderer3d * renderer = SVideo::get()->renderer();
		int viewDirUniformId = driver->getUniformId("viewPos");
		driver->setUniform(viewDirUniformId, (renderer->viewMatrix() * CVec3::zero));
	}
}	// namespace video
}	// namespace rev