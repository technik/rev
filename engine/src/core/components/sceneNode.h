//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#ifndef _REV_CORE_COMPONENT_SCENENODE_H_
#define _REV_CORE_COMPONENT_SCENENODE_H_

#include "transformSrc.h"
#include <set>

namespace rev {
	namespace core {

		class Component;

		/// Nodes are containers of components that share the same transform
		/// You can attach components to a node
		/// You can attach a node to another transform source (nodes, animations, rigid bodies, etc...)
		class SceneNode : public TransformSrc
		{
		public:
			// Constructor & destructor
			SceneNode() {}
			SceneNode(const std::string& _name):mName(_name) {}

			// Node interface
			const std::string&	name		() const { return mName; }
			void				setName		(const std::string& _name) { mName = _name; }

			// transform
			void			move		(const math::Vec3f& _translation);
			void			moveLocal	(const math::Vec3f& _translation);
			void			setPos		(const math::Vec3f& _position);
			void			setPosLocal	(const math::Vec3f& _position);
			void			setRot		(const math::Quatf& _rot);
			void			setRotLocal	(const math::Quatf& _rot);
			void			setTransform(const math::Mat34f& _t);
			void			rotate		(const math::Vec3f& _axis, float _angle);

			// Components
			void			addComponent	(Component * _component);
			void			removeComponent	(Component * _component);

		private:
			std::string				mName;
			std::set<Component*>	mComponents;
		};
	}
}

#endif // _REV_CORE_COMPONENT_SCENENODE_H_
