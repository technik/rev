////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// Generic Syntactical analyzer

#include "parser.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>

using namespace rtl;

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	CParserNode * CParser::parseTokenList(const vector<CToken>& _tokens) const
	{
		// We'll use this stack to store nodes as we close them.
		// If we close a node that has children, we'll take those children from the stack
		// before pushing the new node in.
		vector<CParserNode*>	treeStack;
		unsigned nTokens = _tokens.size();
		for(unsigned i = 0; i < nTokens; ++i)
		{
			fillChartEntry(treeStack, i, _tokens);
		}
		// Now perform reductions on the last entry of the table
		const TChartEntry& lastEntry = mMemoChart.back();
		for(unsigned s = 0; s < lastEntry.size(); ++s)
		{
			if(mRules[lastEntry[s].grammarRule].to.size() == lastEntry[s].redDot)
				finalReduce(treeStack, nTokens, lastEntry[s], _tokens);
			else if(!mRules[lastEntry[s].grammarRule].to[lastEntry[s].redDot].isTerminal)
			{
				closure(nTokens, lastEntry[s]);
			}
		}
		CParseState finalState = { 0, mRules[0].to.size(), 0 };
		addToParseTree(treeStack, finalState, _tokens, _tokens.size());
		return treeStack.back();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::fillChartEntry(vector<CParserNode*>& _treeStack, unsigned _i, const rtl::vector<CToken>& _tokens) const
	{
		const TChartEntry& entry = mMemoChart[_i];
		// Keep iterating as long as the entry keeps growing
		// Note: In this loop is very importan to use integers as index instead of vector iterators.
		// If we were using vector iterators and an internal relocation happend inside the chart entry
		// (which is so likely to happen since we're continuously making it grow), our iterator would
		// become invalid.
		for(unsigned i = 0; i < entry.size(); ++i) // Notice also that entry.size() is dynamic and may change as the table grows.
		{	// Probably, it will be bigger each time we check
			CParseState state = entry[i]; // Note we can't use a reference here for optimization. It must be a full copy.
			// If we used a reference and the entry growed so much that it needed relocation, the reference would get corrupted.
			triStepState(_treeStack, _i, state, _tokens);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::triStepState(vector<CParserNode*>& _treeStack, unsigned _i, const CParseState& _state, const rtl::vector<CToken>& _tokens) const
	{
		// If the state is at the end of the rule, reduction proceeds
		const CGrammarRule& rule = mRules[_state.grammarRule];
		if(_state.redDot == rule.to.size())
		{
			reduce(_treeStack, _i, _state, _tokens); // Reduce
		}
		else
		{
			// Notice we don't need to ensure the size of the rule isn't 0 because if it was, then redDot would have
			// Matched it in the previous check, and we wouldn't be here.
			const CSyntagma& nextSynt = rule.to[_state.redDot];
			if(!nextSynt.isTerminal) // Closure
			{
				closure(_i, _state);
			}
			else if( nextSynt.value == _tokens[_i].type ) // Shift
			{
				shift(_i, _state, _tokens);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::closure(unsigned _i, const CParseState& _state) const
	{
		unsigned ruleKey = mRules[_state.grammarRule].to[_state.redDot].value;
		for(unsigned r = 0; r < mNRules; ++r)
		{
			if(mRules[r].from == ruleKey)
			{
				CParseState newState = { r, 0, _i };
				addState(_i, newState);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::shift(unsigned _i, const CParseState& _state, const rtl::vector<CToken>& _tokens) const
	{
		unsigned nextToken = mRules[_state.grammarRule].to[_state.redDot].value;
		if(nextToken == _tokens[_i].type)
		{
			CParseState newState = { _state.grammarRule, _state.redDot+1, _state.from }; // Move redDot one position
			addState(_i+1, newState); // Add the state to the next entry in the memo chart
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::addToParseTree(rtl::vector<CParserNode*>& _treeStack, const CParseState& _state, const rtl::vector<CToken>& _tokens, unsigned _i) const
	{
		// Build the new node
		CParserNonLeaf * newNode = new CParserNonLeaf();
		newNode->mRule = &mRules[_state.grammarRule];
		const CGrammarRule& rule = mRules[_state.grammarRule];
		unsigned ruleSize = rule.to.size();
		newNode->mChildren.resize(ruleSize);
		unsigned tokenOffset = 1;
		for(int i = ruleSize-1; i >= 0; --i)
		{
			if(rule.to[i].isTerminal)
			{
				CParserLeaf * leaf = new CParserLeaf;
				leaf->mRule = 0; // rule.to[i].value;
				leaf->mToken = _tokens[_i - tokenOffset];
				++tokenOffset;
				newNode->mChildren[i] = leaf;
			}
			else
			{
				CParserNode * child = _treeStack.back();
				_treeStack.pop_back();
				tokenOffset += child->nTokens();
				newNode->mChildren[i] = child;
			}
		}
		// Add the new node to the tree
		_treeStack.push_back(newNode);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::reduce(rtl::vector<CParserNode*>& _treeStack, unsigned _entryIdx, const CParseState _state, const rtl::vector<CToken>& _tokens) const
	{
		const TChartEntry& oldEntry = mMemoChart[_state.from];
		unsigned ruleKey = mRules[_state.grammarRule].from;
		for(unsigned i = 0; i < oldEntry.size(); ++i) // Can't use iterators because oldEntry can grow if we reduce rules with an empty right side
		{
			const CParseState& state = oldEntry[i];
			if(mRules[state.grammarRule].to.size() == state.redDot)
				continue;
			const CSyntagma& synt = mRules[state.grammarRule].to[state.redDot];
			if((!synt.isTerminal) && (synt.value == ruleKey))
			{
				CParseState newState = { state.grammarRule, state.redDot+1, state.from };
				// Create the parse tree
				if(stateFitsTokens(newState, _tokens, _entryIdx))
				{
					addState(_entryIdx, newState);
					addToParseTree(	_treeStack, _state, _tokens, _entryIdx);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CParser::stateFitsTokens(const CParseState& _state, const rtl::vector<CToken>& _tokens, unsigned _i) const
	{
		if(mRules[_state.grammarRule].to.size() == _state.redDot) // State finished, can reduction fit?
		{
			const TChartEntry& oldEntry = mMemoChart[_state.from];
			unsigned ruleKey = mRules[_state.grammarRule].from;
			bool fit = false;
			for(unsigned i = 0; i < oldEntry.size(); ++i) // Can't use iterators because oldEntry can grow if we reduce rules with an empty right side
			{
				const CParseState& state = oldEntry[i];
				if(mRules[state.grammarRule].to.size() == state.redDot)
					continue;
				const CSyntagma& synt = mRules[state.grammarRule].to[state.redDot];
				if((!synt.isTerminal) && (synt.value == ruleKey))
				{
					CParseState newState = { state.grammarRule, state.redDot+1, state.from };
					// Create the parse tree
					fit |= stateFitsTokens(newState, _tokens, _i);
				}
			}
			return fit;
		}
		else
		{
			if(mRules[_state.grammarRule].to[_state.redDot].isTerminal)
			{
				if(mRules[_state.grammarRule].to[_state.redDot].value == _tokens[_i].type)// Is terminal next token?
					return true;
				return false;
			}
			else	// else can next state start with next token?
			{
				bool fit = false;
				for(unsigned r = 0; r < mNRules; ++r)
				{
					CParseState newState = { r, 0, _i };
					fit |= stateFitsTokens(newState, _tokens, _i);
				}
				return fit;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::finalReduce(rtl::vector<CParserNode*>& _treeStack, unsigned _entryIdx, const CParseState _state, const rtl::vector<CToken>& _tokens) const
	{
		const TChartEntry& oldEntry = mMemoChart[_state.from];
		unsigned ruleKey = mRules[_state.grammarRule].from;
		for(unsigned i = 0; i < oldEntry.size(); ++i) // Can't use iterators because oldEntry can grow if we reduce rules with an empty right side
		{
			const CParseState& state = oldEntry[i];
			if(mRules[state.grammarRule].to.size() == state.redDot)
				continue;
			const CSyntagma& synt = mRules[state.grammarRule].to[state.redDot];
			if((!synt.isTerminal) && (synt.value == ruleKey))
			{
				CParseState newState = { state.grammarRule, state.redDot+1, state.from };
				addState(_entryIdx, newState);
				
				addToParseTree(_treeStack, _state, _tokens, _entryIdx);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::initMemoChart(unsigned _nTokens) const
	{
		mMemoChart.resize(_nTokens+1);
		// The chart must start with a state based on the first rule of the grammar
		CParseState firstState = {
			0,	// First grammar rule
			0,	// Red dot before the first element of the rule
			0	// The rule is applied from the very first token
		};
		mMemoChart[0].push_back(firstState);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CParser::validateChart() const
	{
		// Look for a state that repressents the first rule of the grammar, finished after seing the last token
		const TChartEntry& lastEntry = mMemoChart.back();
		TChartEntry::const_iterator end = lastEntry.end();
		const unsigned firstRuleSize = mRules[0].to.size();
		for(TChartEntry::const_iterator i = lastEntry.begin(); i != end; ++i)
		{
			const CParseState& state = *i;
			if((0 == state.grammarRule) && (0 == state.from) && (firstRuleSize == state.redDot))
				return true; // Found
		}
		return false; // Not found, not valid
	}

	//------------------------------------------------------------------------------------------------------------------
	CParserNode * CParser::generateParseTree(const vector<CToken>& _tokens) const
	{
		// Build parsing chart and tree.
		initMemoChart(_tokens.size());	// Initialize the chart
		CParserNode * parseTree = parseTokenList(_tokens);	// Fill the chart and build the tree at the same time
		// ---- Validation ----
		if(!validateChart())
		{
			delete parseTree;
			parseTree = 0;
		}
		// ---- House keeping ----
		mMemoChart.clear();
		return parseTree;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::addState(unsigned _i, const CParseState& _state) const
	{
		TChartEntry& entry = mMemoChart[_i];
		for(TChartEntry::iterator i = entry.begin(); i != entry.end(); ++i)
		{
			if(*i == _state)
				return;
		}
		entry.push_back(_state);
	}

	//------------------------------------------------------------------------------------------------------------------
	CParser::CParser(const CGrammarRule * grammar, unsigned nRules)
		:mRules(grammar)
		,mNRules(nRules)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CParser::stripTokens(rtl::vector<CToken>& _list, unsigned _id) const
	{
		for(rtl::vector<CToken>::iterator i = _list.begin(); i != _list.end(); ++i)
		{
			if(i->type == _id)
			{
				// delete[] i->text;
				_list.erase(i);
			}
		}
	}

}	// namespace rev
