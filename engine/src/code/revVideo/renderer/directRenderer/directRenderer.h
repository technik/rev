////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// direct renderer

#ifndef _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_
#define _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_

#include "revCore/math/matrix.h"
#include "revVideo/renderer/renderer3d.h"
#include "revVideo/videoDriver/shader/vtxShader.h"

namespace rev { namespace video
{
	// Forward declaration
	class CVtxShader;

	class CDirectRenderer: public IRenderer3d
	{
	public:
		// Constructor and destructor
		CDirectRenderer();
		~CDirectRenderer();

		// public interface
		void	renderFrame	();
	};
}
}

#endif // _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_
