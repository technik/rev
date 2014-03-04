//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2014/March/02
//----------------------------------------------------------------------------------------------------------------------
// Utility conversions between bullet and rev
#ifndef _REV_GAME_PHYSICS_UTIL_BULLETUTILS_H_
#define _REV_GAME_PHYSICS_UTIL_BULLETUTILS_H_

#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btQuaternion.h>

#include <revMath/algebra/vector.h>
#include <revMath/algebra/quaternion.h>

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	btVector3 Vec2bt(const math::Vec3f& _v) {
		return btVector3(_v.x,_v.y,_v.z);
	}

	//------------------------------------------------------------------------------------------------------------------
	math::Vec3f bt2Vec(const btVector3& _v) {
		return math::Vec3f(_v.x(),_v.y(),_v.z());
	}

	//------------------------------------------------------------------------------------------------------------------
	math::Quatf bt2Quat(const btQuaternion& _q) {
		return math::Quatf(_q.x(),_q.y(),_q.z(),_q.w());
	}

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_PHYSICS_UTIL_BULLETUTILS_H_