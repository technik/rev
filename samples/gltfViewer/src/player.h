//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <math/numericTraits.h>
#include <math/algebra/vector.h>

#include <string>

namespace rev {

	namespace game {
		class FlyBy;
		class Orbit;
		class SceneNode;
	}

	namespace gfx
	{
		class Camera;
		class CommandList;
		class CommandPool;
		class Device;
		class DoubleBufferSwapChain;
		class Fence;
		class GpuBuffer;
		class RasterPipeline;
		class RenderGeom;
		class RenderTargetView;
		class RootSignature;
	}

	class Player {
	public:

		struct CmdLineOptions
		{
			rev::math::Vec2u windowSize = rev::math::Vec2u(640, 480);
			float vFov = math::radians(45.f);
			std::string scene;

			CmdLineOptions(int _argc, const char** _argv);
		};

		Player()
		{}

		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		bool init(const CmdLineOptions& options);

#ifdef _WIN32
		void onWindowResize(const math::Vec2u& _newSize);
#endif // _WIN32

		bool update();

	private:

		bool initGraphicsDevice();
		void createSwapChain();

		void loadScene(const std::string& scene);
		void initRaytracing();
		void createCamera();

		// Player state
		math::Vec2u m_windowSize;
		int m_backBufferIndex = 0;
		uint64_t m_frameFenceValues[2] = {};
		game::SceneNode* m_camNode;
		gfx::Camera* m_renderCam;

		// Render resources
		gfx::Device* m_gfxDevice;
		gfx::CommandPool* m_copyCommandPool;
		gfx::DoubleBufferSwapChain* m_swapChain;
		gfx::GpuBuffer* m_backBuffers[2];
        gfx::GpuBuffer* m_depthBuffer = nullptr;
        gfx::GpuBuffer* m_gBuffer = nullptr;
        gfx::RenderTargetView* m_gBV = nullptr;
        gfx::RenderTargetView* m_depthBV = nullptr;
		gfx::Fence* m_frameFence;
		gfx::CommandPool* m_frameCmdPools[2];
		gfx::CommandList* m_frameCmdList;
		gfx::RootSignature* m_rasterSignature;
		gfx::RasterPipeline* m_gBufferShader;

		gfx::GpuBuffer* m_sceneGpuBuffer;
		gfx::RenderGeom* m_geom;
	private:

		enum class RenderPath
		{
			Forward,
			Deferred,
			Raytracing
		} m_renderPath = RenderPath::Deferred;

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
	};

}	// namespace rev