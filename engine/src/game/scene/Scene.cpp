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
#include "sceneNode.h"

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	Scene::Scene()
		: mNodeTree(std::make_unique<SceneNode>())
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Scene::load(std::istream& in, const ComponentLoader& loader)
	{
		return parseNodeSubtree(*mNodeTree, in, loader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Scene::save(std::ostream& out, const ComponentSerializer& saver) const
	{
		// Save node tree
		serializeNodeSubtree(*mNodeTree, out, saver);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Scene::parseNodeSubtree(SceneNode& root, std::istream& in, const ComponentLoader& loader)
	{
		if(!parseComponents(root,in,loader))
			return false;
		if(!parseChildren(root,in,loader))
			return false;
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Scene::serializeNodeSubtree(const SceneNode& root, std::ostream& out, const ComponentSerializer& saver)
	{
		saveComponents(root,out,saver);
		saveChildren(root,out,saver);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Scene::parseComponents(SceneNode& root, std::istream& in, const ComponentLoader& loader)
	{
		// Parse root components
		uint32_t nComponents = 0;
		read(in,nComponents);
		for(uint32_t i = 0; i < nComponents; ++i)
		{
			auto c = loader.loadComponent(in);
			if(!c)
				return false;
			root.addComponent(std::move(c));
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Scene::parseChildren(SceneNode& parent, std::istream& in, const ComponentLoader& loader)
	{
		uint32_t nChildren = 0;
		read(in,nChildren);
		for(uint32_t i = 0; i < nChildren; ++i)
		{
			auto child = std::make_shared<SceneNode>();
			if(!parseNodeSubtree(*child,in,loader))
				return false;
			parent.addChild(child);
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Scene::saveComponents(const SceneNode& node, std::ostream& out, const ComponentSerializer& saver)
	{
		uint32_t nComponents = node.components().size();
		write(out,nComponents);
		for(auto& c : node.components())
			saver.save(*c,out);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Scene::saveChildren(const SceneNode& node, std::ostream& out, const ComponentSerializer& saver)
	{
		uint32_t nChildren = node.children().size();
		write(out,nChildren);
		for(auto& c : node.children())
			serializeNodeSubtree(*c,out,saver);
	}

}}
