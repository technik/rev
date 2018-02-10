//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include <math/algebra/vector.h>
#include <memory>

namespace rev { namespace graphics {

	class Material;
	class RenderGeom;
	class RenderObj;

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

		std::shared_ptr<RenderObj> createRenderObj(uint32_t meshId, uint32_t materialId);
		std::shared_ptr<RenderObj> createRenderObj(const std::vector<std::pair<uint32_t,uint32_t>>& meshes);

		const std::vector<std::shared_ptr<Material>>	materials() const { return mMaterials; }

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