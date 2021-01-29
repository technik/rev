//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <game/scene/sceneNode.h>
#include <game/scene/meshRenderer.h>
#include <game/application/base3dApplication.h>
#include <graphics/backend/rasterPipeline.h>
#include <graphics/backend/Vulkan/gpuBuffer.h>
#include <graphics/scene/camera.h>

namespace rev {

	namespace game {
		class FlyBy;
		class Orbit;
	}

	class Player : public game::Base3dApplication
	{
	public:
		virtual std::string name() { return "gltf viewer"; }

		Player() = default;
		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

	private:

		struct CommandLineOptions
		{
			std::string scene;
			std::string environment;
			float fov = 45.f;

			void registerOptions(core::CmdLineParser&);
		} m_options;

		// Extension interface
		// Life cycle
		void getCommandLineOptions(core::CmdLineParser&) override;
		bool init() override;
		void end() override;
		// Main loop
		bool updateLogic(TimeDelta logicDt) override;
		void render(TimeDelta renderDt) override;
		// Events
		void onResize() override;

		// Common components
		void updateUI(float dt);

	private:
		void loadScene(const std::string& scene);
		void createCamera();
		void createFloor();
		void updateSceneBBox();

		// Init ImGui
		void initImGui();

		// Vulkan objects to move into rev::gfx
		vk::Semaphore m_imageAvailableSemaphore;
		vk::RenderPass m_uiPass;
		vk::DescriptorPool m_descPool;
		std::vector<vk::DescriptorSet> m_frameDescs;
		uint32_t m_doubleBufferNdx = 0;
		vk::DescriptorSetLayout m_frameDescLayout;
		vk::PipelineLayout m_gbufferPipelineLayout;
		std::unique_ptr<gfx::RasterPipeline> m_gBufferPipeline;
		std::vector<std::shared_ptr<gfx::GPUBuffer>> m_mtxBuffers;

		// Scene
		std::shared_ptr<game::SceneNode> m_sceneRoot;
		math::AABB	m_globalBBox;
		std::shared_ptr<gfx::GPUBuffer>	m_vtxPosBuffer;
		std::shared_ptr<gfx::GPUBuffer>	m_vtxClrBuffer;
		std::shared_ptr<gfx::GPUBuffer>	m_indexBuffer;
		game::MeshRenderer m_sceneInstances;
		float m_bgColor{};

		// Camera options
		gfx::Camera*						mFlybyCam = nullptr;
		game::FlyBy*						m_flyby;

		struct CameraPushConstants
		{
			math::Mat44f proj;
			math::Mat44f view;
			uint32_t mtxNdx;
		} m_cameraPushC;

	private:

		struct ViewerState
		{
			bool floor = true;
		};

		struct BgOptions
		{
			bool shadows = true;
			float elevation = math::Constants<float>::halfPi*0.4;
			float rotation = 0.f;
			math::Vec3f shadowDir;
		} m_bgOptions;

		struct PostFX
		{
			bool bloom = false;
			float bloomThreshold = 1.f;
		};

		struct RenderOptions
		{
			float EV = 0.f;
			PostFX postOptions;
		};

		size_t m_sceneLoadStreamToken{};
	};

}	// namespace rev