////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, model processor
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on November 20th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry data

// Engine headers
#include <revVideo/scene/model/staticModel.h>

#include "geometry.h"

using namespace rev::video;

namespace modelProcessor
{
	//------------------------------------------------------------------------------------------------------------------
	void CGeometry::addFace( const TVertexInfo& _v1, const TVertexInfo& _v2, const TVertexInfo& _v3)
	{
		unsigned short id1, id2, id3;
		// Get the proper index for each vertex
		id1 = vertexIndex(_v1);
		id2 = vertexIndex(_v2);
		id3 = vertexIndex(_v3);

		// Add indices
		mFaceIndices.push_back(id1);
		mFaceIndices.push_back(id2);
		mFaceIndices.push_back(id3);
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned short CGeometry::vertexIndex( const TVertexInfo& _vtx )
	{
		unsigned queueSize = mVtxQueue.size();
		for(unsigned short i = 0; i < queueSize; ++i)
		{
			if( mVtxQueue[i] == _vtx )
			{
				return i;
			}
		}

		// If we get here, it means the vertex isn't stored yet.
		// So we add it.
		mVtxQueue.push_back(_vtx);
		return (unsigned short(mVtxQueue.size() - 1));
	}

	//------------------------------------------------------------------------------------------------------------------
	void CGeometry::fillModel(CStaticModel * _model) const
	{
		// Allocate memory for vertex data buffers
		float * vertPos = new float[3 * mVtxQueue.size()]; // Vertex positions
		float * vertNrm = new float[3 * mVtxQueue.size()]; // Vertex normals
		float * vertUV  = new float[2 * mVtxQueue.size()]; // Vertex uv coordinates
		// Fill vextex data
		for( unsigned int i = 0; i < mVtxQueue.size(); ++i)
		{
			// 3 components per vertex position
			vertPos[3*i + 0] = mVPositions[mVtxQueue[i].mVIdx-1].x;
			vertPos[3*i + 1] = mVPositions[mVtxQueue[i].mVIdx-1].y;
			vertPos[3*i + 2] = mVPositions[mVtxQueue[i].mVIdx-1].z;
			// 3 components per vertex normal
			vertNrm[3*i + 0] = mNormals[mVtxQueue[i].mNIdx-1].x;
			vertNrm[3*i + 1] = mNormals[mVtxQueue[i].mNIdx-1].y;
			vertNrm[3*i + 2] = mNormals[mVtxQueue[i].mNIdx-1].z;
			// 2 components per texture coordinate
			vertUV[2*i + 0] = mTexCoord[mVtxQueue[i].mTIdx-1].x;
			vertUV[2*i + 1] = mTexCoord[mVtxQueue[i].mTIdx-1].y;
		}
		// Allocate memory for face indices
		unsigned short * faces = new unsigned short [mFaceIndices.size()];
		// Fill face indices
		for(unsigned short i = 0; i < mFaceIndices.size(); ++i)
		{
			faces[i] = mFaceIndices[i]; // So we start from index 0
		}

		// Set data to the model
		_model->setVertexData(unsigned short(mVtxQueue.size()), vertPos, vertNrm, vertUV);
		_model->setFaceIndices(unsigned short(mFaceIndices.size()), faces);
	}

}	// namespace modelProcessor