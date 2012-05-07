////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On May 7th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gui panel

#include "guiPanel.h"

#include <revCore/node/node.h>
#include <revGame/gameClient/gameClient.h>
#include <revVideo/material/basic/plainTextureMaterial.h>
#include <revVideo/material/materialInstance.h>
#include <revVideo/scene/model/quad.h>
#include <revVideo/scene/renderableInstance.h>
#include <revVideo/texture/texture.h>

using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CGuiPanel::CGuiPanel(const char * _texture)
	{
		// Get the texture
		mTexture = CTexture::get(_texture);
		// Texture size
		mSize = mTexture->size();
		// node
		mNode = new CNode();
		// Quad
		mQuad = new CQuad(mSize);
		mMaterial = new CPlainTextureMaterial(mTexture);
		mInstance = new IRenderableInstance(mQuad, new CMaterialInstance(mMaterial));
		mInstance->setScene(SGameClient::get()->scene2d());
		mInstance->attachTo(mNode);
	}

	//------------------------------------------------------------------------------------------------------------------
	CGuiPanel::CGuiPanel(CTexture * _texture)
	{
		// Get the texture
		mTexture = _texture;
		mTexture->getOwnership();
		// Texture size
		mSize = mTexture->size();
		// node
		mNode = new CNode();
		// Quad
		mQuad = new CQuad(mSize);
		mMaterial = new CPlainTextureMaterial(mTexture);
		mInstance = new IRenderableInstance(mQuad, new CMaterialInstance(mMaterial));
		mInstance->setScene(SGameClient::get()->scene2d());
		mInstance->attachTo(mNode);
	}
}	// namespace game
}	// namespace rev