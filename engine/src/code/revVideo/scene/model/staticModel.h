////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model

#ifndef _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_
#define _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_

#include "revCore/resourceManager/resource.h"
#include "revCore/resourceManager/resourceManager.h"
#include "revCore/types.h"
#include "revVideo/scene/renderable.h"

namespace rev { namespace video {

	// Forward declaration
	class CVtxShader;
	
	class CStaticModel: public IRenderable, public TResource<CStaticModel, string>
	{
	public:
		typedef TResourceManager<CStaticModel, string> managerT;
	public:
		static managerT * manager();
		// -- Constructors & virtual destructor --
		CStaticModel(const string& fileName);
		~CStaticModel();

		CVtxShader * shader		() const { return sShader; }
		void	setEnviroment	() const;
		void	render			() const;
	private:
		static CVtxShader * sShader;
		static managerT * sManager;
	private:
		float*			mVertices;
		unsigned short	mNVertices;
		float*			mNormals;
		float*			mUVs;
		unsigned short*	mTriangles;
		unsigned short	mNTriangles;
		unsigned short*	mTriStrip;
		unsigned short	mStripLength;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline CStaticModel::managerT * CStaticModel::manager()
	{
		if (0 == sManager)
			sManager = new managerT();
		return sManager;
	}
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_
