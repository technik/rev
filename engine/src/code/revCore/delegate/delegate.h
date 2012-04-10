////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 10th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Delegates

#ifndef _REV_CORE_DELEGATE_DELEGATE_H_
#define _REV_CORE_DELEGATE_DELEGATE_H_

namespace rev
{
	class CVariant;

	class IDelegate
	{
	public:
		virtual ~IDelegate() {}
		virtual void invoke	(CVariant& _var) = 0;
	};

	template<class T>
	class CObjectDelegate : public IDelegate
	{
		typedef void (T::*MethodT)(CVariant&);
	public:
		// Constrction
		CObjectDelegate (T* _callee, MethodT _method)
			:mObject(_callee)
			,mMethod(_method) {}

		// Invoke the delegate
		void invoke (CVariant& _var)
		{
			mObject->*mMethod(_var);
		}

	private:
		T*		mObject;
		MethodT	mMethod;
	};
}	// namespace rev

#endif // _REV_CORE_DELEGATE_DELEGATE_H_