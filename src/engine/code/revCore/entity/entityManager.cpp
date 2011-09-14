////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entity manager

#include "entityManager.h"

#include "revCore/codeTools/assert/assert.h"
#include "revCore/entity/entity.h"

using namespace rev::codeTools;
using namespace rev::rtl;

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	SEntityManager * SEntityManager::sManager = 0;

	//------------------------------------------------------------------------------------------------------------------
	void SEntityManager::init()
	{
		revAssert(0 == sManager);
		sManager = new SEntityManager();
		revAssert(0 != sManager);
	}

	//------------------------------------------------------------------------------------------------------------------
	SEntityManager* SEntityManager::get()
	{
		revAssert(0 != sManager);
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	void SEntityManager::end()
	{
		revAssert(0 != sManager);
		delete sManager;
		sManager = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	void SEntityManager::updateEntities() const
	{
		poolset<CEntity* const>::const_iterator i;
		poolset<CEntity* const>::const_iterator end = mEntities.end();
		for(i = mEntities.begin(); i != end; ++i)
		{
			(*i)->update();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void SEntityManager::registerEntity(CEntity *const _entity)
	{
		mEntities.push(_entity);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SEntityManager::unregisterEntity(CEntity *const _entity)
	{
		mEntities.erase(_entity);
	}

	//------------------------------------------------------------------------------------------------------------------
	SEntityManager::SEntityManager()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	SEntityManager::~SEntityManager()
	{
		// Delete all entities
		mEntities.clear();
	}
}	// namespace rev

