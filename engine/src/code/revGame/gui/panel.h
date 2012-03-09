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
	class CQuad;
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

		// Accessor methods
		const CVec2&	size	() const;
		void			setSize	(const CVec2& _size);
	private:
		video::CQuad * mQuad;
		CVec2 mSize;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	inline const CVec2& CPanel::size() const
	{
		return mSize;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CPanel::setSize(const CVec2& _size)
	{
		mSize = _size;
	}

}	// namespace game
}	// namespace rev

#endif // _REV_REVGAME_GUI_PANEL_H_
