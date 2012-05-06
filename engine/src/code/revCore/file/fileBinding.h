////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on May 6th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File binding

#ifndef _REV_CORE_FILE_FILEBINDING_H_
#define _REV_CORE_FILE_FILEBINDING_H_

#include <revCore/delegate/delegate.h>

namespace rev
{
	class IFileBinding
	{
	public:
		IFileBinding(const char * _filename);
		virtual ~IFileBinding();
		virtual void reload(const char * _filename) = 0;

	private:
		typedef CObjectDelegate<IFileBinding,const char*>	TDelegate;

		TDelegate*	mDelegate;
	};

	template<class _derivedT>
	class CRecreationFileBinding : public IFileBinding
	{
	public:
		CRecreationFileBinding(const char * _filename)
			:IFileBinding(_filename)
		{}
		virtual ~CRecreationFileBinding(){}
	private:
		void reload(const char* _filename)
		{
			this->~CRecreationFileBinding();
			new(static_cast<_derivedT*>(this))_derivedT(_filename);
		}
	};
}	// namespace rev

#endif // _REV_CORE_FILE_FILEBINDING_H_