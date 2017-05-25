//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <core/components/sceneNode.h>
#include <functional>
#include <cjson/json.h>

namespace rev {
	namespace core {

		class WorldLayer;
		class Component;
		class WorldNode;

		/// A world is a self-contained group of objects (SceneNode), organized in layers, that will be updated together.
		class World {
		public:
			/// \Add the layer and register its factories
			void addLayer(WorldLayer* layer);

			void init();
			/// \brief Update all layers
			void update(float _dt);

			/// \return the new node
			SceneNode*	createNode	(const std::string& _name);
			SceneNode*	findNode	(const std::string& _name) const;
			void		destroyNode	(SceneNode*);
			
		private:
			std::vector<SceneNode*>			mObjects;
		};
} }
