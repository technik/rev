//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <cjson/json.h>
#include <map>
#include <functional>

namespace rev {

	namespace core {
		class Component;
		class World; 
	}

	namespace game {

		class SceneLoader {
		public:
			typedef std::function<core::Component*(const cjson::Json&)>	ComponentFactory;

		public:
			void registerFactory(const std::string& _componentType, ComponentFactory);
			void loadScene(const std::string& _fileName, core::World* _w);

		private:
			std::map<std::string, ComponentFactory>	mFactories;
		};

	}	// namespace game
}
