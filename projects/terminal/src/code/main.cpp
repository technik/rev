//----------------------------------------------------------------------------------------------------------------------
// Rev Terminal
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On January 15th, 2013
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <string>
#include <iostream>

// Rev headers
#include <revScript/variant/variant.h>
#include <revScript/scriptVM.h>

using std::string;
using std::cin;
using rev::script::Variant;

int main (int //_argc
	, const char** //_argv
	)
{
	Variant num(3);

	rev::script::ScriptVM vm;
	vm.setVar("Test01", num);

	Variant dst;
	vm.getVar("Test01", dst);

	std::cout << dst.asInteger();
	
	{
		string code;
		cin >> code;
	}
	//yyparse();
	return 0;
}