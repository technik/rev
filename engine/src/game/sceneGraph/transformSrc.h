//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/12
//----------------------------------------------------------------------------------------------------------------------
// Generic transform source

#ifndef _REV_GAME_SCENEGRAPH_TRANSFORMSRC_H_
#define _REV_GAME_SCENEGRAPH_TRANSFORMSRC_H_

#include <math/algebra/matrix.h>
#include <math/algebra/quaternion.h>
#include <math/algebra/vector.h>

#include <core/containers/vector.h>

namespace rev { namespace game {

	class TransformSrc
	{
	public:
		virtual ~TransformSrc() = 0;

		void attachTo (TransformSrc* _parent);
		void releaseChildren();
		// Note: Any children attached to this will be deleted on parent destruction

		// World referenced accessors
		const math::Vec3f&	position	() const;
		const math::Quatf&	rotation	() const;
		const math::Mat34f&	transform	() const;

	protected:
		TransformSrc();

		void setLocalPos	(const math::Vec3f&, bool _refreshChildren);
		void setLocalRot	(const math::Quatf&, bool _refreshChildren);
		void setLocalTrans	(const math::Mat34f&, bool _refreshChildren);
		void setWorldPos	(const math::Vec3f&, bool _refreshChildren);
		void setWorldRot	(const math::Quatf&, bool _refreshChildren);
		void setWorldTrans	(const math::Mat34f&, bool _refreshChildren);

		const math::Vec3f& localPos() const { return mLocalPos; }
		const math::Quatf& localRot() const { return mLocalRot; }
		const math::Mat34f& localTrans() const { return mLocalTrans; }
		const math::Vec3f& worldPos() const { return mWorldPos; }
		const math::Quatf& worldRot() const { return mWorldRot; }
		const math::Mat34f& worldTrans() const { return mWorldTrans; }

	private:
		void deattach			();
		void refreshChildren	();
		void refreshLocal		();
		void refreshWorld		();

	private:
		// Transform info
		math::Vec3f		mLocalPos;
		math::Quatf		mLocalRot;
		math::Mat34f	mLocalTrans;
		math::Vec3f		mWorldPos;
		math::Quatf		mWorldRot;
		math::Mat34f	mWorldTrans;

		// Hierarchy
		TransformSrc*				mParent;
		std::vector<TransformSrc*>	mChildren;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	inline const math::Vec3f&	TransformSrc::position	() const { return mWorldPos; }
	inline const math::Quatf&	TransformSrc::rotation	() const { return mWorldRot; }
	inline const math::Mat34f&	TransformSrc::transform	() const { return mWorldTrans; }

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENEGRAPH_TRANSFORMSRC_H_
