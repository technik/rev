////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entity

#ifndef _REV_REVCORE_NODE_NODE_H_
#define _REV_REVCORE_NODE_NODE_H_

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
	class CNode: public ITransformSrc
	{
	public:
		// Constructor & destructor
		CNode();
		virtual ~CNode();

		// Accessors
		TReal			deltaTime			()	const;
		void			move				(const CVec3& _translation);
		void			setPos				(const CVec3& _position);
		void			rotate				(const CVec3& _axis, TReal _angle);

		// Components
		void			addComponent		(IComponent * _component);
		void			removeComponent		(IComponent * _component);
		// Time sources
		ITimeSrc*		timeSource			()	const;
		void			setTimeSource		(ITimeSrc* _source);

	private:
		rtl::poolset<IComponent*>	mComponents;
		ITimeSrc*					mTimeSrc;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------
	inline void CNode::move(const CVec3& _translation)
	{
		setPosition(position() + _translation);
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CNode::setPos(const CVec3& _pos)
	{
		setPosition(_pos);
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CNode::rotate(const CVec3& _axis, TReal _angle)
	{
		setRotation(CQuat(_axis, _angle) * rotation());
	}

	//------------------------------------------------------------------------------------------------------------------
	inline ITimeSrc* CNode::timeSource() const
	{
		return mTimeSrc;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CNode::setTimeSource(ITimeSrc* _source)
	{
		mTimeSrc = _source;
	}

}	// namespace rev

#endif // _REV_REVCORE_NODE_NODE_H_

