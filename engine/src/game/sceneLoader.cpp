//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "sceneLoader.h"
#include <cjson/json.h>
#include <fstream>
#include <vector>
#include <core/world/world.h>

using namespace cjson;
using namespace rev::core;
using namespace std;

namespace rev {
	namespace game {

		//--------------------------------------------------------------------------------------------------------------
		void SceneLoader::registerFactory(const std::string& _type, ComponentFactory _f) {
			mFactories.insert(std::make_pair(_type,_f));
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneLoader::loadScene(const std::string& _fileName, core::World& _w) {
			Json sceneData;
			sceneData.parse(ifstream(_fileName));
			for (const auto& objectData : sceneData["objects"]) {
				SceneNode* obj = _w.createNode(objectData["name"]);
				if(objectData.contains("components"))
					for (const auto& c : objectData["components"])
					{
						const std::string& componentType = c["_type"];
						auto iter = mFactories.find(componentType);
						if (iter == mFactories.end())
						{
							cout << "Error: Unable to find factory for component of type " << componentType << "\n";
							continue;
						}
						obj->addComponent(iter->second(c));
					}
			}
		}
} }