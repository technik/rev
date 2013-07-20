//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics rigid body

#include "solidObject.h"
#include <revGame/physics/rigidBody/rigidBody.h>
#include <revGraphics3d/renderable/renderable.h>

using namespace rev::graphics3d;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	void SolidObject::refresh()
	{
		mRenderable->m = mRb->transform();
	}

	//------------------------------------------------------------------------------------------------------------------
	SolidObject* SolidObject::box(float _mass, math::Vec3f& _size)
	{
		RigidBody* body = RigidBody::box(_mass, _size);
		Renderable* renderable = Renderable::box(_size);
		return new SolidObject(body, renderable);
	}

	//------------------------------------------------------------------------------------------------------------------
	SolidObject* SolidObject::ball(float _mass, float _radius, unsigned _sections)
	{
		RigidBody* body = RigidBody::sphere(_mass, _radius);
		Renderable* renderable = Renderable::geoSphere(_radius, 2* _sections, _sections>2?(_sections - 1):2);
		return new SolidObject(body, renderable);
	}

	//------------------------------------------------------------------------------------------------------------------
	SolidObject::SolidObject(RigidBody* _body, graphics3d::Renderable* _renderable)
			:mRb(_body)
			,mRenderable(_renderable)
	{
		mRb->onMove([this](RigidBody*){ // Update renderable's transform whenever rigidbody moves
			mRenderable->m = mRb->transform();
		});
	}

}	// namespace game
}	// namespace rev