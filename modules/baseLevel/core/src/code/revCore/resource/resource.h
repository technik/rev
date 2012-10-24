//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 19th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Base resource

#ifndef _REV_CORE_RESOURCE_RESOURCE_H_
#define _REV_CORE_RESOURCE_RESOURCE_H_

#include <codeTools/assert/assert.h>

namespace rev
{
	// Resource base class
	class Resource
	{
	public:
		void getOwnership(); // Gets the ownership of this resource, so it won't get deleted till you release it.
		virtual bool release	 (); // Release this resource. The resource will delete itself when all its owners release it.
			// Returns true if the resource is still active after release or false if it's been deleted.
	protected:
		Resource(); // Protected constructor prevents the resource to be created without inheritance.
		// Destruction.
		// Virtual destructor ensures correct destruction of derived classes
		virtual ~Resource();
	protected:
		unsigned mReferences;
	};

	// Inline implementation
	//----------------------------------------------------------------------------------------------------------------------
	inline void Resource::getOwnership()
	{
		++mReferences;
	}

	//----------------------------------------------------------------------------------------------------------------------
	inline bool Resource::release()
	{
		assert(mReferences > 0, "Internal error: Trying to release a resource with 0 references");
		--mReferences;
		if(0 == mReferences)
		{
			delete this;
			return false;
		}
		return true;
	}

	//----------------------------------------------------------------------------------------------------------------------
	inline Resource::Resource()
		:mReferences(1) // Resources always get constructed with one reference (i.e. creation implies ownership)
	{}

	//----------------------------------------------------------------------------------------------------------------------
	inline Resource::~Resource()
	{
		// Note: One reference deletion must be allowed so resources can be statically created in the stack.
		// But remember you should never ask for ownership of a stack allocated resource (That situation would assert when
		// the destructor gets called at the end of the resource's life time).
		assert(mReferences < 2, "Internal error: Trying to delete a resource with multiple references");
	}
}	// namespace rev

#endif // _REV_CORE_RESOURCE_RESOURCE_H_
