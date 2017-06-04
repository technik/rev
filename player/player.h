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

namespace rev {

	class Player : public rev::App3d {
	public:
		Player(int _argc, const char** _argv)
			: App3d(_argc, _argv)
		{
			processArgs(_argc, _argv);
			mRenderer.init(&driver3d());
			SceneLoader mLoader;

			// Prepare factories and game world
			mSceneLogic = new LogicLayer;
			mGameWorld.addLayer(mSceneLogic);
			mLoader.registerFactory("Flyby", [=](const Json& _j) {
				LogicComponent* c = new FlyBySrc(1.f);
				mSceneLogic->add(c);
				return c;
			});
			mRenderScene = new RenderLayer(mRenderer);
			mGameWorld.addLayer(mRenderScene);
			mLoader.registerFactory("RenderObj", [=](const Json& _j) { return mRenderScene->createRenderObj(_j); });
			mLoader.registerFactory("Camera", [=](const Json& _j) { return mRenderScene->createCamera(_j); });
			mLoader.registerFactory("Transform", [](const Json& _j) { return AffineTransform::construct(_j); });

			// Expose API
			jsonAPI()->setResponder("/graph", [&](http::Server* _srv, unsigned _conId, const http::Request&) {
				Json graphData;
				mGameWorld.getGraphData(graphData);
				_srv->respond(_conId, http::Response::jsonResponse(graphData));
			});

			// Load scene
			mGameWorld.init();
			mLoader.loadScene(mSceneName, mGameWorld);
		}

		~Player() {
		}

		std::string		mSceneName;

		// World & world layers
		World			mGameWorld;
		RenderLayer*	mRenderScene;
		LogicLayer*		mSceneLogic;

		// Common components
		ForwardRenderer mRenderer;

	private:

		//----------------------------------------------------------------
		void processArgs(int _argc, const char** _argv);

		//----------------------------------------------------------------
		bool frame(float _dt) override;
	};

}	// namespace rev