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
#include "ComponentLoader.h"
#include <core/types/json.h>

namespace rev { namespace game {

	/// Nodes are containers of logically related components
	/// You can attach components to a node
	/// You can attach a node to another node
	class SceneNode
	{
	public:
		void init();
		void update(float _dt);

		// Handle components
		void				addComponent	(Component* _c)
		{
			if(!_c)
				return;
			if(_c->node() != this) {
				_c->attachTo(this);
				mComponents.emplace_back(_c);
			}
		}
		size_t				nComponents		() const					{ return mComponents.size(); }
		Component&			component		(size_t _i) const			{ return *mComponents[_i]; }

		template<class T_>	
		T_*					component		() const {
			for (auto& c : mComponents) {
				if(typeid(*c) == typeid(T_))
					return static_cast<T_*>(c.get());
			}
			return nullptr;
		}

		void serialize(std::ostream& _out) const
		{
			uint32_t nComponents = mComponents.size();
			_out.write((const char*)nComponents, sizeof(uint32_t));
			for(auto& c : mComponents) {
				c->serialize(_out);
			}
		}

		void deserialize(ComponentLoader& loader, std::istream& in)
		{
			in >> name;
			uint32_t nComponents;
			in.read((char*)nComponents, sizeof(uint32_t));
			mComponents.resize(nComponents);
			for(auto& c : mComponents) {
				c = loader.loadComponent(in);
			}
		}

		// Debug info
		std::string name;
		void showDebugInfo() const;

	private:
		std::vector<std::unique_ptr<Component>>	mComponents;
	};
}}
