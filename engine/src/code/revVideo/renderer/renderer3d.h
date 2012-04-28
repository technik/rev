////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3d renderer interface

#ifndef _REV_REVVIDEO_RENDERER_RENDERER3D_H_
#define _REV_REVVIDEO_RENDERER_RENDERER3D_H_

#include "revCore/math/matrix.h"

namespace rev { namespace video
{
	// Forward declarations
	class IVideoDriver;

	// Renderer 3d interface
	class IRenderer3d
	{
	public:
		// virtual destructor
		virtual ~IRenderer3d() {}
		// Interface
		virtual void	renderFrame	() = 0;

		// Current render state
		void			setModelMatrix		(const CMat34& _m);
		void			setViewMatrix		(const CMat34& _m);
		void			setProjectionMatrix	(const CMat4& _m);
		const CMat34&	modelMatrix			() const;
		const CMat34&	viewMatrix			() const;
		const CMat4&	projectionMatrix	() const;
		CMat4			viewProjMatrix		() const;

	private:
		CMat34	mModelMatrix;
		CMat34	mViewMatrix;
		CMat4	mProjectionMatrix;
	};

	//---------------------------------------------------------------------------------------------------------------
	inline const CMat34& IRenderer3d::modelMatrix() const
	{
		return mModelMatrix;
	}

	//---------------------------------------------------------------------------------------------------------------
	inline const CMat34& IRenderer3d::viewMatrix() const
	{
		return mViewMatrix;
	}

	//---------------------------------------------------------------------------------------------------------------
	inline const CMat4& IRenderer3d::projectionMatrix() const
	{
		return mProjectionMatrix;
	}

	//---------------------------------------------------------------------------------------------------------------
	inline CMat4 IRenderer3d::viewProjMatrix() const
	{
		CMat34 invView;
		mViewMatrix.inverse(invView);
		return mProjectionMatrix * invView;
	}



}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_RENDERER_RENDERER3D_H_

