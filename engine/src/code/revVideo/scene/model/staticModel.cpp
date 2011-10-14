////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model

#include "staticModel.h"

#include "revCore/file/file.h"
#include "revCore/codeTools/assert/assert.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CStaticModel::CStaticModel():
		mVertices(0),
		mNormals(0),
		mUVs(0),
		mTriangles(0),
		mNTriangles(0),
		mTriStrip(0),
		mStripLength(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel::CStaticModel(const char * _fileName):
		mVertices(0),
		mNormals(0),
		mUVs(0),
		mTriangles(0),
		mNTriangles(0),
		mTriStrip(0),
		mStripLength(0)
	{
		// Open the file
		char * buffer = bufferFromFile(_fileName);
		char * pointer;
		// Read the header
		unsigned short minEngineVersion = ((unsigned short*)buffer)[0];
		unsigned short nMeshes = ((unsigned short*)buffer)[1];

		rev::codeTools::revAssert(nMeshes == 1 && minEngineVersion <= 1);

		mNVertices = ((unsigned short*)buffer)[2];
		mNTriangles = ((unsigned short*)buffer)[3];
		pointer = buffer + 4*sizeof(unsigned short);

		mVertices = new float[mNVertices * 3];
		float * vBuffer = reinterpret_cast<float*>(pointer);
		for(unsigned i = 0; i < mNVertices * unsigned(3); ++i)
		{
			mVertices[i] = vBuffer[i];
		}

		pointer += 3 * mNVertices * sizeof(float);

		mTriangles = new unsigned short[mNTriangles * 3];
		unsigned short * idxBuffer = reinterpret_cast<unsigned short*>(pointer);
		for(unsigned i = 0; i < mNTriangles * unsigned(3); ++i)
		{
			mTriangles[i] = idxBuffer[i];
		}
		// Clean
		delete buffer;
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel::~CStaticModel()
	{
		if(mVertices)
			delete mVertices;
		if(mTriangles)
			delete mTriangles;
	}

	//------------------------------------------------------------------------------------------------------------------
	float* CStaticModel::vertices() const
	{
		return mVertices;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned short CStaticModel::nVertices() const
	{
		return mNVertices;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModel::setVertices(unsigned short _nVertices, float * _vertices)
	{
		mNVertices = _nVertices;
		mVertices = _vertices;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned short* CStaticModel::triangles() const
	{
		return mTriangles;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned int CStaticModel::nTriangles() const
	{
		return mNTriangles;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModel::setTriangles(unsigned int _nTriangles, unsigned short * _triangles)
	{
		mNTriangles = _nTriangles;
		mTriangles = _triangles;
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
