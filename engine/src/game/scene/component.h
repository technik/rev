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
		virtual	~Component();

		virtual void init	() {}
		virtual void update(float _dt) {}

		// -- attach and dettach --
		// TODO: Is this really necessary?
		void	attachTo(SceneNode * _node);
		void	dettach();

		SceneNode * node() const { return mNode; }

		virtual void serialize(std::ostream& out) const = 0;

	protected:
		Component() = default;

	private:
		SceneNode * mNode = nullptr;
	};

}}	// namespace rev::game