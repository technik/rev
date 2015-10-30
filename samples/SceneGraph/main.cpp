//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Sample to play with shaders

#include <engine.h>
#include <game/scene/camera/flyByCamera.h>
#include <game/geometry/procedural/basic.h>
#include <input/keyboard/keyboardInput.h>
#include <math/algebra/vector.h>
#include <util/app/app3d.h>
#include <video/basicTypes/color.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderContext.h>
#include <video/graphics/renderer/material.h>
#include <video/graphics/renderer/renderMesh.h>
#include <video/graphics/renderer/renderObj.h>
#include <core/memory/newAllocator.h>
#include <core/components/sceneNode.h>
#include <game/physics/rigidBody.h>
#include <game/physics/physicsWorld.h>
#include <vector>

using namespace rev::core;
using namespace rev::game;
using namespace rev::math;
using namespace rev::video;

class SceneDemo : public rev::App3d {
public:
	SceneDemo(int _argc, const char** _argv)
		: App3d(_argc, _argv)
	{
		processArgs(_argc, _argv);

		// Construct global objects
		mRenderer = new ForwardRenderer();
		mRenderer->init<NewAllocator>(&driver3d(), mAlloc);
		mRenderCtxt = new RenderContext();
		mCam = new FlyByCamera(1.57f, 4.f/3.f, 0.001f, 1000.f);
		mCam->setPos({0.f, -10.f, 1.72f});
		mRenderCtxt->setCamera(mCam);
		mWorld = new PhysicsWorld();

		// Construct floor
		Vec3f floorSize(100.f, 100.f, 2.f);
		RenderMesh* mesh = Procedural::box(floorSize);
		RenderObj* obj = new RenderObj(mesh);
		Material * mat = new Material();
		mat->mDiffuse = Color(0.5f);
		obj->mMaterial = mat;
		SceneNode* node = new SceneNode();
		obj->attachTo(node);
		mRenderCtxt->insert(obj);
		RigidBody* floorRb = RigidBody::box(0.f, floorSize);
		node->attachTo(floorRb);
		floorRb->setPosition({0.f, 0.f, -1.f});
		mWorld->addRigidBody(floorRb);

		// Construct sphere
		float sphRad = 1.f;
		RenderMesh* sphMesh = Procedural::geoSphere(sphRad, 16, 7);
		Material* sphMat = new Material();
		sphMat->mDiffuse = Color(1.f,0.2f,0.2f);
		RenderObj* sphObj = new RenderObj(sphMesh);
		sphObj->mMaterial = sphMat;
		SceneNode* sph = new SceneNode();
		sphObj->attachTo(sph);
		mRenderCtxt->insert(sphObj);
		mBallBd = RigidBody::sphere(1.f, sphRad);
		sph->attachTo(mBallBd);
		mBallBd->setPosition({0.f,0.f,2.f});
		mWorld->addRigidBody(mBallBd);
	}

	~SceneDemo() {
		mRenderer->end<NewAllocator>(mAlloc);
	}

	ForwardRenderer* mRenderer;
	RenderContext*	mRenderCtxt;
	NewAllocator mAlloc;
	FlyByCamera* mCam;
	PhysicsWorld* mWorld;
	RigidBody* mBallBd;

private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		_argc;
		_argv;
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {

		// Update objects in the middle
		mCam->update(_dt);

		if(keyboard().pressed(rev::input::KeyboardInput::eSpace))
			mBallBd->setPosition({0.f,0.f,10.f});
		mWorld->simulate(_dt);

		mRenderer->renderContext(*mRenderCtxt);
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	SceneDemo app(_argc,_argv);

	while(app.update());
	return 0;
}