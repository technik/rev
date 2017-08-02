//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base component in the component system
#pragma once

namespace rev {
	namespace game {

		class SceneNode;

		class Component {
		public:
			// -- Constructor & destructor --
			virtual	~Component();

			// --- Logic update
			// Gets called when all components of an object have been created.
			// Does not guarantee that other components's are initialized (their onCreate methods may not be called)
			// Also guarantees the component is already a child of its owner
			// Only gets called when the component gets added to a node before the node is initialized
			virtual void onCreate() {}
			// Gets called when all components of an object have been created.
			// Guarantees that other components's onCreate methods have been called, but not necessarily their start methods.
			virtual void init	() {}
			// Called on every update of the owner node
			virtual void update	() = 0;
			// void onAttach() when the owner gets moved in the hierarchy

			// Access owner node
			SceneNode& node() const { return mNode; }

		protected:
			// Components need a non-null owner node on creation, and can not be relocated later
			Component(SceneNode& _owner) : mNode(_owner) {}

		private:
			SceneNode& mNode;
		};
	}
}
