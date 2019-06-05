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
#include "../backend/gpuTypes.h"
#include <iostream>
#include <vector>
#include <memory>

namespace rev::gfx {

	class RenderGeom {
	public:
		struct VtxFormat {
			enum class Storage : uint8_t {
				None = 0,
				Float32 = 4,
				u8 = 1,
				u16 = 2,
				u32 = 4
			};

			VtxFormat() = default;
			VtxFormat(Storage pos, Storage nrm, Storage tan = Storage::None, Storage uv = Storage::None, Storage weights = Storage::None);

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
			GpuBuffer* data;
			uint32_t byteOffset;
			uint32_t byteStride;
			uint32_t byteLength;
		};

	public:
		RenderGeom() = default;

		// Attribute 0 must be vertex positions, and it will be used to fill the bbox
		RenderGeom(
			const VtxFormat& format,
			const VertexAttribute& indices,
			const VertexAttribute* vtxAttributes,
			uint32_t numAttr);

		//static RenderGeom quad(const math::Vec2f& size);
		//GLuint getVao() const { return m_vao; }
		auto& indices() const { return m_indices; }
		uint32_t numAttributes() const { return m_numAttr; }
		const VertexAttribute* attributes() const { return m_vtxAttributes; }

		const math::AABB& bbox() const { return m_bbox; }
		VtxFormat vertexFormat() const { return m_vtxFormat; }

	private:
		VtxFormat m_vtxFormat;
		math::AABB m_bbox;

		VertexAttribute m_vtxAttributes[4];
		uint32_t m_numAttr = 0;
		VertexAttribute m_indices;
	};

}