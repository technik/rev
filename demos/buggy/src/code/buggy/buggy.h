////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BUGGYDEMO_BUGGY_BUGGY_H_
#define _BUGGYDEMO_BUGGY_BUGGY_H_

// Forward declarations
namespace rev { class CNode; }
namespace rev { namespace game { class CStaticObject; } }

namespace buggyDemo
{
	class CBuggy
	{
	public:
		// Constructor and destructor
		CBuggy();
		~CBuggy();

		// Update
		void update ();

		// Accessor methods
		rev::CNode * node() const;

	private:
		rev::game::CStaticObject * mObject;
	};
}	// namespace buggyDemo

#endif // _BUGGYDEMO_BUGGY_BUGGY_H_