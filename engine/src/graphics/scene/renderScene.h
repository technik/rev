//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include <math/algebra/vector.h>
#include <memory>
#include "renderObj.h"

namespace rev { namespace graphics {

	class RenderScene
	{
	public:
		RenderScene()
		{
			mLightClr = math::Vec3f::ones();
		}

		// Content handling
		size_t registerMaterial(std::shared_ptr<Material> material)
		{
			mMaterials.push_back(material);
			return mMaterials.size() - 1;
		}

		size_t registerMesh(std::shared_ptr<RenderGeom> mesh)
		{
			mMeshes.push_back(mesh);
			return mMeshes.size() - 1;
		}

		std::shared_ptr<RenderObj> createRenderObj(size_t meshId, size_t materialId)
		{
			auto& mesh = mMeshes[meshId];
			auto& mat = mMaterials[materialId];
			auto renderObj = std::make_shared<RenderObj>(mesh, mat);
			mRenderables.emplace_back(renderObj);
			return renderObj;
		}

		std::shared_ptr<RenderObj> createRenderObj(const std::vector<std::pair<size_t,size_t>>& meshes)
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

		// Accessors
		const math::Vec3f& lightClr() const { return mLightClr; }
		auto renderables() const { return mRenderables; }

	private:
		math::Vec3f					mLightClr;

		std::vector<std::shared_ptr<RenderGeom>>	mMeshes;
		std::vector<std::shared_ptr<Material>>		mMaterials;
		std::vector<std::weak_ptr<RenderObj>>		mRenderables;
	};

}}	// namespace rev::graphics