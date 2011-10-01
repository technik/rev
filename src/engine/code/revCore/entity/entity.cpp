////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entity

#include "entity.h"

#include "revCore/component/component.h"
#include "revCore/time/time.h"
#include "revCore/time/timeSrc.h"

using namespace rev::rtl;

namespace rev {

//----------------------------------------------------------------------------------------------------------------------
CEntity::~CEntity()
{
	// Delete all my components
	for(poolset<IComponent*>::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
	{
		(*i)->deattach();
	}
}

//----------------------------------------------------------------------------------------------------------------------
TReal CEntity::deltaTime() const
{
	return mTimeSrc?mTimeSrc->deltaTime():STime::get()->frameTime();
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
