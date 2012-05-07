////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh generator

#include "meshGenerator.h"

#include <revCore/math/noise.h>
#include <revCore/math/vector.h>
#include <revVideo/scene/model/staticModel.h>

using namespace rev::video;

namespace rev { namespace game
{
	//---------------------------------------------------------------------------------------------------------------
	CStaticModel * CMeshGenerator::plane(const CVec2& size)
	{
		CStaticModel * plane = new CStaticModel();
		// Fill vertex data
		CVec2 halfSize = size * 0.5f;
		CVec3 * vertices = new CVec3[4];
		vertices[0] = CVec3(-halfSize.x, -halfSize.y, 0.f);
		vertices[1] = CVec3(-halfSize.x, halfSize.y, 0.f);
		vertices[2] = CVec3(halfSize.x, halfSize.y, 0.f);
		vertices[3] = CVec3(halfSize.x, -halfSize.y, 0.f);
		CVec3 * normals = new CVec3[4];
		for(int i = 0; i < 4; ++i)
			normals[i] = CVec3(0.f, 0.f, 1.f);
		CVec2 * uvs = new CVec2[4];
		uvs[0] = CVec2(0.0f,0.0f);
		uvs[1] = CVec2(0.0f,1.0f);
		uvs[2] = CVec2(1.0f,1.0f);
		uvs[3] = CVec2(1.0f,0.0f);
		plane->setVertexData(4, reinterpret_cast<float*>(vertices),reinterpret_cast<float*>(normals),reinterpret_cast<float*>(uvs));
		// Fill faces
		unsigned short * faces = new unsigned short[6];
		faces[0] = 0;
		faces[1] = 1;
		faces[2] = 2;
		faces[3] = 2;
		faces[4] = 3;
		faces[5] = 0;
		plane->setFaceIndices(6, faces);

		return plane;
	}

	//---------------------------------------------------------------------------------------------------------------
	CStaticModel * CMeshGenerator::box(const CVec3& size)
	{
		// 4 vertices times 6 faces
		CVec3 * verts = new CVec3[24];
		CVec3 * norms = new CVec3[24];
		CVec2 * uvs = new CVec2[24];
		unsigned short * indices = new unsigned short [ 6 * 6 ];
		for(unsigned short i = 0; i < 6; ++i)
		{
			fillBoxFace(i, size, &verts[4*i], &norms[4*i], &uvs[4*i]);
			indices[6*i+0] = 4*i+0;
			indices[6*i+1] = 4*i+3;
			indices[6*i+2] = 4*i+2;
			indices[6*i+3] = 4*i+2;
			indices[6*i+4] = 4*i+1;
			indices[6*i+5] = 4*i+0;
		}
		CStaticModel * box = new CStaticModel();
		box->setVertexData(24, reinterpret_cast<float*>(verts), reinterpret_cast<float*>(norms), reinterpret_cast<float*>(uvs));
		box->setFaceIndices(36, indices);
		return box;
	}

	//---------------------------------------------------------------------------------------------------------------
	inline unsigned nVerticesInSphere(unsigned _nMeridians, unsigned _nParallels)
	{
		// Top and bottom plus _nParallels per meridian
		return 2 + _nMeridians * (_nParallels+1);
	}

	//---------------------------------------------------------------------------------------------------------------
	void fillVectorRing(CVec3 * _dst, unsigned _nVerts, TReal _rad, TReal _height)
	{
		TReal deltaTheta =  2.f * 3.14159265f / (_nVerts-1);
		TReal theta = 0.f;
		for(unsigned i = 0; i < _nVerts-1; ++i)
		{
			_dst[i] = CVec3(_rad*sin(theta), _rad * -cos(theta), _height);
			theta += deltaTheta;
		}
		_dst[_nVerts-1] = _dst[0];
	}

