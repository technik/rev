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
		:isVisible(true)
		,mNVertices(0)
		,mNTriIndices(0)
		,mStripLength(0)
		,mVertices(nullptr)
		,mNormals(nullptr)
		,mUVs(nullptr)
		,mTriStrip(nullptr)
		,mTriangles(nullptr)
		,mTexture(nullptr)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	Renderable::~Renderable()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderable::render() const
	{
		video::Driver3d* driver = video::VideoDriver::getDriver3d();
		
		driver->setAttribBuffer(0, mNVertices, mVertices);
		driver->setAttribBuffer(1, mNVertices, mNormals);
		driver->setAttribBuffer(2, mNVertices, mUVs);
		if(0 != mNTriIndices)
			driver->drawIndexBuffer(mNTriIndices, mTriangles, video::Driver3d::EPrimitiveType::triangles);
		if(0 != mStripLength)
			driver->drawIndexBuffer(mStripLength, mTriStrip, video::Driver3d::EPrimitiveType::triStrip);
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

		// Compute bounding radius
		mBoundingSqRadius = 0.f;
		mBoundingRadius = 0.f;
		if(nullptr != mVertices) {
			for(unsigned i = 0; i < _count; ++i) {
				float nrm = mVertices[i].sqNorm();
				if(nrm > mBoundingSqRadius)
					mBoundingSqRadius = nrm;
			}
			mBoundingRadius = sqrt(mBoundingSqRadius);
		}
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
	Renderable* Renderable::heightField	(uint16_t _n, uint16_t _m, const math::Vec2f& _size, std::function<float(const Vec2u& _idx)> _height) {
		uint16_t width = _m+1;
		uint16_t length = _n+1;
		uint16_t nVertices = width*length;
		Vec3f* vertices = new Vec3f[nVertices];
		Vec3f* normals = new Vec3f[nVertices];
		Vec2f* uvs = new Vec2f[nVertices];
		Vec2f tileSize(_size.x/_m, _size.y/_n);
		for(uint16_t i = 0; i < _n+1; ++i)
		{
			for(uint16_t j = 0; j < _m+1; ++j)
			{
				uint16_t vtxId = j+width*i;
				float pointHeight = _height(Vec2u(i,j));
				vertices[vtxId] = Vec3f(tileSize.x*j,tileSize.y*i,pointHeight);
				normals[vtxId] = Vec3f::zAxis();
				uvs[vtxId] = Vec2f(j*1.f/_m,i*1.f/_n);
			}
		}
		uint16_t indicesPerRow = 2*(2+_m);
		uint16_t* indices = new uint16_t[indicesPerRow*_n];
		for(uint16_t i = 0; i < _n; ++i) // For each row
		{
			uint16_t i0 = i*indicesPerRow;
			uint16_t vtx0 = uint16_t(i*width);
			uint16_t vtx1 = uint16_t((i+1)*width);
			indices[i0] = vtx0;
			for(uint16_t j = 0; j < _m+1; ++j) {
				indices[i0+1+2*j] = uint16_t(vtx0+j);
				indices[i0+2+2*j] = uint16_t(vtx1+j);
			}
			indices[i0+indicesPerRow-1] = vtx1+_m;
		}
		Renderable * heightFieldObject = new Renderable;
		heightFieldObject->setVertexData(nVertices, vertices, normals, uvs);
		heightFieldObject->setFaceIndices(indicesPerRow*_n, indices, true);
		return heightFieldObject;
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