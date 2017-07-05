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
			processArgs(_platformInfo);
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

			// Create extra objects
			StaticRenderMesh* mesh = StaticRenderMesh::loadFromFile("data/unitSphere.rmd");
			float roughness = 0.0f;
			for (int i = -3; i <= 3; ++i) {
				float metalness = 0.0f;
				for (int j = -3; j <= 3; ++j) {
					SceneNode* node = mGameWorld.createNode("procBall");
					RenderObj* obj = new RenderObj(mesh);
					mRenderScene->addRenderObj(obj);
					Effect* fx = new Effect;
					fx->loadFromFile("data/pbr.effect");
					obj->material = new Material;
					obj->material->mEffect = fx;
					obj->material->mFloats.push_back(make_pair("roughness", roughness));
					obj->material->mFloats.push_back(make_pair("metalness", metalness));
					obj->material->mVec3s.push_back(make_pair("albedo", Vec3f(1.f, 0.0f, 0.0f)));
					metalness += 0.11f;
					node->addComponent(obj);
					AffineTransform* m = new AffineTransform;
					m->matrix() = Mat34f::identity();
					m->setPosition(Vec3f(i*1.f, 3.f, j*1.f));
					node->addComponent(m);
				}
				roughness += 0.11f;
			}
		}

		~Player() {
		}

		std::string		mSceneName;

		// World & world layers
		core::World			mGameWorld;
		video::RenderLayer*	mRenderScene;
		game::LogicLayer*		mSceneLogic;

		// Common components
		video::ForwardRenderer mRenderer;

	private:

		//----------------------------------------------------------------
		void processArgs(const PlatformInfo& _info);

		//----------------------------------------------------------------
		bool frame(float _dt) override;
	};

}	// namespace rev