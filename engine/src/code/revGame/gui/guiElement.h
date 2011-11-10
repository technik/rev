////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 10th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#ifndef _REV_REVGAME_GUI_GUIELEMENT_H_
#define _REV_REVGAME_GUI_GUIELEMENT_H_

#include "revCore/node/node.h"
#include "revVideo/scene/renderableInstance.h"
#include "revVideo/scene/videoScene.h"

namespace rev { namespace game
{
	class IGuiElement : public CNode, public video::IRenderableInstance
	{
	public:
		IGuiElement();
		virtual ~IGuiElement();

		static video::CVideoScene * guiScene();

	private:
		static video::CVideoScene * sDefaultGuiScene;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	inline IGuiElement::IGuiElement()
	{
		IRenderableInstance::setScene(guiScene());
		IComponent::attachTo(static_cast<CNode*>(this));
	}

	//------------------------------------------------------------------------------------------------------------------
	inline IGuiElement::~IGuiElement()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	inline video::CVideoScene* IGuiElement::guiScene()
	{
		if( 0 == sDefaultGuiScene)
			sDefaultGuiScene = new video::CVideoScene();
		return sDefaultGuiScene;
	}

}	// namespace game
}	// namespace rev

#endif // _REV_REVGAME_GUI_GUIELEMENT_H_