//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base component in the component system
#ifndef _REV_CORE_COMPONENT_COMPONENT_H_
#define _REV_CORE_COMPONENT_COMPONENT_H_

namespace rev {
	namespace core {

		class SceneNode;

		class Component {
		public:
			// -- Constructor & destructor --
			Component() : mNode(nullptr) {}
			virtual	~Component();

			// -- attach and dettach --
			virtual	void	attachTo(SceneNode * _node);
			virtual void	dettach();

		public:
			SceneNode * node() const { return mNode; }

		private:
			SceneNode * mNode;
		};
	}
}

#endif // _REV_CORE_COMPONENT_COMPONENT_H_
