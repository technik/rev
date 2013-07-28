//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Quadcopter

#include "quad.h"

#include <revGame/physics/rigidBody/rigidBody.h>

using namespace rev::game;
using namespace rev::math;

//----------------------------------------------------------------------------------------------------------------------
Quad::Quad(float _mass, float _target)
	:mTarget(_target)
{
	mBody = SolidObject::box(_mass, Vec3f(0.6f, 0.6f, 0.2f));
	mBody->rigidBody()->setPosition(Vec3f(0.f, 2.f, 1.f));
}

//----------------------------------------------------------------------------------------------------------------------
void Quad::update(float _dt)
{
	float force = 10.f;
	RigidBody * body = mBody->rigidBody();
	if(body->position().z < mTarget)
		body->applyImpulse(Vec3f::zAxis() * force * _dt);
}