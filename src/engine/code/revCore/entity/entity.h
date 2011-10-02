////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entity

#ifndef _REV_REVCORE_ENTITY_ENTITY_H_
#define _REV_REVCORE_ENTITY_ENTITY_H_

#include "revCore/math/vector.h"
#include "revCore/transform/transformSrc.h"
#include "revCore/types.h"
#include "rtl/poolset.h"

namespace rev
{
	// -- Forward references --
	class IComponent;
	class ITimeSrc;

	// Entities are containers of components that share the same transform and time source
	// You can attach components to an entity
	// You can attach an entity to a transform source (nodes, animations, rigid bodies, etc...)
	// You can subscribe an entity to a time source
	class CEntity
	{
	public:
		// Constructor & destructor
		CEntity();
		virtual ~CEntity();

		// Accessors
		TReal			deltaTime			()	const;
		const CVec3&	position			()	const;
		const CMat34&	transform			()	const;

		// Components
		void			addComponent		(IComponent * _component);
		void			removeComponent		(IComponent * _component);
		// Transform sources
		ITransformSrc*	transformSource		()	const;
		void			setTransformSource	(ITransformSrc * _source);
		// Time sources
		ITimeSrc*		timeSource			()	const;
		void			setTimeSource		(ITimeSrc* _source);

	private:
		rtl::poolset<IComponent*>	mComponents;
		ITransformSrc*				mTransformSrc;
		ITimeSrc*					mTimeSrc;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------
	inline const CVec3& CEntity::position() const
	{
		if(mTransformSrc)
			return mTransformSrc->position();
		else
			return CVec3::zero;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline const CMat34& CEntity::transform() const
	{
		if(mTransformSrc)
			return mTransformSrc->transform();
		else
			return CMat34::identity;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline ITransformSrc* CEntity::transformSource() const
	{
		return mTransformSrc;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CEntity::setTransformSource(ITransformSrc* _source)
	{
		mTransformSrc = _source;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline ITimeSrc* CEntity::timeSource() const
	{
		return mTimeSrc;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CEntity::setTimeSource(ITimeSrc* _source)
	{
		mTimeSrc = _source;
	}

}	// namespace rev

#endif // _REV_REVCORE_ENTITY_ENTITY_H_

