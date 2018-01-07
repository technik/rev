//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "renderScene.h"

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	MeshRenderer* RenderScene::createMeshRenderer(const RenderGeom* _geom) {
		mMeshes.emplace_back(new MeshRenderer(_geom));
		return mMeshes.back();
	}

}}	// namespace rev::game