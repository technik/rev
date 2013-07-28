//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Quadcopter

#ifndef _ROSE_AIRCRAFT_QUAD_QUAD_H_
#define _ROSE_AIRCRAFT_QUAD_QUAD_H_

#include <revMath/algebra/matrix.h>
#include <revGame/scene/object/solidObject.h>

class Quad {
public:
	Quad(float _mass, float _targetHeight);

	void update(float _deltaTime);

private:
	float					mTarget;
	rev::game::SolidObject*	mBody;
};

#endif // _ROSE_AIRCRAFT_QUAD_QUAD_H_