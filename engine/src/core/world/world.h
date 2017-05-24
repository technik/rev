//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <core/components/sceneNode.h>

namespace rev {
	namespace core {

		class WorldLayer;

		/// A world is a self-contained group of objects (SceneNode), organized in layers, that will be updated together.
		class World {
		public:
			void addLayer(WorldLayer* layer);
			/// \brief Init the layers and register factories for scene loading
			void init();

			/// \brief Update all layers
			void update(float _dt);

		private:
			void registerFactories();
			
			std::vector<SceneNode>	objects;
		};
} }
