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
	template<class Derived_, class Key_, class Hasher_>
	class ManagedResourceBase : public Resource
	{
	protected:
		ManagedResourceBase();
		virtual ~ManagedResourceBase() = 0; // So that it can't be instantiated without inheritance

	public:
		static void registerResource	(Derived_*, const Key_&);	// Register a resource (does not imply ownership)
		static void	release				(const Key_&);				// Release a resource by key
			   bool	release				();

	public:
		typedef ResourceManager<Derived_, Key_, Hasher_>		Manager;
	protected:
		static	Manager * manager();
		static 	Manager * sManager;

		bool	mRegistered;
	};

	// Simple managed resource
	template<class Derived_, class Key_, class Hasher_ = std::tr1::hash<Key_> >
	class ManagedResource : public ManagedResourceBase<Derived_,Key_,Hasher_>
	{
	public:
		static Derived_ *	get	(const Key_&);
	};

	// Factory managed resource (derived class must feature a factory method)
	template<class Derived_, class Key_, class Hasher_ = std::tr1::hash<Key_> >
	class FactoryManagedResource : public ManagedResourceBase<Derived_,Key_,Hasher_>
	{
	public:
		static Derived_ *	get	(const Key_&);
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	// Destructor body
	template<class Derived_, class Key_, class Hasher_>
	ManagedResourceBase<Derived_,Key_,Hasher_>::~ManagedResourceBase()
	{
		// Intentionally blank
	}

	template<class Derived_, class Key_, class Hasher_>
	ManagedResourceBase<Derived_, Key_, Hasher_>::ManagedResourceBase()
		:mRegistered(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_, class Hasher_>
	void ManagedResourceBase<Derived_,Key_, Hasher_>::registerResource(Derived_* _res, const Key_& _key)
	{
		manager()->registerResource(_res,_key);
		_res->mRegistered = true;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_, class Hasher_>
	void ManagedResourceBase<Derived_,Key_, Hasher_>::release(const Key_& _x)
	{
		manager()->release(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_, class Hasher_>
	bool ManagedResourceBase<Derived_,Key_, Hasher_>::release()
	{
		if(mRegistered)
			return manager()->release(static_cast<Derived_*>(this));
		else
			return Resource::release();
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_, class Key_, class Hasher_>
	inline typename ManagedResourceBase<Derived_,Key_, Hasher_>::Manager * ManagedResourceBase<Derived_,Key_,Hasher_>::manager()
	{
		if(nullptr == sManager)
			sManager = new Manager();
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_,class Key_, class Hasher_>
	Derived_ * ManagedResource<Derived_,Key_, Hasher_>::get(const Key_& _x)
	{
		return ManagedResource<Derived_,Key_,Hasher_>::manager()->get(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Derived_,class Key_, class Hasher_>
	Derived_ * FactoryManagedResource<Derived_,Key_, Hasher_>::get(const Key_& _x)
	{
		Derived_ * resource = FactoryManagedResource<Derived_,Key_,Hasher_>::manager()->get(_x);
		if(nullptr == resource)
			resource = Derived_::factory(_x);
		return resource;
	}

}	// namespace rev

#endif // _REV_CORE_RESOURCE_MANAGEDRESOURCE_H_
