////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 27th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// scripted material

#include "scriptedMaterial.h"

#include <revCore/file/file.h>
#include <revCore/variant/variant.h>
#include <revVideo/videoDriver/shader/pxlShader.h>
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
		script.run(params);
		mShader = CPxlShader::get(params[0].asString());
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptedMaterial::setEnvironment() const
	{
		// Intentionally blank
	}
}	// namespace video
}	// namespace rev