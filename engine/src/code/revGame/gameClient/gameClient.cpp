////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// basic game client

#include "gameClient.h"

// --- Engine headers -----------
#include "revCore/codeTools/log/log.h"
#include "revCore/time/time.h"
#include "revInput/touchInput/touchInput.h"
#include "revVideo/video.h"


// TODO: Remove?
#include "revVideo/camera/orthoCamera.h"
#include "revVideo/color/color.h"
#include "revVideo/material/basic/solidColorMaterial.h"
#include "revVideo/material/materialInstance.h"
#include "revVideo/scene/model/staticModel.h"
#include "revVideo/scene/model/staticModelInstance.h"
#include "revVideo/videoDriver/videoDriver.h"
#include "revVideo/viewport/viewport.h"
#include "revCore/node/node.h"

// --- Active namespaces --------
using namespace rev;
using namespace rev::codeTools;
using namespace rev::input;
using namespace rev::video;

namespace rev { namespace game
{
	CNode * buggy;
	//--------------------------------------------------------------------------
	void CGameClient::init()
	{
		// Init log system
		SLog::init();
		SLog::log("CGameClient::init()");
		// Init time system
		STime::init();
		// Init video system
		SVideo::init();
		// Init audio system
		// Init input system
		STouchInput::init();




		
		CSolidColorMaterial * buggyMaterial = new CSolidColorMaterial(CColor(1.f,1.f,1.f,0.8f));
		CViewport * v1 = new CViewport(CVec2(0.f, 0.f), CVec2(1.f, 1.0f), 0.f);
		COrthoCamera * cam1 = new COrthoCamera(CVec2(900.f, 600.f), -800.f, 800.f);
		v1->setCamera(cam1);
		IRenderable * buggyModelInstance = new CStaticModelInstance(new CStaticModel("buggy.rmd"),
						new IMaterialInstance(buggyMaterial));
		buggy = new CNode;
		
		buggyModelInstance->attachTo(buggy);
		buggy->rotate(CVec3(1.f,0.f,0.f), -1.57f);
	}

	//--------------------------------------------------------------------------
	bool CGameClient::update()
	{
		// Update time system
		STime::get()->update();
		// Update video system and render
		SVideo::get()->update();

		TReal time = buggy->deltaTime();
		buggy->rotate(CVec3(0.f, 1.f, 0.f), time);
		return true;
	}

	//--------------------------------------------------------------------------
	void CGameClient::end()
	{
		// End input system
		// End audio system
		// End video system
		SVideo::end();
		// End time system
		STime::end();
		// End log system
		SLog::log("CGameClient::end()");
		SLog::end();
	}
} // namespace game
} // namespace rev

