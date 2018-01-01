//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <memory>

namespace rev { namespace graphics {

	class Shader {
	public:
		static std::unique_ptr<Shader>	createShader(const char* code);
	};

}}	// namespace rev::graphics
