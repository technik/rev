//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "httpResponse.h"

#include <cassert>
#include <sstream>
#include <core/platform/fileSystem/file.h>

using namespace std;

namespace rev {
	namespace net {
		namespace http {

			//----------------------------------------------------------------------------------------------------------
			Response::Response(const string& _rawResponse) {
				if(_rawResponse.size() >= 2 && _rawResponse.back() == '\n' && _rawResponse[_rawResponse.size()-2] == '\r')
					this->operator<<(_rawResponse);
				else
					this->operator<<(_rawResponse+"\r\n");
			}

			//------------------------------------------------------------------------------------------------------------------
			Response::Response(unsigned _statusCode, const string& _desc)
				: mStatusCode(_statusCode)
				, mStatusDesc(_desc)
			{
				setReady();
			}

			//------------------------------------------------------------------------------------------------------------------
			Response Response::response200(const string& _customMessage) {
				Response r(200, shortDesc(200));
				r.setBody(_customMessage);
				return r;
			}

			//------------------------------------------------------------------------------------------------------------------
			Response Response::response404(const string& _customMessage) {
				Response r(404, shortDesc(404));
				r.setBody(_customMessage);
				return r;
			}

			//------------------------------------------------------------------------------------------------------------------
			Response Response::jsonResponse(const cjson::Json& _payload, unsigned _code) {
				Response r(_code, shortDesc(_code));
				r.setBody(_payload.serialize());
				r.headers()["Content-type"] = "application/json";
				return r;
			}

			//------------------------------------------------------------------------------------------------------------------
			Response Response::htmlResponse(const string& _fileName, unsigned _code) {
				// Open file
				auto pageFile = core::File::manager()->get(_fileName);
				if (pageFile) {
					Response r(_code, shortDesc(_code));
					r.setBody(pageFile->bufferAsText());
					r.headers()["Content-type"] = "text/html";
					return r;
				}
				else return response404();
			}

			//------------------------------------------------------------------------------------------------------------------
			std::string Response::shortDesc(unsigned _code) {
				switch (_code) {
				case 200:
					return "Ok";
				case 404:
					return "Not Found";
				default:
					assert(false);
					return "";
				}
			}

			//------------------------------------------------------------------------------------------------------------------
			int Response::processMessageLine(const string& _status) {
				const string separators = " \t\n\r";
				unsigned versionEnd = _status.find_first_of(separators);
				unsigned codeStart = _status.find_first_not_of(separators, versionEnd + 1);
				mStatusCode = (unsigned)atoi(_status.substr(codeStart).c_str());
				unsigned codeEnd = _status.find_first_of(separators, codeStart + 1);
				mStatusDesc = _status.substr(codeEnd + 1);
				return _status.size();
			}

			//------------------------------------------------------------------------------------------------------------------
			void Response::serializeMessageLine(std::string& _serial) const {
				std::stringstream serial;
				// Status line
				serial << "HTTP/1.1 " << mStatusCode << " " << mStatusDesc << "\r\n";
				_serial.append(serial.str());
			}
}}}