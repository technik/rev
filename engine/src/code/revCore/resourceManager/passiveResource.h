////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 28th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Passive resource

#ifndef _REV_REVCORE_RESOURCEMANAGER_PASSIVERESOURCE_H_
#define _REV_REVCORE_RESOURCEMANAGER_PASSIVERESOURCE_H_

#include "revCore/codeTools/assert/assert.h"
#include "revCore/string.h"

namespace rev
{
	// Forward declaration
	template<class _resourceT>
		class TPassiveResourceManager;

	template<typename _derivedResT>
	class TPassiveResource
	{
	public:
		// Constructor
		TPassiveResource();
		virtual ~TPassiveResource();

		// Public
		void	get() {++mReferences;}	// Gives you ownership on this resource

	public:
		typedef TPassiveResourceManager<_derivedResT> managerT;
		static managerT * manager();

	private:
		static managerT * sManager;

	private:
		unsigned	mReferences;
		string		mKey;

		friend class TPassiveResourceManager<_derivedResT>;
	};

	//------------------------------------------------------------------------------------------------------------------
	template<typename _derivedResT>
	inline typename TPassiveResource<_derivedResT>::managerT * TPassiveResource<_derivedResT>::manager()
	{
		if (0 == sManager)
			sManager = new managerT();
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _derivedResT>
	inline TPassiveResource<_derivedResT>::TPassiveResource(): mReferences(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename _derivedResT>
	inline TPassiveResource<_derivedResT>::~TPassiveResource()
	{
		codeTools::revAssert(0 == mReferences);
	}

}	// namespace rev

#endif // _REV_REVCORE_RESOURCEMANAGER_PASSIVERESOURCE_H_
