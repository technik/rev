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

	//---------------------------------------------------------------------------------------------------------------------
	//CSyntagma script = { eScript, false };
	CSyntagma funcDef = { eFuncDef, false };
	CSyntagma statementList = { eStmtLst, false };
	CSyntagma statement = { eStatement, false };
	CSyntagma assignement = { eAssignement, false };
	CSyntagma funcCall = { eFuncCall, false };
	CSyntagma expression = { eExpression, false };
	CSyntagma vectorSynt = { eVector, false };
	CSyntagma expressionList = { eExpressionLst, false };
	CSyntagma literalSynt = { eLiteral, false };
	CSyntagma boolSynt = { eBool, false };

	CSyntagma floatSynt = { eFloat, true };
	CSyntagma integer = { eInteger, true };
	CSyntagma stringSynt = { eString, true };
	CSyntagma trueSynt = { eTrue, true };
	CSyntagma falseSynt = {eFalse, true };
	CSyntagma openBraces = {eOpenBraces, true };
	CSyntagma closeBraces = { eCloseBraces, true };
	CSyntagma comma = { eComma, true };
	CSyntagma semicolon = {eSemicolon, true };

	CGrammarRule revScriptSyntaxRules[17];

	//---------------------------------------------------------------------------------------------------------------------
	void CRevScriptParser::init()
	{
		// script : stmtLst
		revScriptSyntaxRules[0].from = eScript;
		revScriptSyntaxRules[0].to.push_back(statementList);
		// stmtLst : stmt stmtLst
		revScriptSyntaxRules[1].from = eStmtLst;
		revScriptSyntaxRules[1].to.push_back(statement);
		revScriptSyntaxRules[1].to.push_back(statementList);
		// stmtLst :
		revScriptSyntaxRules[2].from = eStmtLst;
		// stmt : expression ;
		revScriptSyntaxRules[3].from = eStatement;
		revScriptSyntaxRules[3].to.push_back(expression);
		revScriptSyntaxRules[3].to.push_back(semicolon);
		// expression : assignement
		revScriptSyntaxRules[4].from = eExpression;
		revScriptSyntaxRules[4].to.push_back(assignement);
		// expression : literal
		revScriptSyntaxRules[5].from = eExpression;
		revScriptSyntaxRules[5].to.push_back(literalSynt);
		// literal : vector
		revScriptSyntaxRules[6].from = eLiteral;
		revScriptSyntaxRules[6].to.push_back(vectorSynt);
		// literal : integer
		revScriptSyntaxRules[7].from = eLiteral;
		revScriptSyntaxRules[7].to.push_back(integer);
		// literal : float
		revScriptSyntaxRules[8].from = eLiteral;
		revScriptSyntaxRules[8].to.push_back(floatSynt);
		// literal : string
		revScriptSyntaxRules[9].from = eLiteral;
		revScriptSyntaxRules[9].to.push_back(stringSynt);
		// literal : bool
		revScriptSyntaxRules[10].from = eLiteral;
		revScriptSyntaxRules[10].to.push_back(boolSynt);
		// vector : [ exprLst ]
		revScriptSyntaxRules[11].from = eVector;
		revScriptSyntaxRules[11].to.push_back(openBraces);
		revScriptSyntaxRules[11].to.push_back(expressionList);
		revScriptSyntaxRules[11].to.push_back(closeBraces);
		// exprLst : expr , exprLst
		revScriptSyntaxRules[12].from = eExpressionLst;
		revScriptSyntaxRules[12].to.push_back(expression);
		revScriptSyntaxRules[12].to.push_back(comma);
		revScriptSyntaxRules[12].to.push_back(expressionList);
		// exprLst : expr
		revScriptSyntaxRules[13].from = eExpressionLst;
		revScriptSyntaxRules[13].to.push_back(expression);
		// exprLst : 
		revScriptSyntaxRules[12].from = eExpressionLst;
		// bool : true
		revScriptSyntaxRules[15].from = eBool;
		revScriptSyntaxRules[15].to.push_back(trueSynt);
		// bool : false
		revScriptSyntaxRules[16].from = eBool;
		revScriptSyntaxRules[16].to.push_back(falseSynt);

		sParser = new CParser(revScriptSyntaxRules, 17);
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
