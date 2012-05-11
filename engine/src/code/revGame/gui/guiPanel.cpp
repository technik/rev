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
		:mTexture(0)
		,mSize(CVec2(0.f,0.f))
		,mMaterial(0)
		,mInstance(0)
	{
		CTexture * texture =  CTexture::get(_texture);
		// node
		mNode = new CNode();
		// Quad
		mQuad = new CQuad(mSize);
		// Set the texture
		setTexture( texture );
		texture->release();
	}

	//------------------------------------------------------------------------------------------------------------------
	CGuiPanel::CGuiPanel(CTexture * _texture)
		:mTexture(0)
		,mSize(CVec2(0.f,0.f))
		,mMaterial(0)
		,mInstance(0)
	{
		if(0 != _texture)
			setTexture(_texture);
		// Get the texture
		// node
		mNode = new CNode();
		// Quad
		mQuad = new CQuad(mSize);
	}

	//------------------------------------------------------------------------------------------------------------------
	CGuiPanel::~CGuiPanel()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CGuiPanel::setTexture(CTexture * _texture)
	{
		revAssert(0 != _texture, "Invalid texture for panel");
		if(0 != mTexture)
			mTexture->release();
		// Get the texture
		mTexture = _texture;
		mTexture->getOwnership();
		// Texture size
		mSize = mTexture->size();
		// Quad
		mQuad->setSize(mSize);
		if(0 != mMaterial)
			mMaterial->release();
		mMaterial = new CPlainTextureMaterial(mTexture);
		if(mInstance)
			delete mInstance;
		mInstance = new IRenderableInstance(mQuad, new CMaterialInstance(mMaterial));
		mInstance->setScene(SGameClient::get()->scene2d());
		mInstance->attachTo(mNode);
	};
}	// namespace game
}	// namespace rev