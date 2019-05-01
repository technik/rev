//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <memory>
//#include <graphics/backend/OpenGL/openGL.h>
#include <string>
#include <vector>

namespace rev::gfx {

	class Shader {
	public:
		static std::unique_ptr<Shader>	createShader(const char* code);
		static std::unique_ptr<Shader>	createShader(const std::vector<const char*>& code);
	};

} // namespace rev::gfx
