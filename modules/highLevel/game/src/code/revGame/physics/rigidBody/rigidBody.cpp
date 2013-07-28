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
	RigidBody::RigidBody(float _mass, const btVector3& _inertia, btCollisionShape* _shape)
	{
		btTransform initTransform;
		initTransform.setIdentity();

		mMotionState = new btDefaultMotionState(initTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(_mass,mMotionState,_shape,_inertia);
		mBody = new btRigidBody(rbInfo);
		//mBody->setDamping(0.5f, 0.5f);
		mBody->setRestitution(0.8f);
		mBody->setActivationState(DISABLE_DEACTIVATION);

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
		// Invoke callbacks
		for(auto callback : mMoveCb)
			callback(this);
	}

	//-----------------------------------------------------------------------------------------------------------------
	void RigidBody::onMove(std::function<void(RigidBody*)> _callback)
	{
		mMoveCb.push_back(_callback);
	}

	//------------------------------------------------------------------------------------------------------------------
	RigidBody* RigidBody::sphere(float _mass, float _radius)
	{
		btCollisionShape* shape = new btSphereShape(_radius);
		btVector3 inertia;
		shape->calculateLocalInertia(_mass, inertia);
		return new RigidBody(_mass, inertia, shape);
	}

	//------------------------------------------------------------------------------------------------------------------
	RigidBody* RigidBody::box(float _mass, const math::Vec3f& _size)
	{
		btCollisionShape* shape = new btBoxShape(Vec2bt(_size*0.5f));
		btVector3 inertia;
		shape->calculateLocalInertia(_mass, inertia);
		return new RigidBody(_mass, inertia, shape);
	}

}	// namespace game
}	// namespace rev