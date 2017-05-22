//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Generic material
#include "material.h"
#include <video/basicTypes/texture.h>
#include <cjson/json.h>
#include <fstream>

using namespace cjson;
using namespace std;

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		Material* Material::loadFromFile(const std::string& _fileName) {
			Json materialData;
			if(!materialData.parse(ifstream(_fileName)))
				return nullptr;
			Material* m = new Material;
			m->albedo = Texture::load((string)materialData["albedoMap"]);
			return m;
		}
} }