//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/12
//----------------------------------------------------------------------------------------------------------------------
// Generic transform source

#ifndef _REV_GAME_CORE_TRANSFORM_TRANSFORMSRC_H_
#define _REV_GAME_CORE_TRANSFORM_TRANSFORMSRC_H_

#include <revMath/algebra/matrix.h>
#include <revMath/algebra/quaternion.h>
#include <revMath/algebra/vector.h>

#include <vector>

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

	private:
		void deattach			();
		void refreshChildren	();
		void refreshCacheCoords	();

	private:
		// Transform info
		math::Vec3f		mLocalPos;
		math::Quatf		mLocalRot;
		math::Mat34f	mWorldTrans;
		// Cache
		math::Vec3f		mWorldPos;
		math::Quatf		mWorldRot;

		// Hierarchy
		TransformSrc*				mParent;
		std::vector<TransformSrc*>	mChildren;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	const math::Vec3f& TransformSrc::position() const { return mWorldPos; }
	const math::Quatf& TransformSrc::rotation() const { return mWorldRot; }
	const math::Mat34f& TransformSrc::transform() const { return mWorldTrans; }

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_CORE_TRANSFORM_TRANSFORMSRC_H_
