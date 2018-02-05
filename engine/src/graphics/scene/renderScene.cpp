//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "renderScene.h"
#include "renderObj.h"

namespace rev { namespace graphics {

	//------------------------------------------------------------------------------------------------------------------
	std::shared_ptr<RenderObj> RenderScene::createRenderObj(size_t meshId, size_t materialId)
	{
		auto& mesh = mMeshes[meshId];
		auto& mat = mMaterials[materialId];
		auto renderObj = std::make_shared<RenderObj>(mesh, mat);
		mRenderables.emplace_back(renderObj);
		return renderObj;
	}

	//------------------------------------------------------------------------------------------------------------------
	std::shared_ptr<RenderObj> RenderScene::createRenderObj(const std::vector<std::pair<size_t,size_t>>& meshes)
	{
		auto renderObj = std::make_shared<RenderObj>();
		for(auto& m : meshes)
		{
			auto& mesh = mMeshes[m.first];
			auto& mat = mMaterials[m.second];
			renderObj->meshes.push_back(mesh);
			renderObj->materials.push_back(mat);
		}
		mRenderables.emplace_back(renderObj);
		return renderObj;
	}

}}	// namespace rev::graphics