//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#ifndef _REV_CORE_COMPONENT_SCENENODE_H_
#define _REV_CORE_COMPONENT_SCENENODE_H_

#include "transformSrc.h"
#include <vector>

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
			void			move				(const math::Vec3f& _translation);
			void			moveWorld			(const math::Vec3f& _translation);
			void			rotate				(const math::Vec3f& _axis, float _angle);

			void			setPos				(const math::Vec3f& _position)	{ TransformSrc::setPosition(_position); }
			void			setPosWorld			(const math::Vec3f& _position)	{ TransformSrc::setWorldPosition(_position); }
			void			setRot				(const math::Quatf& _rot)		{ TransformSrc::setRotation(_rot); }
			void			setRotWorld			(const math::Quatf& _rot)		{ TransformSrc::setWorldRotation(_rot); }
			void			setTransform		(const math::Mat34f& _t)		{ TransformSrc::setTransform(_t); }
			void			setWorldTransform	(const math::Mat34f& _t)		{ TransformSrc::setWorldTransform(_t); }

			// Components
			void			addComponent	(Component * _component);
			void			removeComponent	(Component * _component);
			size_t			nComponents		() const { return mComponents.size(); }
			Component*		component		(size_t _i) const { return mComponents[_i]; }
			template<class T_>	
			T_*				component() const {
				for (auto c : mComponents) {
					if(typeid(c) == typeid(T_))
						return static_cast<T_*>(c);
				}
			}

			static SceneNode* construct(const cjson::Json&);

		private:
			std::string				mName;
			std::vector<Component*>	mComponents;
		};
	}
}

#endif // _REV_CORE_COMPONENT_SCENENODE_H_
