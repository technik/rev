////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entity manager

#ifndef _REV_REVCORE_ENTITY_ENTITYMANAGER_H_
#define _REV_REVCORE_ENTITY_ENTITYMANAGER_H_

// Engine headers
#include "rtl/poolset.h"

namespace rev
{
// Forward declarations
class CEntity;

//----------------------------------------------------------------------------------------------------------------------
// This class manages the update process of all entities in the game.
// Update order isn't guaranteed to be persistent. Beside that, on some platforms, many updates can happen in parallel
// in multiple threads, so concurrence must be taken into account.
//----------------------------------------------------------------------------------------------------------------------
class SEntityManager
{
public:
	// -- Singleton life cycle --
	static	void			init();
	static	SEntityManager*	get	();
	static	void			end	();

	// -- Entity management --
	void	updateEntities		() const;						///< Update entities

	void	registerEntity		(CEntity *const _entity);	///< Register a new entity.
	void	unregisterEntity	(CEntity *const _entity);	///< Remove an entity from the manager

private:
	SEntityManager	();
	~SEntityManager	();

private:
	static SEntityManager * sManager;
	rtl::poolset<CEntity* const>	mEntities;						///< Registered entities
};

} // namespace rev

#endif // _REV_REVCORE_ENTITY_ENTITYMANAGER_H_
