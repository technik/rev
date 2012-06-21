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
	bool createBoxModel(CVariant& _args, CVariant&) // (name, size)
	{
		if(_args.size() != 2)
		{
			revLogN("Error: createBoxModel requires two parameters:\n\t-A name for the model\n\t-B vec3 size", eError);
		}
		else
		{
			CStaticModel::registerResource(CMeshGenerator::box(_args[1].asVec3()), _args[0].asString());
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool createSphereModel(CVariant& _args, CVariant&) // (name, radius, meridians, parallels)
	{
		if(_args.size() != 4)
		{
			revLogN("Error: createSphereModel requires four parameters:\n\t-A name for the model\n\t-B Model radius\n\t-C number of meridians\n\t-D number of parallels"
				, eError);
		}
		else
		{
			CStaticModel::registerResource(CMeshGenerator::geoSphere(_args[1].asFloat(), _args[2].asInt(), _args[3].asInt()), _args[0].asString());
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool createObject(CVariant& _args, CVariant& _ret) // (model, material, position);
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
		sm->addFunction(&createSphereModel, "createSphereModel");
		sm->addFunction(&createObject, "createObject");
	}

}	// namespace game
}	// namespace rev
