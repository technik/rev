%{

#include <codeTools/log/log.h>
#include <cstring>
#include <rev.tab.h>

namespace rev { namespace script
{
}	// namespace script
}	// namespace token

%}

%%

( |\t|\n|\r)+			{}
[a-zA-Z_][a-zA-Z0-9_]*	{ yyval.mText = new char[yyleng]; strncpy(yyval.mText, yytext, yyleng); return IDENTIFIER; }
;						{ return ';';}
=						{ return '='; }

%%