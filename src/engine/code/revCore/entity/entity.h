////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entity

#ifndef _REV_REVCORE_ENTITY_ENTITY_H_
#define _REV_REVCORE_ENTITY_ENTITY_H_

#include "rtl/poolset.h"

namespace rev
{
	// -- Forward references --
	class IComponent;

	// Entities are updateables with a posible time source and possibly a node.
	// You can attach components to an entity
	// You can attach an entity to a node
	// You can subscribe an entity to a time source
	class CEntity
	{
	public:
		// Constructor & destructor
		CEntity();
		virtual ~CEntity();
		// Update
		virtual	void	update	();
		// Components
				void	addComponent	(IComponent * _component);
				void	removeComponent	(IComponent * _component);
		/// TODO: Add nodes and time sources
	private:
		rtl::poolset<IComponent*>	mComponents;
	};

}	// namespace rev

#endif // _REV_REVCORE_ENTITY_ENTITY_H_

