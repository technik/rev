////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, Shader tool
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 12th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Client

#include "shaderToolClient.h"

#include <revVideo/camera/orthoCamera.h>
#include <revVideo/viewport/viewport.h>

#include <shaderRender/shaderMaterial.h>
#include <shaderRender/surface.h>

using namespace rev;
using namespace rev::video;

//----------------------------------------------------------------------------------------------------------------------
ShaderToolClient::ShaderToolClient()
{
	mCamera = new rev::video::COrthoCamera(rev::CVec2(1.f,1.f),-1.f,1.f);
	mViewport = new CViewport(CVec2(0.f,0.f), CVec2(800.f,640.f), 0.f);
	mCamera->setScene(CVideoScene::defaultScene());
	mViewport->setCamera(mCamera);
	mSurface = new ShaderSurface("shader.vtx");
	mMaterial = new ShaderMaterial("shader.pxl");
	mMaterialInstance = new CMaterialInstance(mMaterial);
	mSurfaceInstance = new SurfaceInstance(mSurface, mMaterialInstance);
}

//----------------------------------------------------------------------------------------------------------------------
ShaderToolClient::~ShaderToolClient()
{
	delete mCamera;
	delete mViewport;
	delete mSurface;
	delete mSurfaceInstance;
	delete mMaterial;
	delete mMaterialInstance;
}

//----------------------------------------------------------------------------------------------------------------------
bool ShaderToolClient::update()
{
	return CGameClient::update();
}