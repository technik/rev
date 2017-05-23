//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <cjson/json.h>
#include <functional>
#include <map>
#include <string>

namespace rev {
	namespace core {

		template<class Derived_>
		struct DataConstructible {
			typedef std::function<Derived_*(const cjson::Json&)>	Factory;
			
			static Derived_* construct(const cjson::Json&);
			static void registerFactory(const std::string&, Factory);

		private:
			typedef std::map<std::string, Factory>	FactoryMap;
			static FactoryMap	sFactories;
		};

		template<class Derived_>
		Derived_* DataConstructible<Derived_>::construct(const cjson::Json& _data) {
			// Locate factory
			auto iter = sFactories.find((std::string)_data["type"]);
			if (iter != sFactories.end()) {
				// Invoke construction
				return (iter->second)(_data);
			}
			return nullptr; // Unable to locate factory
		}

		template<class Derived_>
		void DataConstructible<Derived_>::registerFactory(const std::string& _key, Factory _factory) {
			sFactories.insert(std::make_pair(_key, _factory));
		}
} }