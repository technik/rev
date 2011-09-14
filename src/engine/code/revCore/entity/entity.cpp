////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entity

#include "entity.h"

#include "revCore/component/component.h"
#include "revCore/entity/entityManager.h"
#include "revCore/time/time.h"

using namespace rev::rtl;

namespace rev {

//----------------------------------------------------------------------------------------------------------------------
CEntity::CEntity()
{
	// Register this entity in the entity manager
	SEntityManager::get()->registerEntity(this);
}

//----------------------------------------------------------------------------------------------------------------------
CEntity::~CEntity()
{
	// Delete all my components
	for(poolset<IComponent*>::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
	{
		(*i)->deattach();
	}
	// Unregister this entity from the entity manager
	SEntityManager::get()->unregisterEntity(this);
}

//----------------------------------------------------------------------------------------------------------------------
void CEntity::update()
{
	// Update time
	TReal frameTime = STime::get()->frameTime();
	// Update components
	for(poolset<IComponent*>::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
	{
		(*i)->update(frameTime);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CEntity::addComponent(IComponent * _component)
{
	mComponents.push(_component);
}

//----------------------------------------------------------------------------------------------------------------------
void CEntity::removeComponent(IComponent *_component)
{
	mComponents.erase(_component);
}

}	// namespace rev
