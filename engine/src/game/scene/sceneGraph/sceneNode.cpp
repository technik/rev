//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/15
//----------------------------------------------------------------------------------------------------------------------
// Scene graph node

#include "sceneNode.h"

using namespace rev::math;

namespace rev { namespace game {
	
	//------------------------------------------------------------------------------------------------------------------
	void SceneNode::move(const Vec3f& _translation)
	{
		setLocalPos(localPos() + Mat34f(localRot()).rotate(_translation), true);
	}
	
	//------------------------------------------------------------------------------------------------------------------
	void SceneNode::setPos(const Vec3f& _position)
	{
		setWorldPos(_position, true);
	}
	
	//------------------------------------------------------------------------------------------------------------------
	void SceneNode::rotate(const Vec3f& _axis, float _angle)
	{
		Quatf rotation = Quatf(_axis, _angle);
		setWorldRot(rotation * worldRot(), true);
	}

}	// namespace game
}	// namespace rev