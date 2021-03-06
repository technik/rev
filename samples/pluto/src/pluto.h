//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
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

#include <game/scene/sceneNode.h>
#include <game/application/base3dApplication.h>
#include <gfx/scene/camera.h>
#include <gfx/renderer/RasterScene.h>
#include <gfx/renderer/deferred/DeferredRenderer.h>

namespace rev {

	namespace game {
		class FlyBy;
		class Orbit;
	}

	class Pluto : public game::Base3dApplication
	{
	public:
		virtual std::string name() { return "gltf viewer"; }

		Pluto() = default;
		Pluto(const Pluto&) = delete;
		Pluto& operator=(const Pluto&) = delete;

	private:

		struct CommandLineOptions
		{
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
		void createCamera();

		// Scene
		std::shared_ptr<game::SceneNode> m_sceneRoot;
		gfx::DeferredRenderer::SceneDesc m_sceneGraphics;
		std::shared_ptr<gfx::RasterScene> m_opaqueGeometry;
		float m_bgColor{};

		// Camera options
		gfx::Camera*						mFlybyCam = nullptr;
		game::FlyBy*						m_flyby;

		gfx::DeferredRenderer m_renderer;
		size_t m_geometryStreamToken{};
	};

}	// namespace rev