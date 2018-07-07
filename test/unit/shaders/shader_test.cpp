//----------------------------------------------------------------------------------------------------------------------
// Math unit testing
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <graphics/driver/shaderProcessor.h>

using namespace rev::graphics;
using Context = ShaderProcessor::Context;
using MetaData = ShaderProcessor::MetaData;

//----------------------------------------------------------------------------------------------------------------------
int main() {
	Context context;
	context.m_pendingCode.emplace_back(0, R"(
#include "header.fx"
//#include "nope.fx";
#include "pbr.fx"

uniform sampler2D colorMap;
layout(location = 3) uniform sampler2D normalMap;

#pragma options
#pragma		debug_variants a b

void foo()
{
	// whatever;
	return;
}

)");
	MetaData data;
	std::string code;
	ShaderProcessor::processCode(context, false, code, data);

	// Examine resulting code
	assert(code.find("location") == std::string::npos);
	assert(code.find("\"header.fx\"") == std::string::npos);
	assert(code.find("\"pbr.fx\"") == std::string::npos);
	assert(code.find("\"nope.fx\"") != std::string::npos);
	assert(code.find("normalMap") == std::string::npos);
	assert(code.find("layout") == std::string::npos);
	assert(code.find("debug_variants") == std::string::npos);
	assert(code.find("#pragma") == std::string::npos);

	// Examine metadata
	assert(data.pragmas.size() == 2);
	assert(data.pragmas[0] == "options");
	assert(data.pragmas[1] == "debug_variants a b");

	assert(data.dependencies.size() == 2);
	assert(data.dependencies[0] == "header.fx");
	assert(data.dependencies[1] == "pbr.fx");
	assert(data.uniforms.size() == 1);
	auto& uniform = data.uniforms[0];
	assert(uniform.location == 3);
	assert(uniform.name == "normalMap");
	assert(uniform.type == ShaderProcessor::Uniform::Texture2D);

	return 0;
}