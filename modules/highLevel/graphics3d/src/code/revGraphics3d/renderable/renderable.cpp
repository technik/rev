//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// base renderable type

#include "renderable.h"
#include "renderScene.h"

#include <revVideo/driver3d/driver3d.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev::math;
using namespace rev::video;

namespace rev { namespace graphics3d {

	//------------------------------------------------------------------------------------------------------------------
	Renderable::Renderable()
		:mNVertices(0)
		,mNTriIndices(0)
		,mStripLength(0)
		,mVertices(nullptr)
		,mNormals(nullptr)
		,mUVs(nullptr)
		,mTriStrip(nullptr)
		,mTriangles(nullptr)
	{
		// Register in scene
		RenderScene::get()->add(this);
	}

	//------------------------------------------------------------------------------------------------------------------
	Renderable::~Renderable()
	{
		RenderScene::get()->remove(this);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderable::render() const
	{
		video::Driver3d* driver = video::VideoDriver::getDriver3d();
		
		driver->setAttribBuffer(0, mNVertices, mVertices);
		if(0 != mNTriIndices)
			driver->drawIndexBuffer(mNTriIndices, mTriangles, video::Driver3d::EPrimitiveType::triangles);
		if(0 != mStripLength)
			driver->drawIndexBuffer(mStripLength, mTriStrip, video::Driver3d::EPrimitiveType::triStrip);

		// uint16_t indices[] = { 0, 1, 2 };
		// driver->drawIndexBuffer(3, indices, Driver3d::EPrimitiveType::triangles);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderable::setVertexData(uint16_t _count, Vec3f * _vertPos, Vec3f * _vertNrm, Vec2f* _vertUV)
	{
		// Delete old data (if any)
		if(0 != mNVertices) {
			delete[] mVertices;
			delete[] mNormals;
			delete[] mUVs;
		}
		mNVertices = _count;
		mVertices = _vertPos;
		mNormals = _vertNrm;
		mUVs = _vertUV;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderable::setFaceIndices(uint16_t _nIndices, uint16_t * _indices, bool _strip)
	{
		if(_strip) { // Triangle strip
			if(0 != mStripLength) delete[] mTriStrip;
			mStripLength = _nIndices;
			mTriStrip = _indices;
		} else { // Raw triangles
			if(0 != mNTriIndices) delete[] mTriangles;
			mNTriIndices = _nIndices;
			mTriangles = _indices;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	Renderable* Renderable::plane(const Vec2f& _size)
	{
		Renderable * plane = new Renderable;
		// Fill vertex data
		Vec2f halfSize = _size * 0.5f;
		Vec3f * vertices = new Vec3f[4];
		vertices[0] = Vec3f(-halfSize.x, -halfSize.y, 0.f);
		vertices[1] = Vec3f(-halfSize.x, halfSize.y, 0.f);
		vertices[2] = Vec3f(halfSize.x, halfSize.y, 0.f);
		vertices[3] = Vec3f(halfSize.x, -halfSize.y, 0.f);
		Vec3f * normals = new Vec3f[4];
		for(int i = 0; i < 4; ++i)
			normals[i] = Vec3f(0.f, 0.f, 1.f);
		Vec2f * uvs = new Vec2f[4];
		uvs[0] = Vec2f(0.0f,0.0f);
		uvs[1] = Vec2f(0.0f,1.0f);
		uvs[2] = Vec2f(1.0f,1.0f);
		uvs[3] = Vec2f(1.0f,0.0f);
		plane->setVertexData(4, vertices, normals, uvs);
		// Fill faces
		uint16_t * faces = new uint16_t[6];
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
	void fillBoxFace(int _faceIdx, const Vec3f& _size, Vec3f * _verts, Vec3f * _norms, Vec2f * _uvs)
	{
		Vec3f halfSize = _size * 0.5f;
		Vec3f tangent;
		Vec3f normal;
		Vec3f binormal;
		float height, width, depth;
		switch(_faceIdx % 3)
		{
		case 0:
			height = halfSize.x;
			width = halfSize.z;
			depth = halfSize.y;
			tangent = Vec3f(0.f, 0.f, 1.f);
			normal = Vec3f(1.f, 0.f, 0.f);
			break;
		case 1:
			height = halfSize.y;
			width = halfSize.x;
			depth = halfSize.z;
			tangent = Vec3f(1.f, 0.f, 0.f);
			normal = Vec3f(0.f, 1.f, 0.f);
			break;
		default:
			height = halfSize.z;
			width = halfSize.y;
			depth = halfSize.x;
			tangent = Vec3f(0.f, 1.f, 0.f);
			normal = Vec3f(0.f, 0.f, 1.f);
		}
		if(_faceIdx > 2)
		{
			tangent = tangent * -1.f;
			normal = normal * -1.f;
		}
		binormal = tangent ^ normal;
		// Set normals
		for(int i = 0; i < 4; ++i)
		{
			_norms[i] = normal;
		}
		_verts[0] = normal * height - tangent * width - binormal * depth;
		_verts[1] = normal * height + tangent * width - binormal * depth;
		_verts[2] = normal * height + tangent * width + binormal * depth;
		_verts[3] = normal * height - tangent * width + binormal * depth;
		_uvs[0] = Vec2f(0.0f,0.0f);
		_uvs[1] = Vec2f(0.0f,1.0f);
		_uvs[2] = Vec2f(1.0f,1.0f);
		_uvs[3] = Vec2f(1.0f,0.0f);
	}

	//---------------------------------------------------------------------------------------------------------------
	Renderable* Renderable::box(const Vec3f& _size)
	{
		// 4 vertices times 6 faces
		Vec3f * verts = new Vec3f[24];
		Vec3f * norms = new Vec3f[24];
		Vec2f * uvs = new Vec2f[24];
		uint16_t * indices = new uint16_t[ 6 * 6 ];
		for(uint16_t i = 0; i < 6; ++i)
		{
			fillBoxFace(i, _size, &verts[4*i], &norms[4*i], &uvs[4*i]);
			indices[6*i+0] = 4*i+0;
			indices[6*i+1] = 4*i+3;
			indices[6*i+2] = 4*i+2;
			indices[6*i+3] = 4*i+2;
			indices[6*i+4] = 4*i+1;
			indices[6*i+5] = 4*i+0;
		}
		Renderable * box = new Renderable;
		box->setVertexData(24, verts, norms, uvs);
		box->setFaceIndices(36, indices);
		return box;
	}

	//---------------------------------------------------------------------------------------------------------------
	inline unsigned nVerticesInSphere(uint16_t _nMeridians, uint16_t _nParallels)
	{
		// Top and bottom plus _nParallels per meridian
		return (1+_nMeridians) * (_nParallels+2);
	}

	//---------------------------------------------------------------------------------------------------------------
	void fillVectorRing(Vec3f * _dst, uint16_t _nVerts, float _rad, float _height)
	{
		float deltaTheta =  2.f * 3.14159265f / (_nVerts-1);
		float theta = 0.f;
		for(uint16_t i = 0; i < _nVerts-1; ++i)
		{
			_dst[i] = Vec3f(_rad*sin(theta), _rad * -cos(theta), _height);
			theta += deltaTheta;
		}
		_dst[_nVerts-1] = _dst[0];
	}

	//---------------------------------------------------------------------------------------------------------------
	Vec3f * generateSphereNormals(uint16_t _nMeridians, uint16_t _nParallels)
	{
		// Compute total vertices needed
		unsigned nVerts = nVerticesInSphere(_nMeridians, _nParallels);
		// Allocate needed vertices
		Vec3f * vertices = new Vec3f[nVerts];
		// Add rings of vertices
		float deltaAlpha = 3.14159265f / (1+_nParallels);
		float alpha = (-3.14159265f / 2.f);
		for(uint16_t ring = 0; ring < _nParallels+2; ++ring)
		{
			fillVectorRing(&vertices[ring*(_nMeridians+1)], _nMeridians+1, cos(alpha), sin(alpha));
			alpha += deltaAlpha;
		}
		return vertices;
	}

	//---------------------------------------------------------------------------------------------------------------
	Vec3f * generateSphereVertices(float _radius, uint16_t _nMeridians, uint16_t _nParallels)
	{
		// Compute total vertices needed
		unsigned nVerts = nVerticesInSphere(_nMeridians, _nParallels);
		// Allocate needed vertices
		Vec3f * vertices = generateSphereNormals(_nMeridians, _nParallels);
		for(unsigned i = 0; i < nVerts; ++i)
			vertices[i] *= _radius;
		return vertices;
	}

	//---------------------------------------------------------------------------------------------------------------
	Vec2f * generateSphereUVs(uint16_t _nMeridians, uint16_t _nParallels)
	{
		unsigned nVerts = nVerticesInSphere(_nMeridians, _nParallels);
		Vec2f * uvs = new Vec2f[nVerts];
		float deltaX = 1.f / _nMeridians;
		float deltaY = 1.f/(1+_nParallels);
		float Y = 0.f;
		for(uint16_t j = 0; j < _nParallels+1; ++j)
		{
			float X = 0.f;
			for(uint16_t i = 0; i < _nMeridians; ++i)
			{
				uvs[i+j*(_nMeridians+1)] = Vec2f(X, Y);
				X+=deltaX;
			}
			Y+=deltaY;
		}
		return uvs;
	}

	//---------------------------------------------------------------------------------------------------------------
	unsigned nIndicesInSphere(unsigned _nMeridians, unsigned _nParallels)
	{
		return (2*_nMeridians + 4) * (_nParallels+1);
	}

	//---------------------------------------------------------------------------------------------------------------
	void generateSphereSliceIndices(uint16_t * _dst, uint16_t _nMeridians, uint16_t _v0, bool _inverted)
	{
		uint16_t highVertex0 = _inverted?_v0:(_v0+_nMeridians+1);
		uint16_t lowVertex0 = _inverted?(_v0+_nMeridians+1):_v0;

		_dst[0] = highVertex0;
		for(uint16_t i = 0; i < _nMeridians+1; ++i)
		{
			_dst[2*i+1] = highVertex0 + i;
			_dst[2*i+2] = lowVertex0  + i;
		}
		_dst[3+2*_nMeridians] = lowVertex0 + _nMeridians;
	}

	//---------------------------------------------------------------------------------------------------------------
	uint16_t * generateSphereIndices(uint16_t _nMeridians, uint16_t _nParallels)
	{
		unsigned nIndices = nIndicesInSphere(_nMeridians, _nParallels);
		uint16_t * indices = new uint16_t[nIndices];
		unsigned deltaIndices = 2*_nMeridians+4;
		uint16_t deltaVerts = _nMeridians + 1;
		unsigned accumIndices = 0;
		uint16_t accumVerts = 0;
		for(uint16_t i = 0; i < _nParallels+1; ++i)
		{
			generateSphereSliceIndices(&indices[accumIndices], _nMeridians, accumVerts, true);
			accumIndices += deltaIndices;
			accumVerts += deltaVerts;
		}
		return indices;
	}

	//---------------------------------------------------------------------------------------------------------------
	Renderable * Renderable::geoSphere(float _radius, uint16_t _nMeridians, uint16_t _nParallels)
	{
		// Create vertices
		Vec3f * verts = generateSphereVertices(_radius, _nMeridians, _nParallels);
		// Create normals
		Vec3f * norms = generateSphereNormals(_nMeridians, _nParallels);
		// Create uvs
		Vec2f * uvs = generateSphereUVs(_nMeridians, _nParallels);
		// Create indices
		uint16_t * indices = generateSphereIndices(_nMeridians, _nParallels);
		// Create the model itself
		Renderable * sphere = new Renderable();
		sphere->setVertexData(uint16_t(nVerticesInSphere(_nMeridians,_nParallels)), verts, norms, uvs);
		sphere->setFaceIndices(uint16_t(nIndicesInSphere(_nMeridians, _nParallels)), indices, true);
		return sphere;
	}

}	// namespace graphics3d
}	// rev