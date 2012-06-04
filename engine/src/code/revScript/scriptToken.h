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
	enum ERevScriptToken
	{
		eSpace,
		eLineComment,
		eBlockComment,
		eSemicolon,			// ;
		eFunctionKW,		// function
		eOpenParenthesis,	// (
		eCloseParenthesis,	// )
		eComma,				// ,
		eWhileKW,			// while
		eForKW,				// for
		eOpenCBraces,		// {
		eCloseCBraces,		// }
		eAssignOperator,	// =
		eOpenBraces,		// [
		eCloseBraces,		// ]
		eTrueKW,			// true
		eFalseKW,			// false
		eNullKW,			// null
		eIfKW,				// if
		eElseKW,			// else
		eFloat,
		eInteger,
		eString,
		eIdentifier,
	};

}	// namespace script
}	// namespace rev

#endif //_REV_CORE_SCRIPT_SCRIPTTOKEN_H_