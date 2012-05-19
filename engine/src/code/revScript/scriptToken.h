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
		eCommentToken,
		eSpace,
		eFloat,
		eInteger,
		eString,
		eTrue,
		eFalse,
		eOpenBraces,	// [
		eCloseBraces,	// ]
		eComma,			// ,
		eSemicolon,		// ;
	};

}	// namespace script
}	// namespace rev

#endif //_REV_CORE_SCRIPT_SCRIPTTOKEN_H_