//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 18th, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine's backend is the interface between the bison generated parser
// and the virtual machine itself.
#ifndef _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_
#define _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_

#include <string>
#include <vector>

namespace rev { namespace script
{
	class ExecutionTree;
	class Expression;
	class ScriptVM;

	class ScriptVMBackend
	{
	public:
		ScriptVMBackend					();
		~ScriptVMBackend				();

		// Interface with the virtual machine
		ExecutionTree*	buildExecTree	(const char* _code);

		// Interface with flex/bison
		void			matchAssign		(std::string* _identifier);
		void			matchInteger	(int);
		void			matchIdentifier	(std::string* _identifier);
		void			matchReal		(float);
		unsigned		retrieveCode	(char* _dst, unsigned _maxSize);

	private:
		typedef std::vector<Expression*>	ExpressionStack;
	private:
		const char*		mCodeBuffer;
		unsigned		mCodeCursor;

		ExecutionTree*	mCurrentExecTree;
		ExpressionStack	mStack;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_