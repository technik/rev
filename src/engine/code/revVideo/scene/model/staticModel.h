////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model

#ifndef _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_
#define _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_

namespace rev { namespace video {
	
	class CStaticModel
	{
	public:
		CStaticModel();
		virtual ~CStaticModel();

		float*			vertices	() const;
		unsigned short*	triangles	() const;
		int				nTriangles	() const;
		unsigned short* triStrip	() const;
		int				stripLength	() const;
	protected:
		float*			mVertices;
		unsigned short*	mTriangles;
		int				mNTriangles;
		unsigned short*	mTriStrip;
		int				mStripLength;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_
