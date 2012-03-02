////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BUGGYDEMO_BUGGY_BUGGY_H_
#define _BUGGYDEMO_BUGGY_BUGGY_H_

// Forward declarations
namespace rev {
	class CNode;
	namespace video
	{
		class CStaticModelInstance;
		class IMaterialInstance;
	}	// namespace video
}	// namespace rev

namespace buggyDemo
{
	class CBuggy
	{
	public:
		// -- Constructor and destructor -------------------------------------------------------------------------------
		CBuggy();
		~CBuggy();

		// -- Update ---------------------------------------------------------------------------------------------------
		void update ();

		// -- Accessor methods -----------------------------------------------------------------------------------------
		const rev::CNode * node() const;

	private:
		rev::CNode * mNode;	// Buggy main node
		rev::video::CStaticModelInstance * mModelInstance; // Buggy model
		rev::video::IMaterialInstance * mMaterialInstance;
	};
}	// namespace buggyDemo

#endif // _BUGGYDEMO_BUGGY_BUGGY_H_