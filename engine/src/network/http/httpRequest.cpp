//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "httpRequest.h"

#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;

namespace rev {
	namespace net {
		namespace http {

			//----------------------------------------------------------------------------------------------------------------------
			Request::Request(METHOD _method, const string& _url, // Status line
				const string& _body) // Body
				:mMethod(_method)
				, mUrl(_url)
			{
				setBody(_body);
				setReady();
			}

			//----------------------------------------------------------------------------------------------------------------------
			Request::Request(const string& _raw) {
				// Break the string into parts
				this->operator<<(_raw);
			}

			//----------------------------------------------------------------------------------------------------------------------
			Request Request::jsonRequest(METHOD _method, const std::string& _url, const Json& _payload) {
				Request r(_method, _url, "");
				r.headers()["Content-Type"] = "application/json; charset=UTF-8";
				r.setBody(_payload.dump());
				return r;
			}

			//----------------------------------------------------------------------------------------------------------------------
			int Request::processMessageLine(const std::string& _requestLine) {
				unsigned methodLen = _requestLine.find(" ");
				string method = _requestLine.substr(0, methodLen);
				if (method == "GET")
					mMethod = Get;
				else if (method == "POST")
					mMethod = Post;
				else if (method == "PUT")
					mMethod = Put;
				else {
					cout << "Error: Unsupported method parsing http request\n"
						<< method << "\n";
					return -1;
				}

				unsigned urlEnd = _requestLine.find(" ", methodLen + 1);
				if (urlEnd == string::npos)
					return -1;
				mUrl = _requestLine.substr(methodLen + 1, urlEnd - methodLen - 1);
				if (mUrl.empty())
					return -1;
				return _requestLine.size();
			}

			//----------------------------------------------------------------------------------------------------------------------
			void Request::serializeMessageLine(string& _dst) const {
				stringstream statusLine;
				switch (mMethod)
				{
				case Get:
					statusLine << "GET";
					break;
				case Post:
					statusLine << "POST";
					break;
				case Put:
					statusLine << "PUT";
					break;
				default:
					assert(false);
					break;
				}
				statusLine << " " << mUrl << " HTTP/1.1\r\n";
				_dst.append(statusLine.str());
			}

}	}	}	// namespace rev::net::http