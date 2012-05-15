////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// Generic Syntactical analyzer

#ifndef _REV_CORE_INTERPRETER_PARSER_H_
#define _REV_CORE_INTERPRETER_PARSER_H_

#include <vector.h>
#include "token.h"

namespace rev
{
	struct CSyntagma
	{
		unsigned	value;
		bool		isTerminal;
	};

	struct CGrammarRule
	{
		unsigned		from;	// This rules turns a 'from' syntax element
		rtl::vector<CSyntagma>	to;
		//const CSyntagma	*const to];	// into a list of syntagmas 'to'
	};

	struct CParserNode
	{
		CSyntagma	mSyntagma;
		virtual ~CParserNode() {}
	};

	struct CParserLeaf : public CParserNode
	{
		CToken	mToken;
	};

	struct CParserNonLeaf: public CParserNode
	{
		~CParserNonLeaf() {for(unsigned i=0; i < mChildren.size(); ++i) delete mChildren[i];}
		rtl::vector<CParserNode*>	mChildren;
	};

	class CParser
	{
	public:
		CParser(const CGrammarRule * grammar, unsigned nRules);	// Rule 0 is interpreted as the starting rule
		CParserNode*	generateParseTree(const rtl::vector<CToken>& _tokens);

	private:
		const CGrammarRule*	mRules;
		unsigned			mNRules;
	};
}	// namespace rev

#endif // _REV_CORE_INTERPRETER_PARSER_H_
