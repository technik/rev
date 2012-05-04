////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 3rd, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gui text

#ifndef _REV_GAME_GUI_GUITEXT_H_
#define _REV_GAME_GUI_GUITEXT_H_

// Forward declarations
namespace rev 
{ 
	class CVec2;
	namespace video 
	{
		class CFont;
		class CTextMaterial;
	}	// namespace video
}	// namespace rev

namespace rev { namespace game
{
	class CText
	{
	public:
		CText(const CVec2& _pos, const char * font, unsigned size, const char * text = "");
		~CText();

	private:
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_GUITEXT_H_