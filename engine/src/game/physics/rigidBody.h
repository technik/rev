//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Rigid body
#ifndef _REV_GAME_PHYSICS_RIGIDBODY_H_
#define _REV_GAME_PHYSICS_RIGIDBODY_H_

#include <core/components/transformSrc.h>

class btCollisionShape;
class btRigidBody;

namespace rev {
	namespace video {
		class RenderMesh;
	}

	namespace game {
		class RigidBody : public core::TransformSrc {
		public:
			static RigidBody* concaveMesh(float _mass, const video::RenderMesh* _mesh);
			static RigidBody* sphere(float _mass, float _radius);
			static RigidBody* box	(float _mass, const math::Vec3f& _size);
			~RigidBody();

			void setPosition(const math::Vec3f& _pos);

			void setBouncyness(float _bounce);

		private:
			void updateTransform();

		private:
			RigidBody(float _mass, btCollisionShape* _shape);
			btCollisionShape* mShape;
			btRigidBody* mBody;

			friend class PhysicsWorld;
		};
	}
}

#endif // _REV_GAME_PHYSICS_RIGIDBODY_H_
