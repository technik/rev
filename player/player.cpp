//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include "player.h"
#include <core/platform/platformInfo.h>
#include <video/window/window.h>

using namespace cjson;

using namespace rev::core;
#ifndef ANDROID
using namespace rev::input;
#endif
using namespace rev::math;
using namespace rev::net;
using namespace rev::video;

using namespace std;

namespace rev {

	Player::Player(const StartUpInfo& _platformInfo)
		: rev::App3d(_platformInfo)
	{
		processArgs(_platformInfo);
#ifdef OPENGL_45
		mRenderer.init(&driver3d());
#endif
#ifdef REV_USE_VULKAN
		mRenderer.init(window().frameBuffer()); // Configure renderer to render into the frame buffer

		VkDevice device = driver3d().device();

		// Static data for a triangle
		const vector<Vec3f> rawVertexBuffer = {
			Vec3f(0.0f, 0.0f, 1.f), -Vec3f::yAxis(), // Vec3f(141.f/255.0f, 252.0f/255.0f, 247.0f/255.0f),
			Vec3f(1.f, 0.0f, -1.f), -Vec3f::yAxis(),//Vec3f(141.f/255.0f, 252.0f/255.0f, 247.0f/255.0f),
			Vec3f(-1.f, 0.0f, -1.f), -Vec3f::yAxis()// Vec3f(255.f/255.0f, 51.0f/255.0f, 153.0f/255.0f)
		};

		// Index buffer
		const std::vector<uint16_t> indices = {
			0, 1, 2
		};

		VertexFormat mVertexFormat;
		mVertexFormat.hasPosition = true;
		mVertexFormat.normalFmt = VertexFormat::UnitVecFormat::e3Vec3f;
		mVertexFormat.normalSpace = VertexFormat::NormalSpace::eModel;
		mVertexFormat.nUVs = 0;

		mTriangle = new RenderGeom(mVertexFormat, 3, rawVertexBuffer.data(), 3, indices.data());
		mTriangle->sendBuffersToGPU();
#endif
	}

	//----------------------------------------------------------------
	Player::~Player() {
		VkDevice device = driver3d().device();
	}

	//----------------------------------------------------------------
	void Player::processArgs(const StartUpInfo& _info) {
#ifdef ANDROID
		//mSceneName = "vrScene.scn";
#else
		if (_info.argC > 1) {
//			mSceneName = _info.argV[1];
		}
#endif
	}

	//----------------------------------------------------------------
	bool Player::frame(float _dt) {
		t += _dt;
		mRenderer.beginFrame();
		math::Mat44f worldMtx = math::Mat44f::identity();
		//worldMtx[0][3] = (float)sin(t);
		worldMtx[1][3] = 1.f+(float)sin(t);
		math::Mat44f projMtx = GraphicsDriver::projectionMtx(90.f*3.14f/180.f, 4.f/3.f,0.1f,10.f);
		math::Mat44f wvp = (projMtx*worldMtx);
		mRenderer.render(*mTriangle, wvp);
		mRenderer.endFrame();

		return true;
		//return mGameWorld.update(_dt);
	}

}	// namespace rev