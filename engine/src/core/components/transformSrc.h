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
			TransformSrc() 
				:mParent(0)
				,mLocalTransform(math::Mat34f::identity())
			{
			}
			virtual ~TransformSrc() {}

			// Accessors for local coordinates
			const	math::Vec3f&	position	()	const;
			const	math::Quatf&	rotation	()	const;
			const	math::Mat34f&	transform	()	const;

			// Accessors for local coordinates
			const	math::Vec3f&	worldPosition	()	const;
			const	math::Quatf&	worldRotation	()	const;
			const	math::Mat34f&	worldTransform	()	const;

			// Hierarchy traversal
			TransformSrc*	parent		()	const;
			unsigned		nChildren	()	const;
			TransformSrc*	child		(unsigned _pos);
			void			attachTo	(TransformSrc* _parent);
			void			dettach		();

		protected:
			enum CoordinateSystem {
				local,
				global
			};

			// Interface for derived classes to update source's state
			void			setRotation			(const math::Quatf& _rotation);
			void			setPosition			(const math::Vec3f& _position);
			void			setTransform		(const math::Mat34f& _transform);
			void			setWorldPosition	(const math::Vec3f& _position);
			void			setWorldRotation	(const math::Quatf& _rotation);
			void			setWorldTransform	(const math::Mat34f& _transform);

		private:
			// Methods for internal use
			void			refreshChildren	() const;
			void			refreshWorld	();

			// Internal data
			math::Mat34f				mLocalTransform;
			TransformSrc*				mParent;
			std::vector<TransformSrc*>	mChildren;
		};

		//--------------------------------------------------------------------------------------------------------------
		// Inline methods
		//--------------------------------------------------------------------------------------------------------------
		inline const math::Vec3f&	TransformSrc::position	() const { return mLocalTransform.col(3); }
		inline const math::Quatf&	TransformSrc::rotation	() const { return Quatf(mLocalTransform); }
		inline const math::Mat34f&	TransformSrc::transform	() const { return mLocalTransform; }
		inline TransformSrc*		TransformSrc::parent	()	const { return mParent;  }
		inline unsigned				TransformSrc::nChildren	()	const { return mChildren.size(); }
		inline TransformSrc*		TransformSrc::child		(unsigned _pos) { return mChildren[_pos]; }
	}
}

#endif // _REV_CORE_COMPONENT_TRANSFORMSRC_H_
