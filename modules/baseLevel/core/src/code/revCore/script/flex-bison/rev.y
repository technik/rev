%{
//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// File generated using gnu bison
//----------------------------------------------------------------------------------------------------------------------
// rev grammar

namespace rev { namespace script
{
%}

/* Bison declarations */

%union
{
	int		mInt;
	char*	mText;
	float	mReal;
}

%token<mText>	IDENTIFIER;
%token<mText>	STRING;
%token<mInt>	INTEGER;
%token<mReal>	FLOAT;

%%

script_code:					code_line_lst
							;
code_line_lst:				stmt code_line_lst
							|
							;
stmt:						IDENTIFIER '=' expression ';'
							;
expression:					FLOAT
							;
							
%%

}	// namespace script
} // namespace rev