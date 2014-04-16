//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2014/April/16
//----------------------------------------------------------------------------------------------------------------------
// camera (basically a point of view for render)

#ifndef _REV_GAME_SCENE_CAMERA_CAMERA_H_
#define _REV_GAME_SCENE_CAMERA_CAMERA_H_

#include <math/algebra/matrix.h>

namespace rev {
	namespace game {

		class Camera
		{
		public:
			Camera(const math::Mat34f& _view = math::Mat34f::identity(),
				const math::Mat44f& _proj = math::Mat44f::identity());

			const math::Mat34f& view		() const { return mView; };
			const math::Mat44f& projection	() const { return mProj; };

		protected:
			void			setProjection(const math::Mat44f& _proj) {
				mProj = _proj;
			}
			void			setView(const math::Mat34f& _view) {
				mView = _view;
			}

		private:
			math::Mat34f mView;
			math::Mat44f mProj;
		};

		//------------------------------------------------------------------------------------------------------------------
		// Inline implementation
		//------------------------------------------------------------------------------------------------------------------
		inline Camera::Camera(const math::Mat34f& _view, const math::Mat44f& _proj)
			:mView(_view)
			,mProj(_proj)
		{
		}

	}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GAME_SCENE_CAMERA_CAMERA_H_