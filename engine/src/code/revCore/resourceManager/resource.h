////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 4th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resource

#ifndef _REV_REVCORE_RESOURCEMANAGER_RESOURCE_H_
#define _REV_REVCORE_RESOURCEMANAGER_RESOURCE_H_

#include <revCore/codeTools/assert/assert.h>

namespace rev
{
	// Resource base class
	class IResource
	{
	public:
		void getOwnership(); // Gets the ownership of this resource, so it won't get deleted till you release it.
		virtual bool release	 (); // Release this resource. The resource will delete itself when all its owners release it.
			// Returns true if the resource is still active after release or false if it's been deleted.
	protected:
		IResource();
		// Destruction.
		// Virtual destructor ensures correct destruction of derived classes
		virtual ~IResource();
	protected:
		unsigned mReferences;
	};

	// Inline implementation
	//----------------------------------------------------------------------------------------------------------------------
	inline void IResource::getOwnership()
	{
		++mReferences;
	}

	//----------------------------------------------------------------------------------------------------------------------
	inline bool IResource::release()
	{
		revAssert(mReferences > 0, "Internal error: Trying to release a resource with 0 references");
		--mReferences;
		if(0 == mReferences)
		{
			delete this;
			return false;
		}
		return true;
	}

	//----------------------------------------------------------------------------------------------------------------------
	inline IResource::IResource()
		:mReferences(1) // Resources always get constructed with one reference (i.e. creation implies ownership)
	{}

	//----------------------------------------------------------------------------------------------------------------------
	inline IResource::~IResource()
	{
		// One reference deletion is allowed so resources can be created locally.
		revAssert(mReferences < 2, "Internal error: Trying to delete a resource with multiple references");
	}
}	// namespace rev
/*

#include "revCore/codeTools/assert/assert.h"

namespace rev
{
	// Forward declaration
	template<class _resourceT, typename _keyT>
		class TResourceManager;

	template<typename _derivedResT, typename _keyT>
	class TResource
	{
	public:
		// Constructor
		TResource();
		virtual ~TResource();

		// Public
		void	get() {++mReferences;}	// Gives you ownership on this resource

	public:
		typedef TResourceManager<_derivedResT, _keyT> managerT;
		static managerT * manager();

	private:
		static managerT * sManager;

	private:
		unsigned	mReferences;
		_keyT		mKey;

		friend class TResourceManager<_derivedResT, _keyT>;
	};

	//------------------------------------------------------------------------------------------------------------------
	template<typename _derivedResT, typename _keyT>
	inline typename TResource<_derivedResT, _keyT>::managerT * TResource<_derivedResT, _keyT>::manager()
	{
		if (0 == sManager)
			sManager = new managerT();
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _derivedResT, typename _keyT>
	inline TResource<_derivedResT, _keyT>::TResource(): mReferences(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _derivedResT, typename _keyT>
	inline TResource<_derivedResT, _keyT>::~TResource()
	{
		//codeTools::revAssert(0 == mReferences);
	}

}	// namespace rev
*/
#endif // _REV_REVCORE_RESOURCEMANAGER_RESOURCE_H_
