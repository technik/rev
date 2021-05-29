//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <string>
#include <unordered_map>
#include "httpMessage.h"
#include <nlohmann/json.hpp>

namespace rev {
	namespace net {
		namespace http {

			class Response : public Message {
			public:
				using Json = nlohmann::json;

				Response(const std::string& _rawResponse);
				Response(unsigned _statusCode, const std::string& _desc);
				virtual ~Response() = default;

				unsigned			statusCode() const { return mStatusCode; }
				const std::string&	statusDesc() const { return mStatusDesc; }

				// Different types of response
				static Response		response200(const std::string& _customMessage = "");
				static Response		response404(const std::string& _custimMessage = "Error 404: Not found");
				static Response		jsonResponse(const Json& _payload, unsigned _code = 200);
				static Response		htmlResponse(const std::string& _fileName, unsigned _code = 200);

			private:
				static	std::string	shortDesc(unsigned _code);
				int		processMessageLine(const std::string& _raw) override;
				void	serializeMessageLine(std::string& dst) const override;

				unsigned mStatusCode;
				std::string mStatusDesc;
			};

		}	// namespace http
	}	// namespace net
}	// namespace rev
