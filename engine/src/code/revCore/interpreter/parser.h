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
		unsigned mRule;
		virtual unsigned nTokens() const = 0;
		virtual ~CParserNode() {}
	};

	struct CParserLeaf : public CParserNode
	{
		unsigned nTokens() const { return 1; }
		CToken	mToken;
	};

	struct CParserNonLeaf: public CParserNode
	{
		~CParserNonLeaf() {for(unsigned i=0; i < mChildren.size(); ++i) delete mChildren[i];}
		unsigned nTokens() const {
			unsigned size = 0;
			for(unsigned i=0; i < mChildren.size(); ++i)
				size+= mChildren[i]->nTokens();
			return size;
		}
		rtl::vector<CParserNode*>	mChildren;
	};

	class CParser
	{
	public:
		CParser(const CGrammarRule * grammar, unsigned nRules);	// Rule 0 is interpreted as the starting rule
		CParserNode*	generateParseTree(const rtl::vector<CToken>& _tokens) const;
		void stripTokens(rtl::vector<CToken>& _tkList, unsigned _tokenId) const;

	private:
		struct CParseState
		{
			unsigned	grammarRule;
			unsigned	redDot;
			unsigned	from;

			bool operator==(const CParseState& _x) const
			{
				return grammarRule==_x.grammarRule
					&&redDot==_x.redDot
					&&from==_x.from;
			}
		};

		typedef rtl::vector<CParseState>	TChartEntry;
		typedef rtl::vector<TChartEntry>	TChart;
	private:
		CParserNode*	parseTokenList(const rtl::vector<CToken>& _tokens) const;
		void			initMemoChart(unsigned _nTokens) const;
		bool			validateChart() const; // Evaluates wether the current parsing chart is valid given our reference grammar
		void			fillChartEntry(rtl::vector<CParserNode*>& _treeStack, unsigned i, const rtl::vector<CToken>& _tokens) const;
		// Do closure, reduction or shifting on the given state
		void			triStepState(rtl::vector<CParserNode*>& _treeStack, unsigned _entryIdx, const CParseState& _state, const rtl::vector<CToken>& _tokens) const;
		void			closure(unsigned _entry, const CParseState& _state) const;
		void			shift(unsigned _entry, const CParseState& _state, const rtl::vector<CToken>& _tokens) const;
		void			reduce(rtl::vector<CParserNode*>& _treeStack, unsigned _entryIdx, const CParseState& _state, const rtl::vector<CToken>& _tokens) const;
		void			finalReduce(rtl::vector<CParserNode*>& _treeStack, unsigned _entryIdx, const CParseState& _state, const rtl::vector<CToken>& _tokens) const;
		void			addState(unsigned _entry, const CParseState& _state) const;
		bool			stateFitsTokens(const CParseState& _next, const rtl::vector<CToken>& _tokens, unsigned _i) const;
		void			addToParseTree(rtl::vector<CParserNode*>& _treeStack, const CParseState& _state, const rtl::vector<CToken>& _tokens, unsigned _i) const;

	private:
		const CGrammarRule*	mRules;
		unsigned			mNRules;
		
		mutable TChart	mMemoChart;
	};
}	// namespace rev

#endif // _REV_CORE_INTERPRETER_PARSER_H_
