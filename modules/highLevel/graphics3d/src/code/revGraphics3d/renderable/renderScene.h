//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// render scene

#ifndef _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_
#define _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_

#include <vector>

namespace rev { namespace graphics3d {

	class Renderable;

	class RenderScene
	{
	public:
		static RenderScene* get	();

		void	add				(Renderable*);
		void	remove			(Renderable*);

		void	render			(int _mvp);

	private:
		RenderScene() {}

	private:
		static RenderScene*			sInstance;
		std::vector<Renderable*>	mRenderQueue;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_