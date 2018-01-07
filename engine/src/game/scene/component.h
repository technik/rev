//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base component in the component system
#pragma once

namespace rev { namespace game {

	class SceneNode;

	class Component
	{
	public:
		// -- Constructor & destructor --
		Component() : mNode(nullptr) {}
		virtual	~Component();

		// -- attach and dettach --
		virtual	void	attachTo(SceneNode * _node);
		virtual void	dettach();

		SceneNode * node() const { return mNode; }
	private:
		SceneNode * mNode;
	};

}}	// namespace rev::game