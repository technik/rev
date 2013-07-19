//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Rigid body

#include "rigidBody.h"
#include "../world/physicsWorld.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/revBulletUtils.h>

namespace rev { namespace game {

	//-----------------------------------------------------------------------------------------------------------------
	void RigidBody::initRb(float _mass, const btVector3& _inertia, btCollisionShape* _shape)
	{
		btTransform initTransform;
		initTransform.setIdentity();

		mMotionState = new btDefaultMotionState(initTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(_mass,mMotionState,_shape,_inertia);
		mBody = new btRigidBody(rbInfo);

		PhysicsWorld::get()->addRigidBody(this);
	}
	//-----------------------------------------------------------------------------------------------------------------
	RigidBody::~RigidBody()
	{
	}

	//-----------------------------------------------------------------------------------------------------------------
	void RigidBody::setPosition(const math::Vec3f& _position)
	{
		btMotionState* motionState = mBody->getMotionState();
		btTransform transform;
		motionState->getWorldTransform(transform);
		transform.setOrigin(Vec2bt(_position));
		motionState->setWorldTransform(transform);
		mBody->setMotionState(motionState);
	}

	//-----------------------------------------------------------------------------------------------------------------
	void RigidBody::applyImpulse(const math::Vec3f& _impulse)
	{
		mBody->applyCentralImpulse(Vec2bt(_impulse));
	}

	//-----------------------------------------------------------------------------------------------------------------
	void RigidBody::applyImpulse(const math::Vec3f& _impulse, const math::Vec3f& _position)
	{
		mBody->applyImpulse(Vec2bt(_impulse), Vec2bt(_position));
	}

	//-----------------------------------------------------------------------------------------------------------------
	void RigidBody::refresh()
	{
		btMotionState* motionState = mBody->getMotionState();
		btTransform transform;
		motionState->getWorldTransform(transform);
		btVector3 pos = transform.getOrigin();
		btQuaternion rot = transform.getRotation();
		setLocalPos(bt2Vec(pos), false);
		setLocalRot(bt2Quat(rot), true);
	}

}	// namespace game
}	// namespace rev