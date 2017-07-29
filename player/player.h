//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/platform/platformInfo.h>
#include <util/app/app3d.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/geometry/RenderGeom.h>

#include <vulkan/vulkan.h>

using namespace rev::video;

namespace rev {

	class Player : public rev::App3d {
	public:
		Player(const core::StartUpInfo& _platformInfo);
		~Player();

		video::ForwardRenderer mRenderer;

	private:
		void processArgs(const core::StartUpInfo& _info);
		bool frame(float _dt) override;

	private:
		float t = 0.f;
		video::RenderGeom*	mTriangle;

	};

}	// namespace rev