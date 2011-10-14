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
	class CNode;

	class IComponent
	{
	public:
		// -- Constructor & destructor --
		IComponent	();
		virtual	~IComponent	();

		// -- attach and deattach --
		virtual	void	attachTo	(CNode * _node);
		virtual void	deattach	();

	public:
		CNode * node() const;

	private:
		CNode * mNode;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline IComponent::IComponent():
		mNode(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	inline CNode * IComponent::node() const
	{
		return mNode;
	}

}	// namespace rev

#endif // _REV_REVCORE_COMPONENT_COMPONENT_H_
