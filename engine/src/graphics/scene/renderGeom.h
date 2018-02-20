//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/driver/openGL/openGL.h>
#include <math/algebra/vector.h>
#include <iostream>
#include <vector>

namespace rev { namespace graphics {

	class RenderGeom {
	public:
		struct Vertex {
			math::Vec3f position, tangent, bitangent, normal;
			math::Vec2f	uv;
		};

		RenderGeom() = default;

		RenderGeom(
			const std::vector<Vertex>& _vertices,
			const std::vector<uint16_t>& _indices,
			bool _initGL = true
		)
			: mVertices(_vertices)
			, mIndices(_indices)
		{
			if(_initGL)
				initOpenGL();
		}

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
		}

		void render() const
		{
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT, nullptr);
		}

		void deserialize(std::istream& _in)
		{
			assert(!_in.eof() && !_in.bad());
			// Load header
			uint32_t nVertices = 0, nIndices = 0;
			_in.read((char*)&nVertices, sizeof(nVertices));
			_in.read((char*)&nIndices, sizeof(nIndices));
			// Load vertex data
			mVertices.resize(nVertices);
			_in.read((char*)mVertices.data(), sizeof(Vertex)*nVertices);
			// Load indices
			mIndices.resize(nIndices);
			_in.read((char*)mIndices.data(), sizeof(uint16_t)*nIndices);
			if(!_in)
			{
				if(_in.badbit & _in.rdstate())
					return;
				if(_in.eofbit & _in.rdstate())
					return;
				if(_in.failbit & _in.rdstate())
					return;
				return;
			}
			if(_in.eof())
				return;
			initOpenGL();
		}

		void serialize(std::ostream& _out) const
		{
			// Save header
			uint32_t nVertices = mVertices.size();
			_out.write((char*)&nVertices, sizeof(nVertices));
			uint32_t nIndices = mIndices.size();
			_out.write((char*)&nIndices, sizeof(nIndices));
			// Save vertex data
			_out.write((const char*)mVertices.data(), sizeof(Vertex)*(size_t)nVertices);
			// Save index data
			_out.write((const char*)mIndices.data(), sizeof(uint16_t)*(size_t)nIndices);
		}

	private:
		void initOpenGL() {
			// Create geometry
			glGenVertexArrays(1,&vao);
			glGenBuffers(2,vbo);
			// VAO
			glBindVertexArray(vao);
			// VBO for vertex data
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(
				GL_ARRAY_BUFFER,
				sizeof(Vertex)*mVertices.size(),
				mVertices.data(),
				GL_STATIC_DRAW);
			// VBO for index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER,
				sizeof(uint16_t)*mIndices.size(),
				mIndices.data(),
				GL_STATIC_DRAW);

			// Attributes
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
			glEnableVertexAttribArray(0); // Vertex pos
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(math::Vec3f));
			glEnableVertexAttribArray(1); // Vertex tangent
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(2*sizeof(math::Vec3f)));
			glEnableVertexAttribArray(2); // Vertex bitangent
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3*sizeof(math::Vec3f)));
			glEnableVertexAttribArray(3); // Vertex normal
			glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(4*sizeof(math::Vec3f)));
			glEnableVertexAttribArray(4); // Vertex uv

			// Unbind VAO
			glBindVertexArray(0);
		}

		GLuint vao = 0;
		GLuint vbo[2] = {0,0};

		std::vector<Vertex> mVertices;
		std::vector<uint16_t> mIndices;
	};

}}