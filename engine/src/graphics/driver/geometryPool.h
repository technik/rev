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

#include <graphics/driver/openGL/openGL.h>
#include <memory>
#include <vector>

namespace rev { namespace graphics {

	class RenderObj;
	class RenderGeom;

	//----------------------------------------------------------------------------------------------
	struct GeometryPool
	{
		// TODO: Improve API to allow preallocation of a big buffer for multiple meshes
		// TODO: Use shared_ptr for buffer lifetime management
		// TODO: Internally manage allocations to reuse buffer space when creating meshes with high frequency
		// TODO: Allow flexible vertex formats
		RenderObj*	createGeometry	(
			std::vector<uint8_t> _vtxData,
			std::vector<uint8_t> _ndxData
		);

		void		destroyGeometry	(RenderObj* t);

		void submit(); // Send data to the gpu

		/*struct DataLocator
		{
			GLuint vbo;
			GLvoid* byteOffset;
		};

		GeometryPool()
		{
			glGenBuffers(1,&mVtxData.vbo);
			glGenBuffers(1,&mNdxData.vbo);
		}

		~GeometryPool()
		{
			glDeleteBuffers(1,&mVtxData.vbo);
			glDeleteBuffers(1,&mNdxData.vbo);
		}

		DataLocator addVertexBufferData(GLvoid* data, GLuint size)
		{
			return addBufferData(data, size, mVtxData);
		}

		DataLocator addIndexBufferData(GLvoid* data, GLuint size)
		{
			return addBufferData(data, size, mNdxData);
		}

		void submit()
		{
			// Submit vertex data
			glBindBuffer(GL_ARRAY_BUFFER, mVtxData.vbo);
			glBufferData(
				GL_ARRAY_BUFFER,
				mVtxData.data.size(),
				mVtxData.data.data(),
				GL_STATIC_DRAW);
			// Submit index data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVtxData.vbo);
			glBufferData(
				GL_ARRAY_BUFFER,
				mVtxData.data.size(),
				mVtxData.data.data(),
				GL_STATIC_DRAW);
		}*/

	private:

		/*struct Buffer {
			std::vector<uint8_t>	data;
			GLuint vbo;
		};

		DataLocator addBufferData(GLvoid* _data, GLuint _size, Buffer& _targetBuffer)
		{
			auto offset = _targetBuffer.data.size();
			_targetBuffer.data.resize(offset+_size);
			memcpy(&_targetBuffer.data[offset], _data, _size);
			return { _targetBuffer.vbo, (GLvoid*)offset };
		}

		Buffer mVtxData;
		Buffer mNdxData;*/
	};

}}
