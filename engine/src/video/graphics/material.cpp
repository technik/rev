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
using namespace rev::math;

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		Material* Material::loadFromFile(const std::string& _fileName) {
			Json materialData;
			if(!materialData.parse(ifstream(_fileName)))
				return nullptr;
			Material* m = new Material;
			m->mShader = materialData["shader"];

			const Json& vectors = materialData["vectors"];
			for (auto i = vectors.begin(); i != vectors.end(); ++i) {
				if (i->size() == 3) {
					const Json& v = *i;
					Vec3f vec3(v(0), v(1), v(2));
					m->mVec3s.push_back(std::make_pair(i.key(), vec3));
				}
			}

			const Json& maps = materialData["maps"];
			for (auto i = maps.begin(); i != maps.end(); ++i) {
				const Json& map = *i;
				Texture* t = Texture::loadFromFile((string)map);
				if(!t)
					continue;
				m->mMaps.push_back(make_pair(i.key(), t));
			}

			const Json& floats = materialData["floats"];
			for (auto i = floats.begin(); i != floats.end(); ++i) {
				m->mFloats.push_back(make_pair(i.key(), (float)(*i)));
			}
			return m;
		}
} }