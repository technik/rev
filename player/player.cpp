//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include "player.h"
#include <core/platform/platformInfo.h>

using namespace cjson;

using namespace rev::core;
using namespace rev::game;
#ifndef ANDROID
using namespace rev::input;
#endif
using namespace rev::math;
using namespace rev::net;
using namespace rev::video;

using namespace std;

namespace rev {

	Player::Player(const PlatformInfo& _platformInfo)
		: rev::App3d(_platformInfo)
	{
		processArgs(_platformInfo);
		mRenderer.init(&driver3d());
		SceneLoader mLoader;

		// Prepare factories and game world
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
		mCamTransform = mRenderScene->cameraByNdx(0)->node()->component<AffineTransform>();

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
				obj->material->mVec3s.push_back(make_pair("albedo", Vec3f(0.f, 0.0f, 1.0f)));
				metalness += 0.11f;
				node->addComponent(obj);
				AffineTransform* m = new AffineTransform;
				m->matrix() = Mat34f::identity();
				m->setPosition(Vec3f(i*1.f, 5.f, j*1.f));
				node->addComponent(m);
			}
			roughness += 0.11f;
		}
	}

	//----------------------------------------------------------------
	void Player::processArgs(const PlatformInfo& _info) {
#ifdef ANDROID
		mSceneName = "vrScene.scn";
#else
		if (_info.argC > 2) {
			mSceneName = _info.argV[1];
			string datasetName = _info.argV[2];
			mSlamTracker = new HeadTracker(datasetName);
		}
#endif
	}

	//----------------------------------------------------------------
	bool Player::frame(float _dt) {
		mAccumTime += _dt; // Fixed time step
		while (mAccumTime >= mFixedDt) {
			mAccumTime -= mFixedDt;
			if (!mSlamTracker->update(mFixedDt))
				return false;
			mCamTransform->matrix() = mSlamTracker->headTransform();
			if (!mGameWorld.update(mFixedDt))
				return false;
		}
		return true;
	}

}	// namespace rev