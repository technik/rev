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

#include "renderGeom.h"

namespace rev::gfx
{
	//----------------------------------------------------------------------------------------------
	RenderGeom::VtxFormat::VtxFormat(Storage pos, Storage nrm, Storage tan, Storage uv, Storage weights)
	{
		m_pos = pos;
		m_normal = nrm;
		m_tangent = tan;
		m_uv = uv;
		m_weights = weights;
	}

	//----------------------------------------------------------------------------------------------
	uint32_t RenderGeom::VtxFormat::code() const
	{
		return uint32_t(m_pos) << 24
			| uint32_t(m_normal) << 16
			| uint32_t(m_tangent) << 8
			| uint32_t(m_uv)<<4
			| uint32_t(m_weights);
	}

	//----------------------------------------------------------------------------------------------
	RenderGeom::RenderGeom(
		const Attribute* indices,
		const Attribute* position,
		const Attribute* normal,
		const Attribute* tangent,
		const Attribute* uv0,
		const Attribute* weights,
		const Attribute* joints)
	{
		assert(indices);
		//assert(indices->componentType == GL_UNSIGNED_SHORT || indices->componentType == GL_UNSIGNED_BYTE);
		/*m_indices = *indices;
		assert(position);
		m_vtxAttributes.emplace_back(0, *position);
		m_vtxFormat = VtxFormat(
			VtxFormat::Storage::Float32,
			normal?VtxFormat::Storage::Float32 : VtxFormat::Storage::None,
			tangent?VtxFormat::Storage::Float32 : VtxFormat::Storage::None,
			uv0?VtxFormat::Storage::Float32 : VtxFormat::Storage::None,
			(weights&&joints)?VtxFormat::Storage::Float32 : VtxFormat::Storage::None
		);
		if(normal)
			m_vtxAttributes.emplace_back(1, *normal);
		if(tangent)
			m_vtxAttributes.emplace_back(2, *tangent);
		if(uv0)
			m_vtxAttributes.emplace_back(3, *uv0);
		if(weights)
			m_vtxAttributes.emplace_back(4, *weights);
		if(joints)
			m_vtxAttributes.emplace_back(5, *joints);

		m_bbox = position->bounds;

		initOpenGL();*/
	}

	//----------------------------------------------------------------------------------------------
	void RenderGeom::initOpenGL() {

		// VAO
		/*glGenVertexArrays(1,&m_vao);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices.bufferView->vbo.id());

		// Attributes
		for(auto& [ndx, attribute] : m_vtxAttributes)
		{
			glBindBuffer(GL_ARRAY_BUFFER, attribute.bufferView->vbo.id());
			glVertexAttribPointer(ndx, attribute.nComponents, attribute.componentType, attribute.normalized, attribute.stride, attribute.offset);
			glEnableVertexAttribArray(ndx); // Vertex pos
		}

		// Unbind VAO
		glBindVertexArray(0);*/
	}
	
	//----------------------------------------------------------------------------------------------
	/*RenderGeom RenderGeom::quad(const math::Vec2f& size)
	{
		// Raw data
		auto half_x = 0.5f*size.x();
		auto half_y = 0.5f*size.y();
		std::vector<uint16_t> indices = { 0, 1, 2, 2, 1, 3};
		std::vector<math::Vec3f> vertices = {
			{-half_x, -half_y, 0.f },
		{ half_x, -half_y, 0.f },
		{-half_x,  half_y, 0.f },
		{ half_x,  half_y, 0.f }
		};
		std::vector<math::Vec3f> normals = {
			{0.f, 0.f, 1.f },
			{0.f, 0.f, 1.f },
			{0.f, 0.f, 1.f },
			{0.f, 0.f, 1.f }
		};

		// Buffer views
		// Indices
		/*auto idxBv = std::make_shared<BufferView>();
		idxBv->byteLength = 2*indices.size();
		idxBv->data = indices.data();
		idxBv->byteStride = 0;
		idxBv->vbo = gfx::Buffer(GraphicsDriverGL::get()->allocateStaticBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBv->byteLength, idxBv->data));

		// Vertices
		auto vtxBv = std::make_shared<BufferView>();
		vtxBv->byteLength = sizeof(math::Vec3f)*vertices.size();
		vtxBv->data = vertices.data();
		vtxBv->byteStride = 0;
		vtxBv->vbo = gfx::Buffer(GraphicsDriverGL::get()->allocateStaticBuffer(GL_ARRAY_BUFFER, vtxBv->byteLength, vtxBv->data));

		// Normals
		auto nrmBv = std::make_shared<BufferView>();
		nrmBv->byteLength = sizeof(math::Vec3f)*vertices.size();
		nrmBv->data = normals.data();
		nrmBv->byteStride = 0;
		nrmBv->vbo = gfx::Buffer(GraphicsDriverGL::get()->allocateStaticBuffer(GL_ARRAY_BUFFER, nrmBv->byteLength, nrmBv->data));

		Attribute idxAttr {
			idxBv,
			nullptr,
			GL_UNSIGNED_SHORT,
			1,
			0,
			indices.size(),
			false,
			math::AABB()
		};

		Attribute vtxAttr {
			vtxBv,
			nullptr,
			GL_FLOAT,
			3,
			0,
			vertices.size(),
			false,
			math::AABB()
		};

		Attribute nrmAttr {
			nrmBv,
			nullptr,
			GL_FLOAT,
			3,
			0,
			normals.size(),
			false,
			math::AABB()
		};

		return RenderGeom(&idxAttr, &vtxAttr, &nrmAttr, nullptr, nullptr, nullptr, nullptr);
	}*/
}