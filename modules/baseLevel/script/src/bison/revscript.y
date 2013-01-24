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
	float			real;
	std::string*	text;
}


%token <integer> 	INTEGER
%token <real>		REAL
%token <text>		IDENTIFIER

%%

revScript	:	IDENTIFIER '=' expression ';'		{ gActiveBackend->matchAssign($1); }
			;
expression	:	INTEGER							{ gActiveBackend->matchInteger($1); }
			|	REAL							{ gActiveBackend->matchReal($1);	}
			;

%%

void yyerror(char const*)
{
}