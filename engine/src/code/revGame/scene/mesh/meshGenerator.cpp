////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh generator

#include "meshGenerator.h"

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
		plane->setFaces(2, faces);

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
			indices[6*i+1] = 4*i+1;
			indices[6*i+2] = 4*i+2;
			indices[6*i+3] = 4*i+2;
			indices[6*i+4] = 4*i+3;
			indices[6*i+5] = 4*i+0;
		}
		CStaticModel * box = new CStaticModel();
		box->setVertexData(24, reinterpret_cast<float*>(verts), reinterpret_cast<float*>(norms), reinterpret_cast<float*>(uvs));
		box->setFaces(12, indices);
		return box;
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
}	// namespace game
}	// namespace rev