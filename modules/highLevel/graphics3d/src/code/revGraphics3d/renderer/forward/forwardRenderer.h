//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/10
//----------------------------------------------------------------------------------------------------------------------
// basic forward renderer

#ifndef _REV_GRAPHIS3D_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_GRAPHIS3D_RENDERER_FORWARD_FORWARDRENDERER_H_

#include "../renderer.h"

// Forward declarations
namespace rev { namespace video { 
	class Driver3d;
	class Image;
	class Shader;
}}

namespace rev { namespace graphics3d {

	class ForwardRenderer : public Renderer{
	public:
		ForwardRenderer();
		~ForwardRenderer(){}

		void render(const Camera& _pointOfView, const RenderScene& _scene);

	private:
		video::Driver3d*	mDriver;
		
		video::Shader*		mBasicShader;
		int					mMvpUniform;
		int					mLightUniform;
		int					mViewPosUniform;
		int					mDiffTextUniform;
		video::Image*		mXorText;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHIS3D_RENDERER_FORWARD_FORWARDRENDERER_H_