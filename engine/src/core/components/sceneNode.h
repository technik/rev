//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#ifndef _REV_CORE_COMPONENT_SCENENODE_H_
#define _REV_CORE_COMPONENT_SCENENODE_H_

#include "component.h"
#include <vector>

namespace rev {
	namespace core {

		class Component;
		class World;

		/// Nodes are containers of components that share the same transform
		/// You can attach components to a node
		/// You can attach a node to another transform source (nodes, animations, rigid bodies, etc...)
		class SceneNode
		{
		public:
			// Constructor & destructor
			SceneNode(World* _w) : mWorld(_w) {}
			SceneNode(World* _w, const std::string& _name) : mWorld(_w), mName(_name) {}

			// Node interface
			const std::string&	name			() const					{ return mName; }
			void				setName			(const std::string& _name)	{ mName = _name; }
			World*				world			() const					{ return mWorld; }

			// Handle components
			void				addComponent	(Component * _component);
			void				removeComponent	(Component * _component);
			size_t				nComponents		() const					{ return mComponents.size(); }
			Component*			component		(size_t _i) const			{ return mComponents[_i]; }

			template<class T_>	
			T_*					component		() const {
				for (Component* c : mComponents) {
					if(typeid(*c) == typeid(T_))
						return static_cast<T_*>(c);
				}
				return nullptr;
			}

		private:
			World*					mWorld;
			std::string				mName;
			std::vector<Component*>	mComponents;
		};
	}
}

#endif // _REV_CORE_COMPONENT_SCENENODE_H_
