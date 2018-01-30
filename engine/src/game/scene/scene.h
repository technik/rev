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

#include "ComponentLoader.h"
#include <graphics/scene/renderScene.h>
#include "sceneNode.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace rev { namespace game {

	class Scene
	{
	public:
		bool load(const std::string& fileName); //, component loader? (factory)
		void save(const std::string& fileName); //, component serialzier? (reverse factory)

		// Views
		const graphics::RenderScene& renderable() const;

		// Accessors
		SceneNode*	root() const;
		SceneNode*	findNode(const std::string& name) const;

	private:
		graphics::RenderScene		mGraphics;
		std::unique_ptr<SceneNode>	mNodeTree;
	};

}}
