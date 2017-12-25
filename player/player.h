//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <util/app/app3d.h>
#include <iostream>
#include <game/sceneLoader.h>
#include <game/logicLayer.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/material.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/staticRenderMesh.h>
#include <video/graphics/renderer/renderLayer.h>
#include <cjson/json.h>
#include <core/world/world.h>
#include <core/components/affineTransform.h>
#include <network/http/httpServer.h>
#include <network/http/httpResponse.h>
#include <game/scene/transform/flybySrc.h>
#include <map>

using namespace cjson;
using namespace rev::core;
using namespace rev::game;
using namespace rev::net;
using namespace rev::video;
using namespace std;

namespace rev {

	class Player : public rev::App3d {
	public:
		Player(const PlatformInfo& _platformInfo)
			: rev::App3d(_platformInfo)
		{
			mRenderer.init();
		}

		~Player() {
		}

		// Common components
		video::ForwardRenderer mRenderer;

	private:
		bool frame(float _dt) override;
	};

}	// namespace rev