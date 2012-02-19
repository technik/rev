////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 9th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gui panel

#include "panel.h"

#include "revCore/resourceManager/resourceManager.h"
#include "revVideo/material/basic/plainTextureMaterial.h"
#include "revVideo/material/materialInstance.h"
#include "revVideo/scene/model/quad.h"
#include "revVideo/texture/texture.h"

using rev::video::CPlainTextureMaterial;
using rev::video::CQuad;
using rev::video::CTexture;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CPanel::CPanel(const char * _texture, const CVec3& _pos)
	{
		// Create a material using the given texture
		CTexture * texture = CTexture::manager()->get(_texture);	// Retrieve texture from the manager
		mMaterial = new CPlainTextureMaterial(texture);	// Create a material with this texture
		CTexture::manager()->release(texture); // So we don't have ownership of this texture and mMaterial manages it.
		mMaterialInstance = new video::IMaterialInstance(mMaterial);	// Instantiate the material

		// Create a quad model with the size of the texture
		mQuad = new CQuad(texture->size());
		setRenderable(mQuad);	// Make myself a renderable instance of this quad

		// Move this panel to the given position
		CNode::setPos(_pos);
	}

	//------------------------------------------------------------------------------------------------------------------
	CPanel::~CPanel()
	{
		// Free all resources we allocated
		delete mQuad;
		delete mMaterialInstance;
		delete mMaterial;
	}

}	// namespace video
}	// namespace rev
