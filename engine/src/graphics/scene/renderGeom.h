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

/*#include <graphics/types.h>
#include <math/algebra/vector.h>
#include <math/geometry/aabb.h>
#include <math/geometry/types.h>
#include <iostream>
#include <vector>
#include <memory>*/

namespace rev::gfx {

	/*class RenderGeom {
	public:
		struct BufferView
		{
			Buffer vbo;
			GLint byteStride;
			size_t byteLength;
			const void* data;
		};

		struct Attribute
		{
			template<class T> auto& get(size_t i) const {
				auto rawData = (void*)((size_t)bufferView->data + (int)offset);
				auto formattedData = reinterpret_cast<const T*>(rawData);
				return formattedData[i];
			}
			std::shared_ptr<BufferView> bufferView;
			GLvoid* offset;
			GLenum componentType;
			GLint nComponents;
			GLsizei stride;
			GLsizei count;
			bool normalized;
			math::AABB bounds;
		};

	public:
		RenderGeom() = default;

		RenderGeom(
			const Attribute* indices,
			const Attribute* position,
			const Attribute* normal,
			const Attribute* tangent,
			const Attribute* uv0,
			const Attribute* weights,
			const Attribute* joints);

		static RenderGeom quad(const math::Vec2f& size);

		GLuint getVao() const { return m_vao; }
		auto& indices() const { return m_indices; }
		const math::AABB& bbox() const { return m_bbox; }
		VtxFormat vertexFormat() const { return m_vtxFormat; }

	private:
		VtxFormat m_vtxFormat;
		math::AABB m_bbox;
		GLuint m_vao = 0;
		std::vector<std::pair<GLuint, Attribute>> m_vtxAttributes; // Attribute index, attribute data
		Attribute m_indices;

	private:
		void initOpenGL();
	};*/

}