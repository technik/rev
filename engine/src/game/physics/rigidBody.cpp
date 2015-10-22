//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Physics simulation context
#include "rigidBody.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "bulletMath.h"

namespace rev {
	namespace game {
		//--------------------------------------------------------------------------------------------------------------
		RigidBody* RigidBody::concaveMesh(float _mass, const video::RenderMesh* _mesh) {
			return nullptr;
		}

		//--------------------------------------------------------------------------------------------------------------
		RigidBody* RigidBody::sphere(float _mass, float _radius) {
			btCollisionShape* shape = new btSphereShape(_radius);
			return new RigidBody(_mass, shape);
		}

		//--------------------------------------------------------------------------------------------------------------
		RigidBody* RigidBody::box(float _mass, const math::Vec3f& _size) {
			btCollisionShape* shape = new btBoxShape(rev2bt(_size * 0.5f));
			return new RigidBody(_mass, shape);
		}

		//--------------------------------------------------------------------------------------------------------------
		RigidBody::RigidBody(float _mass, btCollisionShape* _shape) {
			btTransform transform;
			transform.setIdentity();
			btDefaultMotionState* motion = new btDefaultMotionState(transform);
			btVector3 inertia(0.f, 0.f, 0.f);
			btRigidBody::
			btRigidBody::btRigidBodyConstructionInfo rbInfo(_mass, motion, _shape, inertia);
			mBody = new btRigidBody(rbInfo);
		}

		//--------------------------------------------------------------------------------------------------------------
		RigidBody::~RigidBody() {
			delete mBody;
			delete mShape;
		}

		//--------------------------------------------------------------------------------------------------------------
		void RigidBody::setPosition(const math::Vec3f& _pos) {
			btTransform t = mBody->getWorldTransform();
			t.setOrigin(rev2bt(_pos));
			mBody->setWorldTransform(t);
		}
	}
}