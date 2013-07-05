//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// base renderable type

#include "renderable.h"
#include "renderScene.h"

#include <revVideo/driver3d/driver3d.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev { namespace graphics3d {

	//------------------------------------------------------------------------------------------------------------------
	Renderable::Renderable()
		:mVertices(nullptr)
		,mNormals(nullptr)
		,mUVs(nullptr)
	{
		// Register in scene
		RenderScene::get()->add(this);
	}

	//------------------------------------------------------------------------------------------------------------------
	Renderable::~Renderable()
	{
		RenderScene::get()->remove(this);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderable::render()
	{
		video::Driver3d* driver = video::VideoDriver::getDriver3d();
		driver->setAttribBuffer(0, mNVertices, mVertices);
		driver->drawIndexBuffer(mNTriIndices, mTriangles, video::Driver3d::EPrimitiveType::triangles);
	}

}	// namespace graphics3d
}	// rev