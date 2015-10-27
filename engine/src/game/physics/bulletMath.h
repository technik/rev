//----------------------------------------------------------------------------------------------------------------------
// Assembly tool
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// Universidad de Sevilla
//----------------------------------------------------------------------------------------------------------------------
// Useful conversions between rev and bullet math types

#include <btBulletDynamicsCommon.h>
#include <math/algebra/vector.h>

//----------------------------------------------------------------------------------------------------------------------
// Vectors
inline rev::math::Vec3f bt2rev(const btVector3& _v) {
	return{ _v.x(), _v.y(), _v.z() };
}

inline btVector3 rev2bt(const rev::math::Vec3f& _v) {
	return btVector3(_v.x, _v.y, _v.z);
}

//----------------------------------------------------------------------------------------------------------------------
// Quaternions
inline rev::math::Quatf bt2rev(const btQuaternion& _q) {
	return { _q.x(), _q.y(), _q.z(), _q.w() };
}

inline btQuaternion rev2bt(const rev::math::Quatf& _q) {
	return btQuaternion(_q.x, _q.y, _q.z, _q.w);
}

//----------------------------------------------------------------------------------------------------------------------
// Matrices
// TODO