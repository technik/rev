////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lexical analizer of scripts

#include "syntaxParser.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/script/scriptToken.h>
#include <revCore/string.h>

using namespace rev::codeTools;

namespace rev { namespace script
{
	//---------------------------------------------------------------------------------------------------------------------
	void SSyntaxParser::parseTokenListIntoExecutionTree(CStatementList& _dst, const rtl::vector<CScriptToken>& _tokens)
	{
		unsigned cursor = 0;
		while(cursor < _tokens.size())
		{
			IStatement * statement = processStatement(_tokens, cursor);
			if(0 == statement)
				SLog::log("Syntax error, couldn't process statement");
			else
				_dst.addStatement(statement);
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	IStatement * SSyntaxParser::processStatement(const rtl::vector<CScriptToken>& _tokens, unsigned& _cursor)
	{
		CScriptToken::ETokenType currentTokenType = _tokens[_cursor].mType;
		if(currentTokenType == CScriptToken::eOpenCBraces)
		{
			++_cursor;	// Skip {
			IStatement * statement = codeBlockStatement(_tokens, _cursor);
			// Error checking
			if(_tokens[_cursor].mType != CScriptToken::eCloseCBraces)
			{
				SLog::log("Syntax error: Expected \"}\" at position:");
				SLog::log(_cursor);
				return 0;
			}
			++_cursor;	// Skip }
			return statement;
		}
		else if(currentTokenType == CScriptToken::eIf)
		{
			if(_tokens[_cursor+1].mType != CScriptToken::eOpenPar)
			{
				SLog::log("Syntax error: Expected \"(\" after \"if\" at position:");
				SLog::log(_cursor);
				return 0;
			}
			_cursor+=2; // Skip "If ("
			IExpression * expression = processExpression(_tokens, _cursor);
			if(_tokens[_cursor].mType != CScriptToken::eClosePar)
			{
				SLog::log("Syntax error: Expected \")\" at position:");
				SLog::log(_cursor);
				return 0;
			}
			++_cursor; // Skip ")"
			IStatement * possitiveStatement = processStatement(_tokens, _cursor);
			if(_tokens[_cursor].mType == CScriptToken::eElse)
			{
				CIfElseStatement * ifElse= new CIfElseStatement();
				ifElse->mCondition = expression;
				ifElse->mPositiveStatement = possitiveStatement;
				++_cursor;	// Skip else
				ifElse->mNegativeStatement = processStatement(_tokens, _cursor);
				return ifElse;
			}
			else
			{
				CIfStatement * ifStatement = new CIfStatement();
				ifStatement->mCondition = expression;
				ifStatement->mStatement = possitiveStatement;
				return ifStatement;
			}
		}
		else // Single expression statement
		{
			CSingleExpressionStatement * expressionStatement = new CSingleExpressionStatement();
			expressionStatement->mExpression = processExpression(_tokens, _cursor);
			if(_tokens[_cursor].mType != CScriptToken::eSemicolon)
			{
				SLog::log("Syntax error: Expected \";\" at position:");
				SLog::log(_cursor);
				return 0;
			}
			++_cursor; // Skip semicolon
			return expressionStatement;
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	IExpression * SSyntaxParser::processExpression(const rtl::vector<CScriptToken>& _tokens, unsigned& _cursor)
	{
		if(_tokens[_cursor].mType == CScriptToken::eOpenBraces) // List
		{
			CListExpression * list = new CListExpression();
			++_cursor; // Skip [
			while(_tokens[_cursor].mType != CScriptToken::eCloseBraces)
			{
				if (_tokens[_cursor].mType == CScriptToken::eComma)
					++_cursor;
				else
				{
					list->addElement(processExpression(_tokens, _cursor));
				}
			}
			++_cursor; // Skip ]
			return list;
		}
		else if(_tokens[_cursor].mType == CScriptToken::eTrue)
		{
			CBooleanExpression * boolean = new CBooleanExpression();
			boolean->mValue = true;
			++_cursor;
			return boolean;
		}
		else if(_tokens[_cursor].mType == CScriptToken::eFalse)
		{
			CBooleanExpression * boolean = new CBooleanExpression();
			boolean->mValue = false;
			++_cursor;
			return boolean;
		}
		else if(_tokens[_cursor].mType == CScriptToken::eStringLiteral)
		{
			return constantStringExpression(_tokens[_cursor++]);
		}
		else
		{
			// Literal constant expression
			return constantIntegerExpression(_tokens[_cursor++]);
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	CCodeBlockStatement * SSyntaxParser::codeBlockStatement(const rtl::vector<CScriptToken>& _tokens, unsigned& _cursor)
	{
		CCodeBlockStatement * block = new CCodeBlockStatement();
		while(_tokens[_cursor].mType != CScriptToken::eCloseCBraces)
			block->mStatementList.addStatement(processStatement(_tokens, _cursor));
		return block;
	}

	//---------------------------------------------------------------------------------------------------------------------
	CConstantExpression * SSyntaxParser::constantIntegerExpression(const CScriptToken _token)
	{
		// Only integer constants supported
		CConstantExpression * expression = new CConstantExpression();
		expression->mConstant = CVariant(integerFromString(_token.mContent));
		return expression;
	}

	//---------------------------------------------------------------------------------------------------------------------
	CConstantExpression * SSyntaxParser::constantStringExpression(const CScriptToken _token)
	{
		// Only integer constants supported
		CConstantExpression * expression = new CConstantExpression();
		expression->mConstant = CVariant(_token.mContent);
		return expression;
	}
}	// namespace script
}	// namespace rev
