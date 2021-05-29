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

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <graphics/scene/renderGeom.h>

namespace rev { namespace game {

	class ModelAsset
	{
	public:
		ModelAsset(const std::string& fileName)
		{
			std::ifstream in(fileName, std::istream::binary);
			if(in.is_open())
			{
				mName = fileName;
				// Preallocate space
				uint32_t nMeshes;
				in.read((char*)&nMeshes, sizeof(nMeshes));
				// Load meshes
				mMeshes.reserve(nMeshes);
				for(uint32_t i = 0; i < nMeshes; ++i)
				{
					if(!in)
						return;
					if(in.eof())
						return;
					mMeshes.push_back(std::make_shared<graphics::RenderGeom>());
					mMeshes.back()->deserialize(in);
					if(!in)
						return;
					if(in.eof())
						return;
				}
			}
		}

		std::shared_ptr<graphics::RenderGeom>	mesh(uint32_t ndx) const { return mMeshes[ndx]; }

		const std::string& name() const { return mName; }
	private:
		std::string mName;
		std::vector<std::shared_ptr<graphics::RenderGeom>> mMeshes;
	};

}}
