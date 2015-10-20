//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base component in the component system
#ifndef _REV_CORE_COMPONENT_COMPONENT_H_
#define _REV_CORE_COMPONENT_COMPONENT_H_

namespace rev {
	namespace core {

		class Node;

		class Component {
		public:
			// -- Constructor & destructor --
			Component() : mNode(nullptr) {}
			virtual	~Component();

			// -- attach and dettach --
			virtual	void	attachTo(Node * _node);
			virtual void	dettach();

		public:
			Node * node() const { return mNode; }

		private:
			Node * mNode;
		};
	}
}

#endif // _REV_CORE_COMPONENT_COMPONENT_H_
