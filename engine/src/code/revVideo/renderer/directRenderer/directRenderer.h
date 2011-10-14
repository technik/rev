////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// direct renderer

#ifndef _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_
#define _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_

#include "revVideo/renderer/renderer3d.h"
#include "revCore/math/matrix.h"

namespace rev { namespace video
{
	class CDirectRenderer: public IRenderer3d
	{
	public:
		// Constructor and destructor
		CDirectRenderer();
		~CDirectRenderer();

		// public interface
		void	renderFrame	();

	private:
		// Render data
		int mShader;
		CMat4	mMVP;
	};
}
}

#endif // _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_
