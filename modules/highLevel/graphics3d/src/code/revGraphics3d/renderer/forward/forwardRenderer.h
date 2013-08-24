//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/10
//----------------------------------------------------------------------------------------------------------------------
// basic forward renderer

#ifndef _REV_GRAPHIS3D_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_GRAPHIS3D_RENDERER_FORWARD_FORWARDRENDERER_H_

#include "../renderer.h"
#include <revMath/algebra/vector.h>
#include <map>

// Forward declarations
namespace rev { namespace video { 
	class Driver3d;
	class Image;
	class Shader;
}}

namespace rev { namespace graphics3d {

	class Renderable;

	class ForwardRenderer : public Renderer{
	public:
		ForwardRenderer();
		~ForwardRenderer(){}

		void render(const Camera& _pointOfView, const RenderScene& _scene);
		void swapLock() { mCamLocked = !mCamLocked;}

	private:
		void sortQueue(const math::Vec3f& _pointOfView, const RenderScene& _scene);

		video::Driver3d*	mDriver;
		
		video::Shader*		mBasicShader;
		int					mMvpUniform;
		int					mLightUniform;
		int					mViewPosUniform;
		int					mDiffTextUniform;
		video::Image*		mXorText;

		bool				mCamLocked;
		math::Vec3f			mCamLockPos;

		struct RenderDesc {
			const video::Image* texture;
			bool operator< (const RenderDesc& _d) const {
				return texture < _d.texture;
			}
			bool operator== (const RenderDesc& _d) const {
				return texture == _d.texture;
			}
		};
		std::multimap<RenderDesc, const Renderable*>	mRenderQueue;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHIS3D_RENDERER_FORWARD_FORWARDRENDERER_H_