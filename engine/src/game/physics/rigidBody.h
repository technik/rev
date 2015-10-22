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
			RigidBody(float _mass, const video::RenderMesh* _mesh);
			~RigidBody();
		};
	}
}

#endif // _REV_GAME_PHYSICS_RIGIDBODY_H_
