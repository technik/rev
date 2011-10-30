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

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data
	CStaticModel::managerT * CStaticModel::sManager = 0;
	CVtxShader * CStaticModel::sShader = 0;

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
		if(mNormals)
			delete mNormals;
		if(mTriangles)
			delete mTriangles;
		if(mTriStrip)
			delete mTriStrip;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModel::setEnviroment() const
	{
		SVideo::get()->driver()->setRealAttribBuffer(IVideoDriver::eVertex, 3, mVertices);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModel::render() const
	{
		SVideo::get()->driver()->drawIndexBuffer(3*mNTriangles, mTriangles, false);
	}

}	// namespace video
}	// namespace rev
