////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 13th, 2012
////////////////////////////////////////////////////////////////////////////////
// Language tokens

#ifndef _REV_CORE_INTERPRETER_TOKEN_H_
#define _REV_CORE_INTERPRETER_TOKEN_H_

namespace rev
{
	struct CToken
	{
		unsigned	type;	// Token type identifier
		char * text;		// Text that matched the token
		unsigned line;		// Line at which the token was found
		unsigned pos;		// Position of the token inside it's line
	};
}	// namespace rev

#endif // _REV_CORE_INTERPRETER_TOKEN_H_