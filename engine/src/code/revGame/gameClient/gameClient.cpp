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
#include "revGame/gui/panel.h"
#include "revVideo/camera/orthoCamera.h"
#include "revVideo/material/basic/diffuseTextureMaterial.h"
#include "revVideo/material/materialInstance.h"
#include "revVideo/scene/model/staticModelInstance.h"
#include "revVideo/viewport/viewport.h"

// --- Active namespaces --------
using namespace rev;
using namespace rev::codeTools;
using namespace rev::input;
using namespace rev::video;

namespace rev { namespace game
{
	CNode * buggyNode;
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
		
		/*CPanel * panel1 = new CPanel("rocks.png", CVec3(0.f, 0.f, 1.0));
		panel1->setSize(CVec2(50.f, 100.f));
		CPanel * panel0 = new CPanel("buggy.png", CVec3::zero);
		panel0->setSize(CVec2(100.f, 50.f));*/
		// Create the buggy material
		CDiffuseTextureMaterial * buggyMaterial = new CDiffuseTextureMaterial("buggy.png");
		IMaterialInstance * buggyMaterialInstance = new IMaterialInstance(buggyMaterial);
		CStaticModelInstance * buggyModel = new CStaticModelInstance("buggy.rmd", buggyMaterialInstance);
		buggyNode = new CNode();
		buggyModel->attachTo(buggyNode);
		buggyNode->rotate(CVec3(1.f, 0.f, 0.f), -1.5708f);
		
		// Viewport and camera
		CViewport * v1 = new CViewport(CVec2(0.f, 0.f), CVec2(1.f, 1.0f), 0.f);
		COrthoCamera * cam1 = new COrthoCamera(CVec2(800.f, 480.f), -250.f, 250.f);
		//cam1->setScene(IGuiElement::guiScene());
		v1->setCamera(cam1);
	}

	//--------------------------------------------------------------------------
	bool CGameClient::update()
	{
		// Update time system
		STime::get()->update();
		// Update video system and render
		SVideo::get()->update();

		
		buggyNode->rotate(CVec3(0.f, 1.f, 0.f), 0.2f*STime::get()->frameTime());
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

