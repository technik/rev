//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/driver/openGL/openGL.h>
#include <math/algebra/vector.h>
#include <math/geometry/aabb.h>
#include <math/geometry/types.h>
#include <iostream>
#include <vector>
#include <memory>

namespace rev { namespace graphics {

	class RenderGeom {
	public:
		
		struct Vertex {
			math::Vec3f position, tangent, bitangent, normal;
			math::Vec2f	uv;
		};

		struct BufferView
		{
			GLuint vbo;
			GLint byteStride;
			size_t byteLength;
			const void* data;
		};

		struct Attribute
		{
			std::shared_ptr<BufferView> bufferView;
			GLvoid* offset;
			GLenum componentType;
			GLint nComponents;
			GLsizei stride;
			GLsizei count;
			bool normalized;
			math::AABB bounds;
		};

		RenderGeom() = default;

		RenderGeom(
			const Attribute* indices,
			const Attribute* position,
			const Attribute* normal,
			const Attribute* tangent,
			const Attribute* uv0)
		{
			assert(indices);
			assert(indices->componentType == GL_UNSIGNED_SHORT || indices->componentType == GL_UNSIGNED_BYTE);
			m_indices = *indices;
			assert(position);
			m_vtxAttributes.emplace_back(0, *position);
			if(normal)
				m_vtxAttributes.emplace_back(1, *normal);
			if(tangent)
				m_vtxAttributes.emplace_back(2, *tangent);
			if(uv0)
				m_vtxAttributes.emplace_back(3, *uv0);

			m_bbox = position->bounds;
			
			initOpenGL();
		}
		/*
		static RenderGeom quad(const math::Vec2f& size)
		{
			auto half_x = 0.5f*size.x();
			auto half_y = 0.5f*size.y();
			std::vector<uint16_t> indices = { 0, 1, 2, 2, 1, 3};
			std::vector<Vertex> vertexData = {
				{{-half_x, -half_y, 0.f }, { 1.f, 0.f, 0.f }, {0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f }, { 0.f, 0.f }},
				{{ half_x, -half_y, 0.f }, { 1.f, 0.f, 0.f }, {0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f }, { 1.f, 0.f }},
				{{-half_x,  half_y, 0.f }, { 1.f, 0.f, 0.f }, {0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f }, { 0.f, 1.f }},
				{{ half_x,  half_y, 0.f }, { 1.f, 0.f, 0.f }, {0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f }, { 1.f, 1.f }}
			};

			return RenderGeom(vertexData, indices);
		}*/

		GLuint getVao() const { return m_vao; }
		auto& indices() const { return m_indices; }
		const math::AABB& bbox() const { return m_bbox; }

	private:
		math::AABB m_bbox;
		GLuint m_vao = 0;
		std::vector<std::pair<GLuint, Attribute>> m_vtxAttributes; // Attribute index, attribute data
		Attribute m_indices;

	private:
		void initOpenGL() {

			// VAO
			glGenVertexArrays(1,&m_vao);
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices.bufferView->vbo);
			
			// Attributes
			for(auto& [ndx, attribute] : m_vtxAttributes)
			{
				glBindBuffer(GL_ARRAY_BUFFER, attribute.bufferView->vbo);
				glVertexAttribPointer(ndx, attribute.nComponents, attribute.componentType, attribute.normalized, attribute.stride, attribute.offset);
				glEnableVertexAttribArray(ndx); // Vertex pos
			}

			// Unbind VAO
			glBindVertexArray(0);
		}

	};

}}