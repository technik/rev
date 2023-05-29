//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include <math/algebra/vector.h>
#include <gfx/backend/Vulkan/renderContextVulkan.h>
#include <chrono>
#include <memory>

namespace rev::core {
	class CmdLineParser;
}

namespace rev::gfx {
	class Device;
}

namespace rev::game {

	class Base3dApplication
	{
	protected:
		Base3dApplication() = default;
		using TimeDelta = std::chrono::duration<float>;

		struct CommandLineOptions
		{
			math::Vec2i windowPosition { 100, 150 };
			math::Vec2u windowSize { 640, 480 };
			bool fullScreen{ false };

			void registerOptions(core::CmdLineParser&);
		};

		const CommandLineOptions& initOptions() const { m_options; }

	public:
		virtual ~Base3dApplication();
		virtual std::string name() { return "rev application"; }

		void run(int argc, const char** argv);

	protected:
		const math::Vec2u& windowSize() const;

	private: // Extension interface
		// Life cycle
		virtual void getCommandLineOptions(core::CmdLineParser&) {}
		virtual bool init() { return true; }
		virtual void end() {}
		// Main loop
		virtual bool updateLogic(TimeDelta logicDt) = 0;
		virtual void render(TimeDelta renderDt) = 0;
		// Events
		virtual void onResize() {}

	private:
		void initEngineCore();
		bool initGraphics();

	private:
		std::chrono::high_resolution_clock m_appTime;
		CommandLineOptions m_options;
		gfx::RenderContextVulkan::ResizeDelegate m_resizeDelegate;
	};

}