////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On May 7th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gui panel

#ifndef _REV_GAME_GUI_GUIPANEL_H_
#define _REV_GAME_GUI_GUIPANEL_H_

#include <revCore/math/vector.h>

// Forward declarations
namespace rev { class CNode; }
namespace rev { namespace video 
{
	class CTexture;
	class CQuad;
	class IRenderableInstance;
	class CPlainTextureMaterial;
} }

namespace rev { namespace game
{
	class CGuiPanel
	{
	public:
		CGuiPanel(const char * _textureName);
		CGuiPanel(video::CTexture * _texture);
		virtual ~CGuiPanel();

		//void setTexture(const char * _textureName);
		void setTexture(video::CTexture * _texture);
		//
		//void setSize(const CVec2& _size, bool _resize = true);
		//void setPosition(const CVec2& _position);

	private:
		video::CTexture*			mTexture;
		video::CQuad*				mQuad;
		video::IRenderableInstance*	mInstance;
		CVec2						mSize;
		CNode*						mNode;
		video::CPlainTextureMaterial*mMaterial;
	};
}	// namespace gane
}	// namespace rev

#endif // _REV_GAME_GUI_GUIPANEL_H_
