////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// component interface

#include "component.h"

#include "revCore/codeTools/assert/assert.h"
#include "revCore/entity/entity.h"

// Active namespaces
using namespace rev::codeTools;

namespace rev {

//----------------------------------------------------------------------------------------------------------------------
IComponent::~IComponent()
{
	if(0 != mEntity) // If this component is still attached to some entity
	{
		// deattach it
		deattach();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void IComponent::attachTo(CEntity *_entity)
{
	if(0 != mEntity) // deattach from previous owner
	{
		deattach();
	}
	mEntity = _entity;
	_entity->addComponent(this);
}

//----------------------------------------------------------------------------------------------------------------------
void IComponent::deattach()
{
	revAssert(0 != mEntity);
	mEntity->removeComponent(this);
	mEntity = 0;
}

}	// namespace rev
