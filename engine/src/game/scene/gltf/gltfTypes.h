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

#include <string>
#include "../scene.h"
#include <core/types/json.h>
#include <math/algebra/vector.h>

namespace rev { namespace game {
	namespace gltf {

		struct Buffer
		{
			Buffer(const std::string& assetsFolder, const core::Json& desc)
			{
				size_t length = desc["byteLength"];
				raw.resize(length);
				core::File bufferFile(assetsFolder + desc["uri"].get<std::string>());
				// TODO: Error checking with buffer size
				memcpy(raw.data(), bufferFile.buffer(), length);
			}

			std::vector<uint8_t> raw;
		};

		struct BufferView
		{
			uint8_t* data = nullptr;
			GLenum target = 0;
			//size_t size = 0;
			size_t stride = 0;
		};

		struct Accessor
		{
			std::string type;

			enum class ComponentType : uint32_t
			{
				BYTE = 5120,
				UNSIGNED_BYTE = 5121,
				SHORT = 5122,
				UNSIGNED_SHORT = 5123,
				UNSIGNED_INT = 5125,
				FLOAT = 5126
			} componentType;

			uint8_t* element(size_t i) const {
				return &view->data[view->stride*i + offset];
			}

			BufferView* view = nullptr;
			size_t offset = 0;
			size_t count = 0;
		};

		struct Primitive
		{
		public:
			Primitive(const std::vector<Accessor>& accessors, const core::Json& desc)
			{
				auto& attributes = desc["attributes"];
				position = &accessors[attributes["POSITION"]];
				normal = &accessors[attributes["NORMAL"]];
				tangent = &accessors[attributes["TANGENT"]];
				texCoord = &accessors[attributes["TEXCOORD_0"]];

				indices = &accessors[desc["indices"]];

				if(desc.find("material") != desc.end())
					material = desc["material"];
			}

			const Accessor* position = nullptr;
			const Accessor* normal = nullptr;
			const Accessor* tangent = nullptr;
			const Accessor* texCoord = nullptr;

			const Accessor* indices = nullptr;

			int material = -1;
		};

		struct Mesh
		{
			Mesh(const std::vector<Accessor>& accessors, const core::Json& desc)
			{
				for(auto& p : desc["primitives"])
					primitives.emplace_back(accessors, p);
			}

			std::vector<Primitive> primitives;
		};

		struct Light
		{
			Light(const core::Json& desc)
			{
				std::string type = desc["type"];
				if(type == "Directional")
					mType = Dir;
				if(type == "Spot")
					mType = Spot;
				if(type == "Point")
					mType = Point;
				if(type == "Area")
					mType = Area;
				if(mType == Point || mType == Spot)
					mRange = desc["range"];

				auto& baseColorDesc = desc["color"];
				mColor = {
					baseColorDesc[0].get<float>(),
					baseColorDesc[1].get<float>(),
					baseColorDesc[2].get<float>()
				};
			}

			math::Vec3f mColor;
			math::Vec3f mDirection;
			float mAngle;
			float mRange;

			enum Type {
				Dir,
				Spot,
				Point,
				Area
			};

			Type mType;
		};

	}
}}
