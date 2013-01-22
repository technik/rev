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
	Variant num(0);

	rev::script::ScriptVM vm;
	vm.setVar("a", num);

	Variant dst;
	vm.getVar("a", dst);

	std::cout << dst.asInteger() << std::endl;
	
	{
		string code;
		cin >> code;
		vm.execute(code.c_str());
		vm.getVar("a", dst);

		std::cout << dst.asInteger() << std::endl;
		cin >> code;
	}
	return 0;
}