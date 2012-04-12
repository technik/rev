////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, Shader tool
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 12th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Client

#ifndef _SHADERTOOL_CLIENT_SHADERTOOLCLIENT_H_
#define _SHADERTOOL_CLIENT_SHADERTOOLCLIENT_H_

#include <revGame/gameClient/gameClient.h>

// Forward declarations
namespace rev
{
	namespace video
	{
		class COrthoCamera;
		class CViewport;
		class CMaterialInstance;
	}	// namespace video
}	// namespace rev

class ShaderMaterial;
class ShaderSurface;
class SurfaceInstance;

class ShaderToolClient : public rev::game::CGameClient
{
public:
	ShaderToolClient();
	~ShaderToolClient();

	bool update();

private:
	rev::video::COrthoCamera*		mCamera;
	rev::video::CViewport*			mViewport;
	ShaderMaterial*					mMaterial;
	ShaderSurface*					mSurface;
	SurfaceInstance*				mSurfaceInstance;
	rev::video::CMaterialInstance*	mMaterialInstance;
};

#endif // _SHADERTOOL_CLIENT_SHADERTOOLCLIENT_H_