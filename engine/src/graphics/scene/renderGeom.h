//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <math/algebra/vector.h>
#include <graphics/driver/openGL/openGL.h>
#include <vector>

namespace rev { namespace graphics {

	class RenderGeom {
	public:
		RenderGeom(
			const std::vector<math::Vec2f>& _vertices,
			const std::vector<uint16_t>& _indices
		)
			: mNIndices(_indices.size())
		{
			// Create geometry
			glGenVertexArrays(1,&vao);
			glGenBuffers(2,vbo);
			// VAO
			glBindVertexArray(vao);
			// VBO for vertex data
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(
				GL_ARRAY_BUFFER,
				sizeof(math::Vec2f)*_vertices.size(),
				_vertices.data(),
				GL_STATIC_DRAW);
			// VBO for index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER,
				sizeof(uint16_t)*_indices.size(),
				_indices.data(),
				GL_STATIC_DRAW);

			// Attributes
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glEnableVertexAttribArray(0); // Vertex pos
			glBindVertexArray(0);
		}

		void render()
		{
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, mNIndices, GL_UNSIGNED_SHORT, nullptr);
		}

	private:
		GLsizei mNIndices;

		GLuint vao;
		GLuint vbo[2];
	};

}}