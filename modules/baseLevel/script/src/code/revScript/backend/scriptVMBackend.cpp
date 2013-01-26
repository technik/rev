//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 21st, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine's backend is the interface between the bison generated parser
// and the virtual machine itself.

#include "scriptVMBackend.h"
#include "../virtualMachine/executionTree/executionTree.h"

extern rev::script::ScriptVMBackend* gActiveBackend;

int yyparse();

namespace rev { namespace script {

	//----------------------------------------------------------------------------------
	ScriptVMBackend::ScriptVMBackend()
		:mCodeBuffer(nullptr)
		,mCodeCursor(0)
		,mCurrentExecTree(nullptr)
	{ // Intentionally blank
	}

	//----------------------------------------------------------------------------------
	ScriptVMBackend::~ScriptVMBackend()
	{
		if(nullptr != mCurrentExecTree) {
			delete mCurrentExecTree;
		}
		// Clear the stack
		for(auto i = mStack.begin(); i != mStack.end(); ++i)
			delete (*i);
	}

	//----------------------------------------------------------------------------------
	ExecutionTree* ScriptVMBackend::buildExecTree(const char* _code)
	{
		mCodeCursor = 0;
		mCodeBuffer = _code;
		gActiveBackend = this;

		// Create the execution tree we plan to return
		mCurrentExecTree = new ExecutionTree();

		yyparse();
		// TODO: Return an actual execution tree
		ExecutionTree* codeTree = mCurrentExecTree;
		mCurrentExecTree = nullptr;
		return codeTree;
	}

	//----------------------------------------------------------------------------------
	void ScriptVMBackend::matchAssign(std::string* _identifier)
	{
		Expression* rValue = mStack.back();
		mStack.pop_back();
		mCurrentExecTree->addAssign(new AssignStatement(_identifier, rValue));
	}

	//----------------------------------------------------------------------------------
	void ScriptVMBackend::matchInteger(int _integer)
	{
		mStack.push_back(new IntegerExpression(_integer));
	}

	//----------------------------------------------------------------------------------
	void ScriptVMBackend::matchReal(float _real)
	{
		mStack.push_back(new IntegerExpression(_real));
	}

	//----------------------------------------------------------------------------------
	unsigned ScriptVMBackend::retrieveCode(char* _dst, unsigned _maxSize)
	{
		if(mCodeBuffer[mCodeCursor] == '\0')
			return 0;
		for(unsigned i = 0; i < _maxSize; ++i)
		{
			if(mCodeBuffer[mCodeCursor+i] == '\0') {
				mCodeCursor += i;
				return i;
			} else {
				_dst[i] = mCodeBuffer[mCodeCursor+i];
			}
		}
		mCodeCursor += _maxSize;
		return _maxSize;
	}

}	// namespace script
}	// namespace rev