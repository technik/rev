////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// direct renderer

// Engine headers
#include "directRenderer.h"

#include "revCore/node/node.h"
#include "revCore/resourceManager/resourceManager.h"
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

using rtl::pair;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CDirectRenderer::CDirectRenderer()
	{
		IVideoDriver * driver = SVideo::get()->driver();
		driver->setBackgroundColor(CColor::BLACK);
	}

	//------------------------------------------------------------------------------------------------------------------
	CDirectRenderer::~CDirectRenderer()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDirectRenderer::renderFrame()
	{
		// Clean render cache
		invalidateRenderableCache();

		rtl::vector<IRenderableInstance*> alphaInstances;
		// Render
		for(CViewport::TViewportContainer::iterator vp = CViewport::viewports().begin();
			vp != CViewport::viewports().end(); ++vp)
		{
			ICamera * cam = (*vp).second->camera();
			if(cam)
			{
				alphaInstances.clear();
				// Set environment
				setViewMatrix(cam->viewMatrix());
				setProjectionMatrix(cam->projMatrix());
				CVideoScene::TRenderableContainer& renderables = cam->scene()->renderables();
				// Render solid objects, defer objects with alpha
				for(CVideoScene::TRenderableContainer::iterator iter = renderables.begin(); iter != renderables.end(); ++iter)
				{
					if(! (*iter)->materialInstance()->material()->usesAlpha())
						renderElement(*iter);
					else
						alphaInstances.push_back(*iter);
				}
				// Render objects with alpha
				for(rtl::vector<IRenderableInstance*>::iterator iter = alphaInstances.begin(); iter != alphaInstances.end(); ++iter)
				{
						renderElement(*iter);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDirectRenderer::invalidateRenderableCache()
	{
		mCurrentVtxShader = 0;
		mCurrentPxlShader = 0;
		mCurrentShader = 0;
		mCurrentRenderable = 0;
		mCurrentMaterial = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDirectRenderer::setShaders(CVtxShader * _vtx, CPxlShader * _pxl)
	{
		if((_pxl != mCurrentPxlShader) || (_vtx != mCurrentVtxShader))
		{
			invalidateRenderableCache();
			mCurrentVtxShader = _vtx;
			mCurrentPxlShader = _pxl;
			mCurrentShader = CShader::manager()->get(pair<CVtxShader*,CPxlShader*>(_vtx,_pxl));
			mCurrentShader->setEnviroment();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDirectRenderer::renderElement( IRenderableInstance * _x )
	{
		codeTools::revAssert(0 != _x);
		const CMaterialInstance * materialInstance = _x->materialInstance();
		codeTools::revAssert(0 != materialInstance);
		const IMaterial * material = materialInstance->material();
		codeTools::revAssert(0 != material);
		const IRenderable * renderable = _x->renderable();
		codeTools::revAssert(0 != renderable);

		// Update shader cache
		setShaders(renderable->shader(), material->shader());
		// Renderable cache
		if(renderable != mCurrentRenderable)
		{
			mCurrentRenderable = renderable;
			renderable->setEnviroment();
		}
		// Material cache
		if(mCurrentMaterial != material)
		{
			mCurrentMaterial = material;
			material->setEnvironment();
		}
		// Actual render
		_x->setEnviroment();
		materialInstance->setEnvironment();
		renderable->render();
	}
}	// namespace video
}	// namespace rev
