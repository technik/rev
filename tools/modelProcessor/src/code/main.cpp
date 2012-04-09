////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, model processor
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on October 6th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entry point
// Standard headers
#include <string>
#include <fstream>

// Engine headers
#include <revCore/codeTools/assert/assert.h>
#include <revCore/file/file.h>
#include <revCore/math/vector.h>
#include <revVideo/scene/model/staticModel.h>
#include <vector.h>

// Project headers
#include <loaders/obj/objLoader.h>

enum EInputArguments
{
#ifdef _WIN32
	eInputFileArgId = 1,
#else
	eInputFileArgId = 0,
#endif
	eOutputFileArgId
};

// Active namespaces
using namespace rev;
using namespace rev::video;

// Function prototypes
namespace modelProcessor
{
CStaticModel * loadObjModel(const char * _fileName);
void saveRmdModel(const CStaticModel * _model, const char * _fileName);
}	// namespace modelProcessor

// main
int main (int _argc, const char ** _argv)
{
	// ---- Input and output file names ----
	if(_argc <= eInputFileArgId)
		return -1;
	string inputFileName(_argv[eInputFileArgId]);
	string outputFileName;
	if(_argc > eOutputFileArgId)
	{
		// There are more than one parameter, so we have both input and output file names
		outputFileName = string(_argv[eOutputFileArgId]);
	}
	else
	{
		// We've only been given an input file name, so we append the new extension to it.
		outputFileName = inputFileName + ".rmd";
	}

	IModelLoader * loader = new CObjLoader();
	// Import obj
	CStaticModel * model = loader->modelFromFile(inputFileName.c_str());
	if(!model)
		return -1;
	// Save it as rmd
	modelProcessor::saveRmdModel(model, outputFileName.c_str());

	// House keeping
	delete loader;
	delete model;
	return 0;
}

namespace modelProcessor
{
	//------------------------------------------------------------------------------------------------------------------
	const char * nextLine(const char * _buffer)
	{
		unsigned i = 0;
		while(_buffer[i] != '\0') // Run the test till we run out of buffer
		{
			if(_buffer[i] == '\n')
				return &_buffer[i+1];
			i++;
		}
		// If we got here, it means the buffer is over, so we return an empty string
		return '\0';
	}

	//------------------------------------------------------------------------------------------------------------------
	float * bufferFromVector(rtl::vector<CVec3>& _vector)
	{
		unsigned vectorSize = _vector.size();
		float * buffer = new float[3*vectorSize];
		unsigned idx = 0;
		for(rtl::vector<CVec3>::iterator i = _vector.begin(); idx < vectorSize; ++idx, ++i)
		{
			buffer[3*idx+0] = (*i).x;
			buffer[3*idx+1] = (*i).y;
			buffer[3*idx+2] = (*i).z;
		}
		return buffer;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned short * bufferFromVector(rtl::vector<unsigned short>& _vector)
	{
		unsigned vectorSize = _vector.size();
		unsigned short * buffer = new unsigned short[vectorSize];
		unsigned idx = 0;
		for(rtl::vector<unsigned short>::iterator i = _vector.begin(); idx < vectorSize; ++idx, ++i)
		{
			buffer[idx] = (*i);
		}
		return buffer;
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel * loadObjModel(const char * _fileName)
	{
		// Open the input file
		CFile modelFile(_fileName);
		const char * buffer = modelFile.textBuffer();
		// ---------------- Read the data from the buffer ------------------------
		const char * line = buffer;
		CGeometry geometry;
		// Parse the buffer
		while(line)
		{
			// Check the content of the line
			if(line[0] == 'v') // Vertex data
			{
				if(line[1] == 'n') // Vertex normal
				{
					CVec3 normal;
					sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
					geometry.addNormal(normal);
				}
				else if(line[1] == 't') // Texture coordinate
				{
					CVec2 uv;
					sscanf(line, "vt %f %f", &uv.x, &uv.y);
					geometry.addTexCoord(uv);
				}
				else // Vertex position
				{
					CVec3 vertex;
					sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
					geometry.addVertexPos(vertex);
				}
			}else if(line[0] == 'f') // Face
			{
				// Read the triangle
				TVertexInfo v1, v2, v3;
				
				sscanf(line, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
							&v1.mVIdx, &v1.mTIdx, &v1.mNIdx,
							&v2.mVIdx, &v2.mTIdx, &v2.mNIdx,
							&v3.mVIdx, &v3.mTIdx, &v3.mNIdx);
				geometry.addFace(v1, v2, v3);
			}
			line = nextLine(line);
		}
		// Create the model
		CStaticModel * model = new CStaticModel();
		// Fill the data
		geometry.fillModel(model);
		return model;
	}

	//------------------------------------------------------------------------------------------------------------------
	void saveRmdModel(const CStaticModel * _model, const char * _fileName)
	{
		// Open the input file
		std::fstream file;
		file.open(_fileName, std::fstream::binary | std::fstream::out);
		rev::codeTools::revAssert(file.is_open());
		rev::codeTools::revAssert(file.good());
		// Write model header
		// Just one mesh supported right now
		unsigned short minEngineVersion = 1;
		unsigned short nMeshes = 1;
		file.write((const char*)&minEngineVersion, sizeof(unsigned short));
		file.write((const char*)&nMeshes, sizeof(unsigned short));
		// Write each mesh
		//	Mesh header
		unsigned short nVertices = _model->nVertices();
		unsigned short nTriangles = _model->nTriIndices() / 3; //>nTriangles();
		file.write((const char*)&nVertices, sizeof(unsigned short));
		file.write((const char*)&nTriangles, sizeof(unsigned short));
		//	Buffers
		file.write((const char*)_model->vertices(), 3 * _model->nVertices() * sizeof(float));
		file.write((const char*)_model->normals(), 3 * _model->nVertices() * sizeof(float));
		file.write((const char*)_model->uvs(), 2 * _model->nVertices() * sizeof(float));
		//	Indices
		file.write((const char*)_model->triangles(), _model->nTriIndices() * sizeof(unsigned short));
		// Clear
		file.close();
	}
}
