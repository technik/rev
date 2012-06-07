////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script syntax definition

#ifndef _REV_SCRIPT_SYNTAX_H_
#define _REV_SCRIPT_SYNTAX_H_

#include <vector.h>

#include <revCore/variant/variant.h>

namespace rev { namespace script
{
	enum ERevScriptExpressions
	{
		eScript,
		eScriptStream,
		eStmt,
		eFnDef,
		eExpression,
		eConditional,
		eWhileLoop,
		eForLoop,
//		eAssignement,
		eArgLst,
		eArgument,
		eCodeBlock,
		eStmtLst,
		eFnCall,
		eElementAccess,
		eLiteral,
		eInlineVector,
		eBool,
		eExprLst,
		eElseBlock,
	};
}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_SYNTAX_H_