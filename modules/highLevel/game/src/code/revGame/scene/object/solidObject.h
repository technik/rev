//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics rigid body

#ifndef _REV_GAME_SCENE_OBJECT_SOLIDOBJECT_H_
#define _REV_GAME_SCENE_OBJECT_SOLIDOBJECT_H_

#include <revMath/algebra/vector.h>

namespace rev { namespace graphics3d { class Renderable; }}

namespace rev { namespace game {

	class RigidBody;

	class SolidObject {
	public:
		void refresh();

		RigidBody* rigidBody() { return mRb; }

		static SolidObject* box(float _mass, math::Vec3f& _size);
		static SolidObject* ball(float _mass, float _radius, unsigned _sections);

	private:
		SolidObject(RigidBody* _body, graphics3d::Renderable* _renderable);

	private:
		RigidBody* mRb;
		graphics3d::Renderable* mRenderable;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_OBJECT_SOLIDOBJECT_H_