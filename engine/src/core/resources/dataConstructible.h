//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <cjson/json.h>
#include <map>
#include <string>

namespace rev {
	namespace core {

		template<class Derived_>
		struct DataConstructible {
			static Derived_* construct(const cjson::Json&);
			
			typedef std::function<TransformSrc*(const cjson::Json&)>	Factory;
			static void registerFactory(Factory);

		private:
			static std::map<std::string, Factory>	sFactories;
		};

		template<class Derived_>
		DataConstructible<Derived_>::Derived_* DataConstructible<Derived_>::construct(const cjson::Json& _data) {
			// Locate factory
			auto iter = sFactories.find((std::string)_data["_type"]);
			if (iter != sFactories.end()) {
				// Invoke construction
				return (iter->second)(_data);
			}
			return nullptr; // Unable to locate factory
		}
} }