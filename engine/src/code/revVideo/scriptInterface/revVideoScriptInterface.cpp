////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 10th, 2012
////////////////////////////////////////////////////////////////////////////////
// RevVideo script interface

#include "revVideoScriptInterface.h"

#include <revScript/scriptMachine.h>
#include <revVideo/material/basic/diffuseTextureMaterial.h>
#include <revVideo/material/basic/solidColorMaterial.h>
#include <revVideo/material/material.h>

using namespace rev::script;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	bool createDiffuseColorMaterial(CVariant& _args, CVariant&) // (name, color)
	{
		CColor color(CColor::WHITE);
		CVariant& colorVar = _args[1];
		color.r() = float(colorVar[0].asDouble());
		color.g() = float(colorVar[1].asDouble());
		color.b() = float(colorVar[2].asDouble());
		if(colorVar.size() == 4) // Alpha
		{
			color.a() = float(colorVar[3].asDouble());
		}
		IMaterial::registerResource(new CSolidColorMaterial(color), _args[0].asString());
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool createDiffuseTextureMaterial(CVariant& _args, CVariant&) // (material name, texture name)
	{
		IMaterial::registerResource(new CDiffuseTextureMaterial(_args[1].asString()), _args[0].asString());
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	void registerScriptInterface()
	{
		CScriptMachine * sm = CScriptMachine::get();
		
		sm->addFunction(&createDiffuseColorMaterial, "createDiffuseColorMaterial");
		sm->addFunction(&createDiffuseTextureMaterial, "createDiffuseTextureMaterial");
	}

}	// namespace video
}	// namespace rev
