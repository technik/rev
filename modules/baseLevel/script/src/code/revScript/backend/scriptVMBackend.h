//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 18th, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine's backend is the interface between the bison generated parser
// and the virtual machine itself.
#ifndef _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_
#define _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_

namespace rev { namespace script
{
	class ScriptVM;

	class ScriptVMBackend
	{
	public:
		ScriptVMBackend				();

		// Interface with the virtual machine
		void		buildExecTree	(const char* _code);

		// Interface with flex/bison
		void		matchStatement	();
		void		matchExpression	();
		void		matchSum		(int, int);
		void		matchInteger	(int);
		unsigned	retrieveCode	(char* _dst, unsigned _maxSize);

	private:
		const char*	mCodeBuffer;
		unsigned	mCodeCursor;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_BACKEND_SCRIPTVMBACKEND_H_