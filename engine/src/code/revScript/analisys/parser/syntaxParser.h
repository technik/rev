////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script syntax parser

#ifndef _REV_SCRIPT_ANALISYS_PARSER_SINTAXPARSER_H_
#define _REV_SCRIPT_ANALISYS_PARSER_SINTAXPARSER_H_

#include <revCore/interpreter/parser.h>

namespace rev { namespace script
{
	class CRevScriptParser
	{
	public:
		static void		init();
		static CParser*	get();
		static void		end();

	private:
		static CParser*	sParser;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_ANALISYS_PARSER_SINTAXPARSER_H_
