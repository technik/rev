//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "httpMessage.h"

#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;

namespace rev {
	namespace net {
		namespace http {

			//----------------------------------------------------------------------------------------------------------
			const string Message::cContentLengthLabel = "Content-Length";

			//----------------------------------------------------------------------------------------------------------------------
			int Message::operator<<(const string& _raw) {
				// use mBody as a temporal buffer
				mBody.append(_raw);
				while (!mBody.empty()) {
					switch (mState) {
					case ParseState::MessageLine: {
						if (!parseMessageLine())
							return -1;
						break;
					}
					case ParseState::Headers: {
						if (!parseHeaders())
							return -1;
						if (mState == ParseState::Headers) // Not enough text yet
							return 0; // Keep reading
						break;
					}
					case ParseState::Body:
						if (!mRequiredBodyLength || (mBody.size() < mRequiredBodyLength))
							return 0; // Keep reading
						else {
							if (mRequiredBodyLength) {
								unsigned notParsed = mBody.size() - mRequiredBodyLength;
								mBody = mBody.substr(0, mRequiredBodyLength);
								return _raw.size() - notParsed;
							}
							else { // Accept everything
								return _raw.size();
							}
						}
						break;
					default:
						return -1; // Wrong state
					}
				}
				return 0; // Keep reading
			}

			//----------------------------------------------------------------------------------------------------------------------
			bool Message::isComplete() const {
				return mState == ParseState::Complete;
			}

			//----------------------------------------------------------------------------------------------------------------------
			void Message::setBody(const std::string& _body) {
				mBody = _body;
				stringstream ss;
				ss << body().size();
				mHeaders[cContentLengthLabel] = ss.str();
			}

			//----------------------------------------------------------------------------------------------------------------------
			string Message::serialize() const {
				string serial;
				serializeMessageLine(serial);
				serializeHeaders(serial);
				if (mBody.size())
					serial.append(mBody + "\r\n");
				return serial;
			}

			//----------------------------------------------------------------------------------------------------------------------
			bool Message::requiresClose() const {
				auto connHeader = mHeaders.find("Connection");
				return (connHeader != mHeaders.end()) && (connHeader->second == "close");
			}

			//----------------------------------------------------------------------------------------------------------------------
			bool Message::addHeader(const string& _headerLine) {
				unsigned colonPos = _headerLine.find(": ");
				if (colonPos == string::npos)
					return false;
				string label = _headerLine.substr(0, colonPos);
				string content = _headerLine.substr(colonPos + 2);
				mHeaders.insert(make_pair(
					label,
					content
				));
				return true;
			}

			//----------------------------------------------------------------------------------------------------------------------
			bool Message::processHeaders(const string& _headers) {
				string left = _headers;
				while (!left.empty())
				{
					unsigned split = left.find("\r\n");
					string header;
					if (split == string::npos) {
						header = left;
						left.clear();
					}
					else {
						header = left.substr(0, split);
						left = left.substr(split + 2);
					}
					if (!addHeader(header))
						return false;
				}
				auto iter = mHeaders.find(cContentLengthLabel);
				if (iter != mHeaders.end()) {
					mRequiredBodyLength = atoi(iter->second.c_str());
				}
				return true;
			}

			//----------------------------------------------------------------------------------------------------------------------
			bool Message::parseMessageLine() {
				size_t cursor = mBody.find("\r\n");
				if (cursor != string::npos) // Line end found
				{
					int consumed = processMessageLine(mBody.substr(0, cursor));
					if (consumed > 0) // success
					{
						mBody = mBody.substr(consumed + 2);
						mState = ParseState::Headers;
						return true;
					}
					else { // Parsing error
						mState = ParseState::Error;
						return false;
					}
				}
				else // No line end found, need more text
					return true;
			}

			//----------------------------------------------------------------------------------------------------------------------
			bool Message::parseHeaders() {
				bool skipHeaders = mBody.find("\r\n") == 0; // if we find a blank line right at the begining, then there are no headers
				if (skipHeaders) {
					mState = ParseState::Body; // Waiting for the body
					return true;
				}
				size_t consumed = mBody.find("\r\n\r\n");
				if (consumed != string::npos)
				{
					bool success = processHeaders(mBody.substr(0, consumed));
					if (!success) { // Parsing error
						mState = ParseState::Error;
						return false;
					}
					mBody = mBody.substr(consumed + 4);
					mState = ParseState::Body;
					return true;
				}
				return true; // End of headers not yet found
			}

			//----------------------------------------------------------------------------------------------------------------------
			void Message::serializeHeaders(string& _dst) const {
				for (auto i : mHeaders) {
					_dst.append(i.first + ": " + i.second + "\r\n");
				}
				_dst.append("\r\n");
			}
}}}