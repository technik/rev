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

#include <graphics/backend/device.h>
#include <math/algebra/vector.h>
#include <math/geometry/aabb.h>
#include <math/geometry/types.h>
#include <iostream>
#include <vector>
#include <memory>

namespace rev::gfx {

	class RenderGeom {
	public:
		struct VtxFormat {
			enum class Storage : uint8_t {
				None = 0,
				Float32,
				u8,
				u16,
				u32
			};

			VtxFormat() = default;
			VtxFormat(Storage pos, Storage nrm, Storage tan, Storage uv, Storage weights);

			Storage position() const	{ return m_pos; }
			Storage normal() const		{ return m_normal; }
			Storage tangent() const		{ return m_tangent; }
			Storage uv() const			{ return m_uv; }
			Storage weights() const		{ return m_weights; }

			uint32_t code() const;
			static uint32_t invalid() { return 0; }

		private:
			Storage m_pos, m_normal, m_tangent, m_uv, m_weights;
		};

		struct BufferView
		{
			GpuBuffer* vbo;
			uint32_t byteOffset;
			uint32_t byteStride;
			size_t byteLength;
		};

		struct Attribute
		{
			template<class T> auto& get(size_t i) const {
				auto rawData = (void*)((int)bufferView->data + (int)offset);
				auto formattedData = reinterpret_cast<const T*>(rawData);
				return formattedData[i];
			}

			std::shared_ptr<BufferView> bufferView;
			math::AABB bounds;
			uint32_t offset;
			uint32_t stride;
			uint32_t count;
			VtxFormat::Storage componentType;
			uint8_t nComponents;
			bool normalized;
		};

	public:
		RenderGeom() = default;

		RenderGeom(
			const Attribute& indices,
			const Attribute& position,
			const Attribute* normal,
			const Attribute* tangent,
			const Attribute* uv0,
			const Attribute* weights,
			const Attribute* joints);

		//static RenderGeom quad(const math::Vec2f& size);
		//GLuint getVao() const { return m_vao; }
		auto& indices() const { return m_indices; }
		auto& vertices() const { return m_indices; }
		const math::AABB& bbox() const { return m_bbox; }
		VtxFormat vertexFormat() const { return m_vtxFormat; }

	private:
		VtxFormat m_vtxFormat;
		math::AABB m_bbox;

		Attribute m_vertices;
		Attribute m_indices;
	};

}