//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 21st, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine's backend is the interface between the bison generated parser
// and the virtual machine itself.

#include "scriptVMBackend.h"

extern rev::script::ScriptVMBackend* gActiveBackend;

int yyparse();

namespace rev { namespace script {

	//----------------------------------------------------------------------------------
	ScriptVMBackend::ScriptVMBackend()
		:mCodeBuffer(nullptr)
		,mCodeCursor(0)
	{ // Intentionally blank
	}

	//----------------------------------------------------------------------------------
	void ScriptVMBackend::buildExecTree(const char* _code)
	{
		mCodeCursor = 0;
		mCodeBuffer = _code;
		gActiveBackend = this;
		yyparse();
	}

	//----------------------------------------------------------------------------------
	void ScriptVMBackend::matchStatement()
	{
		// TODO
	}

	//----------------------------------------------------------------------------------
	void ScriptVMBackend::matchExpression()
	{
		// TODO
	}

	//----------------------------------------------------------------------------------
	void ScriptVMBackend::matchSum(int, int)
	{
		// TODO
	}

	//----------------------------------------------------------------------------------
	void ScriptVMBackend::matchInteger(int)
	{
		// TODO
	}

	//----------------------------------------------------------------------------------
	unsigned ScriptVMBackend::retrieveCode(char* _dst, unsigned _maxSize)
	{
		for(unsigned i = 0; i < _maxSize; ++i)
		{
			if(mCodeBuffer[mCodeCursor+i] == '\0') {
				return i;
			} else {
				_dst[i] = mCodeBuffer[mCodeCursor+i];
			}
		}
		return _maxSize;
	}

}	// namespace script
}	// namespace rev