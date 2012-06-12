////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 10th, 2012
////////////////////////////////////////////////////////////////////////////////
// RevGame script interface

#include "revGameScriptInterface.h"

#include <revCore/codeTools/log/log.h>
#include <revGame/scene/mesh/meshGenerator.h>
#include <revGame/scene/object/staticObject.h>
#include <revVideo/scene/model/staticModel.h>
#include <revScript/scriptMachine.h>

using namespace rev::script;
using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	bool createBoxModel(CVariant& _args, CVariant&)
	{
		if(_args.size() != 2)
		{
			revLogN("Error: createBoxModel requires two parameters:\n\t-A name for the model\n\t-A vec3 size", eError);
		}
		else
		{
			CStaticModel::registerResource(CMeshGenerator::box(_args[1].asVec3()), _args[0].asString());
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool createObject(CVariant& _args, CVariant& _ret)
	{
		CStaticObject * obj = new CStaticObject(_args[0].asString(), _args[1].asString(), _args[2].asVec3());
		_ret = obj;
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void registerScriptInterface()
	{
		CScriptMachine * sm = CScriptMachine::get();

		sm->addFunction(&createBoxModel, "createBoxModel");
		sm->addFunction(&createObject, "createObject");
	}

}	// namespace game
}	// namespace rev
