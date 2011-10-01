////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// component interface

#ifndef _REV_REVCORE_COMPONENT_COMPONENT_H_
#define _REV_REVCORE_COMPONENT_COMPONENT_H_

#include "revCore/types.h"

namespace rev {
	class CEntity;

	class IComponent
	{
	public:
		// -- Constructor & destructor --
		IComponent	();
		virtual	~IComponent	();

		// -- attach and deattach --
		virtual	void	attachTo	(CEntity * _entity);
		virtual void	deattach	();

	protected:
		CEntity * getEntity() const;

	private:
		CEntity * mEntity;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline IComponent::IComponent():
		mEntity(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	inline CEntity * IComponent::getEntity() const
	{
		return mEntity;
	}

}	// namespace rev

#endif // _REV_REVCORE_COMPONENT_COMPONENT_H_
