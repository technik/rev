//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
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
#include <vector>
#include <math/algebra/vector.h>
#include <memory>
#include <graphics/scene/camera.h>
#include <graphics/scene/Light.h>
#include <graphics/scene/EnvironmentProbe.h>

namespace rev { namespace graphics {

	class Material;
	class RenderGeom;
	class RenderObj;

	class RenderScene
	{
	public:
		RenderScene()
		{
		}

		// Renderables
		std::vector<std::shared_ptr<RenderObj>>& renderables()				{ return m_renderables; }
		const std::vector<std::shared_ptr<RenderObj>>& renderables() const	{ return m_renderables; }

		// Cameras
		void addCamera(const std::weak_ptr<Camera>& cam) { m_cameras.push_back(cam); }
		const auto& cameras() const { return m_cameras; }

		// Lights
		void addLight(const std::shared_ptr<Light>& light) { m_lights.push_back(light); }
		const auto& lights() const { return m_lights;}

		void setEnvironment(const std::shared_ptr<const EnvironmentProbe>& probe) { m_environment = probe; }
		auto& environment() const { return m_environment; }

	private:
		std::vector<std::shared_ptr<RenderObj>>		m_renderables;
		std::vector<std::shared_ptr<Light>>			m_lights;
		std::vector<std::weak_ptr<Camera>>			m_cameras;

		// Environment probe
		std::shared_ptr<const EnvironmentProbe>			m_environment;
	};

}}	// namespace rev::graphics