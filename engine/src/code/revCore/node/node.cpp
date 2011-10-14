////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entity

#include "node.h"

#include "revCore/component/component.h"
#include "revCore/time/time.h"
#include "revCore/time/timeSrc.h"

using namespace rev::rtl;

namespace rev {
	
//------------------------------------------------------------------------------------------------------------------
CNode::CNode():
	mTimeSrc(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
CNode::~CNode()
{
	// Deattach all my components
	for(poolset<IComponent*>::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
	{
		(*i)->deattach();
	}
}

//----------------------------------------------------------------------------------------------------------------------
TReal CNode::deltaTime() const
{
	return mTimeSrc?mTimeSrc->deltaTime():STime::get()->frameTime();
}

//----------------------------------------------------------------------------------------------------------------------
void CNode::addComponent(IComponent * _component)
{
	mComponents.push(_component);
}

//----------------------------------------------------------------------------------------------------------------------
void CNode::removeComponent(IComponent *_component)
{
	mComponents.erase(_component);
}

}	// namespace rev
