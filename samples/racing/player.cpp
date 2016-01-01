//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Racing player
#include "player.h"
#include <math/algebra/vector.h>
#include <game/geometry/procedural/basic.h>
#include <input/keyboard/keyboardInput.h>
#include <video/graphics/renderer/material.h>

using namespace rev::core;
using namespace rev::game;
using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

//----------------------------------------------------------------------------------------------------------------------
Player::Player(PhysicsWorld* _world)
	:mWorld(_world)
{
	Vec3f carSize = {2.f, 4.f, 1.2f};
	// Rigid body
	mBody = RigidBody::box(1.f, carSize);
	_world->addRigidBody(mBody);
	mNode = new SceneNode();
	mNode->attachTo(mBody);
	// Render
	RenderMesh* car = Procedural::box(carSize);
	mFace = new RenderObj(car);
	mFace->attachTo(mNode);
	mFace->mMaterial = new Material();
	mFace->mMaterial->mDiffuse = Color(0.4f, 0.8f, 0.4f);
	// Camera
	mCamPivot = new SceneNode();
	mCamPivot->attachTo(mNode);
	mCamera = new Camera(Mat44f::frustrum(1.5f, 4.f/3.f, 0.1f, 1000.f));
	mCamNode = new SceneNode();
	mCamNode->attachTo(mCamPivot);
	mCamNode->setPos({0.f, -6.f, 1.f});
	mCamera->attachTo(mCamNode);
}

//----------------------------------------------------------------------------------------------------------------------
void Player::update(float _dt) {
	KeyboardInput* input = KeyboardInput::get();
	// Move camera
	float camSpd = 1.f*_dt;
	if(input->held(KeyboardInput::eKeyRight))
		mCamPivot->rotate(Vec3f::zAxis(), camSpd);
	if(input->held(KeyboardInput::eKeyLeft))
		mCamPivot->rotate(Vec3f::zAxis(), -camSpd);
	if(input->held(KeyboardInput::eKeyUp))
		mCamPivot->rotate(mCamPivot->transform().rotate(Vec3f::xAxis()), -0.4*camSpd);
	if(input->held(KeyboardInput::eKeyDown))
		mCamPivot->rotate(mCamPivot->transform().rotate(Vec3f::xAxis()), 0.4*camSpd);

	// Move car
	float v = 0.f;
	if(input->held(KeyboardInput::eW))
		v+=1.f;
	if(input->held(KeyboardInput::eS))
		v-=1.f;

	float vz = 0.f;
	if(input->pressed(KeyboardInput::eSpace))
		vz = 1.f;
	mBody->setLinearVelocity({0.f,v,vz+mBody->linearVelocity().z});

	// Rotate car
	float w = 0.f;
	if(input->held(KeyboardInput::eA))
		w+=1.f;
	if(input->held(KeyboardInput::eD))
		w-=1.f;
	mBody->setAngularVelocity(Vec3f::zAxis() * w);
}