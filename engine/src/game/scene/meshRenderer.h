//--------------------------------------------------------------------------------------------------
// Revolution Engine
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

#include <memory>
#include <vector>
#include <game/scene/transform/transform.h>

namespace rev { namespace game {

	class Transform;

	// Data oriented container of renderable meshes
	class MeshRenderer {
	public:
		// Invalidates the order of renderables
		size_t addMesh(gfx::RasterMesh mesh)
		{
			m_meshes.push_back(mesh);
			return m_meshes.size() - 1;
		}

		// Invalidates the order of renderables
		void addInstance(const Transform* transformSrc, size_t meshNdx)
		{
			m_instanceTransforms.push_back(transformSrc);
			m_instanceMeshes.push_back(meshNdx);
		}

		void updatePoses(math::Mat44f* dst)
		{
			for (size_t i = 0; i < m_instanceTransforms.size(); ++i)
			{
				dst[i] = m_instanceTransforms[i]->absoluteXForm().matrix();
			}
		}

		size_t numInstances() const { return m_instanceTransforms.size(); }
		const gfx::RenderMesh& instanceMesh(size_t i) const {
			return m_meshes[m_instanceMeshes[i]];
		}

	private:
		std::vector<gfx::RenderMesh> m_meshes;
		std::vector<size_t> m_instanceMeshes;
		std::vector<const Transform*> m_instanceTransforms;
	};

}}	// namespace rev::game