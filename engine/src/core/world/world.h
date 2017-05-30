//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
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
			/// \return false if the application should close
			bool update(float _dt);

			/// \return the new node
			SceneNode*	createNode	(const std::string& _name);
			void		destroyNode	(SceneNode*);

			void		getGraphData	(cjson::Json& _dst);
			
		private:
			std::vector<WorldLayer*>	mLayers;
			std::vector<SceneNode*>		mObjects;
		};
} }
