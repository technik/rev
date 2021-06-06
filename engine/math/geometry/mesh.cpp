//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "mesh.h"

namespace rev::math
{
	std::vector<Vec3f> generateNormals(
		size_t numVtx,
		const Vec3f* positions,
		size_t nIndices,
		const uint32_t* indices)
	{
		// Accessor data
		std::vector<Vec3f> normals(numVtx, Vec3f::zero());

		// Traverse each triangle
		for (size_t t = 0; t < nIndices; t += 3)
		{
			auto i0 = indices[t + 0];
			auto i1 = indices[t + 1];
			auto i2 = indices[t + 2];

			Vec3f v0 = positions[i0];
			Vec3f v1 = positions[i1];
			Vec3f v2 = positions[i2];

			Vec3f e1 = Vec3f(v1 - v0);
			Vec3f e2 = Vec3f(v2 - v0);
				// Compute triangle normal
			Vec3f triNormal = cross(e1, e2);
			if (triNormal == Vec3f::zero())
				continue; // Ignore degenerate triangles
			triNormal = normalize(triNormal);

			// And contribute it to each vertex
			normals[i0] += triNormal;
			normals[i1] += triNormal;
			normals[i2] += triNormal;
		}

		// For each vertex, renormalize
		for (size_t v = 0; v < numVtx; ++v)
		{
			normals[v] = normalize(normals[v]);
		}

		return normals;
	}

	std::vector<Vec4f> generateTangentSpace(
		size_t numVtx,
		const Vec3f* positions,
		const Vec2f* uvs,
		const Vec3f* normals,
		size_t nIndices,
		const uint32_t* indices)
	{
		// Accessor data
		std::vector<Vec4f> tangents(numVtx, Vec4f::zero());

		// Accumulate per-triangle normals
		for (int i = 0; i < nIndices; i += 3) // Iterate over all triangles
		{
			auto i0 = indices[i + 0];
			auto i1 = indices[i + 1];
			auto i2 = indices[i + 2];

			Vec2f localUvs[3] = { uvs[i0], uvs[i1], uvs[i2] };
			Vec3f localPos[3] = { positions[i0], positions[i1], positions[i2] };

			Vec2f deltaUV1 = localUvs[1] - localUvs[0];
			Vec2f deltaUV2 = localUvs[2] - localUvs[0];

			Vec3f deltaPos1 = localPos[1] - localPos[0];
			Vec3f deltaPos2 = localPos[2] - localPos[0];

			auto determinant = deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y();

			// Unnormalized tangent
			Vec3f triangleTangent = (deltaPos1 * deltaUV1.x() - deltaUV1.y() * deltaPos2) * (1 / determinant);

			tangents[i0] = tangents[i0] + Vec4f(triangleTangent.x(), triangleTangent.y(), triangleTangent.z(), determinant);
			tangents[i1] = tangents[i1] + Vec4f(triangleTangent.x(), triangleTangent.y(), triangleTangent.z(), determinant);
			tangents[i2] = tangents[i2] + Vec4f(triangleTangent.x(), triangleTangent.y(), triangleTangent.z(), determinant);
		}

		// Orthonormalize per vertex
		for (int i = 0; i < tangents.size(); ++i)
		{
			auto& tangent = tangents[i];
			Vec3f tangent3 = { tangent.x(), tangent.y(), tangent.z() };
			auto& normal = normals[i];

			tangent3 = tangent3 - (dot(tangent3, normal) * normal); // Orthogonal tangent
			tangent3 = normalize(tangent3); // Orthonormal tangent
			tangent = { tangent3.x(), tangent3.y(), tangent3.z(), signbit(-tangent.w()) ? -1.f : 1.f };
		}

		return tangents;
	}
}