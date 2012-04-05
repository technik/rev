////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh generator

#ifndef _REV_GAME_SCENE_MESH_MESHGENERATOR_H_
#define _REV_GAME_SCENE_MESH_MESHGENERATOR_H_

#include <revCore/types.h>

// Forward declarations
namespace rev
{
	class CVec2;
	class CVec3;
	namespace video { class CStaticModel; }
}	// namespace rev

namespace rev { namespace game
{
	class CMeshGenerator
	{
	public:
		static video::CStaticModel * plane		(const CVec2& size);
		static video::CStaticModel * box		(const CVec3& size);
		static video::CStaticModel * geoSphere	(TReal radius, unsigned nMeridians, unsigned nParallels);

	private:
		static void fillBoxFace(int faceIdx, const CVec3& size, CVec3 * verts, CVec3 * norms, CVec2 * uv);
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_MESH_MESHGENERATOR_H_