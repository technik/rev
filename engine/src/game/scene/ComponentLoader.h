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

#include <game/scene/component.h>
#include <game/scene/transform/transform.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace rev { namespace game {

	class ComponentLoader
	{
	public:
		ComponentLoader() {
			registerDefaultFactories();
		}

		using Component = game::Component;
		using CompPtr = std::unique_ptr<Component>;
		using Factory = std::function<CompPtr(const std::string&, std::istream&)>;

		void registerFactory(const std::string& key, const Factory& factory, bool overrideDuplicated)
		{
			if(!overrideDuplicated)
				for(size_t i = 0; i < keys.size(); ++i)
					if(keys[i] == key)
					{
						factories[i] = factory;
						return;
					}
			// No duplicated key, add factory at the end
			keys.push_back(key);
			factories.push_back(factory);
		}

		CompPtr loadComponent(std::istream& in) const
		{
			std::string key;
			in >> key;
			in.get(); // Skip new line
			for(size_t i = 0; i < keys.size(); ++i)
				if(keys[i] == key)
					return factories[i](key,in);
			return nullptr;
		}

	private:
		void registerDefaultFactories()
		{
			registerFactory("Transform", [](const std::string&, std::istream& in){
				return std::make_unique<Transform>(in);
			}, false);
		}

		std::vector<std::string>	keys;
		std::vector<Factory>		factories;
	};

	// TODO:
	class ComponentSerializer
	{
	public:
		void save(const Component&, std::ostream&) const;
	};

}}
