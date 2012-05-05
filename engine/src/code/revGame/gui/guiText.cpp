////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 3rd, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gui text

#include "guiText.h"

#include <revVideo/font/font.h>
#include <revVideo/material/materialInstance.h>
#include <revVideo/scene/model/staticModelInstance.h>

using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
//	CText::CText(const CVec2& _pos, const char * _font, unsigned _size, const char * _text)
//	{
//		mFont = CFont::manager()->get(string(_font));
//		mTextModel = mFont->createText(_text, _size);
//
//		mTextInstance = new CStaticModelInstance(mTextModel, new CMaterialInstance(mMaterial);
//	}
//
//	//------------------------------------------------------------------------------------------------------------------
//	CText::~CText()
//	{
//		video::CFont::manager()->release(mFont);
//		delete mTextModel;
//	}
}	// namespace game
}	// namespace rev