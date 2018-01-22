//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "renderScene.h"
#include <graphics/debug/debugGUI.h>

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	MeshRenderer* RenderScene::createMeshRenderer(const RenderGeom* _geom) {
		mMeshes.emplace_back(new MeshRenderer(_geom));
		return mMeshes.back();
	}

	//------------------------------------------------------------------------------------------------------------------
	void RenderScene::showDebugInfo() {
		gui::beginWindow("Scene");
		gui::colorPicker("Light color", mLightClr);
		gui::endWindow();
	}

}}	// namespace rev::game