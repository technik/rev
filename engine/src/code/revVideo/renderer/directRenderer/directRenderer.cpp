////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// direct renderer

// Engine headers
#include "directRenderer.h"

#include "revCore/node/node.h"
#include "revVideo/camera/camera.h"
#include "revVideo/color/color.h"
#include "revVideo/material/material.h"
#include "revVideo/material/materialInstance.h"
#include "revVideo/scene/renderable.h"
#include "revVideo/scene/renderableInstance.h"
#include "revVideo/scene/videoScene.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/shader/shader.h"
#include "revVideo/videoDriver/shader/vtxShader.h"
#include "revVideo/videoDriver/videoDriver.h"
#include "revVideo/viewport/viewport.h"
#include "rtl/pair.h"

using rtl::pair;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CDirectRenderer::CDirectRenderer()
	{
		IVideoDriver * driver = SVideo::get()->driver();
		driver->setBackgroundColor(CColor::BLACK);
		mVtxShader = CVtxShader::manager()->get("direct.vtx");
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
		CPxlShader * currentPxlShader = 0;
		CShader * currentShader = 0;
		const IRenderable * currentRenderable = 0;
		const IMaterial * currentMaterial = 0;

		for(CViewport::TViewportContainer::iterator i = CViewport::viewports().begin();
			i != CViewport::viewports().end(); ++i)
		{
			CViewport * viewport = (*i).second;
			ICamera * cam = viewport->camera();
			if(cam)
			{
				CVideoScene * scn = cam->scene();
				CVideoScene::TRenderableContainer& renderables = scn->renderables();
				for(CVideoScene::TRenderableContainer::iterator i = renderables.begin(); i != renderables.end(); ++i)
				{
					IRenderableInstance * renderableInstance = *i;
					codeTools::revAssert(0 != renderableInstance);
										
					const IMaterialInstance * materialInstance = renderableInstance->materialInstance();
					codeTools::revAssert(0 != materialInstance);
					
					const IMaterial * material = materialInstance->material();
					codeTools::revAssert(0 != material);

					const IRenderable * renderable = renderableInstance->renderable();
					codeTools::revAssert(0 != renderable);
					// Shader cache
					CPxlShader * pxlShader = material->shader();
					if(pxlShader != currentPxlShader)
					{
						currentPxlShader = pxlShader;
						currentShader = CShader::manager()->get(pair<CVtxShader*,CPxlShader*>(mVtxShader,currentPxlShader));
						currentShader->setEnviroment();
						// Invalidate cache
						currentMaterial = 0;
						currentRenderable = 0;
					}
					// Renderable cache
					if(renderable != currentRenderable)
					{
						currentRenderable = renderable;
						renderable->setEnviroment();
					}
					// Material cache
					if(currentMaterial != material)
					{
						currentMaterial = material;
						material->setEnviroment();
					}
					// Actual render
					materialInstance->setEnviroment();
					renderableInstance->setEnviroment(cam);
					renderable->render();
				}
			}
		}
	}
}	// namespace video
}	// namespace rev
