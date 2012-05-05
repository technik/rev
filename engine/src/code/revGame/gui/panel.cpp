////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 9th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gui panel

#include "panel.h"

#include "revVideo/material/basic/plainTextureMaterial.h"
#include "revVideo/material/material.h"
#include "revVideo/material/materialInstance.h"
#include "revVideo/scene/model/quad.h"
#include "revVideo/texture/texture.h"

using rev::video::CPlainTextureMaterial;
using rev::video::CQuad;
using rev::video::CTexture;
using rev::video::IMaterial;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CPanel::CPanel(const char * _texture, const CVec3& _pos)
	{
		// Create a material using the given texture
		CTexture * texture = CTexture::get(_texture);	// Retrieve texture from the manager
		IMaterial * material = IMaterial::get(_texture);
		if(0 == material)
		{
			material = new CPlainTextureMaterial(texture);
			IMaterial::registerResource(material, _texture);
		}
		CTexture::release(texture); // Relinquish ownership of this texture.
		setMaterial(material);	// Create a material with this texture

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
	}

}	// namespace video
}	// namespace rev
