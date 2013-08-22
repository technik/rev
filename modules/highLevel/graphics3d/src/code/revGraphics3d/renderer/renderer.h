//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/10
//----------------------------------------------------------------------------------------------------------------------
// renderer interface

#ifndef _REV_GRAPHIS3D_RENDERER_RENDERER_H_
#define _REV_GRAPHIS3D_RENDERER_RENDERER_H_

namespace rev { namespace graphics3d {

	class RenderScene;
	class Camera;

	class Renderer {
	public:
		Renderer():mMaxRenderables(0){}
		~Renderer(){}

		virtual void render(const Camera& _pointOfView, const RenderScene& _scene) = 0;

		unsigned mMaxRenderables;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHIS3D_RENDERER_RENDERER_H_