	//---------------------------------------------------------------------------------------------------------------
	CVec3 * generateSphereNormals(unsigned _nMeridians, unsigned _nParallels)
	{
		// Compute total vertices needed
		unsigned nVerts = nVerticesInSphere(_nMeridians, _nParallels);
		// Allocate needed vertices
		CVec3 * vertices = new CVec3[nVerts];
		// Add top and bottom vertices
		vertices[0] = CVec3(0.f, 0.f, -1.f);
		vertices[nVerts-1] = CVec3(0.f, 0.f, 1.f);
		// Add intermediate rings of vertices
		TReal deltaAlpha = 3.14159265f / (1+_nMeridians);
		TReal alpha = (-3.14159265f / 2.f) + deltaAlpha;
		for(unsigned ring = 0; ring < _nMeridians; ++ring)
		{
			fillVectorRing(&vertices[1+ring*(_nParallels+1)], _nParallels+1, cos(alpha), sin(alpha));
			alpha += deltaAlpha;
		}
		return vertices;
	}

	//---------------------------------------------------------------------------------------------------------------
	CVec3 * generateSphereVertices(TReal _radius, unsigned _nMeridians, unsigned _nParallels)
	{
		// Compute total vertices needed
		unsigned nVerts = nVerticesInSphere(_nMeridians, _nParallels);
		// Allocate needed vertices
		CVec3 * vertices = generateSphereNormals(_nMeridians, _nParallels);
		for(unsigned i = 0; i < nVerts; ++i)
			vertices[i] *= _radius;
		return vertices;
	}

	//---------------------------------------------------------------------------------------------------------------
	CVec2 * generateSphereUVs(unsigned _nMeridians, unsigned _nParallels)
	{
		unsigned nVerts = nVerticesInSphere(_nMeridians, _nParallels);
		CVec2 * uvs = new CVec2[nVerts];
		uvs[0] = CVec2(0.5f,0.f);
		uvs[nVerts-1] = CVec2(0.5f,1.f);
		TReal deltaZ = 1.f/(1+_nMeridians);
		TReal deltaX = 1.f / _nParallels;
		TReal X = 0.f;
		for(unsigned i = 0; i < _nParallels+1; ++i)
		{
			TReal Z = 1.f - deltaZ;
			for(unsigned j = 0; j < _nMeridians; ++j)
			{
				uvs[1+i+j*(_nParallels+1)] = CVec2(X, Z);
				Z+=deltaZ;
			}
			X+=deltaX;
		}
		return uvs;
	}

	//---------------------------------------------------------------------------------------------------------------
	unsigned nIndicesInSphere(unsigned _nMeridians, unsigned _nParallels)
	{
		return (2*_nMeridians + 3) * _nParallels;
	}

	//---------------------------------------------------------------------------------------------------------------
	void generateSphereSliceIndices(u16 * _dst, unsigned _nMeridians, unsigned _nParallels, unsigned _slice)
	{
		_dst[0] = 0;
		for(unsigned i = 0; i < _nMeridians; ++i)
		{
			_dst[1+2*i] = u16(2 + _slice + (_nParallels+1) * i);
			_dst[2+2*i] = u16(1 + _slice + (_nParallels+1) * i);
		}
		_dst[2*_nMeridians + 1] = u16(nVerticesInSphere(_nMeridians, _nParallels) -1);
		_dst[2*_nMeridians + 2] = u16(nVerticesInSphere(_nMeridians, _nParallels) -1);
	}

	//---------------------------------------------------------------------------------------------------------------
	u16 * generateSphereIndices(unsigned _nMeridians, unsigned _nParallels)
	{
		unsigned nIndices = nIndicesInSphere(_nMeridians, _nParallels);
		u16 * indices = new u16[nIndices];
		unsigned deltaIndices = 2*_nMeridians+3;
		unsigned accumIndices = 0;
		for(unsigned i = 0; i < _nParallels; ++i)
		{
			generateSphereSliceIndices(&indices[accumIndices], _nMeridians, _nParallels, i);
			accumIndices += deltaIndices;
		}
		return indices;
	}

