////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// component interface

#include "component.h"

#include "revCore/codeTools/assert/assert.h"
#include "revCore/node/node.h"

// Active namespaces
using namespace rev::codeTools;

namespace rev {

//----------------------------------------------------------------------------------------------------------------------
IComponent::~IComponent()
{
	if(0 != mNode) // If this component is still attached to some entity
	{
		// deattach it
		deattach();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void IComponent::attachTo(CNode *_node)
{
	if(mNode != _node)
	{
		if(0 != mNode) // deattach from previous owner
		{
			deattach();
		}
		mNode = _node;
		_node->addComponent(this);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void IComponent::deattach()
{
	revAssert(0 != mNode);
	mNode->removeComponent(this);
	mNode = 0;
}

}	// namespace rev
