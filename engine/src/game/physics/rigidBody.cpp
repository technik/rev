//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Physics simulation context
#include "rigidBody.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "bulletMath.h"
#include <VHACD.h>
#include <video/graphics/renderer/renderMesh.h>
#include <iostream>

using namespace rev::math;

namespace rev {
	namespace game {

		namespace {
			//----------------------------------------------------------------------------------------------------------------------
			btConvexHullShape* createHull(const VHACD::IVHACD::ConvexHull& _piece) {
				btConvexHullShape* blockShape = new btConvexHullShape();
				unsigned nVertices = _piece.m_nPoints;
				for (unsigned i = 0; i < nVertices; ++i) {
					btVector3 v(btScalar(_piece.m_points[3 * i + 0]), btScalar(_piece.m_points[3 * i + 1]), btScalar(_piece.m_points[3 * i + 2]));
					blockShape->addPoint(v, false);
				}
				blockShape->recalcLocalAabb();
				blockShape->setMargin(0.f);
				return blockShape;
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		RigidBody* RigidBody::concaveMesh(float _mass, const video::RenderMesh* _mesh) {


			VHACD::IVHACD::Parameters cdParams;
			VHACD::IVHACD* cdInterface = VHACD::CreateVHACD();
			int * indices = new int[_mesh->nIndices];
			for (unsigned i = 0; i < _mesh->nIndices; ++i)
				indices[i] = _mesh->indices[i];
			cdInterface->Compute(reinterpret_cast<const float*>(_mesh->vertices), 3, _mesh->nVertices, indices, 3, unsigned(_mesh->nIndices / 3), cdParams);
			delete indices;

			// Prepare rigid body's shape
			btCompoundShape* pieceShape = new btCompoundShape(false);
			int nHulls = cdInterface->GetNConvexHulls();
			std::cout << "Found " << nHulls << " convex hulls\n";
			btTransform origin;
			origin.setIdentity();

			for (int i = 0; i < nHulls; ++i) {
				VHACD::IVHACD::ConvexHull hull;
				cdInterface->GetConvexHull(i, hull);
				std::cout << "- vertices: " << hull.m_nPoints << "\n";
				for (unsigned v = 0; v < hull.m_nPoints; ++v)
					std::cout << "(" << hull.m_points[3 * v + 0] << ", " << hull.m_points[3 * v + 1] << ", " << hull.m_points[3 * v + 2] << ")\n";

				btConvexHullShape* hullShape = createHull(hull);
				pieceShape->addChildShape(origin, hullShape);
			}

			// Create rigid body
			return new RigidBody(_mass, pieceShape);
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
			btVector3 inertia;
			_shape->calculateLocalInertia(_mass,inertia);
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
		void RigidBody::setTransform(const math::Mat34f& _t) {
			btTransform t;
			math::Vec3f origin = _t.col(3);
			math::Quatf rot = math::Quatf(_t);
			t.setRotation(rev2bt(rot));
			t.setOrigin(rev2bt(origin));
			mBody->setWorldTransform(t);
			TransformSrc::setPosition(origin, TransformSrc::local);
			TransformSrc::setRotation(rot, TransformSrc::local);
		}

		//--------------------------------------------------------------------------------------------------------------
		void RigidBody::setLinearVelocity(const math::Vec3f& _v) {
			mBody->setLinearVelocity(rev2bt(_v));
		}

		//--------------------------------------------------------------------------------------------------------------
		void RigidBody::setAngularVelocity(const math::Vec3f& _a) {
			mBody->setAngularVelocity(rev2bt(_a));
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