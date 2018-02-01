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
#include "scene.h"
#include "ComponentLoader.h"

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	bool Scene::load(std::istream& in, const ComponentLoader& loader)
	{
		mNodeTree = std::make_unique<SceneNode>();
		return parseNodeSubtree(*mNodeTree, in, loader);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Scene::save(std::ostream& out, const ComponentSerializer& saver) const
	{
		return serializeNodeSubtree(*mNodeTree, out, saver);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Scene::parseNodeSubtree(SceneNode& root, std::istream& in, const ComponentLoader& loader)
	{
		// Parse root components
		uint32_t nComponents = 0;
		read(in,nComponents);
		for(uint32_t i = 0; i < nComponents; ++i)
			root.addComponent(loader.loadComponent(in));
		// Parse children
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Scene::serializeNodeSubtree(const SceneNode& root, std::ostream& out, const ComponentSerializer& saver) const
	{
		// Serialize root components
		// Serialize children
		//for()
		return false;
	}

}}