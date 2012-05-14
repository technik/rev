////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 12th, 2012
////////////////////////////////////////////////////////////////////////////////
// html Dom Tree

//#include "htmlDomTree.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>

namespace rev { namespace game
{
	enum EInsertMode
	{
		eInitial,
		eBeforeHtml,
		eBeforeHead,
		eInHead,
		eAfterHead,
		eInBody,
		eText,
		eAfterBody
	};
	//------------------------------------------------------------------------------------------------------------------
/*	void CHtmlDomTree::loadFromTokenList(const rtl::vector<CHtmlToken>& _tokenList)
	{
		//EInsertMode insertMode = eInitial;
		rtl::vector<CHtmlToken>::const_iterator token = _tokenList.begin();
		while(token->type == CHtmlToken::eSpace) // Consume initial spaces
			++token;
		// Optional !DOCTYPE
		htmlAssert(token->type == CHtmlToken::eOpenTag, "Error: Html must start with an opening tag '<' token.", token->line, token->pos);
		++token;
		htmlAssert(token->type == CHtmlToken::eWord, "Error: Expecting either 'html' or 'DOCTYPE'", token->line, token->pos);
		if(compareString(token->text, "!DOCTYPE"))
			consumeDoctype(token);
		// Open html
		while(token->type == CHtmlToken::eSpace) // Consume spaces
			++token;
		htmlAssert(token->type == CHtmlToken::eOpenTag, "Error: Expecting a '<' token.", token->line, token->pos);
		++token;
		htmlAssert((token->type == CHtmlToken::eWord) && (compareString(token->text, "html")), "Error: Expenting 'html' tag name", token->line, token->pos);
		++token;
		htmlAssert(token->type == CHtmlToken::eCloseTag, "Error: Expecting a '>' token.", token->line, token->pos);
		// Open head
		while(token->type == CHtmlToken::eSpace) // Consume spaces
			++token;
		htmlAssert(token->type == CHtmlToken::eOpenTag, "Error: Expecting a '<' token.", token->line, token->pos);
		++token;
		htmlAssert((token->type == CHtmlToken::eWord) && (compareString(token->text, "head")), "Error: Expenting 'head' tag name", token->line, token->pos);
		++token;
		htmlAssert(token->type == CHtmlToken::eCloseTag, "Error: Expecting a '>' token.", token->line, token->pos);
		// close head
		while(token->type != CHtmlToken::eOpenEndTag)
			++token;
		htmlAssert((token->type == CHtmlToken::eWord) && (compareString(token->text, "head")), "Error: Expenting 'head' tag name", token->line, token->pos);
		++token;
		htmlAssert(token->type == CHtmlToken::eCloseTag, "Error: Expecting a '>' token.", token->line, token->pos);
		// Open head
		while(token->type == CHtmlToken::eSpace) // Consume spaces
			++token;
		htmlAssert(token->type == CHtmlToken::eOpenTag, "Error: Expecting a '<' token.", token->line, token->pos);
		++token;
		htmlAssert((token->type == CHtmlToken::eWord) && (compareString(token->text, "body")), "Error: Expenting 'body' tag name", token->line, token->pos);
		++token;
		htmlAssert(token->type == CHtmlToken::eCloseTag, "Error: Expecting a '>' token.", token->line, token->pos);
		for(; token != _tokenList.end(); ++token)
		{
			// Feed body
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlDomTree::consumeDoctype(rtl::vector<CHtmlToken>::const_iterator& _token)
	{
		++_token;
		while(_token->type == CHtmlToken::eSpace)
			++_token;
		htmlAssert((_token->type == CHtmlToken::eWord) && (compareString(_token->text, "html")), "Error parsing doctype", _token->line, _token->pos);
		++_token;
		while(_token->type == CHtmlToken::eSpace)
			++_token;
		htmlAssert(_token->type == CHtmlToken::eCloseTag, "Error parsing doctype", _token->line, _token->pos);
		++_token;
	}
	
	//------------------------------------------------------------------------------------------------------------------
	void CHtmlDomTree::renderToImage(unsigned char * _dstImg, unsigned _dstWidth, unsigned _dstHeight)
	{
		_dstImg;
		_dstWidth;
		_dstHeight;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlDomTree::htmlAssert(bool _condition, const char * _errorMessage, unsigned _line, unsigned _pos)
	{
		if(!_condition)
		{
			revLogN(_errorMessage, eError);
			revLog("In line ", eError);
			revLog(_line, eError);
			revLog(", position ", eError);
			revLogN(_pos, eError);
			rev::codeTools::SLog::get()->flush();
		}
	}*/
}	// namespace game
}	// namespace rev