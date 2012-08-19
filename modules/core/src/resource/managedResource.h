//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 19th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Resource Manager

#ifndef _REV_CORE_RESOURCE_MANAGEDRESOURCE_H_
#define _REV_CORE_RESOURCE_MANAGEDRESOURCE_H_

#include "resource.h"
#include "resourceManager.h"

namespace rev
{
	template<class Derived_, class Key_>
	class ManagedResourceBase : public Resource
	{
	protected:
		ManagedResourceBase();

	public:
		static void registerResource	(Derived_*, const Key_&);	// Register a resource (does not imply ownership)
		static void	release				(const _keyT&);				// Release a resource by key
			   bool	release				();

	private:
		typedef ResourceManager<Derived_, Key_>		Manager;
		static	Manager * manager();
		static 	Manager * sManager;

		bool	mRegistered;
	};

	// This interface allows different specializations to share common functionality through inheritance of the same base class
	template<class Derived_, class Key_, bool autocreate_>
	class ManagedResource {};

	// Not autocreate specialization
	template<class Derived_, class Key_>
	class ManagedResource<Derived_, Key_, false> : public ManagedResourceBase<Derived_,Key_>
	{
	public:
		static Derived_ *	get	(const Key_&);
	};

	// Autocreate specialization
	template<class Derived_, class Key_>
	class ManagedResource<Derived_, Key_, true> : public ManagedResourceBase<Derived_,Key_>
	{
	public:
		static Derived_ *	get	(const Key_&);
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_>
	ManagedResourceBase<Derived_, Key_>::ManagedResourceBase()
		:mRegistered(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_>
	void ManagedResourceBase<Derived_,Key_>::registerResource(Derived_* _res, const Key_& _key)
	{
		manager()->registerResource(_res,_key);
		_res->mRegistered = true;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_>
	void ManagedResourceBase<Derived_,Key_>::release(const Key_& _x)
	{
		manager()->release(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_>
	bool ManagedResourceBase<Derived_,Key_>::release()
	{
		if(mRegistered)
			return manager()->release(this);
		else
			return Resource::release();
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_>
	inline typename ManagedResourceBase<Derived_,Key_>::Manager * ManagedResourceBase<Derived_,Key_>::manager()
	{
		if(nullptr == sManager)
			sManager = new Manager();
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_,class Key_>
	Derived_ * ManagedResource<Derived_,Key_,false>::get(const Key_& _x)
	{
		return manager()->get(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_,class Key_>
	Derived_ * ManagedResource<Derived_,Key_,true>::get(const Key_& _x)
	{
		Derived_ resource = manager()->get(_x);
		if(nullptr == resource)
			return Derived_::factory(_x);
	}

}	// namespace rev

#endif // _REV_CORE_RESOURCE_MANAGEDRESOURCE_H_
