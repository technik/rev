////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on February 19th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static object

#include "staticObject.h"

#include <revCore/node/node.h>
#include <revGame/gameClient/gameClient.h>
#include <revVideo/scene/model/staticModelInstance.h>

using namespace rev::video;

namespace rev { namespace game
{
	//---------------------------------------------------------------------------------------------------------------
	CStaticObject::CStaticObject(const char * _model, const char * _material, const CVec3& _pos)
	{
		mNode = new CNode();
		mNode->setPos(_pos);
		mModel = new CStaticModelInstance(_model, _material);
		mModel->setScene(SGameClient::get()->scene3d());
		mNode->addComponent(mModel);
	}
}	// namespace game
}	// namespace rev