////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 13th, 2012
////////////////////////////////////////////////////////////////////////////////
// Finite state machines for matching regular expressions

#include "regExpFsm.h"
#include <revCore/codeTools/assert/assert.h>
#include <revCore/string.h>

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	CRegExpFsm::CRegExpFsm(const char * _expression)
		:mRegExp(_expression)
	{
		initBaseFsm();
		unsigned curState = 0;
		unsigned lastState = 0;
		mFinalState = 0;
		unsigned i = 0;
		while('\0' != _expression[i])
		{
			i += parse(&_expression[i], curState, lastState, mFinalState);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CRegExpFsm::initBaseFsm()
	{
		mAcceptingStates = & mFinalState;
		mNAcceptingStates = 1;
		mEmptySymbol = '\0';
		mStartState = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CRegExpFsm::parse(const char * _expression, unsigned& _from, unsigned& _old, unsigned& _highest)
	{
		char c = _expression[0];
		if('?' == c) // Optional element
		{
			addBranch(mEmptySymbol, _old, _from);
			return 1; // Parsed one character
		}
		else if('+' == c) // Once or more
		{
			// Repeat all branches that take from '_old' to '_from', but now going from '_from' to '_from'
			for(char branch = 0x01; branch != 0x00; ++branch)
			{
				rtl::pair<unsigned,char> key = rtl::pair<unsigned,char>(_old,branch);
				TTransitionSet::iterator i = mTransitions.find(key);
				if(i != mTransitions.end())
				{
					addBranch(branch, _from, _from);
				}
			}
			return 1; // Parsed one character
		}
		else if('*' == c) // Once or more
		{
			// Repeat all branches that take from '_old' to '_from', but now going from '_from' to '_from'
			for(char branch = 0x01; branch != 0x00; ++branch)
			{
				rtl::pair<unsigned,char> key = rtl::pair<unsigned,char>(_old,branch);
				TTransitionSet::iterator i = mTransitions.find(key);
				if(i != mTransitions.end())
				{
					addBranch(branch, _from, _from);
				}
			}
			// Add an empty transition
			addBranch(mEmptySymbol, _old, _from);
			return 1; // Parsed one character
		}
		else if('[' == c) // Range element or 'anything but' element
		{
			c = _expression[1];
			if('^' == c) // 'Anything but' element
			{
				++_highest;
				char skip[256];
				unsigned i = 0;
				while((c=_expression[2+i]) != ']')
				{
					skip[i++] = c;
				}
				skip[i] = '\0';
				for(c = 0x01; c != 0x00; ++c)
				{
					if(!isCharacterInString(c, skip))
					{
						addBranch(c, _from, _highest);
					}
				}
				_old = _from;
				_from = _highest;
				return 3+i;
			}
			else // range element
			{
				revAssert(_expression[2] == '-');
				revAssert(_expression[4] == ']');
				++_highest;
				char c0 = _expression[1];
				char c1 = _expression[3];
				for(char c = c0; c != c1+1; ++c)
				{
					addBranch(c, _from, _highest);
				}
				_old = _from;
				_from = _highest;
				return 5;
			}
		}
		else if('|' == c) // Disjunction
		{
			unsigned lastTarget = _highest - 1;
			unsigned old = _old;
			unsigned none = 0;
			return 1 + parse(&_expression[1], old, none, lastTarget);
		}
		else if('\\' == c) // escape
		{
			addBranch(_expression[1], _from, ++_highest);
			_old = _from;
			_from = _highest;
			return 2;
		}
		else // Simple character
		{
			addBranch(c, _from, ++_highest);
			_old = _from;
			_from = _highest;
			return 1; // Parsed one character
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CRegExpFsm::addBranch(char _label, unsigned _from, unsigned _to)
	{
		mTransitions.insert(rtl::pair<rtl::pair<unsigned,char>,unsigned>(rtl::pair<unsigned,char>(_from,_label),_to));
	}

}	// namespace rev
