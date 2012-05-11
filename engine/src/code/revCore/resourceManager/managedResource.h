////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 4th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Managed resource

#ifndef _REV_CORE_RESOURCEMANAGER_MANAGEDRESOURCE_H_
#define _REV_CORE_RESOURCEMANAGER_MANAGEDRESOURCE_H_

#include "resource.h"
#include "resourceManager.h"

namespace rev
{
	template<class _derivedT, class _keyT, bool _autocreate>
	class IManagedResource : public IResource
	{
	public:
		IManagedResource();
		static _derivedT *	get				(_keyT);			// Get a resource by key (implies ownership)
		static void			registerResource(_derivedT*, _keyT);// Register a resource (does not imply ownership)
		static void			release			(_keyT);			// Release a resource by key
		static void			release			(_derivedT*);		// Release a resource given it's pointer
			   bool			release			();

	public:
		typedef CResourceManager<_derivedT, _keyT>	TManager;
	private:
		static	TManager * manager();
		static 	TManager * sManager;

		bool	mRegistered;
	};

	// Autocreate specialization
	template<class _derivedT, class _keyT>
	class IManagedResource<_derivedT, _keyT, true> : public IResource
	{
	public:
		IManagedResource();
		static _derivedT *	get				(_keyT);			// Get a resource by key (implies ownership)
		static void			registerResource(_derivedT*, _keyT);// Register a resource (does not imply ownership)
		static void			release			(_keyT);			// Release a resource by key
		static void			release			(_derivedT*);		// Release a resource given it's pointer
			   bool			release			();

		using IResource::release;	// Unhide IResource::release methods

	public:
		typedef CResourceManager<_derivedT, _keyT>	TManager;
	private:
		static	TManager * manager();
		static 	TManager * sManager;

		bool mRegistered;
	};

	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	IManagedResource<_derivedT,_keyT,_autocreate>::IManagedResource()
		:mRegistered(false)
	{
	}
	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	IManagedResource<_derivedT,_keyT,true>::IManagedResource()
		:mRegistered(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	_derivedT * IManagedResource<_derivedT,_keyT,_autocreate>::get(_keyT _x)
	{
		return manager()->get(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT> // Partial specialization for autocreate
	_derivedT * IManagedResource<_derivedT,_keyT,true>::get(_keyT _x)
	{
		TManager * mgr = manager(); // Get resource manager
		_derivedT * res = mgr->get(_x);
		if( ! res )
		{
			res = new _derivedT(_x);
			res->mRegistered = true;
			mgr->registerResource(res, _x);
		}
		return res;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	void IManagedResource<_derivedT,_keyT,_autocreate>::registerResource(_derivedT * _res, _keyT _x)
	{
		_res->mRegistered = true;
		manager()->registerResource(_res, _x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	void IManagedResource<_derivedT,_keyT,true>::registerResource(_derivedT * _res, _keyT _x)
	{
		_res->mRegistered = true;
		manager()->registerResource(_res, _x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	void IManagedResource<_derivedT,_keyT,_autocreate>::release(_keyT _x)
	{
		manager()->release(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	void IManagedResource<_derivedT,_keyT,true>::release(_keyT _x)
	{
		manager()->release(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	void IManagedResource<_derivedT,_keyT,_autocreate>::release(_derivedT * _res)
	{
		manager()->release(_res);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	void IManagedResource<_derivedT,_keyT,true>::release(_derivedT * _res)
	{
		manager()->release(_res);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	bool IManagedResource<_derivedT,_keyT,_autocreate>::release()
	{
		revAssert(mReferences > 0, "Internal error: Trying to release a resource with 0 references");
		--mReferences;
		if(0 == mReferences)
		{
			if(mRegistered)
			{
				mRegistered = false;
				mReferences = 1;
				manager()->release(static_cast<_derivedT*>(this));
			}
			else
				delete this;
			return false;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	bool IManagedResource<_derivedT,_keyT,true>::release()
	{
		revAssert(mReferences > 0, "Internal error: Trying to release a resource with 0 references");
		--mReferences;
		if(0 == mReferences)
		{
			if(mRegistered)
			{
				mRegistered = false;
				mReferences = 1;
				manager()->release(static_cast<_derivedT*>(this));
			}
			else
				delete this;
			return false;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	typename IManagedResource<_derivedT,_keyT,_autocreate>::TManager * IManagedResource<_derivedT,_keyT,_autocreate>::manager()
	{
		if(!sManager)
			sManager = new TManager();
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	typename IManagedResource<_derivedT,_keyT,true>::TManager * IManagedResource<_derivedT,_keyT,true>::manager()
	{
		if(!sManager)
			sManager = new TManager();
		return sManager;
	}

}	// namespace rev

#endif // _REV_CORE_RESOURCEMANAGER_MANAGEDRESOURCE_H_
