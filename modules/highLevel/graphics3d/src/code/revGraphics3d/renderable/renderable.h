//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// base renderable type

#ifndef _REV_GRAPHICS3D_RENDERABLE_RENDERABLE_H_
#define _REV_GRAPHICS3D_RENDERABLE_RENDERABLE_H_

#include <revMath/algebra/matrix.h>

namespace rev { namespace graphics3d {

	class Renderable
	{
	public:
		Renderable	();
		~Renderable	();

		void	render			() const;

		void	setVertexData	(///< Set new vertex data (deletes any previous data)
								unsigned short _count,	///< \param _count how many vertices
								float * _vertPos,		///< \param _vertPos vertex positions
								float * _vertNrm,		///< \param _vertNrm vertex normals
								float * _vertUV);		///< \param _vertUV vertex uv coordinates

		void	setFaceIndices	(///< Set new triangled faces (deletes any previous faces)
								unsigned short _nIndices,	///< \param _count how many faces
								unsigned short * _indices,	///< \param _indices face indices
								bool _strip = false);		///< \param 

	public:
		math::Mat34f	m;

	private:
		float*			mVertices;
		unsigned short	mNVertices;
		float*			mNormals;
		float*			mUVs;
		unsigned short*	mTriangles;
		unsigned short	mNTriIndices;
		unsigned short*	mTriStrip;
		unsigned short	mStripLength;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHICS3D_RENDERABLE_RENDERABLE_H_