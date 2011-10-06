////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// direct renderer

// Engine headers
#include "directRenderer.h"

#include "revCore/entity/entity.h"
#include "revVideo/camera/camera.h"
#include "revVideo/color/color.h"
#include "revVideo/scene/renderable.h"
#include "revVideo/scene/videoScene.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"
#include "revVideo/viewport/viewport.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CDirectRenderer::CDirectRenderer()
	{
		IVideoDriver * driver = SVideo::get()->driver();
		driver->setBackgroundColor(CColor::BLACK);
		mShader = driver->getShader("direct.vtx", "direct.pxl");

		mMVP = CMat4::identity();
	}

	//------------------------------------------------------------------------------------------------------------------
	CDirectRenderer::~CDirectRenderer()
	{
		//
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDirectRenderer::renderFrame()
	{
		IVideoDriver * driver = SVideo::get()->driver();

		driver->setShader(mShader);

		for(CViewport::TViewportContainer::iterator i = CViewport::viewports().begin();
			i != CViewport::viewports().end(); ++i)
		{
			CViewport * viewport = (*i).second;
			ICamera * cam = viewport->camera();
			if(cam)
			{
				CVec2 position = (*i).second->pos();
				CMat4 viewProj = cam->projMatrix();
				CVideoScene * scn = cam->scene();
				CVideoScene::TRenderableContainer& renderables = scn->renderables();
				for(CVideoScene::TRenderableContainer::iterator i = renderables.begin(); i != renderables.end(); ++i)
				{
					IRenderable * renderable = *i;
					CEntity * entity = renderable->getEntity();
					if(entity)
					{
						mMVP = viewProj * entity->transform();
						driver->setUniform(IVideoDriver::eMVP, mMVP);
						driver->setRealAttribBuffer(IVideoDriver::eVertex, 3, renderable->vertices());
						driver->drawIndexBuffer(3*renderable->nTriangles(), renderable->triangles(), false);
					}
				}
			}
		}

	}
}	// namespace video
}	// namespace rev
