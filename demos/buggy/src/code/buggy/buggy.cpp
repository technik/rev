#include "buggy.h"

#include <revGame/scene/object/staticObject.h>
#include <revGame/scene/mesh/meshGenerator.h>
#include <revVideo/material/basic/solidColorMaterial.h>
#include <revVideo/scene/model/staticModel.h>

using namespace rev;
using namespace rev::game;
using namespace rev::video;

namespace buggyDemo
{
	CBuggy::CBuggy()
	{
		// Register mesh
		CStaticModel::registerResource(CMeshGenerator::box(CVec3(1.0)), "buggy.rmd");
		// Register material
		IMaterial::registerResource(new CSolidColorMaterial(CColor::RED), "buggyMaterial");
		// Create the object
		mObject = new CStaticObject("buggy.rmd", "buggyMaterial");
	}
	
	CBuggy::~CBuggy()
	{
		
		// delete mObject;
	}

	void CBuggy::update()
	{
		// Intentionally blank
	}

	CNode * CBuggy::node() const
	{
		return mObject->node();
	}
}