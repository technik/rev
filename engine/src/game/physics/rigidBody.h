//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Rigid body
#ifndef _REV_GAME_PHYSICS_RIGIDBODY_H_
#define _REV_GAME_PHYSICS_RIGIDBODY_H_

#include <core/components/transformSrc.h>
#include <btBulletDynamicsCommon.h>

class btCollisionShape;
class btRigidBody;

namespace rev {
	namespace video {
		class RenderMesh;
	}

	namespace game {
		class RigidBody : public core::TransformSrc {
		public:
			static RigidBody* concaveMesh	(float _mass, const video::RenderMesh* _mesh);
			static RigidBody* convexHull	(float _mass, const video::RenderMesh* _mesh);
			static RigidBody* sphere(float _mass, float _radius);
			static RigidBody* box	(float _mass, const math::Vec3f& _size);
			~RigidBody();

			void setPosition(const math::Vec3f& _pos);
			void setRotation(const math::Quatf& _rot);
			void setTransform(const math::Mat34f& _t);
			void setLinearVelocity(const math::Vec3f& _v);
			void setAngularVelocity(const math::Vec3f& _axix);

			void setBouncyness(float _bounce);

			void clearForces();

			math::Vec3f	getGravity() const;
			void setGravity(const math::Vec3f& _v);

		private:
			class MotionState : public btMotionState {
			public:
				MotionState(RigidBody* bd);
				// Inherited via btMotionState
				virtual void getWorldTransform(btTransform & worldTrans) const override;
				virtual void setWorldTransform(const btTransform & worldTrans) override;

			private:
				RigidBody* mBd;
			};

		private:
			RigidBody(float _mass, btCollisionShape* _shape);
			btCollisionShape* mShape;
			btRigidBody* mBody;
			MotionState* mMotion;

			friend class RigidBody::MotionState;
			friend class PhysicsWorld;
		};
	}
}

#endif // _REV_GAME_PHYSICS_RIGIDBODY_H_
