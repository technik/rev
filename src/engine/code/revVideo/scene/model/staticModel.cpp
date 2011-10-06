////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model

#include "staticModel.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CStaticModel::CStaticModel()
	{
		mNTriangles = 1;
		mStripLength = 0;
		mTriangles = new unsigned short[3];
		mTriangles[0] = 0;
		mTriangles[1] = 1;
		mTriangles[2] = 2;
		mTriStrip = 0;
		mVertices = new float[3*3];
		mVertices[0] = 0.f;
		mVertices[1] = 1.f;
		mVertices[2] = 0.f;
		mVertices[3] = 1.f;
		mVertices[4] = -1.f;
		mVertices[5] = 0.f;
		mVertices[6] = -1.f;
		mVertices[7] = -1.f;
		mVertices[8] = 0.f;
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel::~CStaticModel()
	{
		delete mTriangles;
	}

	//------------------------------------------------------------------------------------------------------------------
	float* CStaticModel::vertices() const
	{
		return mVertices;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned short* CStaticModel::triangles() const
	{
		return mTriangles;
	}

	//------------------------------------------------------------------------------------------------------------------
	int CStaticModel::nTriangles() const
	{
		return mNTriangles;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned short* CStaticModel::triStrip() const
	{
		return mTriStrip;
	}

	//------------------------------------------------------------------------------------------------------------------
	int CStaticModel::stripLength() const
	{
		return mStripLength;
	}

}	// namespace video
}	// namespace rev
