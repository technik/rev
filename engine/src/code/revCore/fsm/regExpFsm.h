////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 13th, 2012
////////////////////////////////////////////////////////////////////////////////
// Finite state machines for matching regular expressions
#ifndef _REV_CORE_FSM_REGEXPFSM_H_
#define _REV_CORE_FSM_REGEXPFSM_H_

#include "finiteStateMachine.h"

#include <utility.h>
#include <rtl/map.h>

namespace rev
{
	class CRegExpFsm // Regular expression finite state machine
		: public CFiniteStateMachine<char>
	{
	public:
		CRegExpFsm(const char * _regExp); // Build a finite state machine from a regular expression

	private:
		void initBaseFsm();
		unsigned parse(const char * _regExp, unsigned& _from, unsigned& _old, unsigned& _highest);
		void addBranch(char _id, unsigned _from, unsigned _to);

	private:
		unsigned mFinalState;
	};

}	// namespace rev

#endif // _REV_CORE_FSM_REGEXPFSM_H_