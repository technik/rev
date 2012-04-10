////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script syntax parser

#ifndef _REV_SCRIPT_ANALISYS_PARSER_SINTAXPARSER_H_
#define _REV_SCRIPT_ANALISYS_PARSER_SINTAXPARSER_H_

#include <revScript/syntax.h>

namespace rev { namespace script
{
	class CScriptToken;

	class SSyntaxParser
	{
	public:
		static void parseTokenListIntoExecutionTree(CStatementList& _dst, const rtl::vector<CScriptToken>& _tokens);

	private:
		static IStatement*	processStatement	(const rtl::vector<CScriptToken>& _tokens, unsigned& _cursor);
		static IExpression*	processExpression	(const rtl::vector<CScriptToken>& _tokens, unsigned& _cursor);

		static CCodeBlockStatement* codeBlockStatement			(const rtl::vector<CScriptToken>& _tokens, unsigned& _cursor);
		static CConstantExpression*	constantIntegerExpression	(const CScriptToken _token);
		static CConstantExpression*	constantRealExpression		(const CScriptToken _token);
		static CConstantExpression* constantStringExpression	(const CScriptToken _string);
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_ANALISYS_PARSER_SINTAXPARSER_H_
