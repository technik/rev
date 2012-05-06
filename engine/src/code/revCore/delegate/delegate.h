////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 10th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Delegates
// Delegates can be used as a method to handle events. They provide an abstraction layer between event raising and
// event handling.

#ifndef _REV_CORE_DELEGATE_DELEGATE_H_
#define _REV_CORE_DELEGATE_DELEGATE_H_

namespace rev
{
	class CVariant;

	template<class _argT>
	class IDelegate
	{
	public:
		virtual ~IDelegate() {};
		virtual void invoke	(_argT& _var) = 0;
	};

	template<class _argT>
	class IDelegate<_argT*>
	{
	public:
		virtual ~IDelegate() {};
		virtual void invoke	(_argT* _var) = 0;
	};

	template<class T, class _argT>
	class CObjectDelegate : public IDelegate<_argT>
	{
		typedef void (T::*MethodT)(_argT&);
	public:
		// Constrction
		CObjectDelegate (T* _callee, MethodT _method)
			:mObject(_callee)
			,mMethod(_method) {}

		// Invoke the delegate
		void invoke (_argT& _var)
		{
			(mObject->*mMethod)(_var);
		}

	private:
		T*		mObject;
		MethodT	mMethod;
	};

	template<class T, class _argT>
	class CObjectDelegate<T, _argT*> : public IDelegate<_argT*>
	{
		typedef void (T::*MethodT)(_argT*);
	public:
		// Constrction
		CObjectDelegate (T* _callee, MethodT _method)
			:mObject(_callee)
			,mMethod(_method) {}

		// Invoke the delegate
		void invoke (_argT* _var)
		{
			(mObject->*mMethod)(_var);
		}

	private:
		T*		mObject;
		MethodT	mMethod;
	};
}	// namespace rev

#endif // _REV_CORE_DELEGATE_DELEGATE_H_