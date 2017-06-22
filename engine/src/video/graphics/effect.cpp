//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
#include "effect.h"

#include <cjson/json.h>
#include <fstream>

using namespace cjson;
using namespace std;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		Effect* Effect::loadFromFile(const string& _fileName) {
			Json effectData;
			if (!effectData.parse(ifstream(_fileName)))
				return nullptr;
			Effect* fx = new Effect;
			fx->mShader = effectData["shader"];
			fx->blend = string(effectData["blend"]) == "on";
			fx->zWrite = string(effectData["blend"]) == "on";
			return fx;
		}

	}
}