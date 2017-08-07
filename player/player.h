//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/platform/platformInfo.h>
#include <game/scene/component.h>
#include <game/scene/sceneNode.h>
#include <util/app/app3d.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderScene.h>

#include <functional>
#include <map>
#include <vulkan/vulkan.h>

using namespace rev::video;

namespace rev {

	class Player : public rev::App3d {
	public:
		Player(const core::StartUpInfo& _platformInfo);
		~Player();

		video::ForwardRenderer mRenderer;

	private:
		typedef std::function<game::Component*(const cjson::Json&, game::SceneNode&)>	ComponentFactory;

		void processArgs(const core::StartUpInfo& _info);
		bool frame(float _dt) override;

		void createGlobalObjects();
		void registerFactory(const std::string& _type, ComponentFactory _f);
		void prepareFactories();
		void loadSceneFromFile(const std::string&);
		void initGameScene();

	private:
		std::map<std::string, ComponentFactory>	mFactories;

		float t = 0.f;
		std::vector<game::SceneNode*>	mRootGameObjects;
		video::RenderScene				mRenderScene;
	};

}	// namespace rev