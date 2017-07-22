//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <util/app/app3d.h>
#include <iostream>
#include <game/sceneLoader.h>
#include <game/logicLayer.h>
#include <math/algebra/vector.h>
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

#include <vulkan/vulkan.h>

using namespace cjson;
using namespace rev::core;
using namespace rev::game;
using namespace rev::net;
using namespace rev::video;
using namespace rev::math;
using namespace std;

namespace rev {

	class Player : public rev::App3d {
	public:
		Player(const StartUpInfo& _platformInfo);

		~Player();

		std::string		mSceneName;

		// World & world layers
		core::World			mGameWorld;
		video::RenderLayer*	mRenderScene;
		game::LogicLayer*		mSceneLogic;

		// Common components
#ifdef OPENGL_45
		video::ForwardRenderer mRenderer;
#endif

	private:
		VkPipelineLayout pipelineLayout;

		//----------------------------------------------------------------
		void processArgs(const StartUpInfo& _info);

		//----------------------------------------------------------------
		bool frame(float _dt) override;
	};

}	// namespace rev