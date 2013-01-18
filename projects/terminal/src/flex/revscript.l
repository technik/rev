/* Flex syntax file for rev script language */
%{
// This code is to be copied as is
#include <cstdio>
int copyText(char* _dst, unsigned _maxSize);

#define YY_INPUT(buf,result,max_size) { result = copyText(buf, max_size); }

%}

%option yylineno
%option noyywrap

%%

Hello	{ printf("Hello world!"); }

%%

int main(int, const char**)
{
	yylex();
	return 0;
}

int copied = 0;

int copyText(char* _dst, unsigned _maxSize)
{
	const char text[] = "Hello";
	for(unsigned i = 0; i < _maxSize; ++i)
	{
		if(copied < 5) {
			_dst[i] = text[copied++];
		} else {
		return (i==0)?YY_NULL:i; 
		}
	}
	return _maxSize;
}