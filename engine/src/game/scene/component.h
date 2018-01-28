//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base component in the component system
#pragma once

#include <iostream>

namespace rev { namespace game {

	class SceneNode;

	class Component
	{
	public:
		// -- Constructor & destructor --
		Component() = default;
		virtual	~Component();

		virtual void init	() {}
		virtual void update(float _dt) {}

		// -- attach and dettach --
		void	attachTo(SceneNode * _node);
		void	dettach();

		SceneNode * node() const { return mNode; }

		virtual void serialize(std::ostream& _out) const = 0;

		// Debug info
		virtual void showDebugInfo() {}
	private:
		SceneNode * mNode = nullptr;
	};

}}	// namespace rev::game