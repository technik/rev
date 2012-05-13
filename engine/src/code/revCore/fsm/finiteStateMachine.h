////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 13th, 2012
////////////////////////////////////////////////////////////////////////////////
// Finite state machines

#ifndef _REV_CORE_FSM_FINITESTATEMACHINE_H_
#define _REV_CORE_FSM_FINITESTATEMACHINE_H_

#include <utility.h>
#include <rtl/map.h>

namespace rev
{
	template<class _inputT, class _stateT>
	class CFiniteStateMachine
	{
	public:
		class CFiniteStateMachine(rtl::pair<rtl::pair<_stateT,_inputT>, _stateT>* _transitions, unsigned _nTransitions, _inputT _empty, _stateT _start);
		bool accepts(const _inputT * _sequence);

	private:
		typedef rtl::map<rtl::pair<_stateT,_inputT>, _stateT>	TTransitionSet;

		_inputT			mEmptySymbol;
		_stateT			mStartState;
		TTransitionSet	mTransitions;
	};

	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class _inputT, class _stateT>
	CFiniteStateMachine<_inputT, _stateT>::CFiniteStateMachine(
		rtl::pair<rtl::pair<_stateT,_inputT>, _stateT>* _transitions, // Set of transitions that conform this fsm
		unsigned _nTransitions,	// how many transitions are there in the set
		_inputT _empty, // Which symbol should be used to denote an empty transition
		_stateT _start) // Entry point
		:mEmptySymbol(_empty)
		,mStartState(_start)
	{
		for(unsigned i = 0; i < _nTransitions; ++i)
		{
			mTransitions[_transitions[i].first] = _transitions[i].second;
		}
	}

}	// namespace rev

#endif // _REV_CORE_FSM_FINITESTATEMACHINE_H_