	//---------------------------------------------------------------------------------------------------------------
	CStaticModel * CMeshGenerator::geoSphere(TReal _radius, unsigned _nMeridians, unsigned _nParallels)
	{
		// Create vertices
		CVec3 * verts = generateSphereVertices(_radius, _nMeridians, _nParallels);
		// Create normals
		CVec3 * norms = generateSphereNormals(_nMeridians, _nParallels);
		// Create uvs
		CVec2 * uvs = generateSphereUVs(_nMeridians, _nParallels);
		// Create indices
		u16 * indices = generateSphereIndices(_nMeridians, _nParallels);
		// Create the model itself
		CStaticModel * sphere = new CStaticModel();
		sphere->setVertexData(u16(nVerticesInSphere(_nMeridians,_nParallels)),
			reinterpret_cast<float*>(verts),
			reinterpret_cast<float*>(norms),
			reinterpret_cast<float*>(uvs));
		sphere->setFaceIndices(u16(nIndicesInSphere(_nMeridians, _nParallels)), indices, true);
		return sphere;
	}

	//---------------------------------------------------------------------------------------------------------------
	void CMeshGenerator::fillBoxFace(int faceIdx, const CVec3& size, CVec3 * verts, CVec3 * norms, CVec2 * uvs)
	{
		CVec3 halfSize = size * 0.5f;
		CVec3 tangent;
		CVec3 normal;
		CVec3 binormal;
		float height, width, depth;
		switch(faceIdx % 3)
		{
		case 0:
			height = halfSize.x;
			width = halfSize.z;
			depth = halfSize.y;
			tangent = CVec3(0.f, 0.f, 1.f);
			normal = CVec3(1.f, 0.f, 0.f);
			break;
		case 1:
			height = halfSize.y;
			width = halfSize.x;
			depth = halfSize.z;
			tangent = CVec3(1.f, 0.f, 0.f);
			normal = CVec3(0.f, 1.f, 0.f);
			break;
		default:
			height = halfSize.z;
			width = halfSize.y;
			depth = halfSize.x;
			tangent = CVec3(0.f, 1.f, 0.f);
			normal = CVec3(0.f, 0.f, 1.f);
		}
		if(faceIdx > 2)
		{
			tangent = tangent * -1.f;
			normal = normal * -1.f;
		}
		binormal = tangent ^ normal;
		// Set normals
		for(int i = 0; i < 4; ++i)
		{
			norms[i] = normal;
		}
		verts[0] = normal * height - tangent * width - binormal * depth;
		verts[1] = normal * height + tangent * width - binormal * depth;
		verts[2] = normal * height + tangent * width + binormal * depth;
		verts[3] = normal * height - tangent * width + binormal * depth;
		uvs[0] = CVec2(0.0f,0.0f);
		uvs[1] = CVec2(0.0f,1.0f);
		uvs[2] = CVec2(1.0f,1.0f);
		uvs[3] = CVec2(1.0f,0.0f);
	}

	//---------------------------------------------------------------------------------------------------------------
	video::CStaticModel * CMeshGenerator::terrain(unsigned _side, float _gridSize, float _height)
	{
		revAssert(_side < 256, "Terrain is too big");
		CVec3 * verts = generateTerrainVertices(_side,_gridSize,_height);
		CVec3 * norms = generateTerrainNorms(_side,verts);
		CVec2 * uvs =	generateTerrainUVs(_side);
		u16 * indices = generateTerrainTriStrip(_side);
		CStaticModel * terrain = new CStaticModel();
		terrain->setVertexData(verticesInTerrain(_side),
			reinterpret_cast<float*>(verts),
			reinterpret_cast<float*>(norms),
			reinterpret_cast<float*>(uvs));
		terrain->setFaceIndices(indicesInTerrain(_side), indices, true);
		return terrain;
	}

