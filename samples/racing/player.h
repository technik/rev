//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Racing player
#ifndef _REV_SAMPLES_RACING_PLAYER_H_
#define _REV_SAMPLES_RACING_PLAYER_H_

#include <video/basicTypes/camera.h>
#include <game/physics/physicsWorld.h>
#include <game/physics/rigidBody.h>
#include <video/graphics/renderer/renderObj.h>

class Player {
public:
	Player(rev::game::PhysicsWorld* _world);
	~Player();

	void update(float _dt);
	rev::video::Camera* camera();

private:
	rev::video::Camera* mCamera;
	rev::game::PhysicsWorld* mWorld;
	rev::game::RigidBody* mBody;
	rev::video::RenderObj* mFace;
};

#endif // _REV_SAMPLES_RACING_PLAYER_H_