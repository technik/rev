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
#include "component.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

namespace rev { namespace game {

	/// Nodes are containers of logically related components
	/// You can attach components to a node
	/// You can attach a node to another node
	class SceneNode
	{
	public:
		void init();
		void update(float _dt);

		SceneNode() = default;
		SceneNode(const std::string& _name)
			: name(_name)
		{}
		SceneNode(const SceneNode&) = delete;
		SceneNode& operator=(const SceneNode&) = delete;

		// Handle components
		auto	parent			() const { return mParent; }
		void	addChild		(std::shared_ptr<SceneNode> child);
		std::shared_ptr<SceneNode>	createChild(const std::string& name);
		auto&	children		() const { return mChildren; }
		void	addComponent	(std::unique_ptr<Component> _c);
		auto&	components		() const { return mComponents; }

		template<class T_>	
		T_*					component		() const {
			for (auto& c : mComponents) {
				auto rawPtr = c.get();
				if(typeid(*rawPtr) == typeid(T_))
					return static_cast<T_*>(rawPtr);
			}
			return nullptr;
		}

		// This traverse includes the node itself
		template<class Op>
		void traverseSubtree(const Op& op)
		{
			op(*this);
			for(auto& c : mChildren)
			{
				c->traverseSubtree(op);
			}
		}

		template<class T, class ... Args>
		T* addComponent(Args ... args)
		{
			addComponent(std::make_unique<T>(args...));
			return static_cast<T*>(mComponents.back().get());
		}

		std::string name;

	private:
		SceneNode* mParent = nullptr;
		std::vector<std::shared_ptr<SceneNode>> mChildren;
		std::vector<std::unique_ptr<Component>>	mComponents;
	};
}}
