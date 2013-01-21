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
	char*			text;
}


%token <integer> 	INTEGER
%token <text>		IDENTIFIER

%%

revScript	:	statement
			;
statement	:	IDENTIFIER '=' expression
			|	/* empty */
			;
expression	:	sum
			;
sum			:	INTEGER '+' INTEGER		{ gActiveBackend->matchSum(); }
			;

%%

void yyerror(char const*)
{
}