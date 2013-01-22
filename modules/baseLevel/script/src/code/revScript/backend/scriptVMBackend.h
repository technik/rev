//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// On January 18th, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine's backend is the interface between the bison generated parser
// and the virtual machine itself.
#ifndef _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_
#define _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_

#include <string>

namespace rev { namespace script
{
	class ExecutionTree;
	class ScriptVM;

	class ScriptVMBackend
	{
	public:
		ScriptVMBackend					();
		~ScriptVMBackend				();

		// Interface with the virtual machine
		ExecutionTree*	buildExecTree	(const char* _code);

		// Interface with flex/bison
		void			matchStatement	();
		void			matchExpression	();
		void			matchAssign		(std::string* _identifier, int _literal);
		void			matchSum		(int, int);
		void			matchInteger	(int);
		unsigned		retrieveCode	(char* _dst, unsigned _maxSize);

	private:
		const char*		mCodeBuffer;
		unsigned		mCodeCursor;

		ExecutionTree* mCurrentExecTree;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_