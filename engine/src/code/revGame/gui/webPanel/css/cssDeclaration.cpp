////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 27th, 2012
////////////////////////////////////////////////////////////////////////////////
// css declaration

#include "cssDeclaration.h"

#include "cssTokens.h"
#include "lexer/cssLexer.h"
#include "parser/cssParser.h"

#include <revCore/interpreter/parser.h>
#include <revCore/interpreter/token.h>
#include <vector.h>

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CCssDeclaration::CCssDeclaration(const char * _code)
		:color(video::CColor::WHITE)
		,bgColor(0.f,0.f,0.f,0.f)
	{
		rtl::vector<CToken>	tokenList;
		// Lex
		CCssDeclarationLexer::get()->tokenizeCode(tokenList, _code);
		// Parse
		CCssDeclParser::get()->stripTokens(tokenList, eDecSpace);
		CParserNode * cssTree = CCssDeclParser::get()->generateParseTree(tokenList);
		// Process tree
		processTree(cssTree);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CCssDeclaration::processTree(CParserNode * _tree)
	{
		CParserNonLeaf * rootDecl = static_cast<CParserNonLeaf*>(_tree);
		CParserNonLeaf * cssDecl = static_cast<CParserNonLeaf*>(rootDecl->mChildren[0]);
		CParserNonLeaf * typedDecl = static_cast<CParserNonLeaf*>(cssDecl->mChildren[0]);
		CParserNonLeaf * propNode = static_cast<CParserNonLeaf*>(typedDecl->mChildren[0]);
		//CParserNonLeaf * valNode = static_cast<CParserNonLeaf*>(typedDecl->mChildren[2]);
		CParserLeaf * prop = static_cast<CParserLeaf*>(propNode->mChildren[0]);
		//CParserLeaf * val = static_cast<CParserLeaf*>(valNode->mChildren[0]);
		if(prop->mToken.type == eColorProp)
		{
			color = video::CColor::RED;
		}
		typedDecl;
	}
}	// namespace game
}	// namespace game
