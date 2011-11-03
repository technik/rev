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
#include "revVideo/scene/model/quad.h"
#include "revVideo/scene/model/staticModel.h"
#include "revVideo/scene/model/staticModelInstance.h"
#include "revVideo/texture/texture.h"
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


		CQuad * testQuad = new CQuad(CVec2(100.f, 100.f));
		CSolidColorMaterial * material = new CSolidColorMaterial(CColor::WHITE);
		IRenderableInstance * instance = new IRenderableInstance(testQuad, new IMaterialInstance(material));
		instance->attachTo(new CNode());

		CViewport * v1 = new CViewport(CVec2(0.f, 0.f), CVec2(1.f, 1.0f), 0.f);
		COrthoCamera * cam1 = new COrthoCamera(CVec2(100.f, 100.f), -100.f, 100.f);
		v1->setCamera(cam1);
	}

	//--------------------------------------------------------------------------
	bool CGameClient::update()
	{
		// Update time system
		STime::get()->update();
		// Update video system and render
		SVideo::get()->update();
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

