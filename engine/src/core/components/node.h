//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#ifndef _REV_CORE_COMPONENT_NODE_H_
#define _REV_CORE_COMPONENT_NODE_H_

#include "transformSrc.h"
#include <set>

namespace rev {
	namespace core {

		class Component;

		// Nodes are containers of components that share the same transform and time source
		// You can attach components to a node
		// You can attach a node to a transform source (nodes, animations, rigid bodies, etc...)
		// You can subscribe a node to a time source
		class Node : TransformSrc
		{
		public:
			// Constructor & destructor
			Node();
			virtual ~Node();

			// Accessors
			void			move		(const math::Vec3f& _translation);
			void			moveLocal	(const math::Vec3f& _translation);
			void			setPos		(const math::Vec3f& _position);
			void			setPosLocal	(const math::Vec3f& _position);
			void			rotate		(const math::Vec3f& _axis, float _angle);

			// Components
			void			addComponent	(Component * _component);
			void			removeComponent	(Component * _component);

		private:
			std::set<Component*>	mComponents;
		};
	}
}

#endif // _REV_CORE_COMPONENT_NODE_H_
