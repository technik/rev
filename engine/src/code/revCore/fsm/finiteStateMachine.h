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
	template<class _inputT>
	class CFiniteStateMachine
	{
	public:
		CFiniteStateMachine(
			rtl::pair<rtl::pair<unsigned,_inputT>, unsigned>* _transitions,	// Transitions
			unsigned _nTransitions,											// Number of transitions
			unsigned * _accepting,											// Accepting states
			unsigned _nStates,												// Number of accepting states
			_inputT _empty);												// Symbol used for denoting an empty transition
	
		bool accepts(const _inputT * _sequence, unsigned _length) const;
		unsigned consumes(const _inputT * _sequence) const;

	protected:
		CFiniteStateMachine():mAcceptingStates(0) {}
		~CFiniteStateMachine() { if (0!=mAcceptingStates) delete mAcceptingStates; }
		bool isAccepting(unsigned _state) const;
		unsigned acceptsInputInState(_inputT _input, unsigned _state) const;

		typedef rtl::map<rtl::pair<unsigned,_inputT>, unsigned>	TTransitionSet;

		unsigned*		mAcceptingStates;
		unsigned		mNAcceptingStates;
		_inputT			mEmptySymbol;
		unsigned		mStartState;
		TTransitionSet	mTransitions;

	private:
		bool accepts(const _inputT * _sequence, unsigned _length, unsigned _start) const;
	};

	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class _inputT>
	CFiniteStateMachine<_inputT>::CFiniteStateMachine(
			rtl::pair<rtl::pair<unsigned,_inputT>, unsigned>* _transitions,	// Transitions
			unsigned _nTransitions,											// Number of transitions
			unsigned * _accepting,											// Accepting states
			unsigned _nStates,												// Number of accepting states
			_inputT _empty)
		:mNAcceptingStates(_nStates)
		,mEmptySymbol(_empty)
		,mStartState(_start)
	{
		// Store accepting states
		mAcceptingStates = new unsigned[_nStates];
		for(unsigned s = 0; s < _nStates; ++s)
			mAcceptingStates[s] = _nStates;
		// Store transitions
		for(unsigned i = 0; i < _nTransitions; ++i)
			mTransitions[_transitions[i].first] = _transitions[i].second;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _inputT>
	bool CFiniteStateMachine<_inputT>::accepts(const _inputT * _sequence, unsigned _length) const
	{
		return accepts(_sequence, _length, 0);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _inputT>
	bool CFiniteStateMachine<_inputT>::isAccepting(unsigned _state) const
	{
		for(unsigned s = 0; s < mNAcceptingStates; ++s)
			if(mAcceptingStates[s] == _state)
				return true;
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _inputT>
	bool CFiniteStateMachine<_inputT>::accepts(const _inputT * _sequence, unsigned _length, unsigned _start) const
	{
		// End of sequence validation
		if((0 == _length) && (isAccepting(_start)))
			return true;
		// Check wether current input can be handled
		bool handled = false;
		typename TTransitionSet::const_iterator i = mTransitions.find(rtl::pair<unsigned,char>(_start, _sequence[0]));
		if(i != mTransitions.end())
		{
			handled = accepts(&_sequence[1], _length-1, i->second);
		}
		if(!handled)
		{
			i = mTransitions.find(rtl::pair<unsigned,char>(_start, mEmptySymbol));
			if(i != mTransitions.end())
			{
				handled = accepts(_sequence, _length, i->second);
			}
		}
		return handled;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _inputT>
	unsigned CFiniteStateMachine<_inputT>::consumes(const _inputT * _sequence) const
	{
		unsigned consumed = 0;
		unsigned state = 0;
		while(0 != (state=acceptsInputInState(_sequence[consumed], state)))
		{
			++consumed;
			if(isAccepting(state))
				return consumed;
		}
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _inputT>
	unsigned CFiniteStateMachine<_inputT>::acceptsInputInState(_inputT _input, unsigned _state) const
	{
		typename TTransitionSet::const_iterator i = mTransitions.find(rtl::pair<unsigned,char>(_state, _input));
		if(i != mTransitions.end())
		{
			return i->second;
		}
		else
		{
			i = mTransitions.find(rtl::pair<unsigned,char>(_state, mEmptySymbol));
			if(i != mTransitions.end())
			{
				return acceptsInputInState(_input, i->second);
			}
			else
				return 0;
		}
	}

}	// namespace rev

#endif // _REV_CORE_FSM_FINITESTATEMACHINE_H_