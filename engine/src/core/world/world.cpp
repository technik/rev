//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "world.h"
#include "worldLayer.h"
#include <core/components/sceneNode.h>

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		void World::addLayer(WorldLayer* _l) {
			mLayers.push_back(_l);
		}

		//--------------------------------------------------------------------------------------------------------------
		void World::init() {
			for(auto layer : mLayers)
				layer->init();
		}

		//--------------------------------------------------------------------------------------------------------------
		bool World::update(float _dt) {
			for(auto layer : mLayers)
				if(!layer->update(_dt))
					return false;
			return true;
		}

		//--------------------------------------------------------------------------------------------------------------
		SceneNode* World::createNode(const std::string& _name) {
			mObjects.push_back(new SceneNode(this, _name));
			return mObjects.back();
		}

		//--------------------------------------------------------------------------------------------------------------
		void World::destroyNode(SceneNode* _node) {
			// Traverse the list in reverse order because objects at the end (newly created) are more likely to be 
			// destroyed
			for (auto n = mObjects.rbegin(); n != mObjects.rend(); ++n)
			{
				if (*n == _node)
				{
					*n = mObjects.back();
					mObjects.pop_back();
					break;
				}
			}
			delete _node;
		}

		//--------------------------------------------------------------------------------------------------------------
		void World::getGraphData(cjson::Json& _dst) {
			for(auto obj : mObjects)
				_dst.push_back(obj->name());
		}
} }