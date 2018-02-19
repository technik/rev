//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "meshRenderer.h"
#include "sceneNode.h"
#include "transform/transform.h"
#include <graphics/debug/debugGUI.h>
#include <graphics/scene/material.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	MeshRenderer::MeshRenderer(
		std::shared_ptr<graphics::RenderObj> renderable
	)
		: mRenderable(renderable)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::init() {
		mSrcTransform = &node()->component<Transform>()->xForm;
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::update(float _dt) {
		mRenderable->transform = *mSrcTransform;
	}

	//------------------------------------------------------------------------------------------------------------------
	void MeshRenderer::serialize(std::ostream & _out) const {
		assert(false); // Unimplemented
		/*_out << "MeshRenderer\n";
		_out << mModelScene << "\n";
		uint32_t nMeshes = mRenderable->meshes.size();
		_out.write((const char*)&nMeshes, sizeof(nMeshes));

		for(uint32_t i = 0; i < nMeshes; ++i)
		{
			//_out.write((const char*)&mMeshIndices[i], sizeof(uint32_t));
			auto& name =  mRenderable->materials[i]->name;
			_out << (name.empty()? "-" : name) << "\n";
		}*/
	}

}}	// namespace rev::game