////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 28th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resource

#ifndef _REV_REVCORE_RESOURCEMANAGER_RESOURCE_H_
#define _REV_REVCORE_RESOURCEMANAGER_RESOURCE_H_

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

#endif // _REV_REVCORE_RESOURCEMANAGER_RESOURCE_H_
