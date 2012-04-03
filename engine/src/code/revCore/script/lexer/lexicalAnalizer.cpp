////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lexical analizer of scripts

#include "lexicalAnalizer.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/script/scriptToken.h>
#include <revCore/string.h>

namespace rev { namespace script
{
	const char * SLexicalAnalizer::sOperators = "+-*/><=";
	const char * SLexicalAnalizer::sSeparators = " \t\n";

	//------------------------------------------------------------------------------------------------------------------
	int SLexicalAnalizer::parseCodeIntoTokens(const char * _code, rtl::vector<CScriptToken>& _tokenList)
	{
		unsigned cursor = 0;
		// TODO: Improve error messages to display error line and position
		char currentChar = _code[cursor];
		while('\0' != currentChar)
		{
			if(isCharacterInString(currentChar, sSeparators)) // Separator
			{
				++cursor; // Skip separator characters
			}
			else if(compareString("if", &_code[cursor], 2))
			{
				_tokenList.push_back(createToken(CScriptToken::eIf, &_code[cursor], 2));
				cursor+=2;
			}
			else if(compareString("else", &_code[cursor], 4))
			{
				_tokenList.push_back(createToken(CScriptToken::eElse, &_code[cursor], 4));
				cursor+=4;
			}
			else if(isALetter(currentChar) || currentChar == '_') // Identifier
			{
				int correctToken = processIdentifierToken(&_code[cursor], _tokenList);
				if(-1 == correctToken)
				{
					logErrorMessageAndCode(_code, cursor);
					return cursor; // Something went wrong, the token is not valid
				}
				else
					cursor += correctToken;
			}
			else if(isANumber(currentChar)) // Number
			{
				int correctToken = processNumberToken(&_code[cursor], _tokenList);
				if(-1 == correctToken)
				{
					logErrorMessageAndCode(_code, cursor);
					return cursor; // Something went wrong, the token is not valid
				}
				else
					cursor += correctToken;
			}
			else if(currentChar == '/' && (_code[cursor+1] == '/' || _code[cursor+1] == '*')) // Comment
			{
				cursor += skipComment(&_code[cursor]);
			}
			else if(isCharacterInString(currentChar, sOperators))
			{
				char nextChar = _code[cursor+1];
				if((currentChar == '=' && nextChar == '=' )							// ==
				|| (currentChar == '<' && (nextChar == '=' || nextChar == '<'))		// <=,<<
				|| (currentChar == '>' && (nextChar == '=' || nextChar == '>')))	// >=,>>
				{	// Single character operator
					_tokenList.push_back(createToken(CScriptToken::eOperator, &_code[cursor], 2));
					cursor+=2;
				}
				else
				{
					// Two characters operator
					_tokenList.push_back(createToken(CScriptToken::eOperator, &_code[cursor], 1));
					++cursor;
				}
			}
			else if(currentChar == ';')
			{
				_tokenList.push_back(createToken(CScriptToken::eSemicolon, &_code[cursor], 1));
				++cursor;
			}
			else if(currentChar == '(')
			{
				_tokenList.push_back(createToken(CScriptToken::eOpenPar, &_code[cursor], 1));
				++cursor;
			}
			else if(currentChar == ')')
			{
				_tokenList.push_back(createToken(CScriptToken::eClosePar, &_code[cursor], 1));
				++cursor;
			}
			else if(currentChar == '{')
			{
				_tokenList.push_back(createToken(CScriptToken::eOpenCBraces, &_code[cursor], 1));
				++cursor;
			}
			else if(currentChar == '}')
			{
				_tokenList.push_back(createToken(CScriptToken::eCloseCBraces, &_code[cursor], 1));
				++cursor;
			}
			else
			{
				codeTools::SLog::log("Error: Unknown token");
				logErrorMessageAndCode(_code, cursor);
				return cursor;
			}
			currentChar = _code[cursor];
		}
		return -1;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool SLexicalAnalizer::isANumber(char _x)
	{
		return (_x >= '0') && (_x <= '9');
	}

	//------------------------------------------------------------------------------------------------------------------
	bool SLexicalAnalizer::isALetter(char _x)
	{
		return ((_x >= 'a') && (_x <= 'z')) || ((_x >= 'A') && (_x <= 'Z'));
	}

	//------------------------------------------------------------------------------------------------------------------
	bool SLexicalAnalizer::isAlphanumeric(char _x)
	{
		return isALetter(_x) || isANumber(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	int SLexicalAnalizer::processNumberToken(const char * _code, rtl::vector<CScriptToken>& _tokenList)
	{
		unsigned cursor = 0;
		while(isANumber(_code[cursor])) // Go till the end of the number
			++cursor;
		if(_code[cursor] == '.')	// Real literal
		{
			codeTools::SLog::log("Error (Lexical): Real literal constant in script code. Real literals aren't supported yet");
			return -1;
		}
		else // Integer literal
		{
			_tokenList.push_back(createToken(CScriptToken::eIntegerLiteral, _code, cursor));
			return cursor;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	int SLexicalAnalizer::processIdentifierToken(const char * _code, rtl::vector<CScriptToken>& _tokenList)
	{
		unsigned cursor = 0;
		// Go to the end of the token
		while(isAlphanumeric(_code[cursor]) || _code[cursor] == '_')
			++cursor;
		_tokenList.push_back(createToken(CScriptToken::eIdentifier, _code, cursor));
		return cursor; // Unimplemented
	}

	//------------------------------------------------------------------------------------------------------------------
	CScriptToken SLexicalAnalizer::createToken(CScriptToken::ETokenType _type, const char * _code, unsigned length)
	{
		// Create a token
		CScriptToken token;
		// Set token type
		token.mType = _type;
		// Copy the string into the token
		token.mContent = new char[length+1];
		copyStringN(token.mContent, _code, length);
		// Add the token to the list
		return token;
	}

	//------------------------------------------------------------------------------------------------------------------
	void SLexicalAnalizer::logErrorMessageAndCode(const char * _code, unsigned cursor)
	{
		char message[256];
		sprintf(message, "Error parsing script code at position %d\nIn code:", cursor);
		codeTools::SLog::log(message);
		codeTools::SLog::log(_code);
	}

	//------------------------------------------------------------------------------------------------------------------
	int SLexicalAnalizer::skipComment(const char * _code)
	{
		if(_code[1] == '\0')
		{
			codeTools::SLog::log("Unexpected end of file after \"/\"");
			return -1;
		}
		if(_code[1] == '/') // Single line comment
		{
			unsigned cursor = 2;
			while(_code[cursor] != '\n' && _code[cursor] != '\0')
				++cursor;
			return cursor;
		}
		else if (_code[1] == '*') // Comment block
		{
			unsigned cursor = 2;
			bool endFound = false;
			while(!endFound)
			{
				while(_code[cursor] != '*')
					++cursor;
				if(_code[cursor+1] == '/')
					endFound;
				else
					++cursor;
			}
			while(_code[cursor] != '\n' && _code[cursor] != '\0')
				++cursor;
			return cursor;
		}
		else
		{
			codeTools::SLog::log("Error: Unexpected character after \"/\"");
			return -1;
		}
	}

}	// namespace script
}	// namespace rev
