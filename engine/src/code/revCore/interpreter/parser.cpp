////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// Generic Syntactical analyzer

#include "parser.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>

namespace rev
{
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

	//------------------------------------------------------------------------------------------------------------------
	bool addParseState(TChartEntry& _entry, const CParseState& _state)
	{
		for(unsigned i = 0; i < _entry.size(); ++i)
		{
			if(_entry[i] == _state)
				return false;
		}
		_entry.push_back(_state);
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool closure(const CParseState& _state, TChartEntry& _chartEntry, const CGrammarRule * _rules, unsigned _nRules, unsigned tokenTh)
	{
		bool changes = false;
		unsigned ruleKey = _rules[_state.grammarRule].to[_state.redDot].value;
		for(unsigned r = 0; r < _nRules; ++r)
		{
			if(ruleKey == _rules[r].from) // Coincidence!
			{
				CParseState newState = {r, 0, tokenTh};
				changes |= addParseState(_chartEntry, newState);
			}
		}
		return changes;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool shift(TChart& _chart, unsigned i, const CParseState& _state)
	{
		CParseState newState = {_state.grammarRule, _state.redDot+1, _state.from};
		return addParseState(_chart[i+1], newState);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool reduce(TChart& _chart, unsigned i, const CParseState& _state, const CGrammarRule* _rules)
	{
		bool changed = false;
		const TChartEntry& fromEntry = _chart[_state.from];
		for(unsigned s = 0; s < fromEntry.size(); ++s)
		{
			CParseState oldState = fromEntry[s];
			CSyntagma	next = _rules[oldState.grammarRule].to[oldState.redDot];
			if(!next.isTerminal && (next.value == _rules[_state.grammarRule].from))
			{
				CParseState newState = { oldState.grammarRule, oldState.redDot+1, oldState.from };
				changed |= addParseState(_chart[i], newState);
			}
		}
		return changed;
	}

	//------------------------------------------------------------------------------------------------------------------
	CParser::CParser(const CGrammarRule* _rules, unsigned _nRules)
		:mRules(_rules)
		,mNRules(_nRules)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CParserNode * CParser::generateParseTree(const rtl::vector<CToken>& _tokens)
	{
		// Create a memoization chart of parsing states
		TChart	memoChart;
		memoChart.resize(_tokens.size() + 1);
		// Add the starting rule to it
		CParseState startState = { 0, 0, 0 }; // Read 0 tokens in start rule, starting from position 0
		memoChart[0].push_back(startState);
		for(unsigned t = 0; t < _tokens.size()+1; ++t)
		{
			TChartEntry& chartEntry = memoChart[t];
			bool changed = true;
			while(changed)
			{
				changed = false;
				for(unsigned s = 0; s < chartEntry.size(); ++s)
				{
					CParseState state = chartEntry[s];
					// Do clossure, shifting and reduction
					const CGrammarRule& rule = mRules[state.grammarRule];
					unsigned ruleSize = rule.to.size();
					if(state.redDot < ruleSize) // We're not done with this rule, can't reduce
					{
						if(rule.to[state.redDot].isTerminal) // Shift
						{
							changed |= shift(memoChart, t, state);
						}
						else // Closure
						{
							changed |= closure(state, chartEntry, mRules, mNRules, t);
						}
					}
					else // Reduce
					{
						changed |= reduce(memoChart, t, state, mRules);
					}
				}
			}
		}

		//----------------------
		// Log the states chart
		//----------------------
		for(unsigned i = 0; i < memoChart.size(); ++i)
		{
			TChartEntry& entry = memoChart[i];
			revLog(i);
			revLogN(" ---------------------------------");
			for(unsigned j = 0; j < entry.size(); ++j)
			{
				CParseState& state = entry[j];
				revLog("Rule: ");
				revLog(state.grammarRule);
				revLog(", from ");
				revLog(state.from);
				revLog(". dot at ");
				revLogN(state.redDot);
			}
			codeTools::SLog::get()->flush();
		}
		return 0;
	}

}	// namespace rev
