//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/16
//----------------------------------------------------------------------------------------------------------------------
// Basic procedurally generated meshes
#include "basic.h"

using namespace rev::math;
using namespace rev::video;

namespace rev {
	namespace game {

		//--------------------------------------------------------------------------------------------------------------
		video::StaticRenderMesh* Procedural::plane(const math::Vec2f& _size) {
			// Fill vertex data
			Vec2f halfSize = _size * 0.5f;
			Vec3f * vertices = new Vec3f[4];
			vertices[0] = Vec3f(halfSize.x, -halfSize.y, 0.f);
			vertices[1] = Vec3f(halfSize.x, halfSize.y, 0.f);
			vertices[2] = Vec3f(-halfSize.x, halfSize.y, 0.f);
			vertices[3] = Vec3f(-halfSize.x, -halfSize.y, 0.f);
			//Vec3f * normals = new Vec3f[4];
			//for (int i = 0; i < 4; ++i)
			//	normals[i] = Vec3f(0.f, 0.f, 1.f);
			//Vec2f * uvs = new Vec2f[4];
			//uvs[0] = Vec2f(1.0f, 0.0f);
			//uvs[1] = Vec2f(1.0f, 1.0f);
			//uvs[2] = Vec2f(0.0f, 1.0f);
			//uvs[3] = Vec2f(0.0f, 0.0f);
			//plane->setVertexData(4, vertices, normals, uvs);
			// Fill faces
			uint16_t * faces = new uint16_t[6];
			faces[0] = 0;
			faces[1] = 1;
			faces[2] = 2;
			faces[3] = 2;
			faces[4] = 3;
			faces[5] = 0;
			return new StaticRenderMesh(4, vertices, 6, faces);
		}

		//---------------------------------------------------------------------------------------------------------------
		void fillBoxFace(int _faceIdx, const Vec3f& _size, Vec3f * _verts, Vec3f * _norms, Vec2f * _uvs)
		{
			Vec3f halfSize = _size * 0.5f;
			Vec3f tangent;
			Vec3f normal;
			Vec3f binormal;
			float height, width, depth;
			switch (_faceIdx % 3)
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
			if (_faceIdx > 2)
			{
				tangent = tangent * -1.f;
				normal = normal * -1.f;
			}
			binormal = tangent ^ normal;
			// Set normals
			//for (int i = 0; i < 4; ++i)
			//{
			//	_norms[i] = normal;
			//}
			_verts[0] = normal * height - tangent * width - binormal * depth;
			_verts[1] = normal * height + tangent * width - binormal * depth;
			_verts[2] = normal * height + tangent * width + binormal * depth;
			_verts[3] = normal * height - tangent * width + binormal * depth;
			//_uvs[0] = Vec2f(0.0f, 0.0f);
			//_uvs[1] = Vec2f(0.0f, 1.0f);
			//_uvs[2] = Vec2f(1.0f, 1.0f);
			//_uvs[3] = Vec2f(1.0f, 0.0f);
		}

		//---------------------------------------------------------------------------------------------------------------
		StaticRenderMesh* Procedural::box(const Vec3f& _size)
		{
			// 4 vertices times 6 faces
			Vec3f * verts = new Vec3f[24];
			Vec3f * norms = new Vec3f[24];
			Vec2f * uvs = new Vec2f[24];
			uint16_t * indices = new uint16_t[6 * 6];
			for (uint16_t i = 0; i < 6; ++i)
			{
				fillBoxFace(i, _size, &verts[4 * i], &norms[4 * i], &uvs[4 * i]);
				indices[6 * i + 0] = 4 * i + 0;
				indices[6 * i + 1] = 4 * i + 3;
				indices[6 * i + 2] = 4 * i + 2;
				indices[6 * i + 3] = 4 * i + 2;
				indices[6 * i + 4] = 4 * i + 1;
				indices[6 * i + 5] = 4 * i + 0;
			}
			return new StaticRenderMesh(24, verts, 36, indices);
		}

		//---------------------------------------------------------------------------------------------------------------
		inline unsigned nVerticesInSphere(uint16_t _nMeridians, uint16_t _nParallels)
		{
			// Top and bottom plus _nParallels per meridian
			return (1 + _nMeridians) * (_nParallels + 2);
		}

		//---------------------------------------------------------------------------------------------------------------
		void fillVectorRing(Vec3f * _dst, uint16_t _nVerts, float _rad, float _height)
		{
			float deltaTheta = 2.f * 3.14159265f / (_nVerts - 1);
			float theta = 0.f;
			for (uint16_t i = 0; i < _nVerts - 1; ++i)
			{
				_dst[i] = Vec3f(_rad*sin(theta), _rad * -cos(theta), _height);
				theta += deltaTheta;
			}
			_dst[_nVerts - 1] = _dst[0];
		}

