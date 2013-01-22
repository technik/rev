%{
/* Bison grammar file for rev script language */

#include <stdio.h>
#include <string>

#include "scriptVMBackend.h"

// Bison declarations (used during parsing, defined in the epilogue)
int yylex(void);
void yyerror(char const*);

rev::script::ScriptVMBackend*	gActiveBackend = 0;

%}

%union{
	int				integer;
	std::string*	text;
}


%token <integer> 	INTEGER
%token <text>		IDENTIFIER

%%

revScript	:	IDENTIFIER '=' INTEGER ';'		{ gActiveBackend->matchAssign($1, $3); }
			;

%%

void yyerror(char const*)
{
}