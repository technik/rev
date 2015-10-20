//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#ifndef _REV_CORE_COMPONENT_TRANSFORMSRC_H_
#define _REV_CORE_COMPONENT_TRANSFORMSRC_H_

#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>
#include <math/algebra/quaternion.h>
#include <vector>

namespace rev {
	namespace core {

		class TransformSrc {
		public:
			// Constructor and virtual destructor
			TransformSrc() :mParent(0)
			{
				setPosition(math::Vec3f::zero());
				setRotation(math::Quatf::identity());
			}
			virtual ~TransformSrc() {}

			// Accessors for global coordinates
			const	math::Vec3f&	position	()	const; ///< \return global position
			const	math::Quatf&	rotation	()	const; ///< \return rotation in global coordinates
			const	math::Mat34f&	transform	()	const; ///< \return transoform in global coordinates

			// Accessors for local coordinates
			const	math::Vec3f&	localPosition	()	const; ///< \return position in parent's coordinates
			const	math::Quatf&	localRotation	()	const; ///< \return rotation in parent's coordinates
			const	math::Mat34f&	localTransform	()	const; ///< \return transform relative to parent transform source

			// Hierarchy traversal
			TransformSrc*	parent()	const;
			unsigned		nChildren()	const;
			TransformSrc*	child(unsigned _pos);
			void			attachTo(TransformSrc* _parent);
			void			dettach();

		protected:
			enum CoordinateSystem {
				local,
				global
			};

			void			setPosition(const math::Vec3f& _position, CoordinateSystem _base);
			void			setRotation(const math::Quatf& _rotation, CoordinateSystem _base);
			void			setTransform(const math::Mat34f& _transform, CoordinateSystem _base);

		private:
			// Methods for internal use
			void			refreshChildren	() const;
			void			refreshWorld	();
			void			setWorldPos		(const math::Vec3f& _pos);
			void			setLocalPos		(const math::Vec3f& _pos);
			void			setWorldRot		(const math::Quatf& _rot);
			void			setLocalRot		(const math::Quatf& _rot);
			// Internal data
			math::Vec3f					mLocalPos;
			math::Quatf					mLocalRot;
			math::Vec3f					mWorldPos;
			math::Quatf					mWorldRot;
			math::Mat34f				mWorldTrans;
			TransformSrc*				mParent;
			std::vector<TransformSrc*>	mChildren;
		};

		//--------------------------------------------------------------------------------------------------------------
		// Inline methods
		//--------------------------------------------------------------------------------------------------------------
		inline const math::Vec3f&	TransformSrc::position	() const { return mWorldPos; }
		inline const math::Quatf&	TransformSrc::rotation	() const { return mWorldRot; }
		inline const math::Mat34f&	TransformSrc::transform	() const { return mWorldTrans; }
		inline const math::Vec3f&	TransformSrc::localPosition	() const { return mWorldPos; }
		inline const math::Quatf&	TransformSrc::localRotation	() const { return mWorldRot; }
		inline const math::Mat34f&	TransformSrc::localTransform() const { return mWorldTrans; }

	}
}

#endif // _REV_CORE_COMPONENT_TRANSFORMSRC_H_
