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
			btRigidBody::btRigidBodyConstructionInfo rbInfo(_mass, motion, _shape, inertia);
			rbInfo.m_restitution = 0.9f;
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
			TransformSrc::setPosition(_pos, TransformSrc::local);
		}

		//--------------------------------------------------------------------------------------------------------------
		void RigidBody::setBouncyness(float _b) {
			mBody->setRestitution(_b);
		}

		//--------------------------------------------------------------------------------------------------------------
		void RigidBody::updateTransform() {
			btTransform t = mBody->getWorldTransform();
			TransformSrc::setPosition(bt2rev(t.getOrigin()), TransformSrc::local);
			TransformSrc::setRotation(bt2rev(t.getRotation()), TransformSrc::local);
		}
	}
}