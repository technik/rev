#include "buggy.h"

#include <revCore/node/node.h>

#include <revVideo/material/materialInstance.h>
#include <revVideo/material/basic/diffuseTextureMaterial.h>

#include <revVideo/scene/model/staticModelInstance.h>

using namespace rev;

using namespace rev::video;

namespace buggyDemo
{
	CBuggy::CBuggy()
	{
		mNode = new CNode();
		mMaterialInstance = new IMaterialInstance( new CDiffuseTextureMaterial("buggy.png") );
		mModelInstance = new CStaticModelInstance("buggy.rmd", mMaterialInstance);
		mModelInstance->attachTo(mNode);
	}
	
	CBuggy::~CBuggy()
	{
		delete mModelInstance;
		delete mMaterialInstance;
		delete mNode;
	}

	void CBuggy::update()
	{

	}

	const CNode * CBuggy::node() const
	{
		return mNode;
	}
}