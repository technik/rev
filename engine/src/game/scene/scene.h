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

#include <graphics/scene/renderScene.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace rev { namespace game {

	class ComponentLoader;
	class SceneNode;

	class Scene
	{
	public:
		Scene();

		bool load(std::istream& in, const ComponentLoader& loader);

		// Views
		const graphics::RenderScene&	renderable() const	{ return mGraphics; }
		graphics::RenderScene&			renderable()		{ return mGraphics; }

		// Accessors
		SceneNode*	root() const { return mNodeTree.get(); }
		SceneNode*	findNode(const std::string& name) const;

	private:

		static bool parseNodeSubtree		(SceneNode& root, std::istream& in, const ComponentLoader& loader);

		static bool parseComponents(SceneNode& node, std::istream& in, const ComponentLoader& loader);
		static bool parseChildren(SceneNode& parent, std::istream& in, const ComponentLoader& loader);

		template<class T>
		static void read(std::istream& in, T& dst) { in.read((char*)&dst, sizeof(T)); }

		template<class T>
		static void write(std::ostream& in, const T& dst) { in.write((const char*)&dst, sizeof(T)); }

		graphics::RenderScene		mGraphics;
		std::unique_ptr<SceneNode>	mNodeTree;
	};

}}
