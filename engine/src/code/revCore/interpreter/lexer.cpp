////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// Generic lexical analyzer based on regular expression finite state machines

#include "lexer.h"

#include <revCore/fsm/regExpFsm.h>
#include <revCore/string.h>

#include <new>

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	CLexer::CLexer(const CTokenRule* _rules, unsigned _nRules)
		:mNRules(_nRules)
	{
		// Allocate memory for rules and tokens
		mRules = reinterpret_cast<CRegExpFsm*>(new char[sizeof(CRegExpFsm) * mNRules]);
		mTokens = new unsigned[mNRules];
		// Create rules and copy tokens
		for(unsigned i = 0; i < mNRules; ++i)
		{
			new(&mRules[i]) CRegExpFsm(_rules[i].rule);
			mTokens[i] = _rules[i].token;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CLexer::tokenizeCode(rtl::vector<CToken>& _tokenList, const char * _code)
	{
		unsigned cursor = 0;
		while('\0' != _code[cursor])
		{
			CToken t;
			cursor += getToken(t, &_code[cursor]);
			_tokenList.push_back(t);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CLexer::getToken(CToken& _t, const char * _code)
	{
		unsigned len = 1;
		bool matched = false;
		unsigned matchingFsm = 0;
		for(unsigned r = 0; r < mNRules; ++r)
		{
			if(0 != (len = mRules[r].consumes(_code)))
			{
				_t.type = mTokens[r];
				_t.line = 0;
				_t.pos = 0;
				matched = true;
				matchingFsm = r;
				break;
			}
		}
		if(matched)
		{
			while(mRules[matchingFsm].accepts(_code, len+1))
				++len;
			_t.text = new char[len+1];
			copyStringN(_t.text, _code, len);
			_t.text[len+1] = '\0';
			return len;
		}
		else
			return 0;
	}

}	// namespace rev
