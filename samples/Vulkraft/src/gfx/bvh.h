//-------------------------------------------------------------------------------------------------
// Toy path tracer
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
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
#pragma once

#include <math/geometry/aabb.h>
#include <vector>

/// Two way AABB tree
class AABBTree final
{
public:
	using Vec3f = rev::math::Vec3f;

	AABBTree() = default;
	AABBTree(const std::vector<uint16_t>& indices, const std::vector<rev::math::Vec3f>& vertices)
	{
		rev::math::AABB globalBBox;
		globalBBox.clear();
		std::vector<Triangle> triangles = buildTriangles(indices, vertices, globalBBox);
		// Construct tree
		mNodes.reserve(triangles.size()-1);
		mNodes.resize(1);
		initSubtree(mNodes[0], triangles.begin(), triangles.end(), 0, globalBBox);
	}

	struct Node
	{
		int nextOffset = 0;
		int leafMask = 0; // Higher (30) bits are the index of triengles
		uint32_t childBB[2][3]; // Compressed AABBs

		void setChildBBox(int childNdx, const rev::math::AABB& globalBBox, const rev::math::AABB& childBBox)
		{
			rev::math::AABB relBBox = childBBox;
			relBBox.min() = childBBox.min() - globalBBox.min();
			relBBox.max() = childBBox.max() - globalBBox.min();
			Vec3f parentSize = globalBBox.size();
			relBBox.min() = Vec3f(relBBox.min().x() / parentSize.x(), relBBox.min().y() / parentSize.y(), relBBox.min().z() / parentSize.z());
			relBBox.max() = Vec3f(relBBox.max().x() / parentSize.x(), relBBox.max().y() / parentSize.y(), relBBox.max().z() / parentSize.z());
			for(int i = 0; i < 3; ++i)
			{
				int axisMin = int(relBBox.min()[i]*0xffff);
				int axisMax = rev::math::min(0xffff, 1+int(relBBox.max()[i]*0xffff));
				childBB[childNdx][i] = axisMin & 0xffff;
				childBB[childNdx][i] = axisMax << 16;
			}
		}
	};

	const std::vector<Node>& nodes() const { return mNodes; }

private:

	struct Triangle
	{
		int16_t indices[3];
		rev::math::Vec3f centroid;
		rev::math::AABB bbox;
	};

private:

	std::vector<Triangle> buildTriangles(const std::vector<uint16_t>& indices, const std::vector<rev::math::Vec3f>& vtxBuffer, rev::math::AABB& globalBBox)
	{
		std::vector<Triangle> triangles;
		triangles.reserve(indices.size()/3);
		for(int i = 0; i < indices.size(); i+=3)
		{
			Triangle t;
			rev::math::Vec3f vertices[3];
			t.centroid.setZero();
			t.bbox.clear();

			for(int j = 0; j < 3; j++)
			{
				t.indices[j] = indices[i+j];
				vertices[j] = vtxBuffer[t.indices[j]];
				t.centroid = 1.f/3.f * vertices[j] + t.centroid;
				t.bbox.add(vertices[j]);
			}

			globalBBox.add(t.bbox);
			triangles.push_back(t);
		}

		return triangles;
	}

	void initSubtree(
		Node& root, int nextOffset,
		typename std::vector<Triangle>::iterator elementsBegin,
		typename std::vector<Triangle>::iterator elementsEnd,
		unsigned sortAxis,
		rev::math::AABB& subtreeBBox
	);

	void initNodeChild(
		Node& parent,
		int childNdx,
		std::vector<Triangle>::iterator elementsBegin,
		std::vector<Triangle>::iterator elementsEnd,
		unsigned nextAxis,
		rev::math::AABB& childBBox);

	void initLeaf(
		Node& parent,
		int childNdx,
		std::vector<Triangle>::iterator leafIter,
		rev::math::AABB& childBBox);

private:
	std::vector<Node> mNodes;
};

//----------------------------------------------------------------------------------------
void AABBTree::initSubtree(
	Node& subtreeRoot, int nextOffset,
	typename std::vector<Triangle>::iterator elementsBegin,
	typename std::vector<Triangle>::iterator elementsEnd,
	unsigned sortAxis,
	rev::math::AABB& subtreeBBox)
{
	auto nElements = elementsEnd-elementsBegin;
	assert(nElements >= 2); // Otherwise, we should be initializing a leaf

	// Approximately sort triangles along the given axis
	std::sort(elementsBegin, elementsEnd,
		[sortAxis](Triangle& a, Triangle& b) -> bool {
		auto da = a.centroid[sortAxis]; 
		auto db = b.centroid[sortAxis]; 
		return da < db;
	});
	// TODO: Try a bunch of possible splits

	// Find the middle element
	auto middle = elementsBegin+(nElements+1)/2;

	// Init node children
	auto nextAxis = (sortAxis+1)%3;
	rev::math::AABB bboxA, bboxB;
	int childNdx = mNodes.size();
	subtreeRoot.nextOffset = (childNdx<<16) | (nextOffset&0xffff);

	initNodeChild(subtreeRoot, 0, elementsBegin, middle, nextAxis, bboxA);
	initNodeChild(subtreeRoot, 1, middle, elementsEnd, nextAxis, bboxB);
	subtreeBBox = rev::math::AABB(bboxA, bboxB);
	// Set compressed bboxes relative to parent bbox
	if(!(subtreeRoot.leafMask & 1)) // Non-leaf, store compressed bbox
		subtreeRoot.setChildBBox(0, subtreeBBox, bboxA);
	if(!(subtreeRoot.leafMask & 2)) // Non-leaf, store compressed bbox
		subtreeRoot.setChildBBox(1, subtreeBBox, bboxB);
}

//----------------------------------------------------------------------------------------
void AABBTree::initNodeChild(
	Node& parent,
	int childNdx,
	std::vector<Triangle>::iterator elementsBegin,
	std::vector<Triangle>::iterator elementsEnd,
	unsigned nextAxis,
	rev::math::AABB& childBBox)
{
	auto numElements = elementsEnd - elementsBegin;
	if(numElements > 1)
	{
		mNodes.push_back(Node());
		childBBox.clear();
		initSubtree(mNodes.back(), elementsBegin, elementsEnd, nextAxis, childBBox);
	}
	else // Child A is a leaf
	{
		assert(numElements == 1);
		initLeaf(parent, childNdx, elementsBegin, childBBox);
	}
}

//----------------------------------------------------------------------------------------
void AABBTree::initLeaf(
	Node& parent,
	int childNdx,
	std::vector<Triangle>::iterator leafIter,
	rev::math::AABB& childBBox)
{
	parent.leafMask |= 1<<childNdx;
	// Combine bounding boxes
	childBBox = (leafIter+childNdx)->bbox;
	// Store triangle indices in place of the bounding box
	for(int i = 0; i < 3; ++i)
		parent.childBB[childNdx][i] = (leafIter+childNdx)->indices[i];
}