	//---------------------------------------------------------------------------------------------------------------
	CVec3 * CMeshGenerator::generateTerrainVertices(unsigned _side, float _size, float _height)
	{
		CVec3 * verts = new CVec3[verticesInTerrain(_side)];
		// Planar fill
		for(unsigned i = 0; i <= _side; ++i)
		{
			for(unsigned j = 0; j <= _side; ++j)
			{
				float x = (float(i) / _side - 0.5f) * _size;
				float y = (float(j) / _side - 0.5f) * _size;
				float h0n = SNoise::simplex(x*0.005f, y*0.005f);
				float h0 = _height * h0n * h0n;
				float h1 = 0.03125f*_height * SNoise::simplex(x*0.05f, y*0.04f);
				float h2 = 0.00625f*_height * SNoise::simplex(x*0.5f, y*0.4f);
				verts[(_side+1)*i + j] = CVec3(x, y, h0+h1+h2);
			}
		}
		return verts;
	}

	//---------------------------------------------------------------------------------------------------------------
	CVec3 * CMeshGenerator::generateTerrainNorms(unsigned _side, CVec3 * _verts)
	{
		CVec3 * normals = new CVec3[verticesInTerrain(_side)];
		// Quick fill
		for(unsigned i = 0; i <= _side; ++i)
		{
			for(unsigned j = 0; j <= _side; ++j)
			{
				normals[(_side+1)*i + j] = CVec3::zAxis;
			}
		}
		// Real normals
		for(unsigned i = 1; i <= _side-1; ++i)
		{
			for(unsigned j = 1; j <= _side-1; ++j)
			{
				CVec3 dx = (_verts[(_side+1)*(i+1)+j]-_verts[(_side+1)*(i-1)+j]);
				CVec3 dy = (_verts[(_side+1)*i+j+1]-_verts[(_side+1)*i+j-1]);
				normals[(_side+1)*i + j] = (dx ^ dy).normalized();
			}
		}
		return normals;
	}

	//---------------------------------------------------------------------------------------------------------------
	CVec2 * CMeshGenerator::generateTerrainUVs(unsigned _side)
	{
		CVec2 * uvs = new CVec2[verticesInTerrain(_side)];
		for(unsigned i = 0; i <= _side; ++i)
		{
			for(unsigned j = 0; j <= _side; ++j)
			{
				uvs[(_side+1)*i + j] = CVec2(float(i)/_side, float(j)/_side);
			}
		}
		return uvs;
	}

	//---------------------------------------------------------------------------------------------------------------
	u16 * CMeshGenerator::generateTerrainTriStrip(unsigned _side)
	{
		unsigned vtxPerRow = _side+1;
		unsigned indicesPerRow = 2*vtxPerRow + 2; // Repeat first and last indices to be able to concatenate strips
		unsigned nIndices = _side * indicesPerRow;
		u16 * indices = new u16[nIndices];
		for(unsigned row = 0; row < _side; ++row)
		{
			unsigned idx0 = indicesPerRow * row;
			u16 vtx0 = u16(row * vtxPerRow);
			indices[idx0] = u16(vtx0 + vtxPerRow);
			for(unsigned i = 0; i < vtxPerRow; ++i)
			{
				indices[2*i + idx0 + 2] = vtx0 + u16(i);
				indices[2*i + idx0 + 1] = u16(vtx0 + i + vtxPerRow);
			}
			indices[idx0+indicesPerRow-1] = u16(vtx0 + vtxPerRow - 1);
		}
		return indices;
	}

	//---------------------------------------------------------------------------------------------------------------
	u16 CMeshGenerator::verticesInTerrain(unsigned _side)
	{
		return u16((_side+1)*(_side+1));
	}

	//---------------------------------------------------------------------------------------------------------------
	unsigned CMeshGenerator::indicesInTerrain(unsigned _side)
	{
		unsigned vtxPerRow = _side+1;
		unsigned indicesPerRow = 2*vtxPerRow + 2; // Repeat first and last indices to be able to concatenate strips
		return _side * indicesPerRow;
	}
}	// namespace game
}	// namespace rev