		//---------------------------------------------------------------------------------------------------------------
		Vec3f * generateSphereNormals(uint16_t _nMeridians, uint16_t _nParallels)
		{
			// Compute total vertices needed
			unsigned nVerts = nVerticesInSphere(_nMeridians, _nParallels);
			// Allocate needed vertices
			Vec3f * vertices = new Vec3f[nVerts];
			// Add rings of vertices
			float deltaAlpha = 3.14159265f / (1 + _nParallels);
			float alpha = (-3.14159265f / 2.f);
			for (uint16_t ring = 0; ring < _nParallels + 2; ++ring)
			{
				fillVectorRing(&vertices[ring*(_nMeridians + 1)], _nMeridians + 1, cos(alpha), sin(alpha));
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
			for (unsigned i = 0; i < nVerts; ++i)
				vertices[i] *= _radius;
			return vertices;
		}

		//---------------------------------------------------------------------------------------------------------------
		Vec2f * generateSphereUVs(uint16_t _nMeridians, uint16_t _nParallels)
		{
			unsigned nVerts = nVerticesInSphere(_nMeridians, _nParallels);
			Vec2f * uvs = new Vec2f[nVerts];
			float deltaX = 1.f / _nMeridians;
			float deltaY = 1.f / (1 + _nParallels);
			float Y = 0.f;
			for (uint16_t j = 0; j < _nParallels + 1; ++j)
			{
				float X = 0.f;
				for (uint16_t i = 0; i < _nMeridians; ++i)
				{
					uvs[i + j*(_nMeridians + 1)] = Vec2f(X, Y);
					X += deltaX;
				}
				Y += deltaY;
			}
			return uvs;
		}

		//---------------------------------------------------------------------------------------------------------------
		unsigned nIndicesInSphere(unsigned _nMeridians, unsigned _nParallels)
		{
			return (2 * _nMeridians + 4) * (_nParallels + 1);
		}

		//---------------------------------------------------------------------------------------------------------------
		void generateSphereSliceIndices(uint16_t * _dst, uint16_t _nMeridians, uint16_t _v0, bool _inverted)
		{
			uint16_t highVertex0 = _inverted ? _v0 : (_v0 + _nMeridians + 1);
			uint16_t lowVertex0 = _inverted ? (_v0 + _nMeridians + 1) : _v0;

			_dst[0] = highVertex0;
			for (uint16_t i = 0; i < _nMeridians + 1; ++i)
			{
				_dst[2 * i + 1] = highVertex0 + i;
				_dst[2 * i + 2] = lowVertex0 + i;
			}
			_dst[3 + 2 * _nMeridians] = lowVertex0 + _nMeridians;
		}

		//---------------------------------------------------------------------------------------------------------------
		uint16_t * generateSphereIndices(uint16_t _nMeridians, uint16_t _nParallels)
		{
			unsigned nIndices = nIndicesInSphere(_nMeridians, _nParallels);
			uint16_t * indices = new uint16_t[nIndices];
			unsigned deltaIndices = 2 * _nMeridians + 4;
			uint16_t deltaVerts = _nMeridians + 1;
			unsigned accumIndices = 0;
			uint16_t accumVerts = 0;
			for (uint16_t i = 0; i < _nParallels + 1; ++i)
			{
				generateSphereSliceIndices(&indices[accumIndices], _nMeridians, accumVerts, true);
				accumIndices += deltaIndices;
				accumVerts += deltaVerts;
			}
			return indices;
		}

		//---------------------------------------------------------------------------------------------------------------
		StaticRenderMesh * Procedural::geoSphere(float _radius, uint16_t _nMeridians, uint16_t _nParallels)
		{
			// Create vertices
			Vec3f * verts = generateSphereVertices(_radius, _nMeridians, _nParallels);
			// Create normals
			//Vec3f * norms = generateSphereNormals(_nMeridians, _nParallels);
			// Create uvs
			//Vec2f * uvs = generateSphereUVs(_nMeridians, _nParallels);
			// Create indices
			uint16_t * indices = generateSphereIndices(_nMeridians, _nParallels);
			// Create the model itself
			uint16_t nVertices = uint16_t(nVerticesInSphere(_nMeridians, _nParallels));
			uint16_t nIndices = uint16_t(nIndicesInSphere(_nMeridians, _nParallels));
			return new StaticRenderMesh(nVertices, verts, nIndices, indices);
		}
	}
}