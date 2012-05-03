////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 27th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// scripted material

#include "scriptedMaterial.h"

#include <revCore/variant/variant.h>
#include <revScript/script.h>

using namespace rev::script;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CScriptedMaterial::CScriptedMaterial(const char * _scriptFileName)
	{
		// Get parameters from script
		CVariant params;
		CScript	script(_scriptFileName);
		script.run(params);
	}
}	// namespace video
}	// namespace rev