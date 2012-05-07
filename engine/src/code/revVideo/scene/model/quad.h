////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple renderable quads

#ifndef _REV_RREVVIDEO_SCENE_MODEL_QUAD_H_
#define _REV_RREVVIDEO_SCENE_MODEL_QUAD_H_

#include "revVideo/scene/renderable.h"

namespace rev
{ 
	class CVec2;
	namespace video
{
	// Forward declaration
	class CVtxShader;

	class CQuad: public IRenderable
	{
	public:
		// Constructor and destructor
		CQuad(const CVec2& _size);
		~CQuad();

		// Renderable interface
		CVtxShader * shader () const;
		void setEnvironment	() const;
		void render			() const;

	private:
		CVtxShader * mShader;
		float * mVertices;
		float * mUVs;
		unsigned short * mIndices;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline CVtxShader * CQuad::shader() const
	{
		return mShader;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_RREVVIDEO_SCENE_MODEL_QUAD_H_
