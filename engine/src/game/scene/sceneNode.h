//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "component.h"
#include <vector>

namespace rev {
	namespace game {

		class Component;
		class World;

		/// Nodes are containers of components that share the same transform
		/// You can attach components to a node
		/// You can attach a node to another node to make a logic hierarchy
		class SceneNode
		{
		public:
			// Constructors
			SceneNode() {}
			SceneNode(size_t nComponents, size_t nChildren = 0) {
				mComponents.reserve(nComponents);
				mChildren.reserve(nChildren);
			}

			// Logic update
			// Inits all components and children added upto this point, in that order
			void init();
			// Ancestors are guaranteed to be updated before this gets called, but not necessarily ancestor's siblings
			// Components get updated before children
			void update();

			// Hierarchy
			SceneNode* parent() const { return mParent; }
			void attachTo(SceneNode* parent);

			// Handle components
			void							addComponent	(Component * _component);
			const std::vector<Component*>	components		() const { return mComponents; }
			const std::vector<SceneNode*>	children		() const { return mChildren; }

			// Access component structure
			template<class T_>	
			T_*					getComponent	() const {
				for (Component* c : mComponents) {
					if(typeid(*c) == typeid(T_))
						return static_cast<T_*>(c);
				}
				return nullptr;
			}

		private:
			SceneNode*				mParent = nullptr;
			std::vector<Component*>	mComponents;
			std::vector<SceneNode*> mChildren;
		};
	}
}