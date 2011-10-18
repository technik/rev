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
		// -- Constructors & virtual destructor --
		CStaticModel();
		CStaticModel(const char * fileName);
		virtual ~CStaticModel();

		// --- Accessor methods ---
		// Buffers
		float*			vertices	() const;
		unsigned short	nVertices	() const;
		void			setVertices	(unsigned short _nVertices, float* _vertices);
		float*			normals		() const;
		void			setNormals	(float* _normals);
		float*			uvs			() const;
		void			setUVs		(float* _uvs);
		// Indices
		unsigned short*	triangles	() const;
		unsigned int	nTriangles	() const;
		void			setTriangles(unsigned short _nTriangles, unsigned short* _indices);
		unsigned short* triStrip	() const;
		int				stripLength	() const;
	protected:
		float*			mVertices;
		unsigned short	mNVertices;
		float*			mNormals;
		float*			mUVs;
		unsigned short*	mTriangles;
		unsigned short	mNTriangles;
		unsigned short*	mTriStrip;
		unsigned short	mStripLength;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_
