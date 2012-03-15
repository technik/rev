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

namespace rev { namespace video
{
	// Forward declaration
	class CPxlShader;
	class CShader;
	class CVtxShader;
	class IRenderable;
	class IMaterial;
	class IRenderableInstance;

	class CDirectRenderer: public IRenderer3d
	{
	public:
		// Constructor and destructor
		CDirectRenderer();
		~CDirectRenderer();

		// public interface
		void	renderFrame	();

	private:
		void	invalidateRenderableCache	();
		void	setShaders					(CVtxShader * _vtx, CPxlShader * _pxl);
		void	renderElement				( IRenderableInstance * _x );

		// Renderable cache
		CPxlShader*			mCurrentPxlShader;
		CVtxShader*			mCurrentVtxShader;
		CShader*			mCurrentShader;
		const IRenderable*	mCurrentRenderable;
		const IMaterial*	mCurrentMaterial;
	};
}
}

#endif // _REV_REVVIDEO_RENDERER_DIRECTRENDERER_DIRECTRENDERER_H_
