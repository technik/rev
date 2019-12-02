//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
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

namespace rev::gfx {

	struct VtxFormat {

		VtxFormat() = default;
		VtxFormat(bool pos, bool nrm, bool tan, bool uv, bool weights)
		{
			m_code =
				(pos ? Flags::Position : 0)
				| (nrm ? Flags::Normal : 0)
				| (tan ? Flags::Tangent : 0)
				| (uv ? Flags::UV : 0)
				| (weights ? Flags::Weights : 0);
		}

		bool hasPosition() const { return m_code & Flags::Position; }
		bool hasNormal() const { return m_code & Flags::Normal; }
		bool hasTangent() const { return m_code & Flags::Tangent; }
		bool hasUV() const { return m_code & Flags::UV; }
		bool hasWeights() const { return m_code & Flags::Weights; }

		uint16_t code() const { return m_code; }
		bool isValid() const { return m_code == 0; }

		std::string shaderDefines() const
		{
			std::string defines;
			if (hasPosition())
				defines += "#define VTX_POSITION_FLOAT 0\n";
			if (hasNormal())
				defines += "#define VTX_NORMAL_FLOAT 1\n";
			if (hasTangent())
				defines += "#define VTX_TANGENT_FLOAT 2\n";
			if (hasUV())
				defines += "#define VTX_UV_FLOAT 3\n";
			return defines;
		}

	private:
		enum Flags
		{
			Position = 1 << 0,
			Normal = 1 << 1,
			Tangent = 1 << 2,
			UV = 1 << 3,
			Weights = 1 << 4
		};
		uint16_t m_code = 0;
	};

}