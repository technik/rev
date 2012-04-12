////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, Shader tool
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 12th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Render surface

#include <revCore/math/vector.h>
#include <revVideo/scene/renderable.h>
#include <revVideo/scene/renderableInstance.h>

// Forward declarations
namespace rev { namespace video { class CVtxShader; }}

class ShaderSurface : public rev::video::IRenderable
{
public:
	// Create a surface shader from the given file
	ShaderSurface(const char * _vtxShaderFile);
	~ShaderSurface();

private:
	// Shader interface
	rev::video::CVtxShader*	shader			() const;
	void					setEnvironment	() const;
	void					render			() const;

private:
	rev::video::CVtxShader* mShader;
	rev::CVec2		mCoordinates[3];	// Three coordinate pairs, one per vertex
	unsigned short	mIndices[3];		// Indices
};

class SurfaceInstance : public rev::video::IRenderableInstance
{
public:
	SurfaceInstance(rev::video::IRenderable * _renderable = 0, rev::video::CMaterialInstance * _material = 0)
		:rev::video::IRenderableInstance(_renderable, _material ) {}
	void setEnvironment() const {}
};