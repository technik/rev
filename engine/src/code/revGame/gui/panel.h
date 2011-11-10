////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 9th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gui panel

#ifndef _REV_REVGAME_GUI_PANEL_H_
#define _REV_REVGAME_GUI_PANEL_H_

#include "guiElement.h"

namespace rev { namespace video
{
	// Forward declarations
	class CPlainTextureMaterial;
	class CQuad;
	class CTexture;
}	// namespace video
}	// namespace rev

//----------------------------------------------------------------------------------------------------------------------
namespace rev { namespace game
{
	class CPanel: public IGuiElement
	{
	public:
		CPanel(const char * _texture, const CVec3& _pos);
		~CPanel();
	private:
		video::CPlainTextureMaterial * mMaterial;
		video::CQuad * mQuad;
		CVec2 mSize;
	};

}	// namespace game
}	// namespace rev

#endif // _REV_REVGAME_GUI_PANEL_H_
