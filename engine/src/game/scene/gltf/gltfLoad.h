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

#include <string>
#include "../sceneNode.h"
#include <graphics/driver/geometryPool.h>
#include <graphics/scene/animation/animation.h>
#include <graphics/scene/renderScene.h>
#include <memory>
#include <vector>

namespace rev::gfx { class Device; }

namespace rev::game {

	/// Load a gltf scene
	/// Add renderable content to _gfxWorld
	/// filePath must not contain folder, file name and extension
	/// If parentNode is not nullptr, all the scene nodes will be added as children to it
	void loadGLTFScene(
		gfx::Device& gfxDevice,
		SceneNode& parentNode,
		const std::string& filePath,
		gfx::RenderScene& _gfxWorld,
		gfx::GeometryPool& _pool,
		std::vector<std::shared_ptr<SceneNode>>& animNodes,
		std::vector<std::shared_ptr<gfx::Animation>>& _animations);
}
