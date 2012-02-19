////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model

#include "staticModel.h"

#include "revCore/file/file.h"
#include "revCore/codeTools/assert/assert.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/vtxShader.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev {

	TResource<video::CStaticModel, string>::managerT * TResource<video::CStaticModel, string>::sManager = 0;
namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data
	CVtxShader * CStaticModel::sShader = 0;

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel::CStaticModel()
		:mVertices(0)
		,mNormals(0)
		,mUVs(0)
		,mTriangles(0)
		,mNTriangles(0)
		,mTriStrip(0)
		,mStripLength(0)
	{
		// Empty constructor. Use only if you're going to fill data in manually
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel::CStaticModel(const string& _fileName):
		mVertices(0),
		mNormals(0),
		mUVs(0),
		mTriangles(0),
		mNTriangles(0),
		mTriStrip(0),
		mStripLength(0)
	{
		// Cache static model shader
		if(0 == sShader)
			sShader = CVtxShader::manager()->get("staticModel.vtx");
		// Open the file
		char * buffer = bufferFromFile(_fileName.c_str());
		char * pointer;
		// Read the header
		unsigned short minEngineVersion = ((unsigned short*)buffer)[0];
		unsigned short nMeshes = ((unsigned short*)buffer)[1];

		// Validate model header
		rev::codeTools::revAssert(nMeshes == 1 && minEngineVersion <= 1);

		// Read model metrics from header
		mNVertices = ((unsigned short*)buffer)[2];
		mNTriangles = ((unsigned short*)buffer)[3];
		// Move on from the header
		pointer = buffer + 4*sizeof(unsigned short);

		// Read vertex positions
		mVertices = new float[mNVertices * 3];
		float * vBuffer = reinterpret_cast<float*>(pointer);
		for(unsigned i = 0; i < mNVertices; ++i)
		{
			mVertices[3*i+0] = vBuffer[3*i+0];
			mVertices[3*i+1] = vBuffer[3*i+1];
			mVertices[3*i+2] = vBuffer[3*i+2];
		}
		pointer += 3 * mNVertices * sizeof(float);
		// Read vertex normals
		mNormals = new float[mNVertices * 3];
		vBuffer = reinterpret_cast<float*>(pointer);
		for(unsigned i = 0; i < mNVertices; ++i)
		{
			mNormals[3*i+0] = vBuffer[3*i+0];
			mNormals[3*i+1] = vBuffer[3*i+1];
			mNormals[3*i+2] = vBuffer[3*i+2];
		}
		pointer += 3 * mNVertices * sizeof(float);
		// Read vertex texture coordinates
		mUVs = new float[mNVertices * 2];
		vBuffer = reinterpret_cast<float*>(pointer);
		for(unsigned i = 0; i < mNVertices ; ++i)
		{
			mUVs[2*i+0] = vBuffer[2*i];
			mUVs[2*i+1] = vBuffer[2*i+1];
		}
		pointer += 2 * mNVertices * sizeof(float);

		// Read face indices
		mTriangles = new unsigned short[mNTriangles * 3];
		unsigned short * idxBuffer = reinterpret_cast<unsigned short*>(pointer);
		for(unsigned i = 0; i < mNTriangles; ++i)
		{
			mTriangles[3*i+0] = idxBuffer[3*i+0];
			mTriangles[3*i+1] = idxBuffer[3*i+1];
			mTriangles[3*i+2] = idxBuffer[3*i+2];
		}
		// Clean
		delete buffer;
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel::~CStaticModel()
	{
		if(mVertices)
			delete[] mVertices;
		if(mNormals)
			delete[] mNormals;
		if(mUVs)
			delete[] mUVs;
		if(mTriangles)
			delete[] mTriangles;
		if(mTriStrip)
			delete[] mTriStrip;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModel::setEnviroment() const
	{
		IVideoDriver * driver = SVideo::get()->driver();
		driver->setRealAttribBuffer(IVideoDriver::eVertex, 3, mVertices);
		driver->setRealAttribBuffer(IVideoDriver::eNormal, 3, mNormals);
		driver->setRealAttribBuffer(IVideoDriver::eTexCoord, 2, mUVs);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModel::render() const
	{
		SVideo::get()->driver()->drawIndexBuffer(3*mNTriangles, mTriangles, false);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModel::setVertexData(unsigned short _count, float * _vPos, float * _vNrm, float * _vUV)
	{
		// Vertex positions
		if(0 != mVertices)
			delete mVertices;
		mVertices = _vPos;
		// Normals
		if(0 != mNormals)
			delete mNormals;
		mNormals = _vNrm;
		// UV texture coordinates
		if(0 != mUVs)
			delete mUVs;
		mUVs = _vUV;
		// Vertex count
		mNVertices = _count;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModel::setFaces(unsigned short _count, unsigned short * _indices)
	{
		// Update count
		mNTriangles = _count;
		// Update buffer
		if( 0 != mTriangles) // Delete old indices, if any
			delete mTriangles;
		mTriangles = _indices;
	}

}	// namespace video
}	// namespace rev
