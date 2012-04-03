////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script tokens

#ifndef _REV_CORE_SCRIPT_SCRIPTTOKEN_H_
#define _REV_CORE_SCRIPT_SCRIPTTOKEN_H_

namespace rev { namespace script
{
	class CScriptToken
	{
	public:
		enum ETokenType
		{
			eIdentifier,
			eIntegerLiteral,
			eRealLiteral,
			eStringLiteral,
			eOperator,
			eComma,			// ","
			eSemicolon,		// ";"
			eOpenCBraces,	// Curly braces "{"
			eCloseCBraces,	// Curly braces "}"
			eOpenBraces,	// Braces "["
			eCloseBraces,	// Braces "]"
			eOpenPar,		// "("
			eClosePar,		// ")"
			eIf,
			eElse
		};

	public:
		~CScriptToken() {}

	public:
		ETokenType	mType;
		char *		mContent;
	};

}	// namespace script
}	// namespace rev

#endif //_REV_CORE_SCRIPT_SCRIPTTOKEN_H_