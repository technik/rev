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
	}

	namespace gfx
	{
		class CommandList;
		class CommandPool;
		class Device;
		class DoubleBufferSwapChain;
		class Fence;
		class GpuBuffer;
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

		// Common components
		bool update();
		void updateUI(float dt);

	private:

		bool initGraphicsDevice();
		void createSwapChain();

		void loadScene(const std::string& scene);
		void createCamera();
		void createFloor();
		void updateSceneBBox();

		// Player state
		math::Vec2u m_windowSize;
		int m_backBufferIndex = 0;
		uint64_t m_frameFenceValues[2] = {};

		// Render resources
		gfx::Device* m_gfxDevice;
		gfx::CommandPool* m_copyCommandPool;
		gfx::DoubleBufferSwapChain* m_swapChain;
		gfx::GpuBuffer* m_backBuffers[2];
		gfx::Fence* m_frameFence;
		gfx::CommandPool* m_frameCmdPools[2];
		gfx::CommandList* m_frameCmdList;

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