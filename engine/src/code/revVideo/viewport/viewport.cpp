////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 24th, 2011
////////////////////////////////////////////////////////////////////////////////
// Viewports

#include "viewport.h"

using rtl::pair;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// Static definitions
	CViewport::TViewportContainer CViewport::sViewports;

	//------------------------------------------------------------------------------------------------------------------
	CViewport::CViewport(const CVec2& _pos, const CVec2& _size, const TReal _layer):
		mPos(_pos),
		mSize(_size),
		mLayer(_layer),
		mCamera(0)
	{
		sViewports.insert(pair<TReal, CViewport*>(_layer, this));
	}

	//------------------------------------------------------------------------------------------------------------------
	CViewport::~CViewport()
	{
		multimap<TReal, CViewport*>::iterator i = sViewports.find(mLayer);
		for(; (*i).second != this; ++i);
		sViewports.erase(i);
	}
}	// namespace video
}	// namespace rev
