/* Flex syntax file for rev script language */
%{
// This code is to be copied as is
#include <cstdio>
#include <string>

#include "parser.h"
#include "scriptVMBackend.h"

extern rev::script::ScriptVMBackend* gActiveBackend;

#define YY_INPUT(buf,result,max_size) { result = gActiveBackend->retrieveCode(buf, max_size); }
#define YY_NO_UNISTD_H

void copyText(char*& _dst, const char* _src, unsigned _len)
{
	_dst = new char[_len+1];
	for(unsigned i = 0; i < _len; ++i)
		_dst[i] = _src[i];
	_dst[_len] = '\0';
}

%}

%option noyywrap
%option never-interactive

%%

[ \t\r\n]					;
-?[0-9]+\.[0-9]*f?			{ yylval.real = atof(yytext); return REAL; }
-?[0-9]+					{ yylval.integer = atoi(yytext); return INTEGER; }
"+"							{ return '+'; }
"="							{ return '='; }
";"							{ return ';'; }
[a-zA-Z_][a-zA-Z_0-9]*		{ yylval.text = new std::string(yytext, yyleng); return IDENTIFIER; }

%%
