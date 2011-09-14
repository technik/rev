////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// direct renderer

#ifndef _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_
#define _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_

#include "revVideo/renderer/renderer3d.h"

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
		float mVertices[6];
		unsigned short mIndices[3];
	};
}
}

#endif // _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_
