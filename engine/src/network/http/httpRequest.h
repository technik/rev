//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "httpMessage.h"
#include <nlohmann/json.hpp>

namespace rev {
	namespace net {
		namespace http {

			class Request : public Message {
			public:
				using Json = nlohmann::json;

				enum METHOD {
					Get,
					Post,
					Put
				};
				// Construction
				Request(METHOD, const std::string& _url, // Status line
					const std::string& _body); // Body
				Request(const std::string& _rawRequest);
				virtual ~Request() = default;

				// Accessors
				METHOD				method() const { return mMethod; }
				const std::string&	url() const { return mUrl; }

				static Request		jsonRequest(METHOD, const std::string& _url, const Json& _payload);

			private:
				int		processMessageLine(const std::string& _raw) override;
				void	serializeMessageLine(std::string& _dst) const override;

			private:
				METHOD mMethod;
				std::string mUrl;
			};
}}}