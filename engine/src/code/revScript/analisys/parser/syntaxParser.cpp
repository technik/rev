////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lexical analizer of scripts

#include "syntaxParser.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/string.h>
#include <revScript/scriptSyntax.h>
#include <revScript/scriptToken.h>

using namespace rev::codeTools;

namespace rev { namespace script
{
	//---------------------------------------------------------------------------------------------------------------------
	CParser * CRevScriptParser::sParser = 0;

	CGrammarRule revScriptSyntaxRules[13];

	//---------------------------------------------------------------------------------------------------------------------
	void CRevScriptParser::init()
	{
		// ----- Define syntagmas -----
		// Non terminals
		CSyntagma scriptStreamS = {eScriptStream, false };
		CSyntagma stmtS = { eStmt, false };
//		CSyntagma fnDefS = { eFnDef, false };
		CSyntagma expressionS = { eExpression, false };
//		CSyntagma conditionalS = { eConditional, false };
//		CSyntagma whileLoopS = { eWhileLoop, false };
//		CSyntagma forLoopS	= { eForLoop, false };
//		CSyntagma assignementS = { eAssignement, false };
//		CSyntagma argLstS = { eArgLst, false };
//		CSyntagma argument = { eArgument, false };
//		CSyntagma codeBlockS = { eCodeBlock, false };
//		CSyntagma stmtLstS = { eStmtLst, false };
//		CSyntagma fnCall = { eFnCall, false };
//		CSyntagma elementAccessS = { eElementAccess, false };
		CSyntagma literalS = { eLiteral, false };
		CSyntagma inlineVectorS = { eInlineVector, false };
//		CSyntagma boolS = { eBool, false };
		CSyntagma exprLstS = { eExprLst, false };
//		CSyntagma elseBlockS = { eElseBlock, false };
		// Terminals
		CSyntagma semicolonT = { eSemicolon, true };
//		CSyntagma functionKWT = { eFunctionKW, true };
//		CSyntagma openParenthesisT = { eOpenParenthesis, true };
//		CSyntagma closeParenthesisT = { eCloseParenthesis, true };
		CSyntagma commaT = { eComma, true };
//		CSyntagma whileKWT = { eWhileKW, true };
//		CSyntagma forKWT = { eForKW, true };
//		CSyntagma openCBracesT = { eOpenCBraces, true };
//		CSyntagma closeCBracesT = { eCloseCBraces, true };
		CSyntagma assignOperatorT = { eAssignOperator, true };
		CSyntagma openBracesT = { eOpenBraces, true };
		CSyntagma closeBracesT = { eCloseBraces, true };
//		CSyntagma trueKWT = { eTrueKW, true };
//		CSyntagma falseKWT = {eFalseKW, true };
//		CSyntagma nullKWT = { eNullKW, true };
//		CSyntagma ifKWT = { eIfKW, true };
//		CSyntagma elseKWT = { eElseKW, true };
		CSyntagma floatT = { eFloat, true };
//		CSyntagma integerT = { eInteger, true };
		CSyntagma stringT = { eString, true };
		CSyntagma identifierT = { eIdentifier, true };

		// ----- Define syntax rules -----
		// script : scriptStream
		revScriptSyntaxRules[0].from = eScript;
		revScriptSyntaxRules[0].to.push_back(scriptStreamS);
		// scriptStream : stmt scriptStream
		revScriptSyntaxRules[1].from = eScriptStream;
		revScriptSyntaxRules[1].to.push_back(stmtS);
		revScriptSyntaxRules[1].to.push_back(scriptStreamS);
		// scriptStream :
		revScriptSyntaxRules[2].from = eScriptStream;
		// statement : identifier = expression ;
		revScriptSyntaxRules[3].from = eStmt;
		revScriptSyntaxRules[3].to.push_back(identifierT);
		revScriptSyntaxRules[3].to.push_back(assignOperatorT);
		revScriptSyntaxRules[3].to.push_back(expressionS);
		revScriptSyntaxRules[3].to.push_back(semicolonT);
		// expression : identifier
		revScriptSyntaxRules[4].from = eExpression;
		revScriptSyntaxRules[4].to.push_back(identifierT);
		// expression : literal
		revScriptSyntaxRules[5].from = eExpression;
		revScriptSyntaxRules[5].to.push_back(literalS);
		// literal : inlineVector
		revScriptSyntaxRules[6].from = eLiteral;
		revScriptSyntaxRules[6].to.push_back(inlineVectorS);
		// literal : float
		revScriptSyntaxRules[7].from = eLiteral;
		revScriptSyntaxRules[7].to.push_back(floatT);
		// literal : string
		revScriptSyntaxRules[8].from = eLiteral;
		revScriptSyntaxRules[8].to.push_back(stringT);
		// inlineVector : [ exprLst ]
		revScriptSyntaxRules[9].from = eInlineVector;
		revScriptSyntaxRules[9].to.push_back(openBracesT);
		revScriptSyntaxRules[9].to.push_back(exprLstS);
		revScriptSyntaxRules[9].to.push_back(closeBracesT);
		// exprLst : expr, exprLst
		revScriptSyntaxRules[10].from = eExprLst;
		revScriptSyntaxRules[10].to.push_back(expressionS);
		revScriptSyntaxRules[10].to.push_back(commaT);
		revScriptSyntaxRules[10].to.push_back(exprLstS);
		// exprLst : expr
		revScriptSyntaxRules[11].from = eExprLst;
		revScriptSyntaxRules[11].to.push_back(expressionS);
		// exprLst :
		revScriptSyntaxRules[12].from = eExprLst;

		sParser = new CParser(revScriptSyntaxRules, 13);
	}

	//---------------------------------------------------------------------------------------------------------------------
	CParser * CRevScriptParser::get()
	{
		return sParser;
	}

	//---------------------------------------------------------------------------------------------------------------------
	void CRevScriptParser::end()
	{
		delete sParser;
		sParser = 0;
	}

}	// namespace script
}	// namespace rev
