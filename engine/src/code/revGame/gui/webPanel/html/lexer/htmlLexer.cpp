////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// html lexer

#include "htmlLexer.h"

#include <revCore/interpreter/lexer.h>
#include <revCore/interpreter/token.h>
#include <revGame/gui/webPanel/html/htmlTokens.h>

namespace rev { namespace game
{
	const CTokenRule htmlLexRules[] = {
		{eOpenEndTag,	"</"},
		{eOpenTag,		"<"},
		{eCloseTag,		"/?>"},
		{eAssignement,	"="},
		{ePercentage,	"%"},
		{eSpace,		" |\t|\n|\r|\f+"},
		{eInt,			"-?[0-9]+"},
		{eFloat,		"-?[0-9]+.[0-9]*f?"},
		{eColor,		"#[0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F]"},
		{eId,			"[a-z]+"},
		{eWord,			"[^ \t\n\r\f<"},
		{eString,		"\"[^\"]*"},
	};

	CLexer * CHtmlLexer::sHtmlLexer = 0;

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlLexer::init()
	{
		sHtmlLexer = new CLexer(htmlLexRules, sizeof(htmlLexRules)/sizeof(CTokenRule));
	}

	//------------------------------------------------------------------------------------------------------------------
	CLexer * CHtmlLexer::get()
	{
		return sHtmlLexer;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlLexer::end()
	{
		delete sHtmlLexer;
	}
}	// namespace game
}	// namespace rev

/*#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/string.h>

using namespace rev::codeTools;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	void CHtmlLexer::parseHtmlIntoTokens(const char * _html, rtl::vector<CHtmlToken>& _tokens)
	{
		// Constants
		char lineEndings[] = {'\n', 0xA, 0xB, 0xC, '\0'};
		char whiteSpaceCharacters[] = {' ', '\n', '\t', 0xA, 0xB, 0xC, 0xD, '\0'};

		// Code
		revAssert(0 != _html, "Error: Null pointer passed as html code");
		unsigned line = 0;
		unsigned linePos = 0;
		unsigned cursor = 0;
		char c = 0;
		while((c = _html[cursor]) != '\0')
		{
			CHtmlToken t;
			t.line = line;
			t.pos = linePos;
			t.text = 0;
			if(isCharacterInString(c, whiteSpaceCharacters))
			{
				// Space separators
				t.type = CHtmlToken::eSpace;
				while(isCharacterInString(_html[cursor], whiteSpaceCharacters))
				{
					++cursor;
					++linePos;
					if(isCharacterInString(_html[cursor], lineEndings))
					{
						linePos = 0;
						++line;
					}
				}
			}
			else if(c == '<')
			{
				if (compareString(&_html[cursor], "<!--"))
				{ 
					// Begin comment
					cursor+=4;
					linePos +=4;
					// Consume the comment
					while(!((_html[cursor] == '-')
						&&(_html[cursor+1] == '-')
						&&(_html[cursor+2] == '>')))
					{
						++linePos;
						if(isCharacterInString(_html[cursor], lineEndings))
						{
							linePos = 0;
							++line;
						}
						++cursor;
					}
					continue;
				}
				else if(_html[cursor+1] == '/')
				{
					t.type = CHtmlToken::eOpenEndTag;
					cursor+=2;
					linePos+=2;
				}
				else
				{
					t.type = CHtmlToken::eOpenTag;
					++cursor;
					++linePos;
				}
			}
			else if(c == '>') 
			{
				t.type = CHtmlToken::eCloseTag;
				++cursor;
				++linePos;
			}
			else if((c == '/') && (_html[cursor+1] == '>'))
			{
				t.type = CHtmlToken::eCloseTag;
				cursor+=2;
				linePos+=2;
			}
			else
			{
				// Plain text
				t.type = CHtmlToken::eWord;
				char word[512];
				unsigned cursor0 = cursor;
				++cursor;
				++linePos;
				while((!isCharacterInString(_html[cursor], whiteSpaceCharacters))
					&& (_html[cursor] != '<') 
					&& (_html[cursor] != '>')
					&& !((_html[cursor] == '/') && (_html[cursor+1] == '>')))
				{
					++linePos;
					if(isCharacterInString(_html[cursor], lineEndings))
					{
						linePos = 0;
						++line;
					}
					++cursor;
				}
				unsigned i;
				for(i = 0; i < (cursor - cursor0); ++i)
				{
					word[i] = _html[cursor0+i];
				}
				word[i] = '\0';
				char * t_text = new char[stringLength(word)];
				copyString(t_text, word);
				t.text = t_text;
			}
			_tokens.push_back(t);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CHtmlLexer::isANumber(char _x)
	{
		return (_x >= '0') && (_x <= '9');
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CHtmlLexer::isALetter(char _x)
	{
		return ((_x >= 'a') && (_x <= 'z')) || ((_x >= 'A') && (_x <= 'Z'));
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CHtmlLexer::isAlphanumeric(char _x)
	{
		return isALetter(_x) || isANumber(_x);
	}
}	// namespace game
}	// namespace rev*/