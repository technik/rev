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
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <core/platform/fileSystem/fileSystem.h>
#include <memory>

namespace rev::gfx {

	/*class Effect
	{
	public:
		// Effect creation
		Effect(const std::string& _fileName);

		using Property = ShaderProcessor::Uniform;

		const Property* property(const std::string& name) const;
		const std::vector<Property>& properties() const { return m_properties; }
		const std::string& code() const { return m_code; }

		using ReloadCb = std::function<void(const Effect&)>;

		void onReload(ReloadCb cb) {
			m_reloadCbs.push_back(cb);
		}

	private:
		void invokeCallbacks();
		void loadFromFile(const char* _fileName, ShaderProcessor::MetaData& metadata);

		void reload();

		std::vector<ReloadCb>	m_reloadCbs;
		std::vector<Property>	m_properties;
		std::string				m_code;
		std::string				m_fileName;
		using FileListener = std::shared_ptr<core::FileSystem::FileEvent::Listener>;
		std::vector<FileListener> m_fileListeners;
		// TODO: Support shader permutations by defining #pragma shader_option in a shader
		// when the material enables the option, the shader option will be #defined in the material
		// Advanced uses may allow enumerated or integer values for the options
		// TODO: Let a shader specify that it requires specific data from the vertex or fragment stages
		// This should allow optimization of shaders when some computations won't be used.
	};*/

}