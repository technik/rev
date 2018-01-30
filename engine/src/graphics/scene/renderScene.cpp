//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "renderScene.h"
#include <graphics/debug/debugGUI.h>

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	std::unique_ptr<MeshRenderer> RenderScene::createMeshRenderer(std::shared_ptr<const graphics::RenderGeom> _geom) {
		auto mesh = std::make_unique<MeshRenderer>(_geom);
		mMeshes.emplace_back(mesh.get());
		return mesh;
	}

	//------------------------------------------------------------------------------------------------------------------
	std::unique_ptr<MeshRenderer> RenderScene::createMeshRenderer() {
		auto mesh = std::make_unique<MeshRenderer>();
		mMeshes.emplace_back(mesh.get());
		return mesh;
	}

	//------------------------------------------------------------------------------------------------------------------
	void RenderScene::showDebugInfo() {
		gui::beginWindow("Scene");
		gui::colorPicker("Light color", mLightClr);
		gui::endWindow();
	}

}}	// namespace rev::game