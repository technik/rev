%{
/* Bison grammar file for rev script language */

#include <stdio.h>
// Bison declarations (used during parsing, defined in the epilogue)
int yylex(void);
void yyerror(char const*);

%}

%union{
	int	integer;
}


%token <integer> INTEGER

%%

revScript	: INTEGER '+' INTEGER			{ printf("%d\n", $1+$3);}

%%

void yyerror(char const*)